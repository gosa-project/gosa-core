<?php
/*! \brief   connectivity plugin
  \author  Cajus Pollmeier <pollmeier@gonicus.de>
  \version 2.30
  \date    29.03.2005

  This class provides the functionality to read and write all attributes
  relevant for connectivity settings from/to the LDAP. It does syntax checking
  and displays the formulars required.
 */

class connectivity extends plugin
{
  /* Definitions */
  var $plHeadline= "Connectivity";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $ignore_account= TRUE;
  var $plugin= array();
  var $plugin_name= array();
  var $CopyPasteVars = array("plugin","plugin_name");


  function connectivity (&$config, $dn= NULL,$parent =NULL)
  {
    /* Preseed permissions */
    $this->dn= $dn;
    $ui= get_userinfo();

    /* Load accounts */
    foreach ($config->data['TABS']['CONNECTIVITY'] as $plug){
      $name= $plug['CLASS'];
      $this->plugin_name[]= $name;
      $this->plugin[$name]= new $name($config, $dn,$parent);

      /* Acl base && category configuration, 
          these settings will be overloaded in main.inc, 
          if we are editing ourself */
      $this->plugin[$name]-> set_acl_category("users");
      $this->plugin[$name]-> set_acl_base($this->dn);
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    $display= "";

    /* Prepare templating */
    $smarty= get_smarty();

    /* Do we represent a valid account? */
    if ($this->parent === NULL){
      $enabled= true;
      foreach ($this->plugin_name as $name){
        if ($this->plugin[$name]->is_account){
          $enabled= true;
          break;
        }
      }
      if (!$enabled){
        $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
          _("This account has no connectivity extensions.")."</b>";
        $display.= back_to_main();
        return ($display);
      }
    }

    /* Remove checkboxes in single edit mode */
    if ($this->parent !== NULL){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->parent= $this->parent;
      }
    }

    /* Execude  objects */
    $is_first= true;

    $ReadOnly = (!isset($this->parent) || !$this->parent) && !isset($_SESSION['edit']);

    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->ReadOnly = $ReadOnly;
      if (!$is_first){
        $display.= '<p class="seperator">&nbsp;</p>';
      } else {
        $is_first= false;
      }
      $display.= $this->plugin[$name]->execute();
    }

    /* Mark me as connectivity tab */
    $display.= "<input type='hidden' name='connectivityTab'>";

    return($display);
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['connectivityTab'])){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->save_object();
      }
    }
  }

  function check()
  {
    $message= plugin::check();

    foreach ($this->plugin_name as $name){
      $tmp= $this->plugin[$name]->check();

      $message= array_merge($message, $tmp);
    }

    return ($message);
  }

  function set_acl_category($cat)
  {
    plugin::set_acl_category($cat);
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->set_acl_category( $cat);
    }
  }

  function set_acl_base($base)
  {
    plugin::set_acl_base($base);
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->set_acl_base( $base);
    }
  }

  /* Save to LDAP */
  function save()
  {
    /* Save objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      
      if ($this->plugin[$name]->is_account){
        $this->plugin[$name]->save();
      } else {
        $this->plugin[$name]->remove_from_parent();
      }
    }
  }

  function remove_from_parent()
  {
    /* Remove objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      $this->plugin[$name]->remove_from_parent();
    }
  }

  function adapt_from_template($dn)
  {
    /* Adapt objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      $this->plugin[$name]->adapt_from_template($dn);
    }
  }

  /* Prepare the connectivity obj 
   */
  function PrepareForCopyPaste($obj)
  { 
    $tmp = $this->plugin;
    plugin::PrepareForCopyPaste($obj);
    $this->plugin = $tmp;
    foreach( $this->plugin as $key => $plug){
      $this->plugin[$key]->PrepareForCopyPaste($obj);
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
