<?php

class glpiAccount extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= FALSE;
  var $attributes= array("ID","name","contact",
      "tech_num","comments","date_mod","os","location","domain","network","contact_num",
      "model","type","is_template","FK_glpi_enterprise","deleted");

  var $ID                 ;       // Is set if this entry is edited 
  var $name               = "";    // This should be the dn of this entry 
  var $contact            = "";    // Empty
    
  var $comments           = "";    // Comment
  
  var $contact_num        = "";    // Contact person
  var $tech_num           = "";    // Technical responsible person
  
  var $addUser            = "";    // This is used to remember if a dialog was opened for tech_num or contact_num 

  var $date_mod           = "";    // Modification timestamp
  var $os                 = 0;     // Operating system
  var $location           = 0;     // Not used yet
  var $domain             = 0;     // ? Set to 0
  var $network            = 0;     // ? Set to 0 

  var $model              = 0;     // ? Can't remember this, it isn't used in GOsa 
  var $type               = 0;     // System type id
  var $is_template        = 0;     // Used as template ?
  var $FK_glpi_enterprise = 0;     // Manufacturer id
  var $deleted            = "N";   // Deleted entries should have this set to Y

  var $renameTypeDialog   = false;
  var $renameOSDialog     = false;
  var $select_type        ;
  
  /* Not necessary, cause we use mysql databse */
  var $objectclasses= array("whatever");

  /* Used to remember if this was an account (simply: is this an edited entry) */
  var $initialy_was_account = false;

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

  /* Contructor 
     Sets default values and checks if we already have an existing glpi account
   */
  function glpiAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->ui= get_userinfo();

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
    if($this->handle->is_account($this->dn)){
      $this->is_account = true;
      $tmp = ($this->handle->getComputerInformations($this->dn));

      foreach(array("tech_num","os","FK_glpi_enterprise","type","comments","contact_num") as $attr){
        $this->$attr = $tmp[0][$attr];
      }
      $this->usedDevices = $this->handle->getUsedDevices($tmp[0]['ID']);
      $atts = $this->handle->getAssignAttachments($tmp[0]['ID']);
      foreach($atts as $attachment){
        
        $this->usedAttachments[$attachment['FK_doc']]=$attachment['FK_doc']; 
      }
    }else{
      $this->is_account = false;
    }

    /* set defaults */
    $this->name                 = $this->dn;
    $this->orig_dn              = $this->dn;
    $this->initialy_was_account = $this->is_account;


  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /*  Assign smarty defaults 
        To avoid undefined indexes, if there is an error with the glpi db
     */ 
    foreach(array("SystemTypes","SystemTypeKeys","Manufacturers",
                  "OSs","TechnicalResponsibles","InstalledDevices","Attachments","AttachmentKeys",
                  "OSKeys","OSs","ManufacturerKeys","InstalledDeviceKeys") as $attr){
      $smarty->assign($attr,array());
      $smarty->assign($attr."ACL"," disabled ");
    }
    foreach(array("type","FK_glpi_enterprise","os","tech_num","comments","contact_num","AttachmentsDiv") as $attr){
      $smarty->assign($attr,"");
      $smarty->assign($attr."ACL"," disabled ");
    }

    /* Check if there is a glpi database server defined 
      */
    if(!isset($this->config->data['SERVERS']['GLPI'])){
      print_red(_("There is no server with valid glpi database service."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    }

    $this->data = $this->config->data['SERVERS']['GLPI'];

    /* Check if we can call mysql_connect 
       If we can't, there is no the mysql-php extension
     */
    if(!is_callable("mysql_connect")){
      print_red(_("Can't connect to glpi database, the php-mysql extension is missing."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    }

    $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);

    /*  If handle == false, abort
        Seems that the server, username and or password is wrong
     */
    if(!$this->handle->is_connected){
      print_red(_("Can't connect to glpi database, check configuration twice."));
      return($smarty->fetch(get_template_path('glpi.tpl', TRUE)));
    } 

    /*  All checks are ok
        Lets handle Posts, templates etc below ...
     */
    
    $users = $this->handle->getUsers();
    $ldap= $this->config->get_ldap_link();
 
    /* Check for Trading button Post
     */
    if(isset($_POST['Trading'])){
      print_red(_("This feature is not implemented yet."));
    }

    /* Check for Software button Post
     */
    if(isset($_POST['Software'])){
      print_red(_("This feature is not implemented yet."));
    }

    /* Check for Contract button Post
     */
    if(isset($_POST['Contracts'])){
      print_red(_("This feature is not implemented yet."));
    }
 
    /* Add Device was requested, open new dialog
     */
    if(isset($_POST['AddDevice'])){
      $this->dialog =true;
      $this->cur_dialog = new glpiDeviceManagement($this->config,$this->dn,$this->usedDevices);
    }

    /* Attachment pool was closed with use
     */
    if(isset($_POST['UseAttachment'])){
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
    if(isset($_POST['AddAttachment'])){
      $this->cur_dialog = new glpiAttachmentPool($this->config,$this->dn,$this->usedAttachments);
      $this->dialog = true;
    }
    
    /* Remove Attachment fro this tab 
     */
    $once = true;
    foreach($_POST as $name => $value){
      if((preg_match("/^delAttachment_/",$name))&&($once)){
        $once= false;
        $name = preg_replace("/^delAttachment_/","",$name);
        $entry = preg_replace("/_.*$/","",$name);
        if(isset($this->usedAttachments[$entry])){
          unset($this->usedAttachments[$entry]);
        }
      }
    }
    if((isset($_POST['RemoveAttachment']))&&(isset($_POST['Attachments']))){
      foreach($_POST['Attachments'] as $entry){
        if(isset($this->usedAttachments[$entry])){
          unset($this->usedAttachments[$entry]);
        }
      }
    }

    /* We have selected some devices and pressed use button 
     */
    if(isset($_POST['SelectDeviceSave'])){
      $this->cur_dialog->save_object();
      $this->usedDevices= ($this->cur_dialog->getSelected());
      $this->cur_dialog = false;
      $this->dialog = false;
      $this->edit_type=false;
    }

    /* Aborted Device selction 
     */
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

    /* This closes the system type editing dialog
     */
    if(isset($_POST['close_edit_type'])){
      $this->edit_type=false;
      $this->dialog = false;
    }

    if(isset($_POST['Rename_Cancel'])){
      $this->renameTypeDialog = false;
      $this->renameOSDialog = false;
    }

    /* This appends a new system to our sytem types
     */
    if((isset($_POST['add_type']))&&(!empty($_POST['type_string']))){
      $attr = $this->handle->getSystemTypes();
      if(in_array(trim($_POST['type_string']),$attr)){
        print_red(_("Adding new sytem type failed, this system type name is already used.")) ;
      }else{
        $this->handle->addSystemType(trim($_POST['type_string']));  
      }
    }

    /* Remove selected type from our system types list
     */
    if((isset($_POST['del_type']))&&(!empty($_POST['select_type']))){
      $tmp = $this->handle->is_systemTypeUsed($_POST['select_type']);
      if(count($tmp)){
        $names = "";
        foreach($tmp as $name){
          $names .= ", ".$name;
        }
        $names = preg_replace("/^, /","",$names); 
        $names = trim($names);
        if(count($tmp) == 3){
          $names .= " ...";
        }
        print_red(sprintf(_("You can't delete this system type, it is still in use by these system(s) '%s'"),$names));
      }else{
        $this->handle->removeSystemType($_POST['select_type']); 
      } 
    }

    /* Rename selected system type to given string
     */
    if(isset($_POST['Rename_type_OK'])){
      $attr = $this->handle->getSystemTypes();
      if(in_array(trim($_POST['string']),$attr)){
        print_red(_("Rename failed, this system type name is already used.")) ;
      }else{
        $this->renameTypeDialog = false;
        $this->handle->updateSystemType($_POST['string'],trim($this->select_type));
      }
    }
    
  
    if((isset($_POST['rename_type'])&&(!empty($_POST['select_type'])))||($this->renameTypeDialog)){
      if(isset($_POST['select_type'])){
        $this->select_type = $_POST['select_type'];
      }
      $this->renameTypeDialog = true;
      $tmp = $this->handle->getSystemTypes();
       
      $smarty->assign("string",$tmp[$this->select_type]);
      if(isset($_POST['string'])){
        $smarty->assign("string",$_POST['string']);
      }
      $smarty->assign("Method","rename");
      $display= $smarty->fetch(get_template_path('glpi_edit_type.tpl', TRUE));
      return($display);
    }

    

    /* Someone wants to edit the system types ... 
       So, lets open a new dialog which provides some buttons to edit the types
     */
    if($this->edit_type){
      $smarty->assign("Method","edit");
      $smarty->assign("SystemTypes",            $this->handle->getSystemTypes());
      $smarty->assign("SystemTypeKeys",         array_flip($this->handle->getSystemTypes()));
      $display= $smarty->fetch(get_template_path('glpi_edit_type.tpl', TRUE));
      return($display);
    }

    /* System os management
     */
    if(isset($_POST['edit_os'])){
      $this->dialog = true;
      $this->edit_os=true;
    }

    /* Close Operating system dialog
     */
    if(isset($_POST['close_edit_os'])){
      $this->edit_os=false;
      $this->dialog = false;
    }

    /* Add new os to the db
     */
    if((isset($_POST['add_os']))&&(!empty($_POST['is_string']))){
      $attr = $this->handle->getOSTypes();
      if(in_array(trim($_POST['is_string']),$attr)){
        print_red(_("Adding new operating system failed, specifed name is already used.")) ;
      }else{
        $this->handle->addOS(trim($_POST['is_string']));  
      }
    }

    /* Delete selected os from list and db
     */
    if((isset($_POST['del_os']))&&(!empty($_POST['select_os']))){
      $tmp = $this->handle->is_osUsed($_POST['select_os']);
  
      if(count($tmp)){

        $names = "";
        foreach($tmp as $name){
          $names .= ", ".$name;
        }
        $names = preg_replace("/^, /","",$names);
        $names = trim($names);
        if(count($tmp) == 3){
          $names .= " ...";
        }
        print_red(sprintf(_("You can't delete this operating system, it is still in use by these system(s) '%s'"),$names));

      }else{
        $this->handle->removeOS_byID($_POST['select_os']);  
      }
    }

    /* Rename selected os to given string
     */
    if(isset($_POST['Rename_os_OK'])){
      $attr = $this->handle->getOSTypes();
      if(in_array(trim($_POST['string']),$attr)){
        print_red(_("Updating operating system failed, specifed name is already used.")) ;
      }else{
        $this->handle->updateOS($_POST['string'],$this->select_type);
        $this->renameOSDialog = false;
      }
    }
    if((isset($_POST['rename_os'])&&(!empty($_POST['select_os'])))||($this->renameOSDialog)){
      if(isset($_POST['select_os'])){
        $this->select_type = $_POST['select_os'];
      }
      $this->renameOSDialog = true;
      $tmp = $this->handle->getOSTypes();
       
      $smarty->assign("string",$tmp[$this->select_type]);
      if(isset($_POST['string'])){
        $smarty->assign("string",$_POST['string']);
      }
      $smarty->assign("Method","rename");
      $display= $smarty->fetch(get_template_path('glpi_edit_os.tpl', TRUE));
      return($display);
    }

    /* Open dialog to edit os types 
     */
    if($this->edit_os){
      $smarty->assign("Method","edit");
      $smarty->assign("OSs",            $this->handle->getOSTypes());
      $smarty->assign("OSKeys",         array_flip($this->handle->getOSTypes()));
      $display= $smarty->fetch(get_template_path('glpi_edit_os.tpl', TRUE));
      return($display);
    }



    /* Show dialog to select a new contact person
     * Select a contact person
     */
    if(isset($_POST['SelectContactPerson'])){
      $this->addUser = "contact";
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn,"user_tech_num");
    }

    /* Open dialog which allows to edit the manufacturers
     */
    if(isset($_POST['edit_manufacturer'])){
      $this->cur_dialog = new glpiManufacturer($this->config,$this->dn);
      $this->dialog = true;
    }

    /* Close manufacturer editing dialog
     */
    if(isset($_POST['close_edit_manufacturer'])){
      $this->dialog = false;
      $this->cur_dialog = false;
    }

    /* Abort user selection
     */
    $smarty->assign("AbortSelectUser","SelectUserCancel");
    if(isset($_POST['SelectUserCancel'])){
      $this->dialog = false;
      $this->addUser ="";
      $this->cur_dialog = false;
    }

    /* Selecte technical responsible person
     */
    if(isset($_POST['SelectTechPerson'])){
      $this->addUser ="tech";
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn,"user_tech_num");
    }

    /* Technical responsible person selected*/
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

    /* if( cur_dialog != false || cur_dialog != NULL) 
     * There is a dialog which wants to be displayed 
     */
    if($this->cur_dialog){
      $this->cur_dialog->save_object();
      $this->dialog=true;
      $this->cur_dialog->parent = &$this;
      return($this->cur_dialog->execute());
    }else{
      $this->dialog= false;
    }

    /* Assign smarty defaults */ 
    foreach(array("SystemTypes","SystemTypeKeys","Manufacturers","OSs","TechnicalResponsibles","InstalledDevices","Attachments") as $attr){
      $smarty->assign($attr,array());
    }

    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Assign some vars to smarty 
     */
    foreach(array("type","FK_glpi_enterprise","os","tech_num","contact_num","Attachments","InstalledDevices") as $attr){
      $smarty->assign($attr,"");
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove inventory"),
          _("This device has inventory informations enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add inventory"),
          _("This device has inventory informations disabled. You can enable them by clicking below."));
      return ($display);
    }

    /* Assign ACLs to smarty*/
    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Assign system types 
     */
    $smarty->assign("SystemTypeKeys",         array_flip($this->handle->getSystemTypes()));
    $smarty->assign("SystemTypes",            $this->handle->getSystemTypes());
    $smarty->assign("type",                   $this->type);

    /* Assign os types
     */
    $smarty->assign("OSKeys",                 array_flip($this->handle->getOSTypes()));
    $smarty->assign("OSs",                    $this->handle->getOSTypes());
    $smarty->assign("os",                     $this->os);

    /* Dispaly installed devices */
    $smarty->assign("InstalledDevices"        ,$this->getDevices());
    $smarty->assign("InstalledDeviceKeys"     ,array_flip($this->getDevices()));

    /* Append manufacturers
     */
    $smarty->assign("ManufacturerKeys",       array_flip($this->handle->getEnterprises()));
    $smarty->assign("Manufacturers",          $this->handle->getEnterprises());
    $smarty->assign("FK_glpi_enterprise",     $this->FK_glpi_enterprise);

    /* Assign used Attachments
    */

    $divlist = new divSelectBox("glpiAttachmentsList");
    $divlist-> SetHeight(130); 
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
    /* Handle contact person 
       Assign name ... to smarty, if set
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
      $tr = $ldap->cat($users[$this->tech_num], array('givenName', 'sn', 'uid'));
      $tr = $ldap->fetch();
      $str = "";
      if(isset($tr['givenName'][0])){   $str .= $tr['givenName'][0]." ";      }
      if(isset($tr['sn'][0])) {         $str .= $tr['sn'][0]." ";             }
      if(isset($tr['uid'][0])){         $str .= "[".$tr['uid'][0]."]";        }
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
    if(!$this->initialy_was_account){
      return;
    }
    if(function_exists("mysql_pconnect")){
      $this->handle = new glpiDB($this->data['SERVER'],$this->data['LOGIN'],$this->data['PASSWORD'],$this->data['DB']);
      if($this->initialy_was_account){
        $this->handle->removeComputerInformations($this->dn); 
      }
    }else{
      print_red(_("Can't remove glpi account, while mysql extension is missing."));
    }
  
  }

  function getDevices(){
    $ret = array();
    foreach($this->usedDevices as $type => $entries){
      foreach($entries as $ent){
        if(isset($ent['designation'])){
          $ret[] = $ent['designation']." [".$type."]";
        }else{
          $ret[] = $ent['name']." [".$type."]";
        }
      }
    }
    return($ret); 
  }


  /* Save data to object */
  function save_object()
  {
    if(!isset($_POST['glpi_tpl_posted'])) {
      return ;
    }
    plugin::save_object();
    foreach($this->attributes as $attrs){
      if(isset($_POST[$attrs])){
        $this->$attrs = stripslashes ($_POST[$attrs]);
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

      /* check if we have to update, add */
      if($this->initialy_was_account&&$this->is_account){
        $this->handle->updateComputerInformations($attrs,$this->orig_dn);
      }elseif($this->is_account){
        $this->handle->addComputerInformations($attrs,$this->dn);
      }
      $tmp = $this->handle->getComputerInformations($this->dn);
      $this->handle->addDevicesToComputer($this->usedDevices,$tmp[0]['ID']);
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
  

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
