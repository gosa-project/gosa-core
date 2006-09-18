<?php

class glpiManufacturer extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("name","type","address","website","phonenumber","comments","deleted","fax","email");
  var $objectclasses= array("whatever");

  var $ui;

  var $editMode = false;
  var $Edit_Add = "edit";

  var $name       ="";
  var $type       ="";
  var $address    ="";
  var $website    ="";
  var $phonenumber="";
  var $comments   ="";
  var $deleted    ="";
  var $fax        ="";
  var $email      ="";
  var $ID         =-1;

  function glpiManufacturer($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    $this->ui = get_userinfo();  
  }

  function execute()
  {
    plugin::execute();
    $smarty  = get_smarty();
    $display = "";

    /* Remove enterprise from db */
    if((isset($_POST['remove_manu']))&&(isset($_POST['manufacturer']))){

      $tmp = $this->parent->handle->is_manufacturerUsed($_POST['manufacturer']);
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
        print_red(sprintf(_("You can't delete this manufacturer, it is still in use by these system(s) '%s'"),$names));
      }else{
        $this->parent->handle->removeEnterprise($_POST['manufacturer']);
      }
    }

    /* Add new Manufactuer : Open dialog with empty fields */
    if(isset($_POST['add_manu'])){
      $this->editMode = true;
      $this->Edit_Add = "add";
      foreach($this->attributes as $atr){
        $this->$atr = "";
      }
    }

    /* Edit existing manuatctuerer data */
    if((isset($_POST['edit_manu']))&&(isset($_POST['manufacturer']))){
      $this->editMode = true;
      $this->Edit_Add = "edit";
      $tmp = $this->parent->handle->getEnterprise($_POST['manufacturer']);
      $tmp = $tmp[0];
      foreach($this->attributes as $atr){
        $this->$atr = "";
      }
      foreach($this->attributes as $atr){
        if(isset($tmp[$atr])){
          $this->$atr = $tmp[$atr];
        }
      }
      $this->ID = $_POST['manufacturer'];
    }

    /* close Dialog without saving */
    if(isset($_POST['close_manufacturer'])){
      $this->editMode=false;
    }

    /* close dialog an save all changes / adds */
    if(isset($_POST['save_manufacturer'])){
      $tmp = array();
      foreach($this->attributes as $attrs){
        $tmp[$attrs]=$this->$attrs;
      }

      $allok = true;
      if(empty($tmp['name'])){
        print_red(_("Please specify a name."));
        $allok = false;
      }

      $attr = $this->parent->handle->getEnterprises();

      if($this->ID == -1 ){
        if(in_array($tmp['name'],$attr)){
          $allok = false;
          print_red(_("Specified name is already in use, please choose another one."));
        }
      }else{
        unset($attr[$this->ID]);
        if(in_array($tmp['name'],$attr)){
          $allok = false;
          print_red(_("Specified name is already in use, please choose another one."));
        }
      }

      /* all checks are ok , so save changes */
      if($allok){
        if($this->Edit_Add == "add"){
          $this->parent->handle->addEnterprise($tmp);
          $this->editMode=false;
        }else{
          $this->parent->handle->updateEnterprise($tmp,$this->ID);
          $this->editMode=false;
        }
      }
    }

    /* As long as this war is true, we have to display the edit dialog */
    if($this->editMode == true){
      $this->save_object();
      foreach($this->attributes as $attrs){
        $smarty->assign($attrs,$this->$attrs);
      }

      $display.= $smarty->fetch(get_template_path('glpiManufacturerAdd.tpl', TRUE));
      return($display);
    }


    $smarty->assign("Manus",    $this->parent->handle->getEnterprises());
    $smarty->assign("ManuKeys", array_flip($this->parent->handle->getEnterprises()));
    $display.= $smarty->fetch(get_template_path('glpiManufacturer.tpl', TRUE));
    return($display);
  }

  /* Save to LDAP */
  function save()
  {
  }

  function save_object()
  {
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = stripslashes($_POST[$attr]);
      }
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
