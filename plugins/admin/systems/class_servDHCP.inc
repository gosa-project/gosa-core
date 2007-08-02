<?php

class servdhcp extends plugin
{
  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array();

  /* Section storage */
  var $dhcpSections= array();
  var $dhcpObjectCache= array();
  var $current_object= "";

  function servdhcp ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Load information about available services */
    $this->reload(); 
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Section Creation? */
    if (isset($_POST['create_section']) && isset($_POST['section'])){
      $section= $_POST['section'];
      if (isset(dhcpNewSectionDialog::$sectionMap[$section])){
        $this->dialog= new $section($this->dn);
        $this->current_object= "";
      } else {
        $this->dialog= NULL;
      }
    }

    /* Cancel section creation? */
    if (isset($_POST['cancel_section']) || isset($_POST['cancel_dhcp'])){
      $this->dialog= NULL;
    }

    /* Save changes */
    if (isset($_POST['save_dhcp'])){
      $this->dialog->save_object();
      $messages= $this->dialog->check($this->dhcpObjectCache);
      if (count($messages)){
        show_errors($messages);
      } else {
        $dn= $this->dialog->dn;
        $data= $this->dialog->save();
        if ($this->current_object= ""){
          echo "SAVE new entry";
        } else {
          if ($dn != $data['dn']){
            echo "SAVE old entry with new dn";
          } else {
            echo "SAVE old entry";
          }
        }
      }
    }

    /* Remove section? */
    if (isset($_POST['delete_dhcp_confirm'])){
      if (chkacl($this->acl, "delete") == ""){
        unset($this->dhcpSections[$this->current_object]);
        unset($this->dhcpObjectCache[$this->current_object]);
        $this->dhcpObjectCache[$this->current_object]= array();
        foreach ($this->dhcpSections as $key => $value){
          if (preg_match("/".$this->current_object."$/", $key)){
            unset($this->dhcpSections[$key]);
            unset($this->dhcpObjectCache[$key]);
            $this->dhcpObjectCache[$key]= array();
          }
        }
      } else {
        print_red(_("You're not allowed to remove DHCP sections!"));
      }
      $this->dialog= NULL;
    }

