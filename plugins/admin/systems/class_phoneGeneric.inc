<?php

class phoneGeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage phone base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $interfaces= array();
  var $ignore_account= TRUE;

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $macAddress= "";
  var $ipHostNumber= "";
  var $description= "";
  var $orig_dn= "";

  /* attribute list for save action */
  var $attributes= array("cn", "description", "macAddress", "ipHostNumber");
  var $objectclasses= array("top", "goFonHardware");

  function phonegeneric ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }

    /* Save dn for later references */
    $this->orig_dn= $this->dn;
  }

  function execute()
  {
    /* Do we represent a valid phone? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no phone features.")."</b>";
      return($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->acl, $attr));
      $smarty->assign("$attr", $this->$attr);
    }
    $smarty->assign("base_select", $this->base);

    /* Show main page */
    $smarty->assign("netconfig", dirname(__FILE__)."/network.tpl");
    return($smarty->fetch (get_template_path('phone.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();

    /* Save base, since this is no LDAP attribute */
    if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
      $this->base= $_POST['base'];
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    $this->dn= "cn=".$this->cn.",ou=phones,ou=systems,".$this->base;

    /* must: cn, macAddress, ipHostNumber */
    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= "The required field 'Phone name' is not set.";
    }
    if ($this->cn == "0" && chkacl ($this->acl, "cn") == ""){
      $message[]= "The 'Phone name' '0' is reserved and cannot be used.";
    }
    if ($this->macAddress == "" && chkacl ($this->acl, "macAddress") == ""){
      $message[]= "The required field 'MAC-Address' is not set.";
    }
    if ($this->ipHostNumber == "" && chkacl ($this->acl, "ipHostNumber") == ""){
      $message[]= "The required field 'IP-Address' is not set.";
    }

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "phone", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a phone on this 'Base'.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if ($attrs['dn'] != $this->orig_dn){
            $message[]= sprintf (_("There is already an entry '%s' in the base choosen by you"), $this->cn);
            break;
          }
        }
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Remove all empty values */
    if ($this->orig_dn == 'new'){
      $attrs= array();
      foreach ($this->attrs as $key => $val){
        if (is_array($val) && count($val) == 0){
          continue;
        }
        $attrs[$key]= $val;
      }
      $this->attrs= $attrs;
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }

      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
      $this->handle_post_events("modify");
    }
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
