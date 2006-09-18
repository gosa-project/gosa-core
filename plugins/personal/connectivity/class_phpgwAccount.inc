<?php
class phpgwAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PHPGroupware";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users PHPGroupware account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* GW attributes */
  var $phpgwAccountExpires= "-1";
  var $phpgwAccountStatus= "A";
  var $phpgwAccountType= "u";

  /* attribute list for save action */
  var $attributes= array("phpgwAccountExpires", "phpgwAccountStatus", "phpgwAccountType");
  var $objectclasses= array("phpgwAccount");

  var $uid  = "";

  function phpgwAccount ($config, $dn= NULL, $parent=NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

  }

  function execute()
  {
	/* Call parent execute */
//	plugin::execute();
  
    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }
    if ($this->is_account){
      $smarty->assign("phpgwState", "checked");
    } else {
      $smarty->assign("phpgwState", "");
    }

    $smarty->assign('phpgwAccountACL', chkacl($this->acl, 'phpgwAccount'));

    $display.= $smarty->fetch (get_template_path('phpgw.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if(chkacl($this->acl,"phpgwAccount") == ""){
      /* Cancel if there's nothing to do here */
      if (!$this->initially_was_account){
        return;
      }

      plugin::remove_from_parent();
      $ldap= $this->config->get_ldap_link();

      $ldap->cd($this->dn);
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      show_ldap_error($ldap->get_error(), _("Removing PHPgw account failed"));

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove',array("uid" => $this->uid));
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['phpgw'])){
        if (!$this->is_account && $_POST['phpgw'] == "B"){
          $this->is_account= TRUE;
        }
      } else {
        $this->is_account= FALSE;
      }
    }

    plugin::save_object();
    if (isset($_POST["phpgwStatus"])){
      $this->phpgwStatus = "disabled";
    } else {
      $this->phpgwStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
    if(chkacl($this->acl,"phpgwAccount") == ""){
      plugin::save();
      
      /* Write back to ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      show_ldap_error($ldap->get_error(), _("Saving PHPgw account failed"));

      /* Optionally execute a command after we're done */
      if ($this->initially_was_account == $this->is_account){
        if ($this->is_modified){
          $this->handle_post_events("modify",array("uid" => $this->uid));
        }
      } else {
        $this->handle_post_events("add",array("uid" => $this->uid));
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
