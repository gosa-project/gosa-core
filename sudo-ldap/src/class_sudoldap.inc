<?php
/*!
  \brief   sudo-ldap plugin
  \author  Guido Serra <gserra@guidoserra.it>
  \version 1.00
  \date    22.03.2008

  This class provides the functionality to read and write all attributes
  relevant for sudo-ldap from/to the LDAP.
 */

class sudoldap extends plugin
{
  /* Definitions */
  var $plHeadline= "sudo-ldap";
  var $plDescription= "This does something";

  /* Plugin specific values */
 
  var $dialog		= NULL;

  var $attributes	= array("sudoUser", 
				"sudoHost", 
				"sudoCommand", 
				"sudoRunAs", 
				"sudoOption");
  
  var $objectclasses  	= array("sudoRole");
  
  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function sudoldap ($config, $dn= NULL, $parent= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    /* Load bases attributes */
    plugin::plugin($config, $dn, $parent);

    $ldap= $this->config->get_ldap_link();
    
  }


  /* execute generates the html output for this node */
  function execute($isCopyPaste = false)
  {
	/* Call parent execute */
	plugin::execute();
	$display= "";
	$smarty= get_smarty();

	/* Add empty new zone 
	 */
	if(isset($_POST['AddRole']) && chkacl($this->acl,"sudoldap") == ""){
		$this->dialog = new sudoldapEditRole($this->config,$this->dn);
	}

	/* Show dialog 
	 */
	if($this->dialog!= NULL){
		$this->dialog->save_object();
		$this->dialog->parent = $this;
		return($this->dialog->execute());
	}

	$RoleList = new divSelectBox("sudoRoles");
	$RoleList -> SetHeight(300);
	$editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>
	<input type='image' src='images/edittrash.png' name='delZone_%s'>";
	/*
	foreach($this->Zones as $zone => $values ){
		$link = "<a href='?plug=".$_GET['plug']."&act=edit&id=%s'>%s</a>";
		$ZoneList->AddEntry(array(
			array("string" => sprintf($link,base64_encode($zone),getNameFromMix($zone))),
			array("string" => sprintf($link,base64_encode($zone),_("Reverse zone")." : ".getNameFromMix($values['ReverseZone']))),
			array("string" => _("TTL")." : ".$values['sOAttl']),
			array("string" => _("Class")." : ".$values['dNSClass']),
			array("string" => str_replace("%s",base64_encode($zone),$editImg))
            ));
	}    
	*/

	$smarty->assign("sudoldapACL",chkacl($this->acl,"sudoldap"));

	$smarty->assign("RoleList",$RoleList->DrawList());
	$display.= $smarty->fetch(get_template_path('sudoldap.tpl', TRUE));
	return($display);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
