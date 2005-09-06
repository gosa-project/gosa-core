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

  function faiPartitionTable ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read disks from ldap ... and then the partition definitions for the disks.
     */
    if($dn != "new"){
      $this->dn =$dn;

      /* Read all disks from ldap taht are defined fot this partition table 
       */
      $ldap = $this->config->get_ldap_link();
      $ldap->cd ($this->dn);
      $ldap->search("(&(objectClass=FAIclass)(objectClass=FAIpartitionDisk))",array("*"));
      while($object = $ldap->fetch()){
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
          $this->disks[$name]['partitions'][$partition['FAIpartitionNr']] = $partition; 
        }  
      }
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
  
    /* Add Disk to this Partitionset
     * This code adds a new HDD to the disks 
     * A new Dialog will be opened 
     */
    if(isset($_POST['AddDisk'])){
      $this->dialog = new faiPartitionTableEntry($this->config,$this->dn); 
      $this->is_dialog = true;
    }

    /* Edit disk.
     * Open dialog which allows us to edit the selected entry 
     */    
    if((isset($_POST['EditDisk']))&&(isset($_POST['disks']))){
      $this->dialog = new faiPartitionTableEntry($this->config,$this->dn,$this->disks[$_POST['disks']]); 
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
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $disk = $this->dialog->save();
        $this->disks[$disk['cn']]=$disk; 
        unset($this->dialog);
        $this->dialog = NULL;
        $this->is_dialog=false;
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
    
    $disks = $this->getDisks();
    $smarty->assign("disks"   ,$disks);
    $smarty->assign("diskKeys",array_flip($disks));
    $display.= $smarty->fetch(get_template_path('faiPartitionTable.tpl', TRUE));
    return($display);
  }

  function getDisks(){
    /* Return all available disks for this partition table
     * Return in listBox friendly array
     */
    $a_return = array();
    foreach($this->disks as $key => $disk){
      $cnt = count( $disk['partitions']);
      if(!empty($disk['description'])){
        $a_return[$key]=  $disk['cn']." [".$disk['description']."] - "._("Number of Partitions")." :".$cnt;
      }else{
        $a_return[$key]=  $disk['cn']." - "._("Number of Partitions")." :".$cnt;
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
    $message= array();
    if(empty($this->description)){
      $message[]=_("Please enter a valid description.");
    }

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

    /* remove old entry */ 
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn);
    if($ldap->count()){
      $ldap->cd ($this->dn);
      $ldap->rmdir_recursive($this->dn);
      show_ldap_error($ldap->get_error());
    }

    /* Add partition table to ldap
     */
    $ldap->cd($this->dn);
    $ldap->create_missing_trees($this->dn);
    $ldap->cd($this->dn);
    $ldap->add($this->attrs);
    show_ldap_error($ldap->get_error());
  
    /* Append all disks to ldap */
    foreach($this->disks as $cn=>$disk){
      $disk_cn    = "cn=".$cn.",".$this->dn;
      $disk_attrs['cn']           =  $disk['cn'];
      $disk_attrs['description']  =  $disk['description']; 
      $disk_attrs['objectClass']=array("top","FAIclass","FAIpartitionDisk");

      $ldap->cd($disk_cn);
      $ldap->create_missing_trees($disk_cn);
      $ldap->cd($disk_cn);
      $ldap->add($disk_attrs);
      show_ldap_error($ldap->get_error());

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

        $partition_dn                       = "cn=".$partition_attrs['cn'].",".$disk_cn;      
        $partition_attrs['objectClass']     = array("top","FAIclass","FAIpartitionEntry");

        $ldap->cd($partition_dn);
        $ldap->create_missing_trees($partition_dn);
        $ldap->cd($partition_dn);
        $ldap->add($partition_attrs);
        show_ldap_error($ldap->get_error());
      }
    }
    $this->handle_post_events("add");
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
