<?php

class glpiPrinterCartridgesEdit extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array("ID","name","ref","location","type","FK_glpi_enterprise","tech_num","deleted","comments","alarm");
  var $objectclasses        = array("whatever");

  var $printer_type      = 0;

  var $additional_printer= array();
  var $needToCatch       = true;

  var $parent           = NULL;

  var $ID                 = "";
  var $name               = "";
  var $ref                = "";
  var $location           = "";
  var $type               = "";
  var $FK_glpi_enterprise = "";
  var $tech_num           = "";
  var $deleted            = "";
  var $comments           = "";
  var $alarm              = "";  

  var $cur_dialog         = false;
  var $edit_type          =false;


  function glpiPrinterCartridgesEdit (&$config, $dn,$printer_type,$values = NULL )
  {
    plugin::plugin ($config, $dn);
    foreach($this->attributes as $val){
      if(isset($values[$val])){
        $this->$val = $values[$val];
      }else{
        $this->$val = "";
      }
    }
    $this->needToCatch = true;
    $this->printer_type = $printer_type;
  }

  function remove_from_parent()
  {
    if($this->ID > 0){
      $this->parent->handle->removeCartridgeDropdownType($this->ID);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Get all currently used/supported printer types for this cartridge type
     */
    if($this->needToCatch){
      $this->needToCatch = false;
      if(!empty($this->type)){
        $this->additional_printer= $this->parent->handle->getSupportedPrinterTypeIDsForCartridge($this->ID);
      }
    }





  
    /* Printer type management
     */
    if(isset($_POST['edit_type_cartridge'])){
      $this->dialog = true;
      $this->edit_type=true;
    }

    /* This closes the printer type editing dialog
     */
    if(isset($_POST['close_edit_type_cartridge'])){
      $this->edit_type=false;
      $this->dialog = false;
    }

    /* This appends a new printer to our sytem types
     */
    if((isset($_POST['add_cartridge_type']))&&(!empty($_POST['cartridge_type_string']))){
      print "Add";
      $this->parent->handle->addCartridgeDropdownType($_POST['cartridge_type_string']);
    }

    /* Remove selected type from our printer types list
     */
    if((isset($_POST['del_cartridge_type']))&&(!empty($_POST['select_type_cartridge']))){
      $used = $this->parent->handle->getUsedDropdownTypes($_POST['select_type_cartridge']);
      if(!count($used)){
        $this->parent->handle->removeCartridgeDropdownType($_POST['select_type_cartridge']);
      }else{
        print_red(_("Can't delete this entry, it is still in use."));
      }
    }

    /* Rename selected printer type to given string
     */
    /* Update*/
    if((isset($_POST['rename_cartridge_type']))&&(!empty($_POST['select_type_cartridge']))&&(!empty($_POST['cartridge_type_string']))){
      $this->parent->handle->updateCartridgeDropdownType($_POST['cartridge_type_string'],$_POST['select_type_cartridge']);
    }

    /* Someone wants to edit the printer types ...
       So, lets open a new dialog which provides some buttons to edit the types
     */
    if($this->edit_type){
      $smarty->assign("PrinterTypes",            $this->parent->handle->getCartridgeTypes());
      $smarty->assign("PrinterTypeKeys",         array_flip($this->parent->handle->getCartridgeTypes()));
      $display= $smarty->fetch(get_template_path('glpi_edit_cartridge_type.tpl', TRUE));
      return($display);
    }

    /* Assign attributes */ 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    /* Selecte technical responsible person
     */
    if(isset($_POST['SelectCartridgeTechPerson'])){
      $this->cur_dialog= new glpiSelectUser($this->config,$this->dn,"user_cartridge_tech_num");
    }

    /* Abort user selection
     */
    $smarty->assign("AbortSelectUser","SelectCartridgeUserCancel");
    $smarty->assign("SaveSelectUser" ,"user_cartridge_tech_num");
    if(isset($_POST['SelectCartridgeUserCancel'])){
      $this->dialog = false;
      $this->addUser ="";
      $this->cur_dialog = false;
    }

    /* Start ldap, and get all glpiUsers */
    $ldap = $this->config->get_ldap_link();
    $users = ($this->parent->handle->getUsers());

    /* Technical responsible/contact person selected 
    */
    if(isset($_GET['act'])&&($_GET['act']=="user_cartridge_tech_num")){

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
        $this->parent->handle->addUser($tmp,$id);
      }

      /* Re-read users */
      $users = ($this->parent->handle->getUsers());

      /* Get user */
      $tmp = array_flip($users);
      $id=$tmp[$id];
    
      /* Close dialog and use this user */
      $this->tech_num = $id;
      $this->cur_dialog   = false;
      $this->dialog= false;
    }

    /* Execute dialog*/
    if($this->cur_dialog){
      $this->cur_dialog->save_object();
      $this->dialog=true;
      $this->cur_dialog->parent = &$this;
      return($this->cur_dialog->execute());
    }else{
      $this->dialog= false;
    }

    /* Create matrix with all possible printer types 
     */
    $matrix = "<table summary=''><tr>";
    $types = $this->parent->handle->getPrinterTypes();
   
    /* Specifies number of cols */ 
    $spalten = 2;
    
    $spalt = $spalten; 
    foreach($types as $key => $type){
      $spalt -- ; 

      /* Create new row, if cols for this row reached $spalten */
      if($spalt < 0) {
        $spalt = $spalten -1;
        $matrix .= "</tr><tr>";
      }

      /* Avoid deselecting every checkbox */
      if($key == $this->printer_type){
        $matrix.= "<td><input type='checkbox' name='UsePrinterType_".$key."' disabled checked>".$type."</td>";
      }else{
      
        /* Cehck checkbox */
        $check = "";
        if(isset($this->additional_printer[$key])){
          $check = " checked "; 
        }
        $matrix.= "<td><input type='checkbox' name='UsePrinterType_".$key."' ".$check." ><input type='hidden' name='wasOnPage_".$key."' ".$check.">".$type."</td>";
      }
    }
    
    /* Complete table row, to avoid html errors */
    if($spalt >0){
      while($spalt >0){
        $spalt -- ;
        $matrix .="<td>&nbsp;</td>";
      }
    }
    
    $matrix .= "</tr></table>";

     /* Append manufacturers
     */
    $smarty->assign("PrinterTypeMatrix",      $matrix);
    
    $smarty->assign("ManufacturerKeys",       array_flip($this->parent->handle->getEnterprises()));
    $smarty->assign("Manufacturers",          $this->parent->handle->getEnterprises());
    $smarty->assign("FK_glpi_enterprise",     $this->FK_glpi_enterprise);

    $smarty->assign("typeKeys",               array_flip($this->parent->handle->getCartridgeTypes()));
    $smarty->assign("types",                  $this->parent->handle->getCartridgeTypes());
    $smarty->assign("type",                   $this->type);

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


    $display.= $smarty->fetch(get_template_path('glpiPrinterCartridgesEdit.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  /* Check given values */
  function check(){
    /* Call common method to give check the hook */
    $message= plugin::check();
  
    /* Avoid choosing an already used name */
    $types = $this->parent->handle->getCartridgeTypeInformations();
    foreach($types as $type){
      if($type['name'] == $this->name){
        if($type['ID'] != $this->ID){
          $message[]= _("The selected name is already in use.");
        }        
      }
    }  
    return $message;
  }

  /* Save Post data */
  function save_object()
  {
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr= $_POST[$attr];
      }
    }

    /* Get selected checkboxes */
    foreach($_POST as $name => $value){
      if(preg_match("/wasOnPage_/",$name)){
        $id = preg_replace("/wasOnPage_/","",$name);
        if(isset($_POST['UsePrinterType_'.$id])){
          $this->additional_printer[$id]=$id;
        }else{
          if(isset($this->additional_printer[$id])){
            unset($this->additional_printer[$id]);
          }
        }
      }
    }
  }

  /* Create / Update database with this new infos */
  function save()
  {
    $tmp  = array();
    foreach($this->attributes as $attr){
      $tmp[$attr] = $this-> $attr;
    }
    $types = array();
    $types = $this->additional_printer;
    $types[$this->printer_type]=$this->printer_type;
    $this->parent->handle->Add_UpdateCatrigdeType($tmp,$types);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
