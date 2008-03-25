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

  var $attributes	= array("cn",
  				"description",
  				"sudoUser", 
				"sudoHost", 
				"sudoCommand", 
				"sudoRunAs", 
				"sudoOption");
  
  var $objectclasses  	= array("sudoRole");

  var $Roles 		= array();
  
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

	$this->Roles = $this->getRoles();

	/* Edited or Added zone 
	 */
	if((isset($_POST['SaveRoleChanges'])) && is_object($this->dialog)){
		$this->dialog->save_object();

		/* Check for errors  
	 	 */
		if(count($this->dialog->check())){
			foreach($this->dialog->check() as $msgs){
				print_red($msgs); 
			}
		}else{
			/* add new/edited zone 
			 */
			$ret = $this->dialog->save();
			//if(!$this->dialog->isNew){
			//	unset($this->Roles[$this->dialog->OldRoleName]);
			//}
			$this->Roles[$ret['cn']] = $ret;
			$this->dialog = NULL;
		}
	}

	/* Cancel zone edit / new 
	 */
	if(isset($_POST['CancelRoleChanges'])){
		$this->dialog = NULL;
	}

	/* Add empty new zone 
	 */
	//if(isset($_POST['AddRole']) && chkacl($this->acl,"sudoldap") == ""){
	if(isset($_POST['AddRole'])){
		$this->dialog = new sudoldapEditRole($this->config);
	}

	/* Check for edit role request 
     	 */
    	$once = false;
    	foreach( $_POST as $name => $value){
  
      		/* check all post for edit request 
       		 */
      		//if(preg_match("/^editRole_/",$name)&&!$once && chkacl($this->acl,"sudoldap") == ""){
      		if(preg_match("/^editRole_/",$name)&&!$once){
        		$once =true;
        		$tmp = preg_replace("/^editRole_/","",$name);
        		$tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        		$this->dialog= new sudoldapeditRole($this->config,$this->dn,$this->Roles[$tmp]);
      		}

      		/* check posts for delete zone 
       		 */
      		//if(preg_match("/^delRole_/",$name)&&!$once && chkacl($this->acl,"sudoldap") == ""){
      		if(preg_match("/^delRole_/",$name)&&!$once){

        		$once =true;
        		$tmp = preg_replace("/^delRole_/","",$name);
        		$tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
     
        	/* Initiate deletion
        	 */ 
        	$this->RemoveRole($tmp); 
      		}
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
	$editImg = "<input type='image' src='images/edit.png' name='editRole_%s'>
	<input type='image' src='images/edittrash.png' name='delRole_%s'>";
	foreach($this->Roles as $role => $values ){
		$link = "<a href='?plug=".$_GET['plug']."&act=edit&id=%s'>%s</a>";
		$RoleList->AddEntry(array(
			array("string" => sprintf($link,base64_encode($role),$role)),
			//array("string" => sprintf($link,base64_encode($zone),_("Reverse zone")." : ".getNameFromMix($values['ReverseZone']))),
			//array("string" => _("TTL")." : ".$values['sOAttl']),
			//array("string" => _("Class")." : ".$values['dNSClass']),
			array("string" => str_replace("%s",base64_encode($role),$editImg))
            ));
	}    

	$smarty->assign("sudoldapACL",chkacl($this->acl,"sudoldap"));

	$smarty->assign("RoleList",$RoleList->DrawList());
	$display.= $smarty->fetch(get_template_path('sudoldap.tpl', TRUE));
	return($display);
  }

  function getRoles()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=sudoRole)",$this->attributes);
    while ($attrs= $ldap->fetch()){
      	$cn= $attrs['cn'][0];
	foreach($this->attributes as $value){
		if(isset($attrs[$value])){
			$ret[$cn][$value] = $attrs[$value][0];
		}
	}
    }
    return($ret);
  }  

  function RemoveRole($cn){
  	if($cn!=""){
		$ldap= $this->config->get_ldap_link();
		$dn = "cn=".$cn.",ou=SUDOers,".$this->config->current['BASE'];
		$ldap->cd($dn);
      		$ldap->rmdir_recursive($dn);
      		show_ldap_error($ldap->get_error(), _("Removing DNS entries failed"));
		unset($this->Roles[$cn]);
  	}
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
