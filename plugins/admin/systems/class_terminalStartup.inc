<?php
class termstartup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal startup options";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $bootmode= "G";
  var $goLdapServerList= array("default");
  var $gotoBootKernel= "default";
  var $gotoKernelParameters= "";
  var $gotoLdapServer= "";
  var $gotoModules= array();
  var $gotoAutoFs= array();
  var $gotoFilesystem= array();
  var $gotoTerminalPath= "";

  /* attribute list for save action */
  var $attributes= array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters");
  var $objectclasses= array("GOhard");

  /* Helper */
  var $customParameters= "";
  var $orig_dn= "";


  function termstartup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Get arrays */
    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      if (isset($this->attrs["$val"]["count"])){
        for ($i= 0; $i<$this->attrs["count"]; $i++){
          if (isset($this->attrs["$val"][$i])){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }
      sort ($this->$val);
      $this->$val= array_unique($this->$val);
    }

    /* Parse Kernel Parameters to decide what boot mode is enabled */
    if (preg_match("/ splash=silent/", $this->gotoKernelParameters)){
      $this->bootmode= "G";
    } elseif (preg_match("/ debug/", $this->gotoKernelParameters)){
      $this->bootmode= "D";
    } elseif ($this->gotoKernelParameters == "") {
      $this->bootmode= "G";
    } else {
      $this->bootmode= "T";
    }
    if (preg_match("/ o /", $this->gotoKernelParameters)){
      $this->customParameters= preg_replace ("/^.* o /", "", $this->gotoKernelParameters);
    } else {
      $this->customParameters= "";
    }

    $this->orig_dn= $this->dn;
  }

  function execute()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img src=\"images/stop.png\" align=center>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return ($display);
    }

    /* Add module */
    if (isset ($_POST['add_module'])){
      if ($_POST['module'] != "" && chkacl ($this->acl, "gotoModule") == ""){
        $this->add_list ($this->gotoModules, $_POST['module']);
      }
    }

    /* Delete module */
    if (isset ($_POST['delete_module'])){
      if (count($_POST['modules_list']) && chkacl ($this->acl, "gotoModule") == ""){
        $this->del_list ($this->gotoModules, $_POST['modules_list']);
      }
    }

    /* Add autofs */
    if (isset ($_POST['add_autofs'])){
      if ($_POST['autofs'] != "" && chkacl ($this->acl, "gotoAutoFs") == ""){
        $this->add_list ($this->gotoAutoFs, $_POST['autofs']);
      }
    }

    /* Delete autofs */
    if (isset ($_POST['delete_autofs'])){
      if (count($_POST['autofs_list']) && chkacl ($this->acl, "gotoAutoFs") == ""){
        $this->del_list ($this->gotoAutoFs, $_POST['autofs_list']);
      }
    }

    /* Add filesystem */
    if (isset ($_POST['add_fs'])){
      if ($_POST['fstab'] != "" && chkacl ($this->acl, "gotoFilesystem") == ""){
        $this->add_list ($this->gotoFilesystem, $_POST['fstab']);
      }
    }

    /* Delete filesystem */
    if (isset ($_POST['delete_fs'])){
      if (count($_POST['fstab_list']) && chkacl ($this->acl, "gotoFilesystem") == ""){
        $this->del_list ($this->gotoFilesystem, $_POST['fstab_list']);
      }
    }

    /* Show main page */
    $smarty= get_smarty();

    /* Arrays */
    $smarty->assign("ldapservers", $this->config->data['SERVERS']['LDAP']);
    $smarty->assign("gotoLdapServer_select", $this->gotoLdapServer);
    $smarty->assign("gotoLdapServerACL", chkacl($this->acl, "gotoLdapServer"));
    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      $smarty->assign("$val", $this->$val);
    }

    /* Values */
    foreach(array("gotoBootKernel", "customParameters") as $val){
      $smarty->assign($val, $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    /* Radio button group */
    if (preg_match("/G/", $this->bootmode)) {
      $smarty->assign("graphicalbootup", "checked");
    } else {
      $smarty->assign("graphicalbootup", "");
    }
    if (preg_match("/T/", $this->bootmode)) {
      $smarty->assign("textbootup", "checked");
    } else {
      $smarty->assign("textbootup", "");
    }
    if (preg_match("/D/", $this->bootmode)) {
      $smarty->assign("debugbootup", "checked");
    } else {
      $smarty->assign("debugbootup", "");
    }

    /* ACL's */
    foreach (array("gotoKernelParameters", "gotoModules", "gotoFilesystem") as $value){
      $smarty->assign($value."ACL", chkacl($this->acl, "$value"));
    }

    /* Show main page */
    return($smarty->fetch (get_template_path('startup.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();

    /* Save group radio buttons */
    if (chkacl ($this->acl, "bootmode") == "" && isset($_POST["bootmode"])){
      $this->bootmode= $_POST["bootmode"];
    }

    /* Save kernel parameters */
    if (chkacl ($this->acl, "gotoKernelParameters") == "" && isset($_POST["customParameters"])){
      $this->customParameters= $_POST["customParameters"];
    }
  }


  /* Save to LDAP */
  function save()
  {
    /* Find proper terminal path for tftp configuration
       FIXME: This is suboptimal when the default has changed to
       another location! */
    if ($this->gotoTerminalPath == "default"){
      $ldap= $this->config->get_ldap_link();

      /* Strip relevant part from dn, keep trailing ',' */
      $tmp= preg_replace("/^cn=[^,]+,ou=terminals,ou=systems,/i", "", $this->dn);
      $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

      /* Walk from top to base and try to load default values for
         'gotoTerminalPath'. Abort when an entry is found. */
      while (TRUE){
        $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

        $ldap->cat("cn=default,ou=terminals,ou=systems,$tmp".
            $this->config->current['BASE']);
        $attrs= $ldap->fetch();
        if (isset($attrs['gotoTerminalPath'])){
          $this->gotoTerminalPath= $attrs['gotoTerminalPath'][0];
          break;
        }

        /* Nothing left? */
        if ($tmp == ""){
          break;
        }
      }
    }

    /* Add semi automatic values */
    // FIXME: LDAP Server may not be set here...
    $this->gotoKernelParameters= "root=/dev/nfs nfsroot=".
      $this->gotoTerminalPath.
      ",ro,hard,nolock,fg,rsize=8192 ".
      "ip=::::::dhcp LDAP=".$this->gotoLdapServer;

    switch ($this->bootmode){
      case "D":
        $this->gotoKernelParameters.= " debug";
      break;
      case "G":
        $this->gotoKernelParameters.= " splash=silent";
      break;
    }
    if ($this->customParameters != ""){
      $this->gotoKernelParameters.= " o ".$this->customParameters;
    }

    plugin::save();

    /* Add missing arrays */
    foreach (array("gotoFilesystem", "gotoAutoFs", "gotoModules") as $val){
      if (isset ($this->$val) && count ($this->$val) != 0){
        $this->attrs["$val"]= array_unique($this->$val);
      }
    }

    /* Strip out 'default' values */
    if ($this->attrs['gotoLdapServer'] == "default"){
      unset ($this->attrs['gotoLdapServer']);
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("modify");
  }

  /* Add value to array, check if unique */
  function add_list (&$array, $value)
  {
    if ($value != ""){
      $array[]= $value;
      sort($array);
      array_unique ($array);
    }
  }


  /* Delete value to array, check if unique */
  function del_list (&$array, $list)
  {
    $tmp= array();
    foreach ($array as $mod){
      if (!in_array($mod, $list)){
        $tmp[]= $mod;
      }
    }
    $array= $tmp;
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
