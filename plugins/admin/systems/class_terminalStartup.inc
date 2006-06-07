<?php
class termstartup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal startup options";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $bootmode= "G";
  var $goLdapServerList= array();
  var $gotoBootKernel= "";
  var $gotoKernelParameters= "";
  var $gotoLdapServer= "";
  var $gotoModules= array();
  var $gotoTerminalPath= "";
  var $gotoBootKernels= array();

  /* Share */
  var $gotoShares         = array();// Currently Share Option
  var $gotoShare          = "";     // currently selected Share Option
  var $gotoShareSelections= array();// Available Shares for this account in Listbox format
  var $gotoAvailableShares= array();// Available Shares for this account


  /* attribute list for save action */
  var $attributes= array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters");
  var $objectclasses= array("GOhard");

  /* Helper */
  var $customParameters= "";
  var $orig_dn= "";
  var $ignore_account= TRUE;

  function termstartup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->gotoBootKernels = array("default"=>"["._("inherited")."]");

    /* Get arrays */
    foreach (array("gotoModules") as $val){
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

    /* Prepare Shares */
    if((isset($this->attrs['gotoShare']))&&(is_array($this->attrs['gotoShare']))){
      unset($this->attrs['gotoShare']['count']);
      foreach($this->attrs['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = split("\|",$share);
        $tmp2['server']      =$tmp[0];
        $tmp2['name']        =$tmp[1];
        $tmp2['mountPoint']  =$tmp[2];
        $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
      }
    }

    $this->gotoShareSelections= $config->getShareList(true);
    $this->gotoAvailableShares= $config->getShareList(false);

    $this->orig_dn= $this->dn;

    /* Get list of boot kernels */
    if (isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "KERNELS");

      if (!check_command($command)){
        $message[]= sprintf(_("Command '%s', specified as KERNELS hook for plugin '%s' doesn't seem to exist."), $command,
            get_class($this));
      } else {
        $fh= popen($command, "r");
        while (!feof($fh)) {
          $buffer= fgets($fh, 256);
          if(!empty($buffer)){
            $this->gotoBootKernels[]= $buffer;
          }
        }
        pclose($fh);
      }

    }
    $tmp = $this->config->data['SERVERS']['LDAP'];  
    foreach($tmp as $server){
      $visible = $server;
      if($server == "default"){
        $visible = "["._("inherited")."]";
      }
      $this->goLdapServerList[$server] = $visible;
    }

     /* Load hardware list */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".$this->dn."))");
    if ($ldap->count() == 1){
      $map= array("gotoLdapServer");
      $attrs= $ldap->fetch();

      foreach ($map as $name){
        if (!isset($attrs[$name][0])){
          continue;
        }

        switch ($name){
          case 'gotoLdapServer':
            $this->goLdapServerList= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']' ), $this->goLdapServerList);
            break;
        }
      }
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
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

    /* Show main page */
    $smarty= get_smarty();


       /* In this section server shares will be defined
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */
    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));
    $smarty->assign("gotoBootKernels",$this->gotoBootKernels);

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry
     * This entry will be, a combination of mountPoint and sharedefinitions
     */
    if(isset($_POST['gotoShareAdd'])){
      /* We assign a share to this user, if we don't know where to mount the share */
      if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        print_red(_("You must specify a valid mount point."));
      }else{
        $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
        $s_mount = $_POST['gotoShareMountPoint'];
        /* Preparing the new assignment */
        $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
      }
    }

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message
     */
    if((isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
      unset($this->gotoShares[$_POST['gotoShare']]);
    }

    $smarty->assign("gotoShares",$this->printOutAssignedShares());
    $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));

    /* Create divSelectBox for ldap server selection
     */
    $SelectBoxLdapServer = new divSelectBox("LdapServer");
    $SelectBoxLdapServer->SetHeight(80);

    /* Set first entry as selected, if $this->gotoLdapServer is empty
     *  or given entry is no longer available ...
     */
    $found = false;
    foreach($this->goLdapServerList as $server => $name){
      if($this->gotoLdapServer==$server){
        $found = true;
      }
    }
    
    /* Add Entries
     */
    foreach($this->goLdapServerList as $server => $visible){
      $use ="";
      if(($this->gotoLdapServer == $server) || ($found == false)) {
        $found = true;
        $use = " checked ";
      };

      $SelectBoxLdapServer->AddEntry(
          array(
            array("string"=>"<input type='radio' name='gotoLdapServer' value='".$server."' ".$use.">",
                  "attach"=>"style='border-left:0px;'"),
            array("string"=>$visible)
            ));
    }

    $smarty->assign("SelectBoxLdapServer",$SelectBoxLdapServer->DrawList());

    $smarty->assign("gotoShareACL", chkacl($this->acl, "gotoShareACL"));
    foreach (array("gotoModules" ) as $val){
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
    foreach (array("gotoKernelParameters", "gotoModules") as $value){
      $smarty->assign($value."ACL", chkacl($this->acl, "$value"));
    }

    /* Show main page */
    return($smarty->fetch (get_template_path('terminalStartup.tpl', TRUE)));
  }

  function remove_from_parent()
  {
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
            $this->config->current['BASE'], array('gotoTerminalPath'));
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
      "ip=::::::dhcp ldap=".base64_encode($this->gotoLdapServer);

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
    foreach (array("gotoModules") as $val){
      if (isset ($this->$val) && count ($this->$val) != 0){
    
        $this->attrs["$val"]= array_unique($this->$val);
      }
      if(!isset($this->attrs["$val"])) $this->attrs["$val"]=array();
    }

    /* Strip out 'default' values */
    foreach(array("gotoBootKernel","gotoLdapServer") as $value){
      if (!isset($this->attrs[$value]) || $this->attrs[$value] == "default"){
        $this->attrs[$value] = array();
      } 
    }

     /* prepare share settings */
    $tmp = array();
    foreach($this->gotoShares as $name => $settings){
      $tmp2 = split("\|",$name);
      $name = $tmp2[0];
      $tmp[] = $settings['server']."|".$name."|".$settings['mountPoint'];
    }
    $this->attrs['gotoShare']=$tmp;

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving terminal startup settings failed"));
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

   /* Generate ListBox frindly output for the defined shares
   * Possibly Add or remove an attribute here,
   */
  function printOutAssignedShares()
  {
    $a_return = array();
    if(is_array($this->gotoShares)){
      foreach($this->gotoShares as $share){
        $a_return[$share['name']."|".$share['server']]= $share['name']." [".$share['server']."]";
      }
    }
    return($a_return);
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
