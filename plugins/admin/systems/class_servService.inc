<?php

class servservice extends plugin
{
  var $goExportEntry    = array();
  var $goExportEntryList= array();
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
  var $goMailServer     = "";
  var $o_subWindow      = NULL;

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes       = array("goLdapBase","goXdmcpIsEnabled","goFontPath","goExportEntry","goTimeSource");
  var $possible_objectclasses= array( "goShareServer", "goNtpServer", "goServer", "GOhard", "goLdapServer",
                                      "goTerminalServer", "goSyslogServer", "goCupsServer","goMailServer");
  var $objectclasses    = array( "top","goServer", "GOhard"); 
  var $additionaloc     = array( "goShareServer"     => array("goExportEntry"),
                                 "goNtpServer"     => array("goTimeSource"),
                                 "goLdapServer"    => array("goLdapBase"),
                                 "goTerminalServer"=> array("goXdmcpIsEnabled", "goFontPath"),
                                 "goSyslogServer"  => array(),
                                 "goMailServer"    => array(),
                                 "goCupsServer"    => array());

  var $allow_mounts = false; //do we allow mount entries?
  var $mounts_to_add = array();
  var $mounts_to_remove = array();
  var $oldone = NULL; //temp dave for old mount entry

  function servservice ($config, $dn= NULL, $parent= NULL)
  {
    
    plugin::plugin ($config, $dn, $parent);
    
    $ldap = $this->config->get_ldap_link();
    $avl_objectclasses = $ldap->get_objectclasses();
    if (isset($avl_objectclasses["mount"])) {
        $this->allow_mounts = true;
    }
    
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
      if(isset($this->attrs['goExportEntry']['count'])){
        for($i= 0; $i<$this->attrs['goExportEntry']['count']; $i++){
          $entry= $this->attrs['goExportEntry'][$i];
          $tmp[preg_replace('/\|.*$/', '', $entry)]= $entry;
        }
      }
    }
    $this->goExportEntryList = $tmp;

    /* Always is account... */
    $this->is_account= TRUE;

