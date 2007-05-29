<?php
class phpgwAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PHPGroupware";
  var $plDescription= "This does something";

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

    /* Allow account status toogle?  */
    $smarty->assign("phpgwAccountACL", "disabled");
    if(!$this->is_account && chkacl($this->acl,"create") == ""){
      $smarty->assign("phpgwAccountACL", "");
    }elseif($this->is_account && $this->acl == "#all#" ){
      $smarty->assign("phpgwAccountACL", "");
    }

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

      show_ldap_error($ldap->get_error(), _("Removing PHPGroupware account failed"));

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove',array("uid" => $this->uid));
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){

      /* Change state if needed */
      if(!$this->is_account && isset($_POST["phpgw"]) && chkacl($this->acl,"create") == ""){
        $this->is_account = true;
      }elseif($this->is_account && !isset($_POST["phpgw"]) && $this->acl == "#all#"){
        $this->is_account = false;
      }
    }

    plugin::save_object();

    #FIXME seams to be unused code !
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

      show_ldap_error($ldap->get_error(), _("Saving PHPGroupware account failed"));

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
