<?php

class faiPartitionTable extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("cn","description");
  var $objectclasses    = array("top","FAIclass","FAIpartitionTable");

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $disks            = array();  // All defined Disks 
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort

  var $FAIstate         = "";
  var $ui;

  var $view_logged      = FALSE;

  function faiPartitionTable (&$config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    $this->acl ="#all#";

    $this->ui = get_userinfo();    

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read disks from ldap ... and then the partition definitions for the disks.
     */
    if($dn != "new"){
      $this->dn =$dn;

      /* Get FAIstate
       */
      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }

      /* Read all disks from ldap taht are defined fot this partition table 
       */
      $ldap = $this->config->get_ldap_link();
      $ldap->cd ($this->dn);
      $ldap->search("(&(objectClass=FAIclass)(objectClass=FAIpartitionDisk))",array("*"));
      while($object = $ldap->fetch()){

        /* Skip objects, that are tagged as removed */
        if(isset($object['FAIstate'][0])){
          if(preg_match("/removed$/",$object['FAIstate'][0])){
            continue;
          }
        }

        $this->disks[$object['cn'][0]]['status']      = "edited";
        $this->disks[$object['cn'][0]]['dn']          = $object['dn'];
        $this->disks[$object['cn'][0]]['cn']          = $object['cn'][0];
        if(isset($object['description'][0])){
          $this->disks[$object['cn'][0]]['description'] = $object['description'][0];
        }else{
          $this->disks[$object['cn'][0]]['description'] = "";
        }
        $this->disks[$object['cn'][0]]['partitions']   = array();
      }
  
      /* read all partitions for each disk 
       */
      foreach($this->disks as $name => $disk){
        $ldap->cd ($disk['dn']);
        $ldap->search("(&(objectClass=FAIclass)(objectClass=FAIpartitionEntry))",array("*"));
        while($partition = $ldap->fetch()){

          /* Skip objects, that are tagged as removed */
          if(isset($partition['FAIstate'][0])){
            if(preg_match("/removed$/",$partition['FAIstate'][0])){
              continue;
            }
          }

          /* remove count ... from ldap result 
           */
          foreach($partition as $key=>$val){
            if((is_numeric($key))||($key=="count")||($key=="dn")){
              unset($partition[$key]);
            }else{
              $partition[$key] = $val[0];
            }
          }

          /* Append fetched partitions
           */
          $partition['status']="edited";
          $this->disks[$name]['partitions'][$partition['FAIpartitionNr']] = $partition; 
        }  
      }
    }
    ksort($this->disks);
  }


  function acl_base_for_current_object($dn)
  {
    if($dn == "new"){
      if($this->dn == "new"){
        $dn = session::get('CurrentMainBase');
      }else{
        $dn = $this->dn;
      }
    }
    return($dn);
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","fai/".get_class($this),$this->dn);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
 
    /* Add Disk to this Partitionset
     * This code adds a new HDD to the disks 
     * A new Dialog will be opened 
     */
    if(isset($_POST['AddDisk'])){
      $usedDiskNames =array();
      foreach($this->disks as $key=>$disk){
        $usedDiskNames[]= $key;
      }
      $this->dialog = new faiPartitionTableEntry($this->config,$this->dn,$usedDiskNames); 
      $this->dialog->set_acl_base($this->acl_base_for_current_object($this->dn));
      $this->dialog->set_acl_category("fai");
      $this->dialog->FAIstate = $this->FAIstate;
      $this->is_dialog = true;
    }

    /* Edit disk.
     * Open dialog which allows us to edit the selected entry 
     */    

    if($this->dn != "new"){
      session::set('objectinfo',$this->dn);
    }

    if((isset($_POST['EditDisk']))&&(isset($_POST['disks']))){
      $usedDiskNames =array();
      $Udisk = $_POST['disks'][0];
      if(isset($this->disks[$Udisk])){

        foreach($this->disks  as $key=>$disk){
          if($key != $Udisk){
            $usedDiskNames[]= $key;
          }
        }

        /* Set object info string, which will be displayed in plugin info line */ 
        if(isset($this->disks[$Udisk]['dn'])){
          session::set('objectinfo',$this->disks[$Udisk]['dn']);
          $dn = $this->disks[$Udisk]['dn'];
        }else{
          session::set('objectinfo',"");
          $dn = "new";
        }

        $this->dialog = new faiPartitionTableEntry($this->config,$this->dn,$usedDiskNames,$this->disks[$Udisk]); 
        $this->dialog->set_acl_base($this->acl_base_for_current_object($dn));
        $this->dialog->set_acl_category("fai");
        $this->dialog->FAIstate = $this->FAIstate;

        $this->is_dialog = true;
      }
    }

    /* Edit aborted, close dialog, without saving anything
     */
    if(isset($_POST['CancelDisk'])){
      unset($this->dialog);
      $this->dialog = FALSE;
      $this->is_dialog=false;
    }

    /* Dialog saved
     * Save given data from Dialog, if no error is occurred
     */
    if(isset($_POST['SaveDisk'])){

      if($this->FAIstate != "freeze"){
        $this->dialog->save_object();
        if(count($this->dialog->check())){
          foreach($this->dialog->check() as $msg){
            print_red($msg);
          }
        }else{
          $disk = $this->dialog->save();
          if(isset($disk['rename'])){
            if($this->disks[$disk['rename']['from']]['status']=="edited"){
              $this->disks[$disk['rename']['from']]['status']="delete";
            }else{
              unset($this->disks[$disk['rename']['from']]);
            }

            foreach($disk['partitions'] as $key => $val){
              if($disk['partitions'][$key]['status']!="delete"){
                $disk['partitions'][$key]['status']= "new";
              }
            }

            $disk['status']="new";
            $disk['cn']= $disk['rename']['to'];
          }

          $this->disks[$disk['cn']]=$disk; 
          unset($this->dialog);
          $this->dialog = FALSE;
          $this->is_dialog=false;
          ksort($this->disks);
        }
      }else{
        $this->dialog = FALSE;
        $this->is_dialog=false;
      }
    }

    /* Delete selected disk drive from list
     * Assign delete status for all its partitions      
     */
    if((isset($_POST['DelDisk']))&&(!empty($_POST['disks']))){
      if($this->FAIstate != "freeze"){
        foreach($_POST['disks'] as $disk) {

          if(isset($this->disks[$disk])){
            if($this->disks[$disk]['status']=="edited"){
              $this->disks[$disk."-delete"]=$this->disks[$disk];
              unset($this->disks[$disk]);
              $disk = $disk."-delete";        
              $this->disks[$disk]['status']="delete";
              foreach($this->disks[$disk]['partitions'] as $name => $value ){
                if($value['status']=="edited"){
                  $this->disks[$disk]['partitions'][$name]['status']="delete"; 
                }else{
                  unset($this->disks[$disk]['partitions'][$name]);
                }
              }
            }else{
              unset($this->disks[$disk]);
            }
          }
        }
      }
    }

    /* Display dialog if one is defined
     */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    /* Assign all attributes to smarty engine
     */
    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
      if($this->$attrs){
        $smarty->assign($attrs."CHK"," ");
      }else{
        $smarty->assign($attrs."CHK"," disabled ");
      }
    }
   
    $dn = $this->acl_base_for_current_object($this->dn);
    $smarty->assign("sub_object_is_addable",
        preg_match("/c/",$this->ui->get_permissions($dn,"fai/faiPartitionTableEntry")) &&
        !preg_match("/freeze/",$this->FAIstate));

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    $disks = $this->getDisks();
    $smarty->assign("disks"   ,$disks);
    $display.= $smarty->fetch(get_template_path('faiPartitionTable.tpl', TRUE));
    return($display);
  }

  function getDisks(){
    /* Return all available disks for this partition table
     * Return in listBox friendly array
     */
    $a_return = array();
    foreach($this->disks as $key => $disk){

      $dn = "new";
      if(isset($obj['dn'])){
        $dn = $obj['dn'];
      }
      $dn = $this->acl_base_for_current_object($dn);
      $acl = $this->ui->get_permissions($dn,"fai/faiPartitionTableEntry");
      if(preg_match("/(r|w)/",$acl)) {

        if($disk['status'] != "delete"){
          $cnt=0;
          foreach($disk['partitions'] as $val){
            if($val['status']!="delete"){
              $cnt ++;
            }
          }
          if(!empty($disk['description'])){
            if($cnt == 1){
              $a_return[$key]=  $disk['cn']." [".$disk['description']."], ".sprintf(_("%s partition"), $cnt);
            }else{
              $a_return[$key]=  $disk['cn']." [".$disk['description']."], ".sprintf(_("%s partition(s)"), $cnt);
            }
          }else{
            if($cnt == 1){
              $a_return[$key]=  $disk['cn'].", ".sprintf(_("%s partition"), $cnt);
            }else{
              $a_return[$key]=  $disk['cn'].", ".sprintf(_("%s partition(s)"), $cnt);
            }
          }
        }
      }
    }
    return($a_return);
  }


  /* Delete me, and all my subtrees
   */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);

    $faifilter = session::get('faifilter');
    $use_dn = preg_replace("/".normalizePreg(fai_func::get_release_dn($this->dn))."/i", $faifilter['branch'], $this->dn);
    if($faifilter['branch'] == "main"){
      $use_dn = $this->dn;
    }

    fai_func::prepare_to_save_FAI_object($use_dn,array(),true);

    new log("remove","fai/".get_class($this),$use_dn,$this->attributes);   
 
    foreach($this->disks as $disk){

      $disk_dn = "cn=".$disk['cn'].",".$this->dn;
      $use_dn = preg_replace("/".normalizePreg(fai_func::get_release_dn($this->dn))."/i", $faifilter['branch'], $disk_dn);
      if($faifilter['branch'] == "main"){
        $use_dn = $disk_dn;
      }
      fai_func::prepare_to_save_FAI_object($use_dn,array(),true);

      foreach($disk['partitions'] as $key => $partition){    
     
        $partition_dn= "FAIpartitionNr=".$partition['FAIpartitionNr'].",".$disk_dn;      
        $use_dn = preg_replace("/".normalizePreg(fai_func::get_release_dn($this->dn))."/i", $faifilter['branch'], $partition_dn);
        if($faifilter['branch'] == "main"){
          $use_dn = $disk_dn;
        }
        fai_func::prepare_to_save_FAI_object($use_dn,array(),true);
      }
    }
  }


  /* Save data to object 
   */
  function save_object()
  {
    if($this->FAIstate == "freeze") return;
    plugin::save_object();
    foreach($this->attributes as $attrs){
      if(isset($_POST[$attrs])){
        $this->$attrs = $_POST[$attrs];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {

    plugin::save();
    /* Save current settings.
     * 1 : We must save the partition table, with its description and cn 
     * 2 : Append Disk with cn and  description.
     * 3 : Save partitions for each disk
     */  

    $ldap = $this->config->get_ldap_link();

    fai_func::prepare_to_save_FAI_object($this->dn,$this->attrs);
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of FAI/partition table with dn '%s' failed."),$this->dn));

    if($this->initially_was_account){
      new log("modify","fai/".get_class($this),$this->dn,$this->attributes);
    }else{
      new log("create","fai/".get_class($this),$this->dn,$this->attributes);
    }
 
    /* Do object tagging */
    $this->handle_object_tagging();
  
    /* Sort entries, because we must delete entries with status="delete" first */
    $order = array();
    foreach($this->disks as $key => $disk){
      if($disk['status'] == "delete"){
        $order[$key] = $disk;
      }
    }
    foreach($this->disks as $key => $disk){
      if($disk['status'] != "delete"){
        $order[$key] = $disk;
      }
    }

    /* Append all disks to ldap */
    foreach($order as $cn=>$disk){
      $disk_dn                    = "cn=".$disk['cn'].",".$this->dn;
      $disk_attrs['cn']           =  $disk['cn'];
      $disk_attrs['description']  =  $disk['description']; 
      $disk_attrs['objectClass']  =  array("top","FAIclass","FAIpartitionDisk");

      if($disk['status']=="new"){
        $ldap->cat($disk_dn,array("objectClass"));
        if($ldap->count()){
          $disk['status']="edited";
        }
      }

      /* Fix problem with missing objectClass "gosaAdministrativeUnitTag" for disks */
      $ldap->cat($disk_dn,array("objectClass"));
      $attrs = $ldap->fetch();
      if(isset($attrs['objectClass']) && in_array_ics("gosaAdministrativeUnitTag",$attrs['objectClass'])){
        $disk_attrs['objectClass'][] = "gosaAdministrativeUnitTag";
      }

      if($disk['status'] == "delete"){
        fai_func::prepare_to_save_FAI_object($disk_dn,array(),true);
        $this->handle_post_events("remove");
      }elseif($disk['status'] == "edited"){
        fai_func::prepare_to_save_FAI_object($disk_dn,$disk_attrs);
        $this->handle_post_events("modify");
      }elseif($disk['status']=="new"){
        fai_func::prepare_to_save_FAI_object($disk_dn,$disk_attrs);
        $this->handle_post_events("add");
      }

      $this->handle_object_tagging($disk_dn, $this->gosaUnitTag);

      if($disk['status']!="delete")
      /* Add all partitions */
      foreach($disk['partitions'] as $key => $partition){
        $partition_attrs = array();

        foreach($partition as $key => $value){
          if(!empty($value)){
            $partition_attrs[$key]=$value;        
          }else{
            unset($partition_attrs[$key]);        
          }
        }

        $partition_dn= "FAIpartitionNr=".$partition_attrs['FAIpartitionNr'].",".$disk_dn;      
        $partition_attrs['objectClass']= array("top","FAIclass","FAIpartitionEntry");
        $partition_attrs['cn']= $partition_attrs['FAIpartitionNr'];
        
        unset($partition_attrs['status']);
        unset($partition_attrs['old_cn']);

        if($partition['status']=="new"){
          $ldap->cat($partition_dn,array("objectClass"));
          if($ldap->count()){
            $partition['status']="edited";
          }
        }

        if((!isset($partition['FAImountPoint']))||(empty($partition['FAImountPoint']))){
          $partition_attrs['FAImountPoint']="swap";
        }

        /* Fix problem with missing objectClass "gosaAdministrativeUnitTag" for partitions */
        $ldap->cat($partition_dn,array("objectClass"));
        $attrs = $ldap->fetch();
        if(isset($attrs['objectClass']) && in_array_ics("gosaAdministrativeUnitTag",$attrs['objectClass'])){
          $partition_attrs['objectClass'][] = "gosaAdministrativeUnitTag";
        }

        if($partition['status'] == "delete"){
          fai_func::prepare_to_save_FAI_object($partition_dn,array(),true);
          $this->handle_post_events("remove");
        }elseif($partition['status'] == "edited"){
          fai_func::prepare_to_save_FAI_object($partition_dn,$partition_attrs);
          $this->handle_post_events("modify");
        }elseif($partition['status']=="new"){
          fai_func::prepare_to_save_FAI_object($partition_dn,$partition_attrs);
          $this->handle_post_events("add");
        }

        $this->handle_object_tagging($partition_dn, $this->gosaUnitTag);
      }
    }
    $this->handle_post_events("add");
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);
    /* Get FAIstate
     */
    if(isset($source['FAIstate'][0])){
      $this->FAIstate = $source['FAIstate'][0];
    }

    /* Read all disks from ldap taht are defined fot this partition table 
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($source['dn']);
    $ldap->search("(&(objectClass=FAIclass)(objectClass=FAIpartitionDisk))",array("*"));
    while($object = $ldap->fetch()){

      /* Skip objects, that are tagged as removed */
      if(isset($object['FAIstate'][0])){
        if(preg_match("/removed$/",$object['FAIstate'][0])){
          continue;
        }
      }

      $this->disks[$object['cn'][0]]['status']      = "edited";
      $this->disks[$object['cn'][0]]['dn']          = $object['dn'];
      $this->disks[$object['cn'][0]]['cn']          = $object['cn'][0];
      if(isset($object['description'][0])){
        $this->disks[$object['cn'][0]]['description'] = $object['description'][0];
      }else{
        $this->disks[$object['cn'][0]]['description'] = "";
      }
      $this->disks[$object['cn'][0]]['partitions']   = array();
    }

    /* read all partitions for each disk 
     */
    foreach($this->disks as $name => $disk){
      $ldap->cd ($disk['dn']);
      $ldap->search("(&(objectClass=FAIclass)(objectClass=FAIpartitionEntry))",array("*"));
      while($partition = $ldap->fetch()){

        /* Skip objects, that are tagged as removed */
        if(isset($partition['FAIstate'][0])){
          if(preg_match("/removed$/",$partition['FAIstate'][0])){
            continue;
          }
        }

        /* remove count ... from ldap result 
         */
        foreach($partition as $key=>$val){
          if((is_numeric($key))||($key=="count")||($key=="dn")){
            unset($partition[$key]);
          }else{
            $partition[$key] = $val[0];
          }
        }

        /* Append fetched partitions
         */
        $partition['status']="edited";
        $this->disks[$name]['partitions'][$partition['FAIpartitionNr']] = $partition; 
      }  
    }
    ksort($this->disks);
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName" => _("Partition table"),
          "plDescription" => _("FAI partition table"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 26,
          "plSection"     => array("administration"),
          "plCategory"    => array("fai"),
          "plProvidedAcls" => array(
            "cn"                => _("Name")."&nbsp;("._("Read only").")",
            "description"       => _("Description"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
