<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class userManagement extends management
{
  var $plHeadline     = "Users";
  var $plDescription  = "Manage users";
  var $plIcon  = "plugins/users/images/user.png";

  var $sn = "";
  var $givenName = "";

  // Tab definition 
  protected $tabClass = "usertabs";
  protected $tabType = "USERTABS";
  protected $aclCategory = "users";
  protected $aclPlugin   = "user";
  protected $objectName   = "user";

  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;
    
    // Build filter
    $filter = new filter(get_template_path("user-filter.xml", true));
    $filter->setObjectStorage("ou=people,");

    // Build headpage
    $headpage = new listing(get_template_path("user-list.xml", true));
    $headpage->registerElementFilter("lockLabel", "userManagement::filterLockLabel");
    $headpage->registerElementFilter("lockImage", "userManagement::filterLockImage");
    $headpage->setFilter($filter);
    $this->cpHandler = new CopyPasteHandler($this->config);
    $this->snapHandler = new SnapshotHandler($this->config);

    parent::__construct($config, $ui, "users", $headpage);

    $this->registerAction("new",    "newEntry");
    $this->registerAction("edit",   "editEntry");
    $this->registerAction("apply",  "applyChanges");
    $this->registerAction("save",   "saveChanges");
    $this->registerAction("cancel", "cancelEdit");
    $this->registerAction("remove", "removeEntryRequested");
    $this->registerAction("removeConfirmed", "removeEntryConfirmed");

    $this->registerAction("copy",   "copyPasteHandler");
    $this->registerAction("cut",    "copyPasteHandler");
    $this->registerAction("paste",  "copyPasteHandler");

    // Register special user actions 
    $this->registerAction("lock",   "lockEntry");
    $this->registerAction("unlock", "alert");
    $this->registerAction("new_template", "newTemplate");
    $this->registerAction("newfromtpl", "newUserFromTemplate");
    $this->registerAction("templateContinue", "templateContinue");
  }

  // Inject user actions 
  function detectPostActions()
  {
    $action = management::detectPostActions();
    if(isset($_POST['template_continue'])) $action['action'] = "templateContinue";
    return($action);
  }


  function lockEntry($action,$entry)
  {
    $disallowed = array();
    $dns = array();
    foreach($entry as $dn){
      if (!preg_match("/w/",$this->ui->get_permissions($dn,"users/password"))){
        $disallowed[] = $dn;
      }else{
        $allowed[] = $dn;
      }
    }
    if(count($disallowed)){
      msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
    }
    $ldap = $this->config->get_ldap_link();
    foreach($allowed as $dn){
      $ldap->cat($dn, array('userPassword'));
      if($ldap->count() == 1){
        $val = $ldap->fetch();
        $pwd = $val['userPassword'][0];
        $method = passwordMethod::get_method($pwd,$val['dn']);
        $success= false;
        if($method instanceOf passwordMethod){
          if($method->is_locked($this->config,$val['dn'])){
            $success= $method->unlock_account($this->config,$val['dn']);
          }else{
            $success= $method->lock_account($this->config,$val['dn']);
          }

          /* Check for success */
          if (!$success){
            $hn= $method->get_hash_name();
            if (is_array($hn)){
              $hn= $hn[0];
            }
            msg_dialog::display(_("Account locking"),
              sprintf(_("Password method '%s' does not support locking. Account has not been locked!"), $hn),WARNING_DIALOG);
          }
        }else{
          // Can't lock unknown methods.
        }
      }
    }
  }


  function alert($action,$values)
  {
    print_a(array($action,$values));
  }

  function newTemplate($action,$entry)
  {
    $this->newEntry();
    $this->tabObject->set_template_mode ();
  }

 
  function templateContinue()
  {
    $smarty = get_smarty();
    $templates = array();
    $templates['none']= _("none");
    $templates = array_merge($templates,$this->get_templates());

    if (isset($_POST['template_continue'])){
      $message = array();
      if(!isset($_POST['template']) || (empty($_POST['template']))){
        $message[]= msgPool::invalid(_("Template"));
      }
      if(!isset($_POST['sn']) || (empty($_POST['sn']))){
        $message[]= msgPool::required(_("Name"));
      }
      if(!isset($_POST['givenName']) || (empty($_POST['givenName']))){
        $message[]= msgPool::required(_("Given name"));
      }

      /* Show error message / continue editing */
      if (count($message) > 0){
        msg_dialog::displayChecks($message);

        foreach(array("sn", "givenName", "uid", "template") as $attr){
          if(isset($_POST[$attr])){
            $smarty->assign("$attr", $_POST[$attr]);
          }else{
            $smarty->assign("$attr", "");
          }
        }
        $smarty->assign("templates",$templates);
        $smarty->assign("got_uid",$this->got_uid);
        $smarty->assign("edit_uid",false);
        return($smarty->fetch(get_template_path('template.tpl', TRUE)));
      }
    }


    /* Continue template editing */
    if ($_POST['template'] != 'none' && !isset($_POST['uid'])){

      $this->sn             = $_POST['sn'];
      $this->givenName      = $_POST['givenName'];

      /* Check for requred values */
      $message= array();
      if ($this->sn == "") {
        $message[]= msgPool::required(_("Name"));
      }
      if ($this->givenName == "") {
        $message[]= msgPool::required(_("Given name"));
      }

      /* Check if dn is used */
      $dn= preg_replace("/^[^,]+,/i", "", $_POST['template']);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($dn);
      $ldap->search ("(&(sn=".normalizeLdap($this->sn).")(givenName=".normalizeLdap($this->givenName)."))", array("givenName"));
      if ($ldap->count () != 0){
        msgPool::duplicated(_("Name"));
      }

      /* Show error message / continue editing */
      if (count($message) > 0){
        msg_dialog::displayChecks($message);
      } else {
        $attributes= array('sn' => $this->sn, 'givenName' => $this->givenName);
        if ($this->config->get_cfg_value("idGenerator") != ""){
          $uids= gen_uids ($this->config->get_cfg_value("idGenerator"), $attributes);
          if (count($uids)){
            $smarty->assign("edit_uid", "false");
            $smarty->assign("uids", $uids);
            $this->uid= current($uids);
          }
        } else {
          $smarty->assign("edit_uid", "");
          $this->uid= "";
        }
        $this->got_uid= true;
      }

      foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
        $smarty->assign("$attr", $this->$attr);
      }
      if (isset($_POST['template'])){
        $smarty->assign("template", $_POST['template']);
      }
      $smarty->assign("templates",$templates); 
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));
    }

    /* No template. Ok. Lets fill data into the normal user dialog */
    if ($_POST['template'] == 'none'){
      foreach(array("sn", "givenName", "uid") as $attr){
        if (isset($_POST[$attr])){
          $this->tabObject->by_object['user']->$attr= $_POST[$attr];
        }
      }
    }elseif(isset($_POST['uid'])){

      /* Move user supplied data to sub plugins */
      foreach(array("uid","sn","givenName") as $attr){
        $this->$attr = $_POST[$attr];
        $this->tabObject->$attr       = $this->$attr;
        $this->tabObject->by_object['user']->$attr = $this->$attr;
      }

      $template_dn              = $_POST['template'];
      $this->tabObject->adapt_from_template($template_dn, array("uid","cn","givenName","sn"));
      $template_base            = preg_replace("/^[^,]+,".preg_quote(get_people_ou(), '/')."/", '', $template_dn);
      $this->tabObject->by_object['user']->base= $template_base;
    }
  }

  function newUserFromTemplate($action,$entry)
  {

    // By default we set 'dn' to 'new', all relevant plugins will react on this.
    $this->dn= "new";
    $this->got_uid= ($this->config->get_cfg_value("idGenerator") == "");

    $tab = $this->tabClass;
    $this->tabObject= new $tab($this->config,$this->config->data['TABS'][$this->tabType], $this->dn, $this->aclCategory);
    $this->tabObject->by_object['user']->base= $this->headpage->getBase();
    $this->tabObject->set_acl_base($this->headpage->getBase());

    // Use template if there are any of them 
    $templates = array();
    $templates['none']= _("none");
    $templates = array_merge($templates,$this->get_templates());
    if (count($templates)){
      $smarty = get_smarty();
      foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
        $smarty->assign("$attr", "");
      }
      $smarty->assign("template",  array_pop($entry));
      $smarty->assign("templates", $templates);
      $smarty->assign("edit_uid", "");
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));
    }
  }

  function get_templates()
  {
    $templates= array();
    $ldap= $this->config->get_ldap_link();

    /* Create list of templates */
    foreach ($this->config->departments as $key => $value){

      /* Get acls from different ou's */
      $acl = $this->ui->get_permissions($value,$this->aclCategory."/".$this->aclPlugin);

      /* If creation of a new user is allowed, append this template */
      if (preg_match("/c/",$acl)){

        /* Search all templates from the current dn */
        $ldap->cd (get_people_ou().$value);
        $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));

        /* Append */
        if ($ldap->count() != 0){
          while ($attrs= $ldap->fetch()){
            $templates[$ldap->getDN()]= $attrs['uid'][0]." - ".LDAP::fix($key);
          }
        }
      }
    }
    natcasesort ($templates);
    reset ($templates);
    return($templates);
  }




  static function filterLockImage($userPassword)
  {
    $image= "images/empty.png";

    if(isset($userPassword[0]) && preg_match("/^\{[^\}]/",$userPassword[0])){
      if(preg_match("/^[^\}]*+\}!/",$userPassword[0])){
        $image= "images/lists/locked.png";
      }else{
        $image= "images/lists/unlocked.png";
      }
    }

    return $image;
  }


  static function filterLockLabel($userPassword)
  {
    $label= "";

    if(isset($userPassword[0]) && preg_match("/^\{[^\}]/",$userPassword[0])){
      if(preg_match("/^[^\}]*+\}!/",$userPassword[0])){
        $label= _("Unlock account");
      }else{
        $label= _("Lock account");
      }
    }

    return $label;
  }


} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
