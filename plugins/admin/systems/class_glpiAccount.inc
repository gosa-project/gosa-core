<?php

class glpiAccount extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("ID","name","contact",
      "tech_num","comments","date_mod","os","location","domain","network",
      "model","type","is_template","FK_glpi_enterprise","deleted");

  var $ID                 ;
  var $name               ="";
  var $contact            ="";

  var $tech_num           ="";
  var $comments           ="";

  var $date_mod           ="";
  var $os                 =0;
  var $location           =0;
  var $domain             =0;
  var $network            =0; 

  var $model              =0;
  var $type               =0;
  var $is_template        =0;
  var $FK_glpi_enterprise =0;
  var $deleted            ="N";

  var $objectclasses= array("whatever");

  var $initialy_was_account = false;

  var $edit_type    =false;
  var $edit_os    =false;

  var $data;
  var $handle = NULL;

  var $cur_dialog = NULL;

  var $orig_dn;
  var $ui;

  /* Contructor 
     Sets default values and checks if we already have an existing glpi account
   */
  function glpiAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->ui= get_userinfo();

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
      $tmp = ($this->handle->getComputerInformations($this->dn));

      foreach(array("tech_num","os","FK_glpi_enterprise","type","comments") as $attr){
        $this->$attr = $tmp[0][$attr];
      }

    }else{
      $this->is_account = false;
    }

    $this->name = $this->dn;
    $this->orig_dn = $this->dn;
    $this->initialy_was_account = $this->is_account;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    // #fixme
    // Check if mysql a.s.o. is available.


    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
    $users = ($this->handle->getUsers());

    $ldap= $this->config->get_ldap_link();

    if(isset($_POST['AddDevice'])){
      $this->dialog =true;
      $this->cur_dialog = new glpiDeviceManagement($this->config,$this->dn);
    }

    /* We have selected some devices and pressed use button */
    if(isset($_POST['SelectDeviceSave'])){
      $this->cur_dialog->save_object();
      print_a($this->cur_dialog->getSelected());
      $this->cur_dialog = false;
      $this->dialog = false;
      $this->edit_type=false;
    }

    /* Aborted Device selction */
    if(isset($_POST['SelectDeviceCancel'])){
      $this->dialog = false;
      $this->cur_dialog = false;
      $this->edit_type=false;
    }

    /* System type management
     */
    if(isset($_POST['edit_type'])){
      $this->dialog = true;
      $this->edit_type=true;
    }

    if(isset($_POST['close_edit_type'])){
      $this->edit_type=false;
      $this->dialog = false;
    }

    if((isset($_POST['add_type']))&&(!empty($_POST['type_string']))){
      $this->handle->addSystemType($_POST['type_string']);  
    }

    if((isset($_POST['del_type']))&&(!empty($_POST['select_type']))){
      $this->handle->removeSystemType_byID($_POST['select_type']);  
    }

    if((isset($_POST['rename_type']))&&(!empty($_POST['select_type']))&&(!empty($_POST['type_string']))){
      $this->handle->updateSystemType($_POST['type_string'],$_POST['select_type']);
    }

    if($this->edit_type){
      $smarty->assign("SystemTypes",            $this->handle->getSystemTypes());
      $smarty->assign("SystemTypeKeys",         array_flip($this->handle->getSystemTypes()));

      $display= $smarty->fetch(get_template_path('glpi_edit_type.tpl', TRUE));
      return($display);
    }

    /*  ENDE, Systemtype management.
     */

    /* System os management
     */
    if(isset($_POST['edit_os'])){
      $this->dialog = true;
      $this->edit_os=true;
    }

    if(isset($_POST['close_edit_os'])){
      $this->edit_os=false;
      $this->dialog = false;
    }

    if((isset($_POST['add_os']))&&(!empty($_POST['is_string']))){
      $this->handle->addOS($_POST['is_string']);  
    }

    if((isset($_POST['del_os']))&&(!empty($_POST['select_os']))){
      $this->handle->removeOS_byID($_POST['select_os']);  
    }

    if((isset($_POST['rename_os']))&&(!empty($_POST['select_os']))&&(!empty($_POST['is_string']))){
      $this->handle->updateOS($_POST['is_string'],$_POST['select_os']);
    }

    if($this->edit_os){
      $smarty->assign("OSKeys",                 array_flip($this->handle->getOSTypes()));
      $smarty->assign("OSs",                    $this->handle->getOSTypes());

      $display= $smarty->fetch(get_template_path('glpi_edit_os.tpl', TRUE));
      return($display);
    }

    /*  ENDE, os management.
     */

    if(isset($_POST['edit_manufacturer'])){
      $this->cur_dialog = new glpiManufacturer($this->config,$this->dn);
      $this->dialog = true;
    }

    if(isset($_POST['close_edit_manufacturer'])){
      $this->dialog = false;
      $this->cur_dialog = false;
    }

    if(isset($_POST['SelectUserCancel'])){
      $this->dialog = false;
      $this->cur_dialog = false;
    }

    if(isset($_POST['SelectTechPerson'])){
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn);
    }

    /* Technical responsible person selected*/
    if(isset($_GET['act'])&&($_GET['act']=="user_tech_num")){

      /* Get posted id */
      $id = base64_decode($_GET['id']);

      /* Check if user is already created in glpi database */
      if(!in_array($id,$users)){

        /* If this user doesn't exists in glpi db, we must create him */
        $atr = $ldap->fetch($ldap->cat($id));
        $tmp = array();
        $use = array( "cn"              =>"name",
            "mail"            =>"email",
            "telephoneNumber" =>"phone");

        /* Create array */
        foreach($use as $gosa => $glpi){
          if(isset($atr[$gosa])){
            $tmp[$glpi]= $atr[$gosa][0];
          }
        }

        /* Add this user */
        $this->handle->addUser($tmp,$id);
      }

      /* Re-read users */
      $users = ($this->handle->getUsers());

      /* Get user */
      $tmp = array_flip($users);
      $id=$tmp[$id];

      /* Use user id, close dialog */
      $this->tech_num = $id;
      $this->cur_dialog   = false;
      $this->dialog= false;
    }

    if($this->cur_dialog){
      $this->cur_dialog->save_object();
      $this->dialog=true;
      $this->cur_dialog->parent = &$this;
      return($this->cur_dialog->execute());
    }

    /* Assign smarty defaults */ 
    foreach(array("SystemTypes","SystemTypeKeys","Manufacturers","OSs","TechnicalResponsibles","InstalledDevices","Attachments") as $attr){
      $smarty->assign($attr,array());
      $smarty->assign($attr."ACL"," disabled ");
    }

    foreach(array("type","FK_glpi_enterprise","os","tech_num") as $attr){
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
      print_red(_("Can't connect to specified database."));
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
      $display= $this->show_header(_("Remove inventory service"),
          _("This server has inventory features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add inventory service"),
          _("This server has inventory features disabled. You can enable them by clicking below."));
      return ($display);
    }


    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }


    $smarty->assign("SystemTypeKeys",         array_flip($this->handle->getSystemTypes()));
    $smarty->assign("SystemTypes",            $this->handle->getSystemTypes());
    $smarty->assign("type",                   $this->type);

    $smarty->assign("OSKeys",                 array_flip($this->handle->getOSTypes()));
    $smarty->assign("OSs",                    $this->handle->getOSTypes());
    $smarty->assign("os",                     $this->os);

    $smarty->assign("ManufacturerKeys",       array_flip($this->handle->getEnterprisesTypes()));
    $smarty->assign("Manufacturers",          $this->handle->getEnterprisesTypes());
    $smarty->assign("FK_glpi_enterprise",     $this->FK_glpi_enterprise);

    if(isset($users[$this->tech_num])){
      $tr = $ldap->fetch($ldap->cat($users[$this->tech_num]));
      $str = "";
      if(isset($tr['givenName'][0])){
        $str .= $tr['givenName'][0]." ";
      }

      if(isset($tr['sn'][0])){
        $str .= $tr['sn'][0]." ";
      }

      if(isset($tr['uid'][0])){
        $str .= "[".$tr['uid'][0]."]";
      }


      $smarty->assign("tech_num",               $str);
    }else{
      $smarty->assign("tech_num",               _("N/A"));
    }
    $smarty->assign("comments",               $this->comments);

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
    foreach($this->attributes as $attrs){
      if(isset($_POST[$attrs])){
        $this->$attrs = $_POST[$attrs];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    //    if($this->TechnicalResponsible == ""){
    //      $message[] = _("Please select a technical responsible person for this entry.");
    //    }

    return ($message);
  }

  /* Save to LDAP */
  function save()
  {
    $attrs = array();
    $this->date_mod = date("Y-m-d H:i:s");
    foreach($this->attributes as $attr){
      $attrs[$attr] = $this->$attr;
    }
    $attrs['name'] = $this->dn;
    unset($attrs['ID']);
    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
    if($this->initialy_was_account&&$this->is_account){
      $this->handle->updateComputerInformations($attrs,$this->dn);
    }elseif($this->is_account){
      $this->handle->addComputerInformations($attrs,$this->dn);
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