    /* Look for post entries */
    foreach($_POST as $name => $value){
      
      /* Insert new section? */
      if (preg_match('/^insertDhcp_.*_x$/', $name)){
        $dn= base64_decode(preg_replace('/^insertDhcp_([^_]+)_x$/', '\1', $name));
        if (isset($this->dhcpObjectCache[$dn])){
          $this->dialog= new dhcpNewSectionDialog($this->objectType($dn));
          $this->current_object= $dn;
          $this->dialog->acl= $this->acl;
        }
      }

      /* Edit section? */
      if (preg_match('/^editDhcp_.*_x$/', $name)){
        $dn= base64_decode(preg_replace('/^editDhcp_([^_]+)_x$/', '\1', $name));
        if (isset($this->dhcpObjectCache[$dn])){
          $section= $this->objectType($dn);
          $this->current_object= $dn;
          $this->dialog= new $section($this->dhcpObjectCache[$dn]);
        }
      }

      /* Remove section? */
      if (preg_match('/^delDhcp_.*_x$/', $name)){
        $dn= base64_decode(preg_replace('/^delDhcp_([^_]+)_x$/', '\1', $name));
        if (isset($this->dhcpObjectCache[$dn])){
          $this->current_object= $dn;
          $this->dialog= 1;
          $smarty->assign("warning", sprintf(_("You're about to delete the DHCP section '%s'."), $dn));
          return($smarty->fetch(get_template_path('remove_dhcp.tpl', TRUE)));
        }
      }

    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove DHCP service"),
          _("This server has DHCP features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DHCP service"),
          _("This server has DHCP features disabled. You can enable them by clicking below."));
      return ($display);
    }


    /* Show dialog
     */
    if($this->dialog != NULL && !is_int($this->dialog)){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Zones
     */
    $DhcpList = new divSelectBox("dhcpSections");
    $DhcpList->SetHeight(300);

    /* Add entries to divlist
     */
    $editImgIns = "<input type='image' src='images/list_new.png' name='insertDhcp_%s' title='"._("Insert new DHCP section")."'>".
      "<input type='image' src='images/edit.png' name='editDhcp_%s' title='"._("Edit DHCP section")."'>".
      "<input type='image' src='images/edittrash.png' name='delDhcp_%s' title='"._("Remove DHCP section")."'>";
    $editImg = "<input type='image' src='images/edit.png' name='editDhcp_%s' title='"._("Edit DHCP section")."'>".
      "<input type='image' src='images/edittrash.png' name='delDhcp_%s' title='"._("Remove DHCP section")."'>";
    foreach($this->dhcpSections as $section => $values ){
      if (count(dhcpNewSectionDialog::$sectionMap[$this->objectType($section)])){
        $DhcpList->AddEntry(array(
              array("string" => $values),
              array("string" => str_replace("%s",base64_encode($section),$editImgIns), "attach" => "style='text-align:right;'")
              ));
      } else {
        $DhcpList->AddEntry(array(
              array("string" => $values),
              array("string" => str_replace("%s",base64_encode($section),$editImg), "attach" => "style='text-align:right;'")
              ));
      }
    }

    $smarty->assign("dhcpACL",chkacl($this->acl,"servdhcp"));

    /* Display tempalte */
    $smarty->assign("DhcpList",$DhcpList->DrawList());
    $display.= $smarty->fetch(get_template_path('servdhcp.tpl', TRUE));
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
    plugin::save();

    /* Optionally execute a command after we're done */
    #$this->handle_post_events($mode);
  }


  function reload()
  {
    /* Init LDAP and load list */
    $ldap= $this->config->get_ldap_link();
    $ui= get_userinfo();
    $me= $this->dn;

    $list= get_list("(&(objectClass=dhcpService)(|(dhcpPrimaryDN=$me)(dhcpSecondaryDN=$me)(dhcpServerDN=$me)(dhcpFailOverPeerDN=$me)))", $ui->subtreeACL, $this->config->current['BASE'], array("cn"));
    $final= array();
    foreach ($list as $value){

      /* Set header */
      $sortpart= split(",", $value['dn']);
      $sortpart= array_reverse($sortpart);
      $tmp= implode(",", $sortpart);

      $final[$value['dn']]= $tmp."!"._("Global options");

      /* Read all sub entries to place here */
      $ldap->cd($value['dn']);
      $ldap->search("(|(objectClass=dhcpLog)(objectClass=dhcpClass)(objectClass=dhcpSubClass)(objectClass=dhcpLeases)(objectClass=dhcpHost)(objectClass=dhcpGroup)(objectClass=dhcpPool)(objectClass=dhcpSubnet)(objectClass=dhcpSharedNetwork)(objectClass=dhcpOptions)(objectClass=dhcpTSigKey)(objectClass=dhcpDnsZone)(objectClass=dhcpFailOverPeer))", array());

      while ($attrs= $ldap->fetch()){
        $this->dhcpObjectCache[$ldap->getDN()]= $attrs;
        $tmp= preg_replace("/".$value['dn']."/", "", $ldap->getDN());
        $indent= substr_count($tmp, ",");
        $spaces= "";
        for ($i= 0; $i<$indent; $i++){
          $spaces.= "&nbsp;&nbsp;&nbsp;&nbsp;";
        }
        $types= array(  "dhcpLog" => _("Logging"),
            "dhcpService" => _("Global options"),
            "dhcpClass" => _("Class"),
            "dhcpSubClass" => _("Subclass"),
            "dhcpLeases" => _("Lease"),
            "dhcpHost" => _("Host"),
            "dhcpGroup" => _("Group"),
            "dhcpPool" => _("Pool"),
            "dhcpSubnet" => _("Subnet"),
            "dhcpFailOverPeer" => _("Failover peer"),
            "dhcpSharedNetwork" => _("Shared network"));

        foreach ($types as $key => $val){
          if (in_array("$key", $attrs['objectClass'])){
            $type= $val;
            break;
          }
        }

        /* Prepare for sorting... */
        $sortpart= split(",", $ldap->getDN());
        $sortpart= array_reverse($sortpart);
        $tmp= implode(",", $sortpart);
        $final[$ldap->getDN()]= $tmp."!".$spaces.$type." '".$attrs['cn'][0]."'";
      }
    }

    /* Sort it... */
    natsort($final);
    $this->dhcpSections= array();
    foreach ($final as $key => $val){
      $this->dhcpSections[$key]= preg_replace('/^[^!]+!(.*)$/', '\\1', $val);
    }

  }


  function objectType($dn)
  {
    $type= "";
    $types= array("dhcpService", "dhcpClass", "dhcpSubClass", "dhcpHost",
                  "dhcpGroup", "dhcpPool", "dhcpSubnet", "dhcpSharedNetwork");

    foreach ($this->dhcpObjectCache[$dn]['objectClass'] as $oc){
      if (in_array($oc, $types)){
        $type= $oc;
        break;
      }
    }

    /* That should not happen... */
    if ($type == ""){
      print_red(_("DHCP configuration set is unknown. Please contact your system administrator."));
    }
    
    return ($type);
  }

}

//vim:tabstop=2:expandtab:softtab=2:shiftwidth=2:filetype=php:syntax:ruler:
?>
