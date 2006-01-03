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

  function glpiManufacturer($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->ui = get_userinfo();  
  }

  function execute()
  {
    $smarty  = get_smarty();
    $display = "";

    if((isset($_POST['remove_manu']))&&(isset($_POST['manufacturer']))){
      $this->parent->handle->removeEnterprisesType($_POST['manufacturer']);
    }

    if(isset($_POST['add_manu'])){
      $this->editMode = true;
      $this->Edit_Add = "add";
      foreach($this->attributes as $atr){
        $this->$atr = "";
      }
    }

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

    if(isset($_POST['close_manufacturer'])){
      $this->editMode=false;
    }

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

      if($allok){
        if($this->Edit_Add == "add"){
          $this->parent->handle->addEnterprisesType($tmp);
          $this->editMode=false;
        }else{
          $this->parent->handle->updateEnterprisesType($tmp,$this->ID);
          $this->editMode=false;
        }
      }
    }

    if($this->editMode == true){
      $this->save_object();
      foreach($this->attributes as $attrs){
        $smarty->assign($attrs,$this->$attrs);
      }

      $display.= $smarty->fetch(get_template_path('glpiManufacturerAdd.tpl', TRUE));
      return($display);
    }


    $smarty->assign("Manus",    $this->parent->handle->getEnterprisesTypes());
    $smarty->assign("ManuKeys", array_flip($this->parent->handle->getEnterprisesTypes()));
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
        $this->$attr = $_POST[$attr];
      }
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
