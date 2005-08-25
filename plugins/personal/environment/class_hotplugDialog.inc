<?php

class hotplugDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array("HOT_name","HOT_description","HOT_id");
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  

  var $HOT_name                 = "";
  var $HOT_description          = "";
  var $HOT_id                   = "";

  var $hotplugDevices           = ""; 
  var $hotplugDeviceList        = array(); 

  function hotplugDialog ($config, $dn= NULL,$use_existing=false )
  {
    $this->use_existing = $use_existing;
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    foreach($this->attributes as $s_attr){
      $smarty->assign($s_attr,$this->$s_attr);
    }

    if($this->use_existing){
      $this->hotplugDevices = $this->getHotplugs();
      $smarty->assign("hotplugDevices",$this->hotplugDevices);
      $smarty->assign("hotplugDeviceKeys",array_flip($this->hotplugDevices));
      $display.= $smarty->fetch(get_template_path('hotplugDialog.tpl', TRUE,dirname(__FILE__)));
    }else{
      $display.= $smarty->fetch(get_template_path('hotplugDialogNew.tpl', TRUE,dirname(__FILE__)));
    }
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['dialogissubmitted'])){
      foreach($this->attributes as $s_attr){
        if(isset($_POST[$s_attr])){
          $this->$s_attr = $_POST[$s_attr];
        }else{
          $this->$s_attr = false;
        }
      }
      plugin::save_object();
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    if(!$this->use_existing){
      if((empty($this->HOT_name))||(preg_match("/[^a-z0-9]/i",$this->HOT_name))){
        $message[]=_("Please specify a valid name.");
      }
      if((empty($this->HOT_description))||(preg_match("/[^a-z0-9!\"?.,;:-_\(\) ]/i",$this->HOT_description))){
        $message[]=_("Please specify a valid description.");
      }
      if((empty($this->HOT_id))){
        $message[]=_("Please specify a valid id.");
      }
    }else{
      if((!isset($_POST['hotplugName']))||(empty($_POST['hotplugName']))){
        $message[] = _("Please select an entry or press cancel.");
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* return generated entry from input fields*/
    if(!$this->use_existing){
      $a_return=array();
      $a_return['name']= $this->HOT_name; 
      $a_return['description']= $this->HOT_description; 
      $a_return['id']= $this->HOT_id;
      return($a_return); 
    }else{
      //return selected entry from select box
      $entry = $this->hotplugDeviceList[$_POST['hotplugName']];
      return $entry;
    }
  }

  function getHotplugs(){
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    $ldap->search("(objectClass=gotoEnvironment)",array("gotoHotplugDevice")); 

    $a_return = array();  
    $this->hotplugDeviceList = array();
    while($attr = $ldap->fetch()){
      if(isset($attr['gotoHotplugDevice'])){
        unset($attr['gotoHotplugDevice']['count']);
        foreach($attr['gotoHotplugDevice'] as $device){
          $tmp =$tmp2= array();
          $tmp = split("\|",$device);
          $a_return[$tmp[0]]= $tmp[0]." [".$tmp[1]."] ".$tmp[2];
          
          $tmp2['name']         = $tmp[0];
          $tmp2['description'] = $tmp[1];
          $tmp2['id']           = $tmp[2];

          $this->hotplugDeviceList[$tmp[0]]=$tmp2;
        }
      }
    }
  return($a_return);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
