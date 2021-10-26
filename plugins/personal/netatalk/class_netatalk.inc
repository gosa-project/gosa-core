<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2006  Gina Haeussge <osd@foosel.net>
   Copyright (C) 2006  Bernd Zeimetz <bernd@zeimetz.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*! \brief   netatalk plugin
  \author  Gina Haeussge <osd@foosel.net>
  \author  Bernd Zeimetz <bernd@zeimetz.de>
  \version 0.1
  \date    21.3.2006

  This class provides the functionality to read and write all attributes
  relevant for netatalk from/to the LDAP. It does syntax checking
  and displays the formulars required.
 */

class netatalk extends plugin {

  /* Definitions */
  var $plHeadline     = "Netatalk";
  var $plDescription  = "Manage Netatalk account";

  var $view_logged = FALSE;

  /* Plugin specific values */
  var $apple_user_homepath_raw   = "";
  var $apple_user_homeurl_raw     = "";
  var $apple_user_homeurl_xml     = "";
  var $apple_user_homeurl       = "";
  var $apple_user_homeDirectory   = "";
  var $apple_user_share       = "";
  var $shares             = array();
  var $shares_settings        = array();
  var $selectedshare        = "";
  var $mountDirectory         = "/Network/Servers";

  /* Attributes to save to LDAP */
  var $attributes = array ("apple_user_homeurl", "apple_user_homeDirectory");
  var $CopyPasteVars= array("apple_user_homeurl", "apple_user_homeDirectory",
      "apple_user_share","shares_settings","apple_user_homepath_raw",
      "apple_user_homeurl_raw","apple_user_homeurl_xml","apple_user_homeurl",
      "selectedshare","mountDirectory");

  /* Attributes to use in smarty template */
  var $smarty_attributes = array ("apple_user_homepath_raw", "shares", "selectedshare");

  /* Attributes to save from $_POST */
  var $post_attributes = array ("netatalkShare"         => "apple_user_share", 
                                "netatalkUserHomepath"  => "apple_user_homepath_raw");

  /* Objectclasses */
  var $objectclasses = array ("apple-user");
  var $uid ="";  

  /* The constructor just saves a copy of the config. You may add what ever you need. */
  function __construct(&$config, $dn = NULL) 
  {

    /* Include config object */
    $this->config = $config;
    plugin::__construct($config, $dn);

    /* set user id */    
    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Netatalk attribute include '-' and we can't handle thos attribute names.
       Copy all thos attribute into a useable name.
     */
    foreach($this->attributes as $val) {
      $name = str_replace('_', '-', $val);
      if (isset($this->attrs[$name][0])) {
        $this->$val = $this->attrs[$name][0];
      }
    }

    /* Extract homepath value 
     */
    if (strlen($this->apple_user_homeDirectory) >0) {
      $this->apple_user_homepath_raw = 
        substr($this->apple_user_homeDirectory, strrpos($this->apple_user_homeDirectory, '/') + 1 );
    }

    /* get share list an set default values */
    $this->get_netatalk_shares(); 
    $this->apple_user_share = $this->selectedshare;

    if(!$this->is_account){
        $this->apple_user_share = key($this->shares);
    }

    /* Save initial account state */
    $this->initially_was_account = $this->is_account;
  }



