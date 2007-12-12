<?php

class servdhcp extends goService
{
  /* attribute list for save action */
  var $attributes= array("dhcpServiceDN");
  var $objectclasses= array("dhcpServer");
  var $conflicts        = array("servdhcp");
  var $dhcpServiceDN= "";

  /* Section storage */
  var $dhcpSections= array();
  var $dhcpObjectCache= array();
  var $current_object= "";
  var $types= array();
  var $serviceDN= "";

	var $quote_option = array("domain-name");

  var $orig_dn = "";

  var $dhcp_server_list   = array("ENTRIES"=> array(),"FOR_LIST"=> array());
  var $take_over_id       = -1;
  var $display_warning  = TRUE;

  function servdhcp (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $this->serviceDN = "cn=dhcp,".$dn;
    $this->orig_dn = $dn;

    $this->DisplayName = _("DHCP service");

    $this->types= array(  "dhcpLog" => _("Logging"),
            "dhcpService" => _("Global options"),
            "dhcpClass" => _("Class"),
            "dhcpSubClass" => _("Subclass"),
            "dhcpHost" => _("Host"),
            "dhcpGroup" => _("Group"),
            "dhcpPool" => _("Pool"),
            "dhcpSubnet" => _("Subnet"),
            "dhcpFailOverPeer" => _("Failover peer"),
            "dhcpSharedNetwork" => _("Shared network"));


    /* Backport: PHP4 compatibility  */
    foreach($this->types as $type => $translation){
	    $this->types[strtolower($type)] = $translation;
    }

    /* Load information about available services */
    $this->reload(); 
    if (!count($this->dhcpSections)){
      $this->is_account= FALSE;
      $this->dhcp_server_list = $this->get_list_of_dhcp_servers();
    }
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("dns_take_over",FALSE);
    $display= "";


    /*****************/
    /* Handle Take Over Actions 
    /*****************/

    /* Give smarty the required informations */
    $smarty->assign("dhcp_server_list", $this->dhcp_server_list['FOR_LIST']);
    $smarty->assign("dhcp_server_list_cnt", count($this->dhcp_server_list['FOR_LIST']));
    
    /* Take over requested, save id */
    if(isset($_POST['take_over_src']) && isset($_POST['take_over'])){
      $id = $_POST['take_over_src'];
      if(isset($this->dhcp_server_list['ENTRIES'][$id])){
        $this->take_over_id = $id;      
      }
    }
 
    /* Abort take over action */ 
    if(isset($_POST['cancel_take_over'])){
      $this->dialog =false;
      $this->take_over_id = -1;
      $this->dhcp_server_list = $this->get_list_of_dhcp_servers();
    }

    /* Display informartion about take over that will be started when saving this server 
     *  and hide default dhcp output
     */
    if($this->take_over_id != -1){

      $this->dialog = FALSE;
      $id = $this->take_over_id;
      $smarty->assign("dns_take_over",TRUE);
      $warning = sprintf(_("You are going to migrate the DHCP setup from server '%s'."),
        $this->dhcp_server_list['ENTRIES'][$id]['cn'][0]);
      $warning.= "&nbsp;"._("The migration will be started when you save this system. To cancel this action, use the cancel button below.");

      if($this->display_warning){
        print_red($warning);
        $this->display_warning = FALSE;
      }
      return($smarty->fetch(get_template_path('servdhcp.tpl', TRUE)));
    }

    
    /*****************/
    /* List handling  
    /*****************/

    /* Section Creation? */
    if (isset($_POST['create_section']) && isset($_POST['section'])){
      $section= $_POST['section'];
      $tmp = new dhcpNewSectionDialog(NULL);
      if (isset($tmp->sectionMap[$section])){
        $this->dialog= new $section($this->current_object);
        $this->current_object= "";
      } else {
        $this->dialog= FALSE;
      }
    }

    /* Cancel section creation? */
    if (isset($_POST['cancel_section']) || isset($_POST['cancel_dhcp'])){
      $this->dialog= FALSE;
    }

    /* Save changes */
    if (isset($_POST['save_dhcp'])){
      $this->dialog->save_object();
      $messages= $this->dialog->check();
      if (count($messages)){
        show_errors($messages);
      } else {
        $dn= $this->dialog->dn;
        $class= get_class($this->dialog);
        $type= $this->types[$class];
		if(empty($this->serviceDN)){
			$indent= substr_count(preg_replace("/".$this->dn."/", '', $dn), ",") -1;
		}else{
			$indent= substr_count(preg_replace("/".$this->serviceDN."/", '', $dn), ",");
		}
		$spaces= "";
		for ($i= 0; $i<$indent; $i++){
          $spaces.= "&nbsp;&nbsp;&nbsp;&nbsp;";
        }
        $data= $this->dialog->save();
        if ($this->current_object == ""){
          /* New object */
          $newsects= array();
          foreach ($this->dhcpSections as $key => $dsc){
            $newsects[$key]= $dsc;
            if ($key == $dn){
              $spaces.= "&nbsp;&nbsp;&nbsp;&nbsp;";
              $newsects[$data['dn']]= "$spaces$type '".preg_replace('/^[^=]+=([^,]+),.*$/', '\1', $data['dn'])."'";
            }
          }
          $this->dhcpObjectCache[$data['dn']]= $data;
          $this->dhcpSections= $newsects;
        } else {
          if ($dn != $data['dn']){
            /* Old object, new name */
            $this->dhcpObjectCache[$dn]= array();
            $this->dhcpObjectCache[$data['dn']]= $data;

            /* If we renamed a section, we've to rename a couple of objects, too */
            foreach ($this->dhcpObjectCache as $key => $dsc){
              if (preg_match("/,$dn$/", $key)){
                $new_dn= preg_replace("/,$dn$/", ",".$data['dn'], $key);
                $dsc['MODIFIED']= TRUE;
                $this->dhcpObjectCache[$new_dn]= $dsc;
                unset($this->dhcpObjectCache[$key]);
              }
            }
            $newsects= array();
            foreach ($this->dhcpSections as $key => $dsc){
              if ($key == $dn){
                $newsects[$data['dn']]= "$spaces$type '".preg_replace('/^[^=]+=([^,]+),.*$/', '\1', $data['dn'])."'";
                continue;
              }
              if (preg_match("/,$dn$/", $key)){
                $new_dn= preg_replace("/,$dn$/", ",".$data['dn'], $key);
                $newsects[$new_dn]= $dsc;
              } else {
                $newsects[$key]= $dsc;
              }
            }
            $this->dhcpSections= $newsects;

          } else {
            /* Old object, old name */
            $this->dhcpObjectCache[$data['dn']]= $data;
          }
        }
        $this->dialog= FALSE;
      }
    }

    /* Remove section? */
    if (isset($_POST['delete_dhcp_confirm'])){
      if ($this->acl_is_removeable()){
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
      $this->dialog= FALSE;
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

    if(isset($_GET['act']) && $_GET['act']=="edit" && isset($_GET['id'])){
      $dn = base64_decode($_GET['id']);
      if (isset($this->dhcpObjectCache[$dn])){
        $section= $this->objectType($dn);
        $this->current_object= $dn;
        $this->dialog= new $section($this->dhcpObjectCache[$dn]);
      }
    }
    

    if(isset($_GET['act']) && $_GET['act']=="edit" && isset($_GET['id'])){
      $dn = base64_decode($_GET['id']);
      if (isset($this->dhcpObjectCache[$dn])){
        $section= $this->objectType($dn);
        $this->current_object= $dn;
        $this->dialog= new $section($this->dhcpObjectCache[$dn]);
      }
    }
    

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
#      $display= $this->show_header(_("Remove DHCP service"),
#          _("This server has DHCP features enabled. You can disable them by clicking below."));

      if (!count($this->dhcpObjectCache)){
        $attrs= array();
        $attrs['dn']= 'cn=dhcp,'.$this->dn;
        $attrs['cn']= array('dhcp');
        $attrs['objectClass']= array('top', 'dhcpService');
        $attrs['dhcpPrimaryDN']= array($this->dn);
        $attrs['dhcpStatements']= array("default-lease-time 600",
                                        "max-lease-time 1200",
                                        "authoritative",
                                        "ddns-update-style none");
        $attrs['MODIFIED']= TRUE;
        $this->dhcpSections['cn=dhcp,'.$this->dn]= _("Global options");
        $this->dhcpObjectCache['cn=dhcp,'.$this->dn]= $attrs;
      }

    } else {
#      $display= $this->show_header(_("Add DHCP service"),

#          _("This server has DHCP features disabled. You can enable them by clicking below."));
      return ($display);
    }


    /* Show dialog
     */
    if(isset($this->dialog) && is_object($this->dialog)){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Zones
     */
    $DhcpList = new divSelectBox("dhcpSections");
    $DhcpList->SetHeight(400);

    /* Add entries to divlist
     */
    $editImgIns = "<input type='image' src='images/list_new.png' name='insertDhcp_%s' title='"._("Insert new DHCP section")."'>".
      "<input type='image' src='images/edit.png' name='editDhcp_%s' title='"._("Edit DHCP section")."'>".
      "<input type='image' src='images/edittrash.png' name='delDhcp_%s' title='"._("Remove DHCP section")."'>";
    $editImgInsNoDel = "<input type='image' src='images/list_new.png' name='insertDhcp_%s' title='"._("Insert new DHCP section")."'>".
      "<input type='image' src='images/edit.png' name='editDhcp_%s' title='"._("Edit DHCP section")."'>";
    $editImg = "<input type='image' src='images/edit.png' name='editDhcp_%s' title='"._("Edit DHCP section")."'>".
      "<input type='image' src='images/edittrash.png' name='delDhcp_%s' title='"._("Remove DHCP section")."'>";
      	
    $tmp = new dhcpNewSectionDialog(NULL);
    foreach($this->dhcpSections as $section => $values ){
    
      $values = "<a href='?plug=".$_GET['plug']."&act=edit&id=".base64_encode($section)."'>".$values."</a>";
	
      if (count($tmp->sectionMap[$this->objectType($section)])){
        if ($this->objectType($section) == "dhcpService"){
          $DhcpList->AddEntry(array(
              array("string" => $values),
              array("string" => str_replace("%s",base64_encode($section),$editImgInsNoDel), "attach" => "style='text-align:right;'")
              ));
        } else {
          $DhcpList->AddEntry(array(
              array("string" => $values),
              array("string" => str_replace("%s",base64_encode($section),$editImgIns), "attach" => "style='text-align:right;'")
              ));
        }
      } else {
        $DhcpList->AddEntry(array(
              array("string" => $values),
              array("string" => str_replace("%s",base64_encode($section),$editImg), "attach" => "style='text-align:right;'")
              ));
      }
    }

    /* Display tempalte */
    $smarty->assign("DhcpList",$DhcpList->DrawList());
    $display.= $smarty->fetch(get_template_path('servdhcp.tpl', TRUE));
    return($display);
  }


  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    /* Remove subtrees */
    $ldap= $this->config->get_ldap_link();
    foreach ($this->dhcpObjectCache as $dn => $content){
      if ($this->objectType($dn) == 'dhcpService'){
        $ldap->rmdir_recursive($dn);
        show_ldap_error($ldap->get_error(), _("Removing DHCP entries failed"));
      }
    }

    /* Remove from self */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();
    
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs);

    show_ldap_error($ldap->get_error(), _("Removing DHCP entries failed"));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
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
    /* Take over handling 
     * - Load servdhcp class and dhcpObjectCache for the source dhcp setup.
     * - Assign dhcpObjectCache to this configuration. 
     * - Save this setup and remove source setup from ldap.
     */
    if($this->take_over_id != -1){
      $id = $this->take_over_id;
      $src = preg_replace("/cn=dhcp,/","",$this->dhcp_server_list['ENTRIES'][$id]['dn']);
      $tmp = new servdhcp ($this->config, $src);
      $this->orig_dn = $src;
      $this->dhcpObjectCache =  $tmp->dhcpObjectCache;
    }

    /* Save dhcp setttings */
    $ldap= $this->config->get_ldap_link();
    foreach ($this->dhcpObjectCache as $dn => $data){

      if($this->dn != $this->orig_dn){
        $dn = preg_replace("/".normalizePreg($this->orig_dn)."$/i",$this->dn,$dn);
      }

      /* Remove entry? */
      if (count($data) == 0){
        /* Check if exists, then remove... */
        if($ldap->cat($dn)){
          $ldap->rmdir_recursive($dn);
          show_ldap_error($ldap->get_error(), _("Can't remove DHCP object!"));
        }
        continue;
      }

      /* Opdate dhcp option 'server-name' to actual server name */
      if($this->dn != $this->orig_dn){
        $fixed = FALSE;
        foreach(array("dhcpHost","dhcpSubnet","dhcpGroup","dhcpSharedNetwork") as $object){
          if(in_array($object,$data['objectClass']) && isset($data['dhcpOption'])){
            foreach($data['dhcpOption'] as $key => $option){
              if(preg_match("/^server-name /",$option)){
                $data['dhcpOption'][$key] = "server-name ".$this->cn;
                $data['MODIFIED'] = TRUE; 
                break;
              }
            }
          }

          /* Skip next loops if entry is updated */
          if($fixed){
            break;
          }
        }
      }

      /* Modify existing entry? */
      if (isset($data['MODIFIED']) || $this->orig_dn != $this->dn){

        if($ldap->cat($dn)){
          $modify= TRUE;
        } else {
          $modify= FALSE;
        }

        /* Build new entry */
        $attrs= array();
        foreach ($data as $attribute => $values){
          if ($attribute == "MODIFIED" || $attribute == "dn"){
            continue;
          }
      
          if(in_array($attribute,array("dhcpPrimaryDN","dhcpSecondaryDN","dhcpServerDN","dhcpFailOverPeerDN"))){
            foreach($values as $v_key => $value){
              $values[$v_key] = preg_replace("/".normalizePreg($this->orig_dn)."$/i",$this->dn,$value);
            }
          }

          if (count($values)){

            if($attribute == "dhcpOption"){
              foreach($values as $key => $value){
                $option_name = trim(preg_replace("/[^ ]*$/","",$value));
                $option_value= trim(preg_replace("/^[^ ]*/","",$value));
                if(in_array($option_name,$this->quote_option)){
                  $values[$key] = $option_name." \"".$option_value."\"";
                }
              }
            }
            if (count($values) == 1){
              $attrs[$attribute]= $values[0];
            } else {
              $attrs[$attribute]= $values;
            }
          } else {
            if ($modify){
              $attrs[$attribute]= array();
            }
          }
        }

        $ldap->cd($dn);
        if ($modify){
          $ldap->modify($attrs);
          show_ldap_error($ldap->get_error(), _("Can't save DHCP object!"));

          /* Optionally execute a command after we're done */
          $this->handle_post_events("modify");
        } else {
          $ldap->add($attrs);
          show_ldap_error($ldap->get_error(), _("Can't save DHCP object!"));

          /* Optionally execute a command after we're done */
          $this->handle_post_events("create");
        }
      }
    }

    $this->dhcpServiceDN= $this->serviceDN;
    if($this->dn != $this->orig_dn){
      $this->dhcpServiceDN= preg_replace("/".normalizePreg($this->orig_dn)."$/i",$this->dn,$this->dhcpServiceDN);
    }

    /* Replace 'new' dn */ 
    if(preg_match("/new$/",$this->dhcpServiceDN)){
      $this->dhcpServiceDN = preg_replace("/new$/",$this->dn,$this->dhcpServiceDN);
    }

    plugin::save();
    
    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs);

