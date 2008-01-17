<?php

class SelectDeviceType extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("ObjectGroup","SystemType");
  var $objectclasses= array("whatever");

  var $ObjectGroups   = array();
  var $SystemTypes       = array();

  var $ObjectGroup    = "";
  var $SystemType        = "";

  function SelectDeviceType  (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Get object groups */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
    $this->ObjectGroups['none']=_("none");
    while($attrs = $ldap->fetch()){
      $this->ObjectGroups[$attrs['dn']]= $attrs['cn'][0];
    }

    $this->SystemTypes =array("workstation"=>_("Workstation"), "terminal"=>_("Terminal"), "server"=>_("Server"));
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $smarty->assign("ObjectGroups"    ,$this->ObjectGroups);
    $smarty->assign("ObjectGroupKeys" ,array_flip($this->ObjectGroups));
    $smarty->assign("SystemTypes"     ,$this->SystemTypes);
    $smarty->assign("SystemTypeKeys"  ,array_flip($this->SystemTypes)); 

    $smarty->assign("ObjectGroup",$this->ObjectGroup);
    $smarty->assign("SystemType",$this->SystemType);

    $display.= $smarty->fetch(get_template_path('SelectDeviceType.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
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
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