    /* Check if goShareServer is defined */
    if((isset($this->attrs['objectClass']))&&(is_array($this->attrs['objectClass']))){ 
      if(in_array("goShareServer",$this->attrs['objectClass'])){
        $this->goShareServer = true;    
      }
    }
  }

  function addToList($entry){
    $key =  key($entry);
    $this->goExportEntryList[$key]=$entry[$key];
  }

  function deleteFromList($id){
    /* Check if the share is used by someone */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(|(gotoProfileServer=*|$id)(gotoShare=*|$id|*||*))", array("cn"));
    $cnt= $ldap->count();
    if ($cnt){
      $msg= sprintf(_("The share can't be removed since it is still used by %d users:"), $cnt);
      $msg.= "<br><br><ul>";
      while ($attrs= $ldap->fetch()){
        $msg.= "<li>".$attrs["cn"][0]."</li>";
      }
      $msg.= "</ul>"._("Please correct the share-/profile settings of these users");
      print_red($msg);
      
    } else {
      /* Finally remove it */
      unset($this->goExportEntryList[$id]);
    }
  }
  
  function addToMountList($entry) {
    $key =  key($entry);
    $type = $this->get_share_type($entry[$key]);
    if (($type == "netatalk") || ($type == "NFS")) {
        $this->mounts_to_add[$entry[$key]] = $entry[$key];
      unset($this->mounts_to_remove[$entry[$key]]);
    }
  }
  
  function deleteFromMountList($entry) {
    $type = $this->get_share_type($entry);
    if (($type == "netatalk") || ($type == "NFS")) {
      $this->mounts_to_remove[$entry] = $entry;
      unset($this->mounts_to_add[$entry]);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("staticAddress", "");

    if((isset($_POST['DelNfsEnt'])) && (isset($_POST['goExportEntryList'])) && chkacl($this->acl,"goExportEntry") == ""){
      if($this->allow_mounts){
        foreach($_POST['goExportEntryList'] as $entry){
          $this->deleteFromMountList($this->goExportEntryList[$entry]);
        }
      }
      foreach($_POST['goExportEntryList'] as $entry){
        $this->deleteFromList($entry);
      }
    }

    if(isset($_POST['NewNfsAdd']) && chkacl($this->acl,"goExportEntry") == ""){
      $this->oldone = NULL;
      $this->o_subWindow = new servnfs($this->config,$this->acl, $this->allow_mounts, $this->dn);
      $this->dialog = true;
    }

    if((isset($_POST['NewNfsEdit']))&&(isset($_POST['goExportEntryList'])) && chkacl($this->acl,"goExportEntry") == ""){
      $entry = $this->goExportEntryList[$_POST['goExportEntryList'][0]];
      $add_mount=isset($this->mounts_to_add[$entry]);
      $this->oldone=$entry;
      $this->o_subWindow = new servnfs($this->config,$this->acl,$this->allow_mounts,$this->dn,$entry,$add_mount);
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
        $this->addToList($newone);
        if($this->allow_mounts){
          if($this->oldone) {
              $this->deleteFromMountList($this->oldone);
          }
          if ($this->o_subWindow->should_create_mount()) {
              $this->addToMountList($newone);
          }
        }
        unset($this->o_subWindow);
        $this->dialog = false;
      }
    }
    
    /* Cancel NFS setup */
    if(isset($_POST['NFScancel'])){
      $this->oldone = NULL;
      unset($this->o_subWindow);
      $this->dialog = false;
    }
  
    /* Execute NFS setup dialog*/
    if(isset($this->o_subWindow)){
      return $this->o_subWindow->execute(); 
    }

    /* Here we add a new entry  */
    if(isset($_POST['NewNTPAdd']) && $_POST['NewNTPExport'] != "" && chkacl($this->acl,"goNtpServer") == "") {
      $this->goTimeSource[$_POST['NewNTPExport']]= $_POST['NewNTPExport'];
      asort($this->goTimeSource);
    }

    /* Deleting an Entry, is a bit more complicated than adding one*/
    if(isset($_POST['DelNTPEnt']) && chkacl($this->acl,"goNtpServer") == "") {
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
    ksort($this->goExportEntryList);
    foreach($this->goExportEntryList as $name=>$values){
       $tmp = split("\|",$values);
       $tellSmarty[$name] = $tmp[0]." ".$tmp[4]." (".$tmp[2].")";
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
      $smarty->assign("goExportEntryACL", chkacl($this->acl, "goExportEntry"));
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
    plugin::save_object();
    if (isset($_POST['servicetab'])){
      $tmp = $this->goTimeSource;
  
      if(isset($_POST['goLdapBase']) && chkacl($this->acl,"goLdapBase") == ""){
        $this->goLdapBase = $_POST['goLdapBase'];
      }
  
      /* Save checkbox state */
      foreach ($this->additionaloc as $oc => $dummy){
//        if($oc == "goNtpServer") continue;
        if (chkacl($this->acl, $oc) == ""){
          if (isset($_POST[$oc]) && $_POST[$oc] == '1'){
            $this->objectclasses[$oc]= $oc;
          } else {
            unset($this->objectclasses[$oc]);
          }
        }
      }

      /* Save xdmcp is enabled flag */
      if(chkacl($this->acl,"goXdmcpIsEnabled") == ""){
        if (isset($_POST['goXdmcpIsEnabled'])){
          $this->goXdmcpIsEnabled= "true";
        } else {
          $this->goXdmcpIsEnabled= "false";
        }
      }
        
      /* Save xdmcp is enabled flag */
      if(chkacl($this->acl,"goExportEntry") == ""){
        if (isset($_POST['goShareServer'])){
          $this->goShareServer = true;
        } else {
          $this->goShareServer = false;
        }
      }
      $this->goTimeSource = array();
      $this->goTimeSource = $tmp;
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
  
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
    foreach (array("goTimeSource"=>"goTimeSource", "goExportEntryList"=>"goExportEntry") as $source => $destination){
      $this->attrs[$destination]= array();
      foreach ($this->$source as $element){
        $this->attrs[$destination][]= $element;
      }
    }
    
    /* Process netatalk mounts */
    if($this->allow_mounts) {
      $this->process_mounts();
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
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving server service object failed"));
    
    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }
  
  function process_mounts() {
    
    $clip = "cn=" . $this->cn . ",ou=servers,ou=systems,";
    $mountsdn = "cn=mounts," . substr($this->dn, strlen($clip));
    
    $mounts = array(
      "objectClass" => "container",
      "cn" => "mounts"
    );
    
    # load data from mounts container
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($mountsdn, array('dn'));
    $attrs = $ldap->fetch();
    
    # mounts container not present yet, so we create it
    if (count($attrs) == 0) {
        $ldap->cd($mountsdn);
        $ldap->add($mounts);
        show_ldap_error($ldap->get_error(), _("Creating mount container failed"));
        gosa_log("Mount container '$mountsdn' has been created");
    }

    # remove deleted mounts from the container
    foreach ($this->mounts_to_remove as $entry) {
      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";

      $ldap->cat($mountdn, array('dn'));
      $attrs = $ldap->fetch();

      if (count($attrs) != 0) {
        $ldap->rmdir($mountdn);
        show_ldap_error($ldap->get_error(), _("Removing mount container failed"));
        gosa_log("Mount object '".$mountdn."' has been removed");
      }
    }

    # add new mounts to the container
    foreach ($this->mounts_to_add as $entry) {

      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";  
      $ldap->cd($mountdn);
      $ldap->add($mount);
      show_ldap_error($ldap->get_error(), _("Saving mount container failed"));
      gosa_log("Mount object '".$mountdn."' has been added");
    }
  }
  
  function get_share_type($share) {
    $tmp = split("\|", $share);
    return $tmp[2];
  }


  function returnMountEntry($entry) {
  $item = split("\|", $entry);
    $name = $item[0];
    $description = $item[1];
    $type = $item[2];
    $charset = $item[3];
    $path = $item[4];
    $options = $item[5];
    
    switch ($type) {
      case "netatalk" : {
            $mount = array(
            "mountDirectory" => "/Network/Servers/",
            "mountOption" => array(
              "net",
              "url==afp://;AUTH=NO%20USER%20AUTHENT@".$this->cn."/$name/"
            ),
            "mountType" => "url",
            "objectClass" => "mount",
              "cn" => $this->cn .":/".$name
          );
          break;
      }
      case "NFS" : {
        $mount = array(
            "mountDirectory" => "/Network/Servers/",
            "mountOption" => "net",
            "mountType" => "nfs",
            "objectClass" => "mount",
              "cn" => $this->cn .":".$path
          );
          break;
      }        
      default : {
        continue;
      }
    }
  return $mount;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
