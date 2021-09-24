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
  var $plDescription= "Manage connectivity user settings";
  var $plIcon = "plugins/connectivity/images/plugin.png";
  var $matIcon = "cable";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $ignore_account= TRUE;
  var $plugin= array();
  var $plugin_name= array();
  var $CopyPasteVars = array("plugin","plugin_name");

  var $multiple_support = TRUE;

  function __construct(&$config, $dn= NULL,$parent =NULL)
  {
    /* Preseed permissions */
    $this->initTime = microtime(TRUE);
    $this->dn= $dn;
    $ui= get_userinfo();

    $this->config = $config;
  
    /* Load accounts */
    foreach ($config->data['TABS']['CONNECTIVITY'] as $plug){
      if (!class_available($plug['CLASS']) || !plugin_available($plug['CLASS'])) {
      	continue;
      }

      $name= $plug['CLASS'];
      $this->plugin_name[]= $name;
      $this->plugin[$name]= new $name($config, $dn,$parent);

      /* Acl base && category configuration, 
          these settings will be overloaded in main.inc, 
          if we are editing ourself */
      $this->plugin[$name]-> set_acl_category("users");
      $this->plugin[$name]-> set_acl_base($this->dn);
    }

    // Create statistic table entry
    stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
            $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    $display= "";

    /* Prepare templating */
    $smarty= get_smarty();

    $theme = getThemeName();

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
        $display= "<img alt=\"\" src=\"images/small-error.png\" align=\"middle\">&nbsp;<b>".
          msgPool::noValidExtension()."</b>";
        $display.= back_to_main();
        return ($display);
      }
    }

    if ($this->parent !== NULL){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->parent= $this->parent;
      }
    }

    /* Execude  objects */
    $is_first= true;

    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->read_only = &$this->read_only;
      if (!$is_first){
        switch ($theme) {
          case 'classic':
            $display.= "<hr>";
            break;
          
          default:
            $display.= "<hr class='divider'>";
            break;
        }
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
      if($this->plugin[$name]->is_account){
        $tmp= $this->plugin[$name]->check();
        $message= array_merge($message, $tmp);
      }
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
    // Append parent to sub-plugins.
    if ($this->parent !== NULL){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->parent= $this->parent;
      }
    }

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
    // Append parent to sub-plugins.
    if ($this->parent !== NULL){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->parent= $this->parent;
      }
    }

    /* Remove objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      $this->plugin[$name]->remove_from_parent();
    }
  }

  function adapt_from_template($dn, $skip= array())
  {
    /* Adapt objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      $this->plugin[$name]->adapt_from_template($dn, $skip);
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


  function enable_multiple_support()
  {
    plugin::enable_multiple_support();
    
    foreach($this->plugin_name as $key => $name){
      if($this->plugin[$name]->multiple_support){
        $this->plugin[$name]->enable_multiple_support();  
      }else{
        unset($this->plugin_name[$key]);
        unset($this->plugin[$name]);
      }
    }
  }
  
  
  function multiple_execute()
  {
    return($this->execute());
  }
  
  /* Save data to object */
  function multiple_save_object()
  {
    if (isset($_POST['connectivityTab'])){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->multiple_save_object();
      }
    }
  }

  function multiple_check()
  {
    $message = plugin::multiple_check();
    foreach ($this->plugin_name as $name){
      $message = array_merge($message,$this->plugin[$name]->multiple_check());
    }
    return($message);
  }

  function get_multi_init_values()
  {
    $ret = array();
    foreach($this->plugin as $name => $plugin){
      $ret = array_merge($ret,$plugin->get_multi_init_values());
    }
    return($ret);
  }

  function init_multiple_support($attrs,$attr)
  {
    foreach($this->plugin as $name => $plugin){
      $this->plugin[$name]->init_multiple_support($attrs,$attr);
    }
  }

  function get_multi_edit_values()
  {
    $ret['plugin'] = &$this->plugin;
    return($ret);
  }

  function set_multi_edit_values($values)
  {
    foreach($values['plugin'] as $name => $plugin){
      $this->plugin[$name]->set_multi_edit_values($plugin->get_multi_edit_values());
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Connectivity"),
          "plDepends"       => array("user"),
          "plPriority"      => 20,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plDescription"       => _("Connectivity add-on"),
          "plSelfModify"        => TRUE,

          "plProvidedAcls"  => array()
          ));
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
