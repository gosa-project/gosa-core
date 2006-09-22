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
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog

  var $FAIstate         = "";

  function faiPartitionTable ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    $this->acl ="#all#";

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read disks from ldap ... and then the partition definitions for the disks.
     */
    if($dn != "new"){
      $this->dn =$dn;

      /* Set acls
       */
      $ui   = get_userinfo();
      $acl  = get_permissions ($this->dn, $ui->subtreeACL);
      $acli = get_module_permission($acl, "FAIclass", $this->dn);
      $this->acl=$acli;

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

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

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
      $this->dialog->acl = $this->acl;
      $this->dialog->FAIstate = $this->FAIstate;
      $this->is_dialog = true;
    }

    /* Edit disk.
     * Open dialog which allows us to edit the selected entry 
     */    

    if($this->dn != "new"){
      $_SESSION['objectinfo']= $this->dn;
    }

    if((isset($_POST['EditDisk']))&&(isset($_POST['disks']))){
      $usedDiskNames =array();

      $Udisk = $_POST['disks'][0];
      
      foreach($this->disks  as $key=>$disk){
        if($key != $Udisk){
          $usedDiskNames[]= $key;
        }
      }
      $this->dialog = new faiPartitionTableEntry($this->config,$this->dn,$usedDiskNames,$this->disks[$Udisk]); 
      $this->dialog->acl = $this->acl;
      $this->dialog->FAIstate = $this->FAIstate;
     
      /* Set object info string, which will be displayed in plugin info line */ 
      if(isset($this->disks[$Udisk]['dn'])){
        $_SESSION['objectinfo'] = $this->disks[$Udisk]['dn'];
      }else{
        $_SESSION['objectinfo'] = "";
      }
    
      $this->is_dialog = true;
    }

    /* Edit aborted, close dialog, without saving anything
     */
    if(isset($_POST['CancelDisk'])){
      unset($this->dialog);
      $this->dialog = NULL;
      $this->is_dialog=false;
    }

    /* Dialog saved
     * Save given data from Dialog, if no error is occured
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
          $this->dialog = NULL;
          $this->is_dialog=false;
          ksort($this->disks);
        }
      }else{
        $this->dialog = NULL;
        $this->is_dialog=false;
      }
    }

    /* Delete selected disk drive from list
     * Assign delete status for all its partitions      
     */
    if((isset($_POST['DelDisk']))&&(!empty($_POST['disks']))){
      if($this->FAIstate != "freeze"){
        foreach($_POST['disks'] as $disk) {
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

    /* Display dialog if one is defined
     */
    if(isset($this->dialog)){
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
   
    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
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
    return($a_return);
  }


  /* Delete me, and all my subtrees
   */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->rmdir_recursive($this->dn);
    $this->handle_post_events("remove");    
  
    /* This cannot be removed... */
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

    if($this->new){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }else{
      /* Add partition table to ldap
       */
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
    }
    show_ldap_error($ldap->get_error(), _("Saving FAI partition table base failed"));
  
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
        $ldap->cd($disk_dn);
        $ldap->rmdir_recursive($disk_dn);
        show_ldap_error($ldap->get_error(), _("Removing FAI partition table failed"));
      }elseif($disk['status']== "edited"){
        if(empty($disk_attrs['description'])){
          $disk_attrs['description']=array();
        }
        $ldap->cd($disk_dn);
        $this->cleanup();
        $ldap->modify ($disk_attrs); 
        show_ldap_error($ldap->get_error(), _("Saving FAI partition table failed"));
      }elseif($disk['status']== "new"){
        if(empty($disk_attrs['description'])){
          unset($disk_attrs['description']);
        }
        $ldap->cd($this->config->current['BASE']);
        $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $disk_dn));
        $ldap->cd($disk_dn);
        $ldap->add($disk_attrs);
        show_ldap_error($ldap->get_error(), _("Saving FAI partition table failed"));
      }else{
        print_red("unknown status while saving disks");
      }

      $this->handle_object_tagging($disk_dn, $this->gosaUnitTag);

      /* Collect all opperations. Delete first than add new entries .*/
      $Todo['delete'] = array();
      $Todo['rest'] = array();

      /* Skip partition handling if current disk is marked as deleted */
      if($disk['status']!="delete"){

        /* Add all partitions */
        foreach($disk['partitions'] as $key => $partition){

          /* Create attribute array */
          $partition_attrs = array();
          foreach($partition as $key => $value){
            if(!empty($value)){
              $partition_attrs[$key]=$value;        
            }else{
              unset($partition_attrs[$key]);        
            }
          }

          /* Create partition dn and add necessary attributes */
          $partition_dn= "FAIpartitionNr=".$partition_attrs['FAIpartitionNr'].",".$disk_dn;      
          $partition_attrs['objectClass']= array("top","FAIclass","FAIpartitionEntry");
          $partition_attrs['cn']= $partition_attrs['FAIpartitionNr'];

          /* Unset status attributes */
          unset($partition_attrs['status']);
          unset($partition_attrs['old_cn']);

          /* Check if this partition is realy new .. */
          if($partition['status']=="new"){
            $ldap->cat($partition_dn,array("objectClass"));
            if($ldap->count()){
              $partition['status']="edited";
            }
          }

          /* Set mount point top swap if it is emtpy */
          if((!isset($partition['FAImountPoint']))||(empty($partition['FAImountPoint']))){
            $partition_attrs['FAImountPoint']="swap";
          }

          /* Collect all operations */
          if(($partition['status'] == "delete")&&($disk['status']!="new")){
            $Todo['delete'][$partition_dn] = $partition_attrs;
          }elseif($partition['status'] == "new"){
            $Todo['rest'][$partition_dn] = $partition_attrs;
          }elseif($partition['status'] == "edited"){
            $Todo['rest'][$partition_dn] = $partition_attrs;
          } 
        }
      }

      /* First of all, delete old entries */
      foreach($Todo['delete'] as $partition_dn => $attrs){
          $ldap->cd($partition_dn);
          $ldap->rmdir_recursive($partition_dn);
          show_ldap_error($ldap->get_error(), _("Removing FAI partition table entry failed"));
      }
    
      /* Add/edit entries */
      foreach($Todo['rest'] as $partition_dn => $partition_attrs){

        /* Check if entry exists */
        $ldap->cat($partition_dn);
        if($ldap->count()){

          /* Fix problem with missing objectClass "gosaAdministrativeUnitTag" for partitions */
          $attrs = $ldap->fetch();
          if(isset($attrs['objectClass']) && in_array_ics("gosaAdministrativeUnitTag",$attrs['objectClass'])){
            $partition_attrs['objectClass'][] = "gosaAdministrativeUnitTag";
          }
          if(empty($partition_attrs['description'])){
            $partition_attrs['description']=array();
          }
          $ldap->cd($partition_dn);
          $this->cleanup();
          $ldap->modify ($partition_attrs); 
          show_ldap_error($ldap->get_error(), _("Saving FAI partition table entry failed"));
        }else{

          if(empty($partition_attrs['description'])){
            unset($partition_attrs['description']);
          }

          $ldap->cd($this->config->current['BASE']);
          $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $partition_dn));
          $ldap->cd($partition_dn);
          $ldap->add($partition_attrs);
          show_ldap_error($ldap->get_error(), _("Saving FAI partition table entry failed"));
        }
      }
      $this->handle_object_tagging($partition_dn, $this->gosaUnitTag);
    }
    $this->handle_post_events("add");
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
