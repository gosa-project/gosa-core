<?php

class faiScript extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIscript");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAIscriptEntry";
  var $subClasses       = array("top","FAIclass","FAIscriptEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "faiScriptEntry";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description","FAIpriority","FAIscript"); 

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  function faiScript ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */
    if($dn != "new"){
      $this->dn =$dn;

      /* Read all leaf objects of this object (For FAIscript this would be FAIscriptEntry)
       */
      $ldap     = $this->config->get_ldap_link();
      $ldap->cd ($this->dn);
      $ldap->search("(&(objectClass=FAIclass)(objectClass=".$this->subClass."))",$this->subAttributes);

      while($object = $ldap->fetch()){
        /* Set status for save management */
  
        foreach($this->subAttributes as $attrs){
          if(!isset($object[$attrs][0])){
            $this->SubObjects[$object['cn'][0]][$attrs]="";
          }else{
            $this->SubObjects[$object['cn'][0]][$attrs]=$object[$attrs][0];
          }
        }
      
        $this->SubObjects[$object['cn'][0]]['status']      = "edited";
        $this->SubObjects[$object['cn'][0]]['dn']          = $object['dn'];
      }
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new sub object */
    if(isset($_POST['AddSubObject'])){
      $this->dialog= new $this->subClassName($this->config,"new");
      $this->is_dialog=true;
    }

    /* Edit selected Sub Object */
    if((isset($_POST['EditSubObject']))&&(isset($_POST['SubObject']))){
      $this->dialog= new $this->subClassName($this->config,$this->dn,$this->SubObjects[$_POST['SubObject']]);
      $this->is_dialog=true;
    }
    
    /* Remove Sub object */
    if((isset($_POST['DelSubObject']))&&(isset($_POST['SubObject']))){
      if($this->SubObjects[$_POST['SubObject']]['status'] == "edited"){
        $this->SubObjects[$_POST['SubObject']]['status']= "delete";
      }else{
        unset($this->SubObjects[$_POST['SubObject']]);
      }
    }

    /* Save Dialog */
    if(isset($_POST['SaveSubObject'])){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)>0){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $obj = $this->dialog->save();
        if(isset($obj['remove'])){
          if($this->SubObjects[$obj['remove']['from']]['status']=="edited"){
            $this->SubObjects[$obj['remove']['from']]['status'] = "delete";
          }elseif($this->SubObjects[$obj['remove']['from']]['status']=="new"){
            unset($this->SubObjects[$obj['remove']['from']]);
          }
          $obj['status'] = "new";
          $this->SubObjects[$obj['remove']['to']] = $obj;
          unset($this->SubObjects[$obj['remove']['to']]['remove']);
        }else{
          $this->SubObjects[$obj['cn']]=$obj;
        }
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=NULL;
      }
    }

    /* Cancel Dialog */
    if(isset($_POST['CancelSubObject'])){
      $this->is_dialog=false; 
      unset($this->dialog);
      $this->dialog=NULL;
    }

    /* Print dialog if $this->dialog is set */
    if($this->dialog){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return($display);
    }

    $smarty->assign("SubObjects",$this->getList());
    $smarty->assign("SubObjectKeys",array_flip($this->getList()));
    /* Assign variables */
    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
    }

    $display.= $smarty->fetch(get_template_path('faiScript.tpl', TRUE));
    return($display);
  }

  /* Generate listbox friendly SubObject list
  */
  function getList(){
    $a_return=array();
    foreach($this->SubObjects as $obj){
      if($obj['status'] != "delete"){
        $a_return[$obj['cn']]= $obj['cn']." [".$obj['description']."]";
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
    $str = utf8_encode("üצה");
    if((empty($this->description))||(preg_match("/[^a-z0-9".$str."\.,;:-_\? ]/i",$this->description))){
      $message[]=_("Please enter a valid description.");
    }
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
 
    $ldap = $this->config->get_ldap_link();
  
    /* Write FAIscript to ldap*/ 
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
 
    /* Prepare FAIscriptEntry to write it to ldap
     * First sort array.
     *  Because we must delete old entries first.
     * After deletion, we perform add and modify 
     */
    $Objects = array();
    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] == "delete"){
        $Objects[$name] = $obj; 
      }
    }
    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] != "delete"){
        $Objects[$name] = $obj; 
      }
    }

    foreach($Objects as $name => $obj){
      $tmp = array();
      foreach($this->subAttributes as $attrs){
        if(empty($obj[$attrs])){
          $obj[$attrs] = array();
        }
        $tmp[$attrs] = $obj[$attrs];
      }    
        
      $tmp['objectClass'] = $this->subClasses;
      
      $sub_dn = "cn=".$obj['cn'].",".$this->dn;
 
      if($obj['status'] == "delete"){
        $ldap->cd($sub_dn);
        $ldap->rmdir_recursive($sub_dn);
        $this->handle_post_events("remove");
      }elseif($obj['status'] == "edited"){
        $ldap->cd($sub_dn);
        $ldap->modify($tmp);
        $this->handle_post_events("modify");
      }elseif($obj['status']=="new"){
        $ldap->cd($sub_dn);
        $ldap->create_missing_trees($sub_dn);
        $ldap->cd($sub_dn);
        $ldap->add($tmp); 
        $this->handle_post_events("add");
      }
      show_ldap_error($ldap->get_error()); 
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
