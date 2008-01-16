<?php

class msgplug extends plugin
{
  /* Definitions */
  var $plHeadline= "Notifications";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array("target", "nmessage");
  var $objectclasses= array();

  /* Helpers */
  var $target= "group";
  var $nmessage= "";

  var $targets= array();
  var $users= array();
  var $groups= array();
  var $recipients= array();
  var $show_templates= false;
  var $templates= array();
  var $template= "";
  var $finalized= false;
  var $module = "msgplug";
  var $view_logged = FALSE;

  function msgplug (&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $ui= get_userinfo();
    $tag= $ui->gosaUnitTag;

    /* Preset values */
    $this->targets= array("user" => _("Users"), "group" => _("Groups"));
    asort($this->targets);

    $res = get_list("(objectClass=gosaAccount)", "users", $this->config->current['BASE'],array('uid', 'cn'),GL_SUBSEARCH);
    foreach($res as $key => $attrs){
      $this->users['U:'.$attrs['uid'][0]]= $attrs['cn'][0].' ['.$attrs['uid'][0].']';
    }
    ksort($this->users);

    $res = get_list("(objectClass=posixGroup)", "groups", $this->config->current['BASE'],array('cn','description'));
    foreach($res as $key => $attrs){
      $dsc= "";
      if (isset($attrs['description'][0])){
        $dsc= $attrs['description'][0];
      }
      $this->groups['G:'.$attrs['cn'][0]]= $attrs['cn'][0].' ['.$dsc.']';
    }
    ksort($this->users);


    /* Load templates */
    if (isset($this->config->current['NOTIFYDIR'])){
      $dir= $this->config->current['NOTIFYDIR'];
      if (is_dir($dir) && is_readable($dir)){

        /* Look for files and build the vacation array */
        $dh= opendir($dir);
        while ($file = readdir($dh)){
          $description= $this->parse_notification("$dir/$file");
          if ($description != ""){
            $this->templates["$dir/$file"]= $description;
          }
        }
        closedir($dh);
      }

      /* Enable templates if there are some... */
      if (count($this->templates)){
        $this->show_templates= true;
      }
    }
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","msgplug/".get_class($this),$this->dn);
    }

    /* Send message? */
    if (isset($_POST['send']) && $this->acl_is_writeable("notify")){

      /* Do we have recipients? */
      if (count($this->recipients)){

        /*Permissions ok? */
        if (!$this->acl_is_writeable('notify')){
	  msg_dialog::display(_("Permission error"), _("You have no permissions to send a message!"), ERROR_DIALOG);
        } else {
          $cmd= $this->config->search("msgplug", "NOTIFY_COMMAND",array('menu'));
          if ($cmd == ""){
	    msg_dialog::display(_("Configuration error"), sprintf(_("Missing '%s' directive in configuration!"), "notify_command"), ERROR_DIALOG);
          } else {
            $parameters= base64_encode($this->nmessage) ." ";
            foreach ($this->recipients as $key => $value){
              $parameters.= "$key ";
            }
            exec ("$cmd $parameters", $dummy, $retval);
            if ($retval != 0){
              msg_dialog::display(_("Configuration error"), sprintf(_("'%s' defined for the '%s' directive cannot be executed!"), $cmd, "notify_command"), ERROR_DIALOG);
            }
            $this->finalized= true;
          }
        }
      } else {
	msg_dialog::display(_("Error"), _("Please specify at least one recipient to send a message!") , ERROR_DIALOG);
      }
    }

    /* Bounce back to the original dialog */
    if (isset($_POST['continue'])){
      $this->finalized= false;
    }

    /* Add to list? */
    if (isset($_POST['add']) && isset($_POST['source']) && $this->acl_is_writeable("notify")){
      foreach ($_POST['source'] as $key){
        if ($this->target == 'user'){
          if(isset($this->users[$key])){
            $this->recipients[$key]= $this->users[$key];
          }
        }
        if ($this->target == 'group'){
          if(isset($this->groups[$key])){
            $this->recipients[$key]= $this->groups[$key];
          }
        }
      }
      ksort($this->recipients);
    }

    /* Remove from list? */
    if (isset($_POST['del']) && isset($_POST['recipient'])){
      foreach ($_POST['recipient'] as $key){
          unset($this->recipients[$key]);
      }
    }

    /* Import message? */
    if (isset($_POST["import_template"]) && isset($this->templates[$_POST["nmessage_template"]])){
      $contents= "";
      $lines= file($_POST["nmessage_template"]);
      foreach ($lines as $line){
        if (!preg_match('/^DESC:/', $line)){
          $contents.= $line;
        }
      }

      /* Replace attributes */
      $ui= get_userinfo();
      $contents= preg_replace('/%self/', $ui->cn, $contents);

      /* Save message */
      $this->nmessage= htmlspecialchars($contents);
    }

    $smarty= get_smarty();

    /* Assign possible target types */
    $smarty->assign("targets", $this->targets);
    foreach ($this->attributes as $attr){
      $smarty->assign($attr, $this->$attr);
    }

    /* Generate list */
    $tmp= array();
    foreach (array("user" => "users", "group" => "groups") as $field => $arr){
      if ($this->target == $field){
        foreach ($this->$arr as $key => $value){
          if (!isset($this->recipients[$key])){
            $tmp[$key]= $value;
          }
        }
      }
    }
    $smarty->assign('sources', $tmp);
    $smarty->assign('recipients', $this->recipients);

    /* Assign ACL */
    $smarty->assign('nmessageACL', $this->getacl("notify"));

    /* Handle templates */
    $smarty->assign('show_templates', $this->show_templates?"true":"false");
    $smarty->assign('message_templates', $this->templates);
    $smarty->assign('template', $this->template);
    $smarty->assign('finished', $this->finalized?"true":"false");

    /* Show main page */
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }


  function parse_notification($file)
  {
    $desc= "";

    if (is_file($file)){
      $fh = fopen($file, "r");
      $line= fgets($fh, 256);

      if (!preg_match('/^DESC:/', $line)){
	msg_dialog::display(_("Error"), sprintf(_("Cannot find a DESC tag in file '%s'!"), $file), ERROR_DIALOG);
        return $desc;
      }
      fclose ($fh);

      $desc= trim(preg_replace('/^DESC:\s*/', '', $line));
    }

    return $desc;
  }

  
  function save_object()
  {
    plugin::save_object();
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }
  }

  
  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
        "plShortName"   => _("Notification"),
        "plDescription" => _("Notification plugin"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 89,
        "plSection"     => array("addon"),
        "plCategory"    => array("msgplug" => array("objectClass" => "none", "description" => _("Notification plugin"))),

        "plProvidedAcls" => array(
            "notify"          => _("Allow sending notifications")
          )
        ));
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
