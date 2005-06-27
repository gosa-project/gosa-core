<?php

class servservice extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  var $goExportEntry  = array();
  var $goTimeSource   = array();
  var $goLdapBase= "";
  var $goXdmcpIsEnabled ="";
  var $goFontPath= "";
  var $goNTPServer="";
  var $goLdapServer="";
  var $goTerminalServer="";
  var $goSyslogServer="";
  var $goCupsServer="";

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes       = array("goLdapBase","goXdmcpIsEnabled","goFontPath");
  var $possible_objectclasses= array( "goNfsServer", "goNtpServer", "goServer", "goLdapServer",
                                      "goTerminalServer", "goSyslogServer", "goCupsServer");
  var $objectclasses    = array("top","goServer"); 
  var $additionaloc     = array( "goNfsServer"     => array("goExportEntry"),
                                 "goNtpServer"     => array("goTimeSource"),
                                 "goLdapServer"    => array("goLdapBase"),
                                 "goTerminalServer"=> array("goXdmcpIsEnabled", "goFontPath"),
                                 "goSyslogServer"  => array(),
                                 "goCupsServer"    => array());

  function servservice ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    
    /* Assemble final object class list */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->attrs['objectClass']) && in_array($oc, $this->attrs['objectClass'])){
        $this->objectclasses[$oc]= $oc;
      }
    }

    /* Load arrays */
    foreach (array("goTimeSource", "goExportEntry") as $name){
      $tmp= array();
      if (isset($this->attrs[$name])){
        for ($i= 0; $i<$this->attrs[$name]['count']; $i++){
          $tmp[$this->attrs[$name][$i]]= $this->attrs[$name][$i];
        }
      }
      $this->$name= $tmp;
    }

    /* Always is account... */
    $this->is_account= TRUE;
  }


  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();

    $smarty->assign("staticAddress", "");

    /* Here we add a new entry  */
    if(isset($_POST['NewNfsAdd']) && $_POST['NewNfsExport'] != "") {
      $this->goExportEntry[$_POST['NewNfsExport']]= $_POST['NewNfsExport'];
      asort($this->goExportEntry);
    }

    /* Deleting an Entry, is a bit more complicated than adding one*/
    if(isset($_POST['DelNfsEnt']) && isset($_POST['goExportEntry'])) {
      foreach ($_POST['goExportEntry'] as $entry){
        if (isset($this->goExportEntry[$entry])){
          unset($this->goExportEntry[$entry]);
        }
      }
    }

    /* Here we add a new entry  */
    if(isset($_POST['NewNTPAdd']) && $_POST['NewNTPExport'] != "") {
      $this->goTimeSource[$_POST['NewNTPExport']]= $_POST['NewNTPExport'];
      asort($this->goTimeSource);
    }

    /* Deleting an Entry, is a bit more complicated than adding one*/
    if(isset($_POST['DelNTPEnt'])) {
      foreach ($_POST['goTimeSource'] as $entry){
        if (isset($this->goTimeSource[$entry])){
          unset($this->goTimeSource[$entry]);
        }
      }
    }

    /* Attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign("$attr"."ACL", chkacl($this->acl, $attr));
      $smarty->assign($attr."State","");
    }

    /* Arrays */
    foreach (array("goTimeSource", "goExportEntry") as $name){
      $smarty->assign("$name", $this->$name);
      $smarty->assign("$name"."ACL", chkacl($this->acl, $name));
      $smarty->assign($name."State","");
    }

    /* Classes... */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->objectclasses[$oc])){
        $smarty->assign("$oc", "checked");
        $smarty->assign("$oc"."State", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));

      } else {
        $smarty->assign("$oc", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));
        $smarty->assign("$oc"."State", "disabled");
      }
    }

    /* Different handling for checkbox */
    if($this->goXdmcpIsEnabled == "true"){
      $smarty->assign("goXdmcpIsEnabled","checked");
    } else {
      $smarty->assign("goXdmcpIsEnabled","");
    }

    return($smarty->fetch (get_template_path('servservice.tpl', TRUE)));
  }


  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['servicetab'])){
      plugin::save_object();

      /* Save checkbox state */
      foreach ($this->additionaloc as $oc => $dummy){
        if (chkacl($this->acl, $oc) == ""){
          if (isset($_POST[$oc]) && $_POST[$oc] == '1'){
            $this->objectclasses[$oc]= $oc;
          } else {
            unset($this->objectclasses[$oc]);
          }
        }
      }

      /* Save xdmcp is enabled flag */
      if (isset($_POST['goXdmcpIsEnabled'])){
        $this->goXdmcpIsEnabled= "true";
      } else {
        $this->goXdmcpIsEnabled= "false";
      }
        
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
  
    
  
    if((isset($_POST['goTerminalServer']))&&(empty($this->goFontPath))){
      $message[]=_("Termial server, must have fontpath specified.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Normalize lazy objectclass arrays */
    $objectclasses= array();
    foreach($this->objectclasses as $oc){
      $objectclasses[]= $oc;
    }
    $this->objectclasses= $objectclasses;

    plugin::save();

    $tmp= array();

    /* Remove all from this plugin */
    foreach($this->attrs['objectClass'] as $oc){
      if (!in_array_ics($oc, $this->possible_objectclasses)){
        $tmp[]= $oc;
      }
    }

    /* Merge our current objectclasses */
    foreach($this->objectclasses as $oc){
      if (!in_array_ics($oc, $tmp)){
        $tmp[]= $oc;
      }
    }

    /* Reassign cleaned value */
    $this->attrs['objectClass']= $tmp;

    /* Remove illegal attributes */
    foreach ($this->additionaloc as $oc => $attrs){
      if (!in_array($oc, $this->objectclasses)){
        foreach ($attrs as $attr){
          $this->attrs[$attr]= array();
        }
      }
    }

    /* Arrays */
    foreach (array("goTimeSource", "goExportEntry") as $name){
      $this->attrs[$name]= array();
      foreach ($this->$name as $element){
        $this->attrs[$name][]= $element;
      }
    }

    /* Write to LDAP */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());
    
    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
