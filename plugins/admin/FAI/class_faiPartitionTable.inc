<?php

class faiPartitionTable extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description");
  var $objectclasses= array("top","FAIclass","FAIpartitionTable");

  var $cn           = "";
  var $description  = "";
  var $disks   = array();
  var $is_dialog    = false;
  var $dialog       = NULL;  
  function faiPartitionTable ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->partitions = array();
  
    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap taht belong to this entry
     * First read disks from ldap ... and then the partition definitions for the disks
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

    if((isset($_POST['EditDisk']))&&(isset($_POST['disks']))){
      $this->dialog = new faiPartitionTableEntry($this->config,$this->dn,$this->disks[$_POST['disks']]); 
      $this->is_dialog = true;
    }

    if(isset($_POST['CancelDisk'])){
      unset($this->dialog);
      $this->dialog = NULL;
      $this->is_dialog=false;
    }
  
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

    if(isset($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
      if($this->$attrs){
        $smarty->assign($attrs."CHK"," ");
      }else{
        $smarty->assign($attrs."CHK"," disabled ");
      }
    }

    $disks = array();
    foreach($this->partitions as $part){  
      $disks[$part['cn']] = $part['cn']." [".$part['description']."]";
    }


    $disks = $this->getDisks();
    $smarty->assign("disks"   ,$disks);
    $smarty->assign("diskKeys",array_flip($disks));
    $display.= $smarty->fetch(get_template_path('faiPartitionTable.tpl', TRUE));
    return($display);
  }

  function getDisks(){
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

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
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

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
   
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn);
    if($ldap->count()){
      $ldap->cd ($this->dn);
      $ldap->rmdir_recursive($this->dn);
      show_ldap_error($ldap->get_error());
    }
    $ldap->cd($this->dn);
    $ldap->create_missing_trees($this->dn);
    $ldap->cd($this->dn);
    $ldap->add($this->attrs);
    show_ldap_error($ldap->get_error());
  
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
