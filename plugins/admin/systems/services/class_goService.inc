<?php

class goService extends plugin{
	
  var $cli_summary      = "This plugin is used within the ServerService plugin.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array();
  var $attributes       = array();
  var $StatusFlag       = "";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts            = array();
  var $dn                   = NULL;
  var $cn                   = "";
  var $DisplayName          = "";
  var $view_logged  =FALSE;

   
  /* Construcktion */ 
  function goService(&$config,$dn)
  {
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Empty service");
  }

  
  /* Create content */
  function execute()
  {
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    $str ="<div style='width:100%; text-align:right;'>".
          "  <input type='submit' name='SaveService' value='"._("Save")."'>&nbsp;".
          "  <input type='submit' name='CancelService' value='"._("Cancel")."'>".
          "</div>";
    return($str);
  }


  /* Get service information for serverService plugin */
  function getListEntry()
  {
    
    $this->updateStatusState();

    /* Assign status flag */
    if(!empty($this->StatusFlag)){
      $flag                   = $this->StatusFlag;
      $fields['Status']       = $this->$flag;
    }else{
      $fields['Status']       = "";
    }

    /* Name displayed in service overview */
    $fields['Message']      = _("Empty service");

    /* Allow/disallow some functions */
    $fields['AllowStart']   = $this->acl_is_writeable("start");
    $fields['AllowStop']    = $this->acl_is_writeable("stop");
    $fields['AllowRestart'] = $this->acl_is_writeable("restart");
    $fields['AllowRemove']  = $this->acl_is_removeable();
    $fields['AllowEdit']    = true;
    return($fields);
  }


  /* Remove service */
  function remove_from_parent()
  {
    if(!$this->initially_was_account || !$this->acl_is_removeable()){
      return;
    }
    
    plugin::remove_from_parent();

    /* Remove status flag, it is not a memeber of 
        this->attributes, so ensure that it is deleted too */
    if(!empty($this->StatusFlag)){
      $this->attrs[$this->StatusFlag] = array();
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

    new log("remove","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of server services/".get_class($this)." - (".$this->DisplayName.") with dn '%s' failed."),$this->dn));
    $this->handle_post_events("remove");
  }


  /* Save service */
  function save()
  {
    plugin::save();
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
    if($this->initially_was_account){
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("modify");
    }else{
      $this->handle_post_events("add");
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of server services/".get_class($this)." - (".$this->DisplayName.") with dn '%s' failed."),$this->dn));
  }


  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;

    /* Can't set status flag for new services (Object doesn't exists in ldap tree) */
    if(!$this->initially_was_account) return;

    /* Can't set status flag, if no flag is specified  */
    if(empty($this->StatusFlag)){
      return;
    }

    /* Get object (server), update status flag and save changes */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for server services/".get_class($this)." - (".$this->DisplayName.") with dn '%s' failed."),$this->dn));
      $this->action_hook();
    }
  }


  function check()
  { 
    $message = plugin::check();
    return($message);
  }
  

  function save_object()
  {
    plugin::save_object();
  }  

  
  function action_hook($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= $this->config->search(get_class($this), "ACTION_HOOK",array('menu','tabs'));
    if ($command != ""){

      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);

      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      /* If there are still some %.. in our command, try to fill these with some other class vars */
      if(preg_match("/%/",$command)){
        $attrs = get_object_vars($this);
        foreach($attrs as $name => $value){
          if(!is_string($value)) continue;
          $command= preg_replace("/%$name/", $value, $command);
        }
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as ACTION_HOOK for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }


  /* Get updates for status flag */
  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