  /* Get netatalk shares */
  function get_netatalk_shares()
  {
    /* Get netatalk shares */
    $this->shares = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=mount)(|(mountType=url)(mountType=nfs))(cn=*))");

    while ($attrs = $ldap->fetch()){
      $tmp  = explode(":", $attrs["cn"][0]);
      $host = trim($tmp[0]);
      $dir  = trim($tmp[1]);
      $mountType = trim($attrs["mountType"][0]);
      if ($mountType == "url") {
        $mountTypeReal = "netatalk";
      } else {
        $mountTypeReal = $mountType;
      } 
      $share = $attrs["cn"][0]. " (" . $mountTypeReal . ")";
      $this->shares[$share] = $share;
      $this->shares_settings[$share]["mountType"]=$mountType;
      $this->shares_settings[$share]["dir"]=$dir;
      $this->shares_settings[$share]["host"]=$host;

      $oldShare=substr($this->apple_user_homeDirectory, 0, strrpos($this->apple_user_homeDirectory, '/'));
      $newShare=($this->mountDirectory . "/". $host . $dir );
      if (strcmp($oldShare, $newShare)==0) {
        $this->selectedshare = $share;
      }
    }
    asort($this->shares);
  }


  /* Execute the plugin, produce the output. */
  function execute() 
  {
    plugin :: execute();

    $theme = getThemeName();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Use the smarty templating engine here... */
    $smarty = get_smarty();
    $display = "";

    /* Do we need to flip is_account state? */
    if (isset ($_POST['modify_state'])) {
      $this->is_account = !$this->is_account;
    }

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent === NULL) {
      switch ($theme) {
        case 'classic':
          $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
          break;
        
        default:
          $symbol = "<i class='material-icons'>report</i>";
          break;
      }
      $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("Netatalk"))."</div>";
      $display .= back_to_main();
      return $display;
    }

    /* Show tab dialog headers */
    if ($this->parent !== NULL) {
      if ($this->is_account) {
        $display = $this->show_disable_header(msgPool::removeFeaturesButton(_("Netatalk")), 
            msgPool::featuresEnabled(_("Netatalk")));
      } else {
        $errmsg="";
        $obj = $this->parent->by_object['posixAccount'];
        if  (!($obj->is_account) ) {
          $display = $this->show_enable_header(msgPool::addFeaturesButton(_("Netatalk")), 
              msgPool::featuresDisabled(_("Netatalk"), _("POSIX")), TRUE);
        } elseif (count($this->shares)== 0) {
          $display = $this->show_enable_header(msgPool::addFeaturesButton(_("Netatalk")), 
              msgPool::featuresDisabled(_("Netatalk"), _("Netatalk or NFS share")), TRUE);
        } else {
          $display = $this->show_enable_header(msgPool::addFeaturesButton(_("Netatalk")), 
              msgPool::featuresDisabled(_("Netatalk")));
        } 
        return ($display);
      }
    }

    /* Assign attributes 
     */
    foreach ($this->smarty_attributes as $val) {
      $smarty->assign("$val", set_post($this-> $val));
    }

    /* Assign ACLs 
     */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $desc){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Let smarty fetch and process the page. */
    $display .= ($smarty->fetch(get_template_path('netatalk.tpl', TRUE, dirname(__FILE__))));
    return ($display);
  }


  /* Check if we have correct data */
  function check() 
  {
    $message = array ();
    if (strlen($this->apple_user_share) == 0) {
      $message[] = msgPool::required(_("Share"));
    }
    return ($message);
  }


  /* Save to LDAP */
  function save() 
  {

    /* remove a / at the end of the homepath, we neither need it there nor
     * do we want to check for it later.
     */
    if(substr($this->apple_user_homepath_raw, -1, 1) === '/') {
      $this->apple_user_homepath_raw=substr($this->apple_user_homepath_raw, 0, -1);
    }

    $mountType=$this->shares_settings[$this->apple_user_share]["mountType"];
    $dir=$this->shares_settings[$this->apple_user_share]["dir"];
    $host=$this->shares_settings[$this->apple_user_share]["host"];

    /* Convert raw data to wished format */
    if ($this->is_account) {
      if($mountType=="url") {
        $this->apple_user_homeurl_xml = '<home_dir><url>afp://'.$host.$dir . '</url><path>'.
          $this->apple_user_homepath_raw.'</path></home_dir>';
        $this->apple_user_homeurl = $this->apple_user_homeurl_xml;
      } else {
        $this->apple_user_homeurl = "";
      }
      $this->apple_user_homeDirectory = $this->mountDirectory . '/' . $host .
        $dir . '/' . $this->apple_user_homepath_raw;
    } else {
      $this->apple_user_homeurl = "";
      $this->apple_user_homeDirectory = "";
    }

    $ldap = $this->config->get_ldap_link();
    plugin :: save();

    /* Transform variable names from '_' to '-'.
     */
    foreach ($this->attributes as $val) {
      unset($this->attrs[$val]);
      $name = str_replace('_', '-', $val);
      if ($this->$val != "") {
        $this->attrs[$name] = $this->$val;
      } else {
        $this->attrs[$name] = array();
      }
    }

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify($this->attrs);

    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account) {
      if ($this->is_modified) {
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }
  }


  /* Use Save_object for every Post handling 
   */
  function save_object() 
  {
    if (isset ($_POST['netatalkTab'])) {
      /* Save ldap attributes */
      plugin :: save_object();

      foreach($this->post_attributes as $acl => $val) {
        if(!preg_match("/w/",$this->getacl($acl))) continue;
        if (isset ($_POST[$val])) {
          $this->$val = get_post($val);
        } else {
          $this->$val = "";
        }
      }
      $this->apple_user_homeurl_raw = 'afp://' . $this->apple_user_share;
    }
  }


  function remove_from_parent() 
  {

    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account) {
      return;
    }

    /* include global link_info */
    $ldap = $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin :: remove_from_parent();
    $this->cleanup();

    /* Attribute name conversion "_" to "-" */
    foreach($this->attributes as $val){
      unset($this->attrs[$val]);
      $name = preg_replace("/_/","-",$val);
      $this->attrs[$name] = array();
    }

    @ DEBUG(DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $this->attributes, "Save");
    $ldap->cd($this->dn);

    $ldap->modify($this->attrs);

    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* remove the entry from LDAP */
    unset ($this->attrs['uid']);

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove', array("uid" => $this->uid));
  }


  /* Return plugin informations for acl handling*/
  static function plInfo()
  {
    return (array(
          "plDescription"     => _("Netatalk"),
          "plSelfModify"      => TRUE,
          "plDepends"         => array("user"),
          "plPriority"        => 6,
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"         => array(),
          "plRequirements"=> array(
              'ldapSchema' => array('apple-user' => ''),
              'onFailureDisablePlugin' => array(get_class())),
          "plProvidedAcls"  => array(
            "netatalkUserHomepath"   =>  _("User home path"),
            "netatalkShare"          =>  _("Share"))
          ));
  }

  function adapt_from_template($dn, $skip= array())
  {
    /* As the 'apple-user-homeDirectory' LDAP attribute is stored as
     * 'apple_user_homeDirectory' internally by GOsa (to avoid problem with
     * dashes as variable names), we need to add the dashed version to
     * $this->attributes so that the adapt_from_template function in the plugin
     * class will find the dashed LDAP attribute and substitute possibly
     * placeholders like %uid. We remove the dashed version of
     * 'apple_user_homeDirectory' from the attributes again afterwards. */
    array_push($this->attributes, "apple-user-homeDirectory");
    plugin::adapt_from_template($dn, $skip);
    array_pop($this->attributes);
  
    /* Loop through the available shares and find out which is the share from
     * 'apple-user-homeDirectory'. Remove its host and directory (as well as
     * the default 'mountDirectory' prefix) to get 'apple_user_homepath_raw'
     * and store that share as 'selectedshare' so that it gets displayed by
     * default in the netatalk tab. */
    foreach($this->shares as $share) {
      $apple_user_homepath = $this->mountDirectory."/".
			$this->shares_settings[$share]["host"].
			$this->shares_settings[$share]["dir"]."/";
      if (preg_match("#$apple_user_homepath#", $this->{'apple-user-homeDirectory'})) {
        $this->apple_user_homepath_raw =  preg_replace("#$apple_user_homepath#", "", $this->{'apple-user-homeDirectory'});
        $this->selectedshare = $share;
      }
    }
  }   
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