    show_ldap_error($ldap->get_error(), _("Saving DHCP service failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }

    /* Take over handling
     * - Remove old dhcp config from source server 
     */
    if($this->take_over_id != -1){
      $id = $this->take_over_id;
      $src = $this->dhcp_server_list['ENTRIES'][$id]['dn'];
      $tmp = new servdhcp ($this->config, $src);
      $tmp->remove_from_parent();
    }
  }


  function reload()
  {
    /* Init LDAP and load list */
    $ldap= $this->config->get_ldap_link();
    $ui= get_userinfo();
    $me= $this->dn;

    $filter = "(&(objectClass=dhcpService)(|(dhcpPrimaryDN=$me)(dhcpSecondaryDN=$me)(dhcpServerDN=$me)(dhcpFailOverPeerDN=$me)))";

    $list= get_list($filter, array("server"), $this->config->current['BASE'], array("cn"),GL_SIZELIMIT | GL_SUBSEARCH);
    $final= array();

    foreach ($list as $value){

      /* Set header */
      $sortpart= split(",", $value['dn']);
      $sortpart= array_reverse($sortpart);
      $tmp= implode(",", $sortpart);

      $final[$value['dn']]= $tmp."!"._("Global options");

      /* Read all sub entries to place here */
      $ldap->cd($value['dn']);
      $ldap->search("(|(objectClass=dhcpService)(objectClass=dhcpLog)(objectClass=dhcpClass)(objectClass=dhcpSubClass)(objectClass=dhcpHost)(objectClass=dhcpGroup)(objectClass=dhcpPool)(objectClass=dhcpSubnet)(objectClass=dhcpSharedNetwork)(objectClass=dhcpOptions)(objectClass=dhcpTSigKey)(objectClass=dhcpDnsZone)(objectClass=dhcpFailOverPeer))", array());
      $this->serviceDN= $value['dn'];

      while ($attrs= $ldap->fetch()){
        $sattrs= array();
        for ($i= 0; $i<$attrs['count']; $i++){
          $sattrs[$attrs[$i]]= $attrs[$attrs[$i]];
          unset($sattrs[$attrs[$i]]['count']);
        }
        $sattrs['dn']= $ldap->getDN();

        foreach($sattrs as $name => $values){
          if($name == "dhcpOption"){
            foreach($values as $key => $value){
              $value_name = trim(preg_replace("/[^ ]*$/","",$value));
              $value_value= trim(preg_replace("/^[^ ]*/","",$value));
              if(in_array($value_name,$this->quote_option)){
                $value_value = preg_replace("/^\"/","",$value_value);
                $value_value = preg_replace("/\"$/","",$value_value);
                $sattrs[$name][$key] = $value_name." ".$value_value;
              }
            }
          }
        }

        $this->dhcpObjectCache[$ldap->getDN()]= $sattrs;
        $tmp= preg_replace("/".$this->serviceDN."/", "", $ldap->getDN());
        $indent= substr_count($tmp, ",");
        $spaces= "";
        for ($i= 0; $i<$indent; $i++){
          $spaces.= "&nbsp;&nbsp;&nbsp;&nbsp;";
        }

        foreach ($this->types as $key => $val){
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


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("DHCP service"),
          "plDescription" => _("DHCP service")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 84,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            
          )     
    ));
  }


  function take_over_service()
  {

  }


  function get_list_of_dhcp_servers()
  {
    $ret = array("ENTRIES"=> array(),"FOR_LIST"=> array());
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=goServer)(dhcpServiceDN=*))",array("dn","cn","dhcpServiceDN"));
    while($attrs = $ldap->fetch()){

      /* Skip own config */
      if($this->dn != "new" && preg_match("/".normalizePreg($this->dn)."$/",$attrs['dn'])){
        continue;
      }

      $ret['ENTRIES'][] = $attrs;
    }
    foreach($ret['ENTRIES'] as $key => $data){
      $ret['FOR_LIST'][$key] = $data['cn'][0];
    }
    return($ret);
  }


  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("DHCP service");
    $fields['AllowEdit']  = true;
    return($fields);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
