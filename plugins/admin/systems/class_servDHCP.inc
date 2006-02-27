<?php

class servdhcp extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  var $dialog ;

  function servdhcp ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->dialog = new baseSelectDialog($config);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $this->dialog->save_object(); 
    if($this->dialog->isSelected()){
      $base = $this->dialog->isSelected();
    }  
    if($this->dialog->isClosed()){
      echo "blub";
    }
    echo $base;
    return($this->dialog->execute());

    $display.= $smarty->fetch(get_template_path('servdhcp.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Optionally execute a command after we're done */
    #$this->handle_post_events($mode);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
