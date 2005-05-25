<?php


/* FIXME: the complete acl stuff will be redesigned to be more intuitive,
          which will be possible after introducing templates for html pages.
          just be patient. */


class acl extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage group ACL's";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Helpers */
  var $department= "";
  var $objects= array();
  var $object= "";
  var $current_acl= "";
  var $selfflag= FALSE;


  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("gosaObject");

  function acl ($config, $dn= NULL)
  {
	plugin::plugin($config, $dn);

	/* WorkAround */
	include "acl_definition.inc";
	$this->objects= $ACLD;
	$this->object= reset(array_keys($this->objects));

	$this->selfflag= FALSE;
	if (isset($this->attrs['gosaSubtreeACL'][0])){
		$this->current_acl= preg_replace("/^[^:]*:/", "", $this->attrs['gosaSubtreeACL'][0]);
		if (preg_match("/^!:/", $this->attrs['gosaSubtreeACL'][0])){
			$this->selfflag= TRUE;
		}
	}

	/* This is allways true */
	$this->is_account= TRUE;
  }

  function execute()
  {
	/* Do we represent a valid group? */
	if (!$this->is_account && $this->parent == NULL){
		$display= "<img src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
		_("This 'dn' is no acl container.")."</b>";
		return ($display);
	}

	/* Show main page */
	$smarty= get_smarty();
	if ($_SESSION['js']==FALSE){
                $smarty->assign("javascript", "false");
        } else {
                $smarty->assign("javascript", "true");
        }
	$smarty->assign("object", $this->object);
	$obj= array();
	foreach($this->objects as $key => $value){
		$obj[$key]= $key;
	}
	if ($this->selfflag){
		$smarty->assign("selfflag", "checked");
	} else {
		$smarty->assign("selfflag", "");
	}
	$smarty->assign("objects", $obj);
	$display= $smarty->fetch (get_template_path('acl.tpl', TRUE));

	/* Show acl stuff */
	$this->acl= get_module_permission (array($this->current_acl), $this->object, "");
	$display.= $this->print_attributes ($this->objects[$this->object]);
	return ($display);
  }


  function remove_from_parent()
  {
	plugin::remove_from_parent();
	
	$this->attrs['gosaSubtreeACL']= array();

	$ldap= $this->config->get_ldap_link();
	$ldap->cd($this->dn);
	$ldap->modify($this->attrs);
	show_ldap_error($ldap->get_error());
  }


  /* Save data to object */
  function save_object()
  {
	plugin::save_object();

	if (!isset($_POST['object'])){
		return;
	}

	/* Strip of old information */
	if ($this->object == "all"){
		$this->current_acl= preg_replace ( "/[,]?all[,]?/", "", $this->current_acl);
	}
	$this->current_acl= preg_replace ( "/[,]?$this->object#[^,]*[,]?/", "", $this->current_acl);

	/* assemble new attributes */
	$attrs= "";
	if (isset($_POST['all'])){
		$attrs.= "#all";
	}
	if ($this->object != "all" && !isset($_POST['all']) && $this->object != ""){
		foreach ($this->objects[$this->object] as $key => $val){
			if (is_integer($key)){
				$aname= $val;
			} else {
				$aname= $key;
			}
			if (isset($_POST[$aname])){
				$attrs.= "#$aname";
			}
		}
	}

	/* append information */
	if ($this->current_acl != "" && $attrs != ""){
		$this->current_acl.= ",";
	}
	if ($attrs != ""){
		$tmp= $this->object;
		$attrs= $tmp.$attrs;
	}
	$this->current_acl.= $attrs;
	if (preg_match("/all#all/", $this->current_acl)){
		$this->current_acl= "all";
	}
	if (preg_match("/^all,/", $this->current_acl)){
		$this->current_acl= "all";
	}

	/* Save current object selection */
	if (isset($_POST['object'])){
		$this->object= $_POST['object'];
	}
	if (isset($_POST['selfflag'])){
		$this->selfflag= TRUE;
	} else {
		$this->selfflag= FALSE;
	}

  }


  /* Save to LDAP */
  function save()
  {
	/* Write back to LDAP */
	$ldap= $this->config->get_ldap_link();

	/* Read stuff and only modify subtreeACL entries */
	plugin::save();

	if ($this->current_acl != ""){
		if ($this->selfflag){
			$this->attrs['gosaSubtreeACL']= "!:".$this->current_acl;
		} else {
			$this->attrs['gosaSubtreeACL']= ":".$this->current_acl;
		}
	} else {
		$objectclasses= array();
		foreach ($this->attrs['objectClass'] as $oc){
			if (!preg_match('/gosaObject/i', $oc)){
				$objectclasses[]= $oc;
			}
		}
		$this->attrs['objectClass']= $objectclasses;
		$this->attrs['gosaSubtreeACL']= array();
	}

	/* Modify class */
	$ldap->cd($this->dn);
	$ldap->modify($this->attrs);
	show_ldap_error($ldap->get_error());
  }


  function print_attributes ($name)
  {
  	$display= "<input type=checkbox name=\"all\" value=\"1\"";
	if (chkacl ($this->acl, "all") == ""){
		$display.= " checked";
	}
	$display.= "><b>"._("All fields are writeable")."</b><br>";

	# Put strings in right order
	asort ($name);

	# Generate checklist
	$display.= "<table style=\"width:100%;\">\n";
	$switch= 0;
	foreach ($name as $key => $val){
		if ($switch == 0){
			$display.= " <tr>\n";
		}

		if (is_integer($key)){
			$display.= "<td><input type=checkbox name=\"$val\" value=\"1\" ";
			if (chkacl ($this->acl, "$val") == ""){
				$display.= "checked";
			}
			$display.= ">$val</td>";
		} else {
			$display.= "<td><input type=checkbox name=\"$key\" value=\"1\"";
			if (chkacl ($this->acl, "$key") == ""){
				$display.= "checked";
			}
			$display.= ">"._($val)."</td>";
		}

		if ($switch++ == 1){
			$display.= " </tr>\n";
			$switch= 0;
		}
	}

	if ($switch == 1){
		$display.= "<td></td></tr>";
	}
	$display.= "</table>\n";

	return ($display);
  }

}

?>
