<?php

class servservice extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  var $goExportEntry    = array();
  var $goTimeSource     = array();
  var $goLdapBase       = "";
  var $goXdmcpIsEnabled = "";
  var $goFontPath       = "";
  var $goNTPServer      = "";
  var $goShareServer    = false;
  var $goLdapServer     = "";
  var $goTerminalServer = "";
  var $goSyslogServer   = "";
  var $goCupsServer     = "";
  var $o_subWindow      = NULL;

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes       = array("goLdapBase","goXdmcpIsEnabled","goFontPath");
  var $possible_objectclasses= array( "goShareServer", "goNtpServer", "goServer", "goLdapServer",
                                      "goTerminalServer", "goSyslogServer", "goCupsServer");
  var $objectclasses    = array( "top","goServer"); 
  var $additionaloc     = array( "goShareServer"     => array("goExportEntry"),
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
    foreach (array("goTimeSource") as $name){
      $tmp= array();
      if (isset($this->attrs[$name])){
        for ($i= 0; $i<$this->attrs[$name]['count']; $i++){
          $tmp[$this->attrs[$name][$i]]= $this->attrs[$name][$i];
        }
      }
      $this->$name= $tmp;
    }

    $tmp =array();
    $tmp2=array();
    if(isset($this->attrs['goExportEntry'])){
      unset($this->attrs['goExportEntry']['count']);
      if((isset($this->attrs['goExportEntry']))&&(isset($this->attrs['goExportEntry']))){
        foreach($this->attrs['goExportEntry'] as $entry){
          $tmp2= split("\|",$entry);
          $tmp[$tmp2[0]]= $entry;
        }
      }
    }
    $this->goExportEntry = $tmp;

    /* Always is account... */
    $this->is_account= TRUE;

    if(is_array($this->attrs['objectClass'])){ 
      if(in_array("goShareServer",$this->attrs['objectClass'])){
        $this->goShareServer = true;    
      }
    }
  }

  function addToList($entry){
    $key =  key($entry);
    $this->goExportEntry[$key]=$entry[$key];
  }

  function deleteFromList($id){
    unset($this->goExportEntry[$id]);
  }


  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();

    $smarty->assign("staticAddress", "");

    if((isset($_POST['DelNfsEnt']))&&(isset($_POST['goExportEntry']))){
      $this->deleteFromList($_POST['goExportEntry']);
    }

    if(isset($_POST['NewNfsAdd'])){
      $this->o_subWindow = new servnfs($this->config, $this->dn);
      $this->dialog = true;
    }

    if((isset($_POST['NewNfsEdit']))&&(isset($_POST['goExportEntry']))){
      $entry = $this->goExportEntry[$_POST['goExportEntry']];
      $this->o_subWindow = new servnfs($this->config, $this->dn,$entry);
      $this->dialog = true;
    }

    if(isset($this->o_subWindow)){
    $this->o_subWindow->save_object(TRUE);
    }

    /* Save NFS setup */
    if(isset($_POST['NFSsave'])){
      if(count($this->o_subWindow->check())>0){
        foreach($this->o_subWindow->check() as $msg) {
          print_red($msg);
        }
      }else{
        $this->o_subWindow->save_object();
        $newone = $this->o_subWindow->save();
        $this->addToList($newone) ;
        unset($this->o_subWindow);
        $this->dialog = false;
      }
    }
    
    /* Cancel NFS setup */
    if(isset($_POST['NFScancel'])){
      unset($this->o_subWindow);
      $this->dialog = false;
    }
  
    /* Execute NFS setup dialog*/
    if(isset($this->o_subWindow)){
      return $this->o_subWindow->execute(); 
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
    
    $tellSmarty=array();
    foreach($this->goExportEntry as $name=>$values){
       $tmp = split("\|",$values);
       $tellSmarty[$name] = $tmp[0]." ".$tmp[4];
    }
    $smarty->assign("goExportEntry",array_keys($tellSmarty));
    $smarty->assign("goExportEntryKeys",($tellSmarty));
    $smarty->assign("goExportEntryACL", chkacl($this->acl, "goExportEntry"));

    $smarty->assign("goTimeSource", $this->goTimeSource);
    $smarty->assign("goTimeSourceACL", chkacl($this->acl, "goTimeSource"));
    $smarty->assign("goTimeSourceState","");
    

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

    if(!$this->goShareServer){
      $smarty->assign("goShareServerState", " disabled ");
      $smarty->assign("goExportEntryACL", " disabled ");
    }else{
      $smarty->assign("goShareServerState", "  ");
      $smarty->assign("goExportEntryACL", "  ");
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
        
      /* Save xdmcp is enabled flag */
      if (isset($_POST['goShareServer'])){
        $this->goShareServer = true;
      } else {
        $this->goShareServer = false;
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
  
    if((isset($_POST['goTerminalServer']))&&(empty($this->goFontPath))){
      $message[]=_("Terminal server, must have fontpath specified.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
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

    /* Arrays */
    foreach (array("goTimeSource", "goExportEntry") as $name){
      $this->attrs[$name]= array();
      foreach ($this->$name as $element){
        $this->attrs[$name][]= $element;
      }
    }
 
    /* Remove illegal attributes */
    foreach ($this->additionaloc as $oc => $attrs){
      if (!in_array($oc, $this->objectclasses)){
        foreach ($attrs as $attr){
          $this->attrs[$attr]= array();
        }
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
