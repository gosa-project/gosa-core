<?php

class glpiPrinterAccount extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= FALSE;
  var $attributes= array("ID","name","contact","ramSize","flags_serial","flags_par","flags_usb",
      "tech_num","comments","date_mod","location","domain","network","contact_num","serial","otherserial",
      "type","is_template","FK_glpi_enterprise","deleted");

  var $ID                 ;       // Is set if this entry is edited 
  var $name               = "";    // This should be the dn of this entry 
  var $FK_glpi_enterprise = 0;     // Manufacturer id
  var $tech_num           = "";    // Technical responsible person
  var $contact_num        = "";    // Contact person
  
  var $comments           = "";    // Comment
  
  var $type               = 0;     // printer type id
  var $serial             = "";   
  var $otherserial        = "";  
  var $ramSize            = 0;
  var $flags_serial       = false;
  var $flags_par          = false;
  var $flags_usb          = false;

  var $date_mod           = "";    // Modification timestamp
  
  var $location           = 0;     // Not used yet
  var $domain             = 0;     // ? Set to 0
  var $network            = 0;     // ? Set to 0 

  var $is_template        = 0;     // Used as template ?
  var $contact            = "";    // Empty
  var $deleted            = "N";   // Deleted entries should have this set to Y

  var $rename             = false;
  var $select_type        ;

  var $editManufacturer   = false;

  /* Not necessary, cause we use mysql databse */
  var $objectclasses= array("whatever");

  /* Used to remember if this was an account (simply: is this an edited entry) */
  var $initially_was_account = false;

  /* Remember current dialog */
  var $edit_type            = false;
  var $edit_os              = false;

  var $data;
  var $handle = NULL;               // Glpi class handle used to query database

  var $cur_dialog = NULL;           // This contains the sub dialog handle

  var $orig_dn;                     // To check if dn, has changed 
  var $ui;                          // Some GOsa specific user informations 
  
  var $usedDevices      = array();  // Which devices are currently selected 
  var $usedAttachments  = array();  // Used Attachments 
  var $usedCartridges   = array();  // Used Cartridges

  var $view_logged      = FALSE;

  /* Contructor 
     Sets default values and checks if we already have an existing glpi account
   */
  function glpiPrinterAccount (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    $this->ui= get_userinfo();

    $this->is_account = false;

    /* Abort class construction, if no db is defined */
    if(!isset($this->config->data['SERVERS']['GLPI'])){
      return;
    }

    // Get informations about databse connection
    $this->data = $this->config->data['SERVERS']['GLPI'];

    // Abort if mysql extension is missing 
    if(!is_callable("mysql_connect")){
      return;
    }

    // Create handle of glpi class, and check if database connection is established 
    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);

    if(!$this->handle->is_connected){
      return;
    } 

    // If this dn is already used in database, then get all informations for this entry 
    if($this->handle->is_printer_account($this->dn)){
      $this->is_account = true;
      $tmp = ($this->handle->getPrinterInformations($this->dn));

      foreach(array("tech_num","FK_glpi_enterprise","type","comments","contact_num","flags_serial","flags_par","flags_usb","ramSize") as $attr){
        $this->$attr = $tmp[0][$attr];
      }
      
      $atts = $this->handle->getAssignPrinterAttachments($tmp[0]['ID']);
      foreach($atts as $attachment){
        $this->usedAttachments[$attachment['FK_doc']]=$attachment['FK_doc']; 
      }

      $cart= $this->handle->getUsedCartridges($tmp[0]['ID']);
      foreach($cart as $key => $cartridge){
        $this->usedCartridges[$key]=$cartridge; 
        $this->usedCartridges[$key]['status']="exists"; 
      }


    }else{
      $this->is_account = false;
    }

    /* set defaults */
    $this->name                 = $this->dn;
    $this->orig_dn              = $this->dn;
    $this->initially_was_account = $this->is_account;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","printer/".get_class($this),$this->dn);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $smarty->assign("CartridgesACL",$this->getacl("Cartridges"));

    /*  Assign smarty defaults 
        To avoid undefined indexes, if there is an error with the glpi db
     */ 
    foreach(array("PrinterTypeKeys","PrinterTypes","ManufacturerKeys","Manufacturers",
                  "Attachments","AttachmentKeys","CartridgeKeys","Cartridges") as $attr){
      $smarty->assign($attr,array());
    }
    foreach(array("type","FK_glpi_enterprise","tech_num","contact_num","comments","flags_serial","flags_par","flags_usb","AttachmentsDiv") as $attr){
      $smarty->assign($attr,"");
    }

    /* Assign acls */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Check if there is a glpi database server defined 
      */
    if(!isset($this->config->data['SERVERS']['GLPI'])){
      print_red(_("There is no server with valid glpi database service."));
      return($smarty->fetch(get_template_path('glpiPrinter.tpl', TRUE)));
    }

    $this->data = $this->config->data['SERVERS']['GLPI'];

    /* Check if we can call mysql_connect 
       If we can't, there is no the mysql-php extension
     */
    if(!is_callable("mysql_connect")){
      print_red(_("Can't connect to glpi database, the php-mysql extension is missing."));
      return($smarty->fetch(get_template_path('glpiPrinter.tpl', TRUE)));
    }

    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);

    /*  If handle == false, abort
        Seems that the server, username and or password is wrong
     */
    if(!$this->handle->is_connected){
      print_red(_("Can't connect to glpi database, check configuration twice."));
      return($smarty->fetch(get_template_path('glpiPrinter.tpl', TRUE)));
    } 

    /*  All checks are ok
        Lets handle Posts, templates etc below ...
     */
    
    $users = $this->handle->getUsers();
    $ldap= $this->config->get_ldap_link();

    
    /*  ##########################################################################
     *  Some tab management   
     */

    /* Do we need to flip is_account state? */
    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_disable_header(_("Remove inventory"),
          _("This server has inventory features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_enable_header(_("Add inventory"),
          _("This server has inventory features disabled. You can enable them by clicking below."));
      return ($display);
    }

    
    /*  ##########################################################################
     *  Printer type management 
     *  Dialog 
     */

    /* Rename was requested */
    if(isset($_POST['Rename_PType_OK']) && $this->acl_is_writeable("type")){
      $tmp = $this->handle->getPrinterTypes();
      $allok = true;
      foreach($tmp as $id => $name){
        if(trim($name) == trim($_POST['string'])){
          $allok = false;
        }
      }
      if($allok){
        $this->handle->updatePrinterType($_POST['string'],$this->select_type);
        $this->rename = false;
      }else{
        print_red(sprintf(_("Can't rename given printer type to '%s', because this type name already exists."),$_POST['string']));
      }
    }

    /* abort rename 
     */
    if(isset($_POST['Rename_Cancel'])){
      $this->rename = false;
    }

    /* Printer type management
     */
    if(isset($_POST['edit_type']) && $this->acl_is_writeable("type")){
      $this->dialog = true;
      $this->edit_type=true;
    }

    /* This closes the printer type editing dialog
     */
    if(isset($_POST['close_edit_type'])){
      $this->edit_type=false;
      $this->dialog = false;
    }

    /* This appends a new printer to our sytem types
     */
    if((isset($_POST['add_type']))&&(!empty($_POST['type_string'])) && $this->acl_is_writeable("type")){
    
      $tmp = $this->handle->getPrinterTypes();
      $allok = true;
      foreach($tmp as $id => $name){
        if(trim($name) == trim($_POST['type_string'])){
          $allok = false;
        }
      }
      if($allok){
        $this->handle->addPrinterType($_POST['type_string']);  
      }else{
        print_red(sprintf(_("Can't rename given printer type to '%s', because this type name already exists."),$_POST['type_string']));
      }
    }

    /* Remove selected type from our printer types list
     */
    if((isset($_POST['del_type']))&&(!empty($_POST['select_type'])) && $this->acl_is_writeable("type")){
      $tmp = $this->handle->is_printerTypeUsed($_POST['select_type']);
      if(count($tmp)){
        $str = "";
        foreach($tmp as $id => $name){
          $str .= $name.", ";
        }
        $str = preg_replace("/, $/","",$str); 
        print_red(sprintf(_("Can't delete printer type, it is still in use by '%s'."),$str));
      }else{
        $this->handle->removePrinterType($_POST['select_type']);  
      }
    }

    /* Rename selected printer type to given string
     */
    if((isset($_POST['rename_type']))&&(!empty($_POST['select_type']))||($this->rename) && $this->acl_is_writeable("type")){
      $this->rename = true;

      $smarty->assign("Method","rename");

      $tmp = $this->handle->getPrinterTypes();

      if(isset($_POST['select_type'])){
        $this->select_type = $_POST['select_type'];
      }  
      $smarty->assign("string",$tmp[$this->select_type]);
      if(isset($_POST['string'])){
        $smarty->assign("string",$_POST['string']);
      }

      $display= $smarty->fetch(get_template_path('glpi_edit_printer_type.tpl', TRUE));
      return($display);
    }  

    /* Someone wants to edit the printer types ... 
       So, lets open a new dialog which provides some buttons to edit the types
     */
    if($this->edit_type){
      $smarty->assign("Method","edit");
      $smarty->assign("PrinterTypes",            $this->handle->getPrinterTypes());
      $smarty->assign("PrinterTypeKeys",         array_flip($this->handle->getPrinterTypes()));
      $display= $smarty->fetch(get_template_path('glpi_edit_printer_type.tpl', TRUE));
      return($display);
    }


    /*  ##########################################################################
     *  Edit manufacturers 
     *  Dialog 
     */

    /* Open dialog which allows to edit the manufacturers
     */
    if(isset($_POST['edit_manufacturer']) && $this->acl_is_writeable("FK_glpi_enterprise")){
      $this->cur_dialog = new glpiManufacturer($this->config,$this->dn);
      $this->dialog = true;
      $this->editManufacturer =true;
    }

    /* Close manufacturer editing dialog
     */
    if((isset($_POST['close_edit_manufacturer']))&&($this->editManufacturer)){
      $this->dialog = false;
      $this->cur_dialog = false;
      $this->editManufacturer=false;
    }


    /*  ##########################################################################
     *  Technical responsible person
     *  Contact person 
     *  Dialog 
     */

    /* Show dialog to select a new contact person
     * Select a contact person
     */
    if(isset($_POST['SelectContactPerson']) && $this->acl_is_writeable("contact_num")){
      $this->addUser = "contact";
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn,"user_tech_num");
    }

    /* Selecte technical responsible person
     */
    if(isset($_POST['SelectTechPerson']) && $this->acl_is_writeable("tech_num")){
      $this->addUser ="tech";
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn,"user_tech_num");
    }

    /* Abort user selection
     */
    $smarty->assign("AbortSelectUser","SelectUserCancel");
    if(isset($_POST['SelectUserCancel'])){
      $this->dialog = false;
      $this->addUser ="";
      $this->cur_dialog = false;
    }

    /* Technical responsible/contact person selected */
    if(isset($_GET['act'])&&($_GET['act']=="user_tech_num")){

      /* Get posted id */
      $id = base64_decode($_GET['id']);

      /* Check if user is already created in glpi database */
      if(!in_array($id,$users)){

        /* If this user doesn't exists in glpi db, we must create him */
        $ldap->cat($id, array('cn', 'mail', 'telephoneNumber'));
        $atr = $ldap->fetch();
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
      if($this->addUser == "tech"){
        $this->tech_num = $id;
      }else{
        $this->contact_num = $id;
      }
      $this->cur_dialog   = false;
      $this->dialog= false;
    }


    /*  ##########################################################################
     *  Handle attachments 
     */
    
    /* Attachment pool was closed with use
     */
    if(isset($_POST['UseAttachment']) && $this->acl_is_writeable("Attachments")){
      if(count($this->cur_dialog->check())){
        foreach($this->cur_dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $this->cur_dialog->save_object();
        $this->usedAttachments = $this->cur_dialog->save();
        $this->cur_dialog = false;
        $this->edit_type = false; 
      }
    }
   
    /* Attachment pool was closed with abort
     */ 
    if(isset($_POST['AbortAttachment'])){
      $this->cur_dialog = false;
      $this->edit_type = false; 
    }

    /* Open Attachment pool to add/edit Attachments
     */
    if(isset($_POST['AddAttachment']) && $this->acl_is_writeable("Attachments")){
      $this->cur_dialog = new glpiAttachmentPool($this->config,$this->dn,$this->usedAttachments);
      $this->dialog = true;
    }
    
    /* Remove Attachment from this tab 
     */
    $once = true;
    foreach($_POST as $name => $value){
      if((preg_match("/^delAttachment_/",$name))&&($once) && $this->acl_is_writeable("Attachments")){
        $once= false;
        $name = preg_replace("/^delAttachment_/","",$name);
        $entry = preg_replace("/_.*$/","",$name);
        if(isset($this->usedAttachments[$entry])){
          unset($this->usedAttachments[$entry]);
        }
      }
    }
    if((isset($_POST['RemoveAttachment']))&&(isset($_POST['Attachments'])) && $this->acl_is_writeable("Attachments")){
      if(isset($this->usedAttachments[$_POST['Attachments']])){
        unset($this->usedAttachments[$_POST['Attachments']]);
      }
    }

    /*  ##########################################################################
     *  Printer Cartridge handling 
     */

    /* Abort cartridge select dialog
     */
    if(isset($_POST['SelectCartridgeCancel'])){
      $this->cur_dialog = false;
      $this->edit_type = false; 
    }

    /* Get selected cartridges and add them to our list 
     */ 
    if(isset($_POST['SelectCartridgeSave'])){
      $this->cur_dialog->save_object();
      $carts = $this->cur_dialog->save();
      foreach($carts as $cart){
        $cart['status'] = "new";
        $this->usedCartridges[] = $cart;
      }
      $this->cur_dialog = false;
      $this->edit_type = false; 
    }

    /* Remove cartridge  
     */
    if((isset($_POST['RemoveCartridge']))&&(isset($_POST['Cartridges'])) && $this->acl_is_writeable("ManageCartridges")){

      foreach($_POST['Cartridges'] as $cartID){

        if(isset($this->usedCartridges[$cartID])){
          if($this->usedCartridges[$cartID]['status'] == "exists"){
            $this->usedCartridges[$cartID]['status'] = "deleted";
          }else{
            unset($this->usedCartridges[$cartID]);
          }
        }
      }
    }

    /* Open Attachment pool to add/edit Attachments
     */
    if(isset($_POST['AddCartridge']) && $this->acl_is_writeable("ManageCartridges")){
      $this->cur_dialog = new glpiPrinterCartridges($this->config,$this->dn,$this->type);
      $this->dialog = true;
    }


    /*  ##########################################################################
     *  Draw Dialogs
     */
    /* if( cur_dialog != false || cur_dialog != NULL) 
     * There is a dialog which wants to be displayed 
     */
    if($this->cur_dialog && is_object($this->cur_dialog)){
      $this->cur_dialog->save_object();
      $this->dialog=true;
      $this->cur_dialog->parent = &$this;
      return($this->cur_dialog->execute());
    }else{
      $this->dialog= false;
    }


    /*  ##########################################################################
     *  Assign listbox / checkbox .... values to smarty  
     */
    /* Assign smarty defaults */ 
    foreach(array("PrinterTypes","PrinterTypeKeys","Manufacturers","TechnicalResponsibles","Attachments","Cartridges") as $attr){
      $smarty->assign($attr,array());
    }

    /* Assign some vars to smarty 
     */
    foreach(array("type","FK_glpi_enterprise","tech_num","contact_num","flags_serial","flags_par","flags_usb") as $attr){
      $smarty->assign($attr,"");
    }

    $smarty->assign("comments",               $this->comments);
    $smarty->assign("flags_serial",            $this->flags_serial);
    $smarty->assign("flags_par",               $this->flags_par);
    $smarty->assign("flags_usb",               $this->flags_usb);

    /* Assign system types 
     */
    $smarty->assign("PrinterTypes",           $this->handle->getPrinterTypes());
    $smarty->assign("PrinterTypeKeys",        array_flip($this->handle->getPrinterTypes()));
    $smarty->assign("type",                   $this->type);

    /* Append manufacturers
     */
    $smarty->assign("ManufacturerKeys",       array_flip($this->handle->getEnterprises()));
    $smarty->assign("Manufacturers",          $this->handle->getEnterprises());
    $smarty->assign("FK_glpi_enterprise",     $this->FK_glpi_enterprise);

    /* Assign used Attachments
    */

    $divlist = new divSelectBox("glpiAttachmentsList");
    $divlist-> SetHeight(120);
    $atts = $this->getUsedAttachments(true);
    $downlink = "<a href='get_attachment.php?id=%s' target='_blank'>%s</a>";
    $del_link = "<input type='image' src='images/edittrash.png' name='delAttachment_%s'>";
    foreach($atts as $id => $attachment){
      $divlist->AddEntry
          (
        array(
            array("string"=>$attachment['name']),
            array("string"=>$attachment['mime']),
            array("string"=>sprintf($downlink,$id,$attachment['filename'])),
            array("string"=>sprintf($del_link,$attachment['ID']),"attach"=>"style='border-right:0px;'"),
             )
          );
    }

    $smarty->assign("AttachmentsDiv"          ,$divlist->DrawList());
    $smarty->assign("Attachments",            $this->getUsedAttachments());
    $smarty->assign("AttachmentKeys",         array_flip($this->getUsedAttachments()));

    /* Assign Cartridges 
     */
    $smarty->assign("Cartridges",            $this->getUsedCartridges());
    $smarty->assign("CartridgeKeys",         $this->getUsedCartridges(true));

    /*  ##########################################################################
     *  Assign contact and technical responsible person 
     */
    if(isset($users[$this->contact_num])){
      $ldap->cat($users[$this->contact_num], array('givenName', 'sn', 'uid'));
      $tr = $ldap->fetch();
      $str = "";
      if(isset($tr['givenName'][0])){   $str .= $tr['givenName'][0]." ";      }
      if(isset($tr['sn'][0])) {         $str .= $tr['sn'][0]." ";             }
      if(isset($tr['uid'][0])){         $str .= "[".$tr['uid'][0]."]";        }
      $smarty->assign("contact_num",               $str);
    }else{
      $smarty->assign("contact_num",               _("N/A"));
    }

    /* Handle tech person 
       Assign name ... to smarty, if set
     */ 
    if(isset($users[$this->tech_num])){
      $ldap->cat($users[$this->tech_num], array('givenName', 'sn', 'uid'));
      $tr = $ldap->fetch();
      $str = "";
      if(isset($tr['givenName'][0])){   $str .= $tr['givenName'][0]." ";      }
      if(isset($tr['sn'][0])) {         $str .= $tr['sn'][0]." ";             }
      if(isset($tr['uid'][0])){         $str .= "[".$tr['uid'][0]."]";        }
      $smarty->assign("tech_num",               $str);
    }else{
      $smarty->assign("tech_num",               _("N/A"));
    }

    /* If theres a cartridge selected, you can't change the printer type.
     */ 
    $disp = true;

    foreach($this->usedCartridges as $cart){
      if($cart['status'] != "deleted"){
        $disp = false;
      }
    }
    if($disp==false){
      $smarty->assign("typeACL", $this->getacl("type",true));
    }

    $display.= $smarty->fetch(get_template_path('glpiPrinter.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
    if($this->initially_was_account){
      $this->handle->removePrinterInformations($this->dn);
      new log("remove","printer/".get_class($this),$this->dn); 
    }
  }


  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['glpiPrinterFlagsPosted'])){
      plugin::save_object();
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }
      }

      if(isset($_POST['FK_glpi_enterprise']) && $this->acl_is_writeable("FKglpienterprise")){
        $this->FK_glpi_enterprise = $_POST['FK_glpi_enterprise'];
      }

      foreach(array("flags_serial","flags_par","flags_usb") as $checkboxes){
        if($this->acl_is_writeable($checkboxes)){
          if(isset($_POST[$checkboxes])){
            $this->$checkboxes = 1;
          }else{
            $this->$checkboxes = 0;
          }
        }
      }
    }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    //    if($this->TechnicalResponsible == ""){
    //      $message[] = _("Please select a technical responsible person for this entry.");
    //    }

    return ($message);
  }

  /* Save to LDAP */
  function save()
  {
    if($this->is_account){
        $attrs = array();
      $this->date_mod = date("Y-m-d H:i:s");
      foreach($this->attributes as $attr){
        $attrs[$attr] = $this->$attr;
      }
      $attrs['name'] = $this->dn;
      unset($attrs['ID']);
      $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
      if($this->initially_was_account&&$this->is_account){
        $this->handle->updatePrinterInformations($attrs,$this->dn);
        new log("modify","printer/".get_class($this),$this->dn); 
      }elseif($this->is_account){
        $this->handle->addPrinterInformations($attrs,$this->dn);
        new log("create","printer/".get_class($this),$this->dn); 
      }
      $tmp = $this->handle->getPrinterInformations($this->dn);
      $this->handle->addAttachmentsToPrinter($this->usedAttachments,$tmp[0]['ID']);

      foreach($this->usedCartridges as $cart){
        if($cart['status'] == "deleted"){
          $this->handle->removeCartridgeFromPrinter($cart['ID']);
        }elseif($cart['status'] == "new"){
          $this->handle->addCartridgeFromPrinter($tmp[0]['ID'],$cart['type_ID']);
        }
      }
    }
  }

  /* Return used attachments */
  function getUsedAttachments($divlist = false)
  {
    $atts =$this->handle->getAttachments();
    $ret = array();
    foreach($atts as $entry){
      if(in_array($entry['ID'],$this->usedAttachments)){
        if($divlist){
          $ret[$entry['ID']] = $entry;
        }else{
          $cm ="";
          if(isset($entry['comment'])){
            $cm=" [".$entry['comment']."]";
          }
          if(isset($entry['mime'])){
            $cm.=" -".$entry['mime']."";
          }

          $ret[$entry['ID']]= $entry['name'].$cm;
        }
      }
    }
    return($ret);
  }

  function getUsedCartridges($flip = false)
  {
    $ret = array();
    foreach($this->usedCartridges as $key => $value){
      if($value['status'] == "deleted") continue;
      if($flip){
        $ret[$key] = $key;    
      }else{
        $ret[$key] = $value['name']." [".$value['type_name']."] "._("since")." :".$value['date_use'];
      }
    }
    return($ret);
  }  

     /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Glpi"),
          "plDescription" => _("Printer inventory extension"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 10,
          "plSection"     => array("administration"),
          "plCategory"    => array("printer"),

          "plProvidedAcls"=> array(
            "flags_serial"         => _("Supports serial interface"),
            "flags_par"            => _("Supports parallel interface"),
            "flags_usb"            => _("Supports usb interface"),
            "tech_num"             => _("Technical responsible"),
            "comments"             => _("Comments"),
            "location"             => _("Location"),
            "contact_num"          => _("Contact person"),
            "type"                 => _("Type"),
            "FKglpienterprise"   => _("Manufacturer"),

            "Attachments"          => _("Attachments"),
        
            "ManageCartridges"            => _("Cartridge settings"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
