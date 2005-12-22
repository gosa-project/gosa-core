<?php

class glpiAccount extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  var $initialy_was_account = false;

  var $data;
  var $handle = NULL;

  /* Contructor 
     Sets default values and checks if we already hae an existing glpi account
  */
  function glpiAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    if(!isset($this->config->data['SERVERS']['GLPI'])){
      return;
    }
    
    $this->data = $this->config->data['SERVERS']['GLPI'];
    
    if(!is_callable("mysql_connect")){
      return;
    }
    
    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
    
    if(!$this->handle->is_connected){
      return;
    } 

    if($this->handle->is_account($this->dn)){
      $this->is_account = true;
    }else{
      $this->is_account = false;
    }

    $this->initialy_was_account = $this->is_account;
  }

  function execute()
  {
  	/* Call parent execute */
	  plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Assign smarty defaults */ 
    foreach(array("SystemTypes","Manufacturers","OSs","TechnicalResponsibles","InstalledDevices","Attachments") as $attr){
      $smarty->assign($attr,array());
      $smarty->assign($attr."ACL"," disabled ");
    }

    foreach(array("SystemType","Manufacturer","OS","TechnicalResponsible") as $attr){
      $smarty->assign($attr,"");
      $smarty->assign($attr."ACL"," disabled ");
    }

    /* Perform some checks
     *  arround the database
     */ 
    if(!isset($this->config->data['SERVERS']['GLPI'])){
      print_red(_("There is no valid configuration for glpi database available."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    }
    $this->data = $this->config->data['SERVERS']['GLPI'];

    if(!is_callable("mysql_connect")){
      print_red(_("The php mysql extension is missing. Can't perform any queries."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    }

    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);

    if(!$this->handle->is_connected){
      print_red(_("Can't copnnect to specified database."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    }

    /* If we reach this this 
     * the database connection seams to ok
     */

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove DNS service"),
          _("This server has DNS features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DNS service"),
          _("This server has DNS features disabled. You can enable them by clicking below."));
      return ($display);
    }


    foreach(array("SystemType","Manufacturer","OS","TechnicalResponsible","InstalledDevices","Attachments") as $attr){
      $smarty->assign($attr,"");
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    

    $smarty->assign("SystemTypes",$this->handle->getSystemTypes());
    $smarty->assign("SystemType","os");

    $smarty->assign("OSs",$this->handle->getOSTypes());
    $smarty->assign("OS","is");

    $smarty->assign("Manufacturers",$this->handle->getEnterprisesTypes());
    $smarty->assign("Manufacturer","still");

    $smarty->assign("TechnicalResponsibles",array("os","is","still","missing"));
    $smarty->assign("TechnicalResponsible","missing");


    $display.= $smarty->fetch(get_template_path('glpi.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
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
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
