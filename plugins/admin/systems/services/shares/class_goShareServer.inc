<?php

class goShareServer extends goService{

  var $cli_summary      = "This plugin is used within the ServerService Pluign \nand indicates that this server supports shares.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goShareServer");
  var $attributes       = array("goExportEntry");
  var $StatusFlag       = "goShareServerStatus";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goShareServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn                   = "";
  var $goShareServerStatus  = "";
  var $goExportEntry        = array();
  var $allow_mounts         = false;
  var $mounts_to_remove     = array();
  var $mounts_to_add        = array();
  var $view_logged  =FALSE;

  function goShareServer(&$config,$dn)
  {
    goService::goService($config,$dn);

    $this->DisplayName = _("File service");

    $tmp =array();
    if(isset($this->attrs['goExportEntry'])){
      if(isset($this->attrs['goExportEntry']['count'])){
        for($i= 0; $i<$this->attrs['goExportEntry']['count']; $i++){
          $entry= $this->attrs['goExportEntry'][$i];
          $tmp[preg_replace('/\|.*$/', '', $entry)]= $entry;
        }
      }
    } 
    $this->goExportEntryList = $tmp;

    $ldap = $this->config->get_ldap_link();
    $avl_objectclasses = $ldap->get_objectclasses();
    if (isset($avl_objectclasses["mount"])) {
      $this->allow_mounts = true;
    }
  }


  function execute()
  { 
    $smarty = get_smarty(); 

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }


    if((isset($_POST['DelNfsEnt']))&&(isset($_POST['goExportEntryList'])) && ($this->acl_is_writeable("name"))){
      if($this->allow_mounts){
        foreach($_POST['goExportEntryList'] as $entry){
          $this->deleteFromMountList($this->goExportEntryList[$entry]);
        }
      }
      foreach($_POST['goExportEntryList'] as $entry){
        $this->deleteFromList($entry);
      }
    }

    if(isset($_POST['NewNfsAdd']) && ($this->acl_is_writeable("name"))){
      $this->oldone = NULL;
      $this->o_subWindow = new servnfs($this->config, $this);
      $this->o_subWindow->set_acl_category("server");
      $this->o_subWindow->set_acl_base($this->dn);
      $this->dialog = true;
    }

    if((isset($_POST['NewNfsEdit']))&&(isset($_POST['goExportEntryList']))){
      $entry = $this->goExportEntryList[$_POST['goExportEntryList'][0]];
      $add_mount=isset($this->mounts_to_add[$entry]);
      $this->oldone=$entry;
      $this->o_subWindow = new servnfs($this->config,$this,$entry,$add_mount);
      $this->o_subWindow->set_acl_base($this->dn);
      $this->o_subWindow->set_acl_category("server");
      $this->dialog = true;
    }
    if(isset($this->o_subWindow)){
      $this->o_subWindow->save_object(TRUE);
    }

    /* Save NFS setup */
    if(isset($_POST['NFSsave']) && isset($this->o_subWindow) && is_object($this->o_subWindow)){
      if(count($this->o_subWindow->check())>0){
        foreach($this->o_subWindow->check() as $msg) {
          print_red($msg);
        }
      }else{
        $this->o_subWindow->save_object();
        $newone = $this->o_subWindow->save();

        $this->addToList($newone);
        if($this->allow_mounts){
          if($this->oldone != NULL) {
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

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    /* Set acls */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    $smarty->assign("createable",$this->acl_is_createable());
    $smarty->assign("removeable",$this->acl_is_removeable());

    $tellSmarty= array();
    ksort($this->goExportEntryList);
    foreach($this->goExportEntryList as $name=>$values){
      $tmp = split("\|",$values);
      $tellSmarty[$name] = $tmp[0]." ".$tmp[4]." (".$tmp[2].")";
    }
    $smarty->assign("goExportEntry",array_keys($tellSmarty));
    $smarty->assign("goExportEntryKeys",($tellSmarty));
    return($smarty->fetch(get_template_path("goShareServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']    = _("File service (Shares)");
    $fields['AllowEdit']  = true;
    return($fields);
  }


  function save()
  {
    plugin::save();

    /* Arrays */
    foreach (array("goExportEntryList"=>"goExportEntry") as $source => $destination){
      $this->attrs[$destination]= array();
      foreach ($this->$source as $element){
        $this->attrs[$destination][]= $element;
      }
    }


    /* Process netatalk mounts */
    if($this->allow_mounts) {
      $this->process_mounts();
    }

    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system server/shares with dn '%s' failed."),$this->dn));
    if($this->initially_was_account){
      $this->handle_post_events("modify");
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      $this->handle_post_events("add");
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }
  }


  function check()
  { 
    $message = plugin::check();
    return($message);
  }


  function save_object()
  {
    if(isset($_POST['goShareServerPosted'])){
      plugin::save_object();
    }
  } 

  function addToList($entry){
    $key =  key($entry);
    $this->goExportEntryList[$key]=$entry[$key];
  }

  function deleteFromList($id)
  {
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

   function process_mounts() {

    $clip = "cn=" . $this->cn . ",".get_ou('serverou');
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
        show_ldap_error($ldap->get_error(), sprintf(_("Creating system server/shares (mount container) with dn '%s' failed."),$this->dn)); 
        new log("modify","server/".get_class($this),$mountsdn,array_keys($mounts),$ldap->get_error());
    }

    # remove deleted mounts from the container
    foreach ($this->mounts_to_remove as $entry) {
      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";

      $ldap->cat($mountdn, array('dn'));
      $attrs = $ldap->fetch();

      if (count($attrs) != 0) {
        $ldap->rmdir($mountdn);
        show_ldap_error($ldap->get_error(), sprintf(_("Removing system server/shares (mount container) with dn '%s' failed."),$this->dn)); 
        new log("remove","server/".get_class($this),$mountdn,array_keys($mount),$ldap->get_error());
      }
    }

    # add new mounts to the container
    foreach ($this->mounts_to_add as $entry) {
      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";
      $ldap->cd($mountdn);
      $ldap->add($mount);
      show_ldap_error($ldap->get_error(), sprintf(_("Saving system server/shares (mount container) with dn '%s' failed."),$this->dn)); 
      new log("create","server/".get_class($this),$mountdn,array_keys($mount),$ldap->get_error());
    }
  }

  function addToMountList($entry) 
  {
    if($this->acl_is_writeable("name")){
      $key =  key($entry);
      $type = $this->get_share_type($entry[$key]);
      if (($type == "netatalk") || ($type == "NFS")) {
        $this->mounts_to_add[$entry[$key]] = $entry[$key];
        unset($this->mounts_to_remove[$entry[$key]]);
      }
    }
  }

  function deleteFromMountList($entry) 
  {
    if($this->acl_is_writeable("name")){
      $type = $this->get_share_type($entry);
      if (($type == "netatalk") || ($type == "NFS")) {
        $this->mounts_to_remove[$entry] = $entry;
        unset($this->mounts_to_add[$entry]);
      }
    }
  }

  function get_share_type($share) 
  {
    $tmp = split("\|", $share);
    return $tmp[2];
  }

  function returnMountEntry($entry)
  {
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


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $tmp =array();
    if(isset($source['goExportEntry'])){
      if(isset($source['goExportEntry']['count'])){
        for($i= 0; $i<$source['goExportEntry']['count']; $i++){
          $entry= $source['goExportEntry'][$i];
          $tmp[preg_replace('/\|.*$/', '', $entry)]= $entry;
        }
      }
    }
    $this->goExportEntryList = $tmp;
    $this->goExportEntry = $tmp;
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("File service (Shares)"),
          "plDescription" => _("File service - Shares")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 90,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
              "name"        => _("Name"),
              "netatalkmount" => _("Apple mounts"),
              "description" => _("Description"),
              "type"        => _("Type"),
              "charset"     => _("Charset"),
              "path"        => _("Path"),
              "option"      => _("Option"),
              "volume"      => _("Volume"))

          ));
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
