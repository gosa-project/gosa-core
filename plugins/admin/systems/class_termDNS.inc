<?php

class termDNS extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("ipHostNumber","macAddress");
  var $objectclasses= array("whatever");

  var $ipHostNumber ="";
  var $macAddress ="";  

  function termDNS ($config, $dn,$objectClasses)
  {
    $this->objectclasses= $objectClasses;
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
	/* Call parent execute */
    $smarty= get_smarty();
    $display= "";
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    $smarty->assign("staticAddress","<font class=\"must\">*</font>");
    $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
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
    $message= array();

    return ($message);
  }


  /* Save to LDAP */
  function save($dn)
  {
    $org_dn = $dn;
    $this->dn = $dn;
    plugin::save();

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
