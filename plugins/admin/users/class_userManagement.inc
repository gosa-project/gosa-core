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
  var $uid = "";
  var $got_uid = "";
  var $edit_uid = "";

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
    $this->registerAction("lockUsers",   "lockUsers");
    $this->registerAction("unlockUsers", "lockUsers");
    $this->registerAction("new_template", "newTemplate");
    $this->registerAction("newfromtpl", "newUserFromTemplate");
    $this->registerAction("templateContinue", "templateContinue");
    $this->registerAction("templatize", "templatizeUsers");
    $this->registerAction("templatizeContinue", "templatizeContinue");
  }

  // Inject user actions 
  function detectPostActions()
  {
    $action = management::detectPostActions();
    if(isset($_POST['template_continue'])) $action['action'] = "templateContinue";
    if(isset($_POST['templatize_continue'])) $action['action'] = "templatizeContinue";
    return($action);
  }



  function alert($action,$values)
  {
    print_a(array($action,$values));
  }

  /*! \brief  Intiates template creation. 
   */ 
  function newTemplate($action,$entry)
  {
    $this->newEntry();
    $this->tabObject->set_template_mode ();
  }


  /*! \brief  Intiates user creation. 
   *          If we've user templates, then the user will be asked to use to use one. 
   *          -> See 'templateContinue' for further handling.
   */ 
  function newUserFromTemplate($action="",$target=array(),$all=array())
  {
    // Call parent method, it knows whats to do, locking and so on ...
    management::newEntry($action,$target,$all);

    // Reset uid selection.
    $this->got_uid= "";

    // Use template if there are any of them 
    $templates = array();
    $templates['none']= _("none");
    $templates = array_merge($templates,$this->get_templates());

    // We've templates, so preset the current template and display the input dialog.
    if (count($templates)){
      $smarty = get_smarty();
      foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
        $smarty->assign("$attr", "");
      }
      $smarty->assign("template",  array_pop($target));
      $smarty->assign("templates", $templates);
      $smarty->assign("edit_uid", "");
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));

      // -> See 'templateContinue' for further handling!
    }
  }



  /*! \brief  Intiates user creation. 
   *          If we've user templates, then the user will be asked 
   *           if he wants to use one. 
   *          -> See 'templateContinue' for further handling.
   */ 
  function newEntry($action="",$target=array(),$all=array())
  {
  
    // Call parent method, it manages everything, locking, object creation...
    management::newEntry($action,$target,$all);
    
    // If we've at least one template, then ask the user if he wants to use one?
    $templates = array();
    $templates['none']= _("none");
    $templates = array_merge($templates,$this->get_templates());

    // Display template selection
    if (count($templates)){
      $smarty = get_smarty();
  
      // Set default variables, normally empty.
      foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
        $smarty->assign($attr, "");
      }
      $smarty->assign("template", "none");
      $smarty->assign("templates", $templates);
      $smarty->assign("edit_uid", "");
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));

      // -> See 'templateContinue' for further handling!
    }
  }


  /* !\brief  This method is called whenever a template selection was displayed.
   *          Here we act on the use selection. 
   *          - Does the user want to create a user from template?
   *          - Create user without template?
   *          - Input correct, every value given and valid? 
   */ 
  function templateContinue()
  {
    // Get the list of available templates.
    $templates = array();
    $templates['none']= _("none");
    $templates = array_merge($templates,$this->get_templates());

    // Input validation, if someone wants to create a user from a template
    //  then validate the given values.
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

    /********************
     * 1   We've had input errors - Display errors and show input dialog again. 
     ********************/

    if (count($message) > 0){
      msg_dialog::displayChecks($message);

      // Preset input fields with user input. 
      $smarty = get_smarty();
      foreach(array("sn", "givenName", "uid", "template") as $attr){
        if(isset($_POST[$attr])){
          $smarty->assign("$attr", get_post($attr));
        }else{
          $smarty->assign("$attr", "");
        }
      }

      $smarty->assign("templates",$templates);
      $smarty->assign("got_uid", $this->got_uid);
      $smarty->assign("edit_uid",false);
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));
    }


    /********************
     * 2   There was a template selected, now ask for the uid.
     ********************/

    if ($_POST['template'] != 'none' && !isset($_POST['uid'])){

      // Remember user input.
      $smarty = get_smarty();
      $this->sn             = $_POST['sn'];
      $this->givenName      = $_POST['givenName'];

      // Avoid duplicate entries, check if such a user already exists.
      $dn= preg_replace("/^[^,]+,/i", "", $_POST['template']);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($dn);
      $ldap->search ("(&(sn=".normalizeLdap($this->sn).")(givenName=".normalizeLdap($this->givenName)."))", array("givenName"));
      if ($ldap->count () != 0){
        msg_dialog::displayChecks(array(msgPool::duplicated(_("Name"))));
      }else{

        // Preset uid field by using the idGenerator 
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

      // Assign user input 
      foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
        $smarty->assign("$attr", $this->$attr);
      }
      if (isset($_POST['template'])){
        $smarty->assign("template", $_POST['template']);
      }
      $smarty->assign("templates",$templates); 
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));
    }


    /********************
     * 3   No template - Ok. Lets fill the data into the user object and skip templating here. 
     ********************/
    if ($_POST['template'] == 'none'){
      foreach(array("sn", "givenName", "uid") as $attr){
        if (isset($_POST[$attr])){
          $this->tabObject->by_object['user']->$attr= $_POST[$attr];
        }
      }
      
      // The user Tab object is already instantiated, so just go back and let the 
      //  management class do the rest.
      return("");
    }


    /********************
     * 4   Template selected and uid given - Ok, then lets adapt tempalte values. 
     ********************/
    if(isset($_POST['uid'])){

      // Move user supplied data to sub plugins 
      foreach(array("uid","sn","givenName") as $attr){
        $this->$attr = $_POST[$attr];
        $this->tabObject->$attr       = $this->$attr;
        $this->tabObject->by_object['user']->$attr = $this->$attr;
      }

      // Adapt template values.
      $template_dn              = $_POST['template'];
      $this->tabObject->adapt_from_template($template_dn, array("uid","cn","givenName","sn"));
      $template_base            = preg_replace("/^[^,]+,".preg_quote(get_people_ou(), '/')."/", '', $template_dn);
      $this->tabObject->by_object['user']->base= $template_base;

      // The user Tab object is already instantiated, so just go back and let the 
      //  management class do the rest.
      return("");
    }
  }


  /* !\brief  This method applies a template to a set of users.
   */ 
  function templatizeUsers($action="",$target=array(),$all=array())
  {
    $this->dns = array();
    if(count($target)){

      // Get the list of available templates.
      $templates = $this->get_templates();

      // Check entry locking
      foreach($target as $dn){
        if (($user= get_lock($dn)) != ""){
          return(gen_locked_message ($user, $dn));
        }
        $this->dns[] = $dn;
      }
          
      // Display template
      $smarty = get_smarty();
      $smarty->assign("templates", $templates);
      return($smarty->fetch(get_template_path('templatize.tpl', TRUE)));
    }
  }


  /* !\brief  This method is called whenever the templatize dialog was used.
   */ 
  function templatizeContinue()
  {
    // Template readable? 
    $template= get_post('template');
    $acl = $this->ui->get_permissions($template, $this->aclCategory."/".$this->aclPlugin);
    if (preg_match('/r/', $acl)){
      $tab = $this->tabClass;
      foreach ($this->dns as $dn){

        // User writeable
        $acl = $this->ui->get_permissions($dn,  $this->aclCategory."/".$this->aclPlugin);
        if (preg_match('/w/', $acl)){
          $this->tabObject= new $tab($this->config, $this->config->data['TABS'][$this->tabType], $dn, $this->aclCategory);
          $this->tabObject->adapt_from_template($template, array("sn", "givenName", "uid"));
          $this->tabObject->save();
        } else {
          msg_dialog::display(_("Permission error"), msgPool::permModify($dn), ERROR_DIALOG);
        }
      }
    } else {
      msg_dialog::display(_("Permission error"), msgPool::permView($template), ERROR_DIALOG);
    }

    // Cleanup!
    $this->remove_lock(); 
    $this->closeDialogs();
  }


  /* !\brief  Lock/unlock multiple users.
   */ 
  function lockUsers($action,$target,$all)
  {
    if($action == "lockUsers"){
      $this->lockEntry($action,$target, $all, "lock");
    }else{
      $this->lockEntry($action,$target, $all, "unlock");
    }
  }

  
  /* !\brief  Locks/unlocks the given user(s).
   */ 
  function lockEntry($action,$entry, $all, $type = "toggle")
  {
    
    // Filter out entries we are not allowed to modify
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

    // Try to lock/unlock the rest of the entries.
    $ldap = $this->config->get_ldap_link();
    foreach($allowed as $dn){
      $ldap->cat($dn, array('userPassword'));
      if($ldap->count() == 1){

        // We can't lock empty passwords.
        $val = $ldap->fetch();
        if(!isset($val['userPassword'])){
          continue;
        }

        // Detect the password method and try to lock/unlock.
        $pwd = $val['userPassword'][0];
        $method = passwordMethod::get_method($pwd,$val['dn']);
        $success= true;
        if($method instanceOf passwordMethod){
          if($type == "toggle"){
            if($method->is_locked($this->config,$val['dn'])){
              $success= $method->unlock_account($this->config,$val['dn']);
            }else{
              $success= $method->lock_account($this->config,$val['dn']);
            }
          }elseif($type == "lock" && !$method->is_locked($this->config,$val['dn'])){
            $success= $method->lock_account($this->config,$val['dn']);
          }elseif($type == "unlock" && $method->is_locked($this->config,$val['dn'])){
            $success= $method->unlock_account($this->config,$val['dn']);
          }

          // Check if everything went fine.
          if (!$success){
            $hn= $method->get_hash_name();
            if (is_array($hn)){
              $hn= $hn[0];
            }
            msg_dialog::display(_("Account locking"),
                sprintf(_("Password method '%s' does not support locking. Account (%s) has not been locked!"), 
                  $hn,$dn),WARNING_DIALOG);
          }
        }else{
          // Can't lock unknown methods.
        }
      }
    }
  }


  /* !\brief  This method returns a list of all available templates.
   */ 
  function get_templates()
  {
    $templates= array();
    $ldap= $this->config->get_ldap_link();
    foreach ($this->config->departments as $key => $value){
      $acl = $this->ui->get_permissions($value,$this->aclCategory."/".$this->aclPlugin);
      if (preg_match("/c/",$acl)){

        // Search all templates from the current dn.
        $ldap->cd (get_people_ou().$value);
        $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));
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
