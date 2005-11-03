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

  function connectivity ($config, $dn= NULL)
  {
    /* Preseed permissions */
    $this->dn= $dn;
    $ui= get_userinfo();
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);

    /* Load accounts */
    foreach ($config->data['TABS']['CONNECTIVITY'] as $plug){
      $name= $plug['CLASS'];
      $this->plugin_name[]= $name;
      $this->plugin[$name]= new $name($config, $dn);
      $this->plugin[$name]->acl= get_module_permission($acl, "$name", $ui->dn);
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
    if ($this->parent == NULL){
      $enabled= false;
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
    if ($this->parent != NULL){
      foreach ($this->plugin_name as $name){
        $this->plugin[$name]->parent= $this->parent;
      }
    }

    /* Execude  objects */
    $is_first= true;
    foreach ($this->plugin_name as $name){
      if (!$this->plugin[$name]->is_account && $this->parent == NULL) continue;
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
    $message= array();

    foreach ($this->plugin_name as $name){
      $tmp= $this->plugin[$name]->check();

      $message= array_merge($message, $tmp);
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Save objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      if ($this->plugin[$name]->is_account){
        $this->plugin[$name]->save();
        $this->plugin[$name]->postcreate();
      } else {
        $this->plugin[$name]->remove_from_parent();
        $this->plugin[$name]->postremove();
      }
    }
  }

  function remove_from_parent()
  {
    /* Remove objects */
    foreach ($this->plugin_name as $name){
      $this->plugin[$name]->dn= $this->dn;
      $this->plugin[$name]->remove_from_parent();
      $this->plugin[$name]->postremove();
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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
