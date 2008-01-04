<?php

class glpiPrinterCartridges extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array();
  var $objectclasses    = array("whatever");

  var $parent           = NULL;       // Contains parent class, to access glpi db handle
  var $usedCartridges   = array();    // IDs of used cartridges for this printer 
  var $PrinterType      = 0;          // Specifies which cartridge types are available

  var $cur_dialog       = false;
  var $cur_sub_dialog   = false;

  var $editManufacturer = false;
  var $del              = 0;


  function glpiPrinterCartridges (&$config,$dn,$type)
  {
    plugin::plugin ($config, $dn);

    /* Assign some basic settings */
    $this->ui = get_userinfo();  
    if(!session::is_set('glpiCartridgeRegex')){
      session::set('glpiCartridgeRegex',"*");
    }
  
    /* Only display cartridges for this type of printer */
    $this->PrinterType = $type;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
    $display ="";
    $smarty = get_smarty();
    $filter = $_SESSION['glpiCartridgeRegex'];

    /* Filter settings, remove double* */
    if(isset($_GET['search'])){
      $filter = preg_replace("/\*\**/","*",$_GET['search']."*");
    }elseif(isset($_POST['cartridge_regex'])){
      $filter = preg_replace("/\*\**/","*",$_POST['cartridge_regex']);
    }
    if(empty($filter)) {
      $filter = "*";
    }
    session::set('glpiCartridgeRegex',$filter);

    /* Open dialog which allows to edit the manufacturers
     */
    if(isset($_POST['edit_manufacturer_cartridges'])){
      $this->cur_sub_dialog = new glpiManufacturer($this->config,$this->dn);
      $this->dialog = true;
      $this->editManufacturer =true;
    }

    /* Close manufacturer editing dialog
     */
    if((isset($_POST['close_edit_manufacturer']))&&($this->editManufacturer)){
      $this->dialog = false;
      $this->cur_sub_dialog = false;
      $this->editManufacturer=false;
    }

    /* Check poted vars and do the requested job, but only once */
    $only_once = true;
    foreach($_POST as $name => $value){

      /* We have to create a new cartridge */
      if(preg_match("/^newcartridge/",$name)&&($only_once)){
        $this->cur_dialog = new glpiPrinterCartridgesEdit($this->config,$this->dn,$this->PrinterType); 
        $only_once = false;
      }
    }
  
    /* Edit cartridge */
    if(isset($_GET['act'])&&$_GET['act']=="edit_cartridge"){
      $val = ($this->parent->handle->getCartridgeTypeInformations($_GET['id']));
      $this->cur_dialog = new glpiPrinterCartridgesEdit($this->config,$this->dn,$this->PrinterType,$val[$_GET['id']]); 
    }
 
    /* remove cartridge */ 
    if(isset($_GET['act'])&&$_GET['act']=="del_cartridge"){
        /* remove attach from db */
        $this->del = $_GET['id'];
        $val = ($this->parent->handle->getCartridgeTypeInformations($_GET['id']));
        $smarty->assign("warning", sprintf(_("You're about to delete the glpi cartridge type '%s'."), $val[$this->del]['name']));
        return($smarty->fetch(get_template_path('remove_glpi.tpl', TRUE)));
    }

    if(isset($_POST['delete_cancel'])){
      $this->del = false;
    }

    /* Delete this entry */
    if(isset($_POST['delete_glpi_confirm'])&&($this->del)) {
      $tmp = $this->parent->handle->is_cartridgeTypeUsed($this->del);
      if(count($tmp)){

        $str = "";
        foreach($tmp as $id => $name){
          $str .= $name.", "; 
        }
        $str = preg_replace("/, $/","",$str);

        print_red(sprintf(_("You can't delete this cartridge type, it is still in use by this printer(s) '%s'."),$str));
      }else{
        $val = ($this->parent->handle->getCartridgeTypeInformations($this->del));
        $this->cur_dialog = new glpiPrinterCartridgesEdit($this->config,$this->dn,$this->PrinterType,$val[$this->del]); 
        $this->cur_dialog->parent = $this->parent;
        $this->cur_dialog->remove_from_parent();
        $this->cur_dialog= false;
      } 
    }

    /* Abort edit / add dialog */
    if(isset($_POST['CancelCartridge'])){
      $this->cur_dialog = false;
    }
  
    /* Save changes if check is ok */
    if(isset($_POST['SaveCartridge'])){
      $this->cur_dialog->save_object();
      if(count($this->cur_dialog->check())) {
        foreach($this->cur_dialog->check() as $checks){
          print_red($checks);
        }
      }else{
        $this->cur_dialog->save();
        $this->cur_dialog = false;
      }
    }

    /* If we have a dialog open, display it */
    if($this->cur_sub_dialog){
      $this->cur_sub_dialog->parent = $this->parent;
      $this->cur_sub_dialog->save_object();
      return($this->cur_sub_dialog->execute());
    }
    /* If we have a dialog open, display it */
    if($this->cur_dialog){
      $this->cur_dialog->parent = $this->parent;
      $this->cur_dialog->save_object();
      return($this->cur_dialog->execute());
    }

    /* Create divlist */
    $divlist = new divlist("glpi devices");
    $divlist->SetPluginMode();
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("Cartridges"), "attach" => "style=''"),
          array("string" => _("Action"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));

    /* Links for editing,adding.... */
    $edit =     "<a href='?plug=".$_GET['plug']."&amp;act=edit_cartridge&amp;id=%s'>%s</a>";
    $editdel =  "<a href='?plug=".$_GET['plug']."&amp;act=edit_cartridge&amp;id=%s'><img src='images/edit.png' alt='"._("Edit")."' border=0></a>";
    $editdel.= "<a href='?plug=".$_GET['plug']."&amp;act=del_cartridge&amp;id=%s'><img src='images/edittrash.png' alt='"._("Delete")."' border=0></a>";
   
    /* Checkbox for selection of some cartridges */ 
    $useCartridge = "<input type='hidden' name='wasOnPage_%s' value='%s'><input type='checkbox' value='%s' name='useCartridge_%s' %CHECKED%>";

    /* Add cartridges */ 
    $cart = $this->getCartridgeTypes();

    /* Remove typically error possibilities */
    $f = str_replace("/","\/",$filter);
    $f = str_replace(".","\.",$f);
    $f = str_replace("*",".*",$f);

    /* Assign cartridges */
    foreach($cart as $key=>$cartr){

      /* Skip if filter doesn't match*/
      if(!preg_match("/^".$f."$/i",$cartr['cartridgeName'])){
        continue;
      }

      /* check if this cartridge is selected */
      $chk = "";
      if(isset($this->usedCartridges[$key])){
        $chk = " checked ";
      }

      /* Add fields */
      $field1 = array("string" => preg_replace("/%s/",($key),preg_replace("/%CHECKED%/",$chk,$useCartridge)), 
                      "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($edit,($key),$cartr['cartridgeName']."&nbsp;[".$cartr['cartridgeTypeName']."]"), 
                      "attach" => "style=''");
      $field3 = array("string" => sprintf($editdel,($key),($key)), 
                      "attach" => "style='width:60px;border-right:0px;text-align:right;'");

      $divlist->AddEntry(array($field1,$field2,$field3));
      
    }
   
    /* Create list header */ 
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' align='middle' 
      src='images/zip.png'  title='"._("New monitor")."' alt='"._("M")."' name='newcartridge'>&nbsp;".
      "</div>";

    /* Tell smarty some vars */
    $filter= $_SESSION['glpiCartridgeRegex'];
    $smarty->assign("devicehead", $listhead);
    $smarty->assign("devices", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info_small.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("cartridge_regex", $filter);

    $display.= $smarty->fetch(get_template_path('glpiPrinterCartridges.tpl', TRUE));
    return($display);
  }

  /* Save device to glpi database 
   * If this is a new device, create a new entry, else update this entry 
   */
  function save()
  {
    $carts = $this->getCartridgeTypes();
    $ret = array();
    foreach($this->usedCartridges as $key){
      $tmp =array();
      $tmp['date_use']        = date("Y-m-d");
      $tmp['ID']              = -1;
      $tmp['type_ID']         = $carts[$key]['cartridgeID'];
      $tmp['name']            = $carts[$key]['cartridgeName'];
      $tmp['FK_glpi_printers']= -1;
      $tmp['type_name']       = $carts[$key]['cartridgeTypeName'];
      $ret[$key] = $tmp;
    }
    return($ret);
  }

  /* this only gets all already defined devices */
  function reload()
  {
    $this->devices = $this->parent->handle->getDevices();
    ksort($this->devices);
  }

  /* This funtions saves all POST variables.
     The variable must be in the array $this->EditEntry 
  */
  function save_object()
  {
    /* Checkbox handling 
     * Check which checkbox is selected 
     */
    foreach($_POST as $name => $value){
      if(preg_match("/wasOnPage/",$name)){
        $id = preg_replace("/wasOnPage_/","",$name);
        if(isset($_POST['useCartridge_'.$id])){
          $this->usedCartridges[$id]=$id;
        }else{
          unset($this->usedCartridges[$id]);
        }
      }
    }
  }

  /* This function cehck all created devices if you wan't to create device specific check 
      use >>if($attr['device_type']=="moboard")<< to create a device type depending check
   */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    return($message);
  }

  /* Return cartriges for our divlist */
  function getCartridgeTypes()
  {
    $ret = array();
    $ret = $this->parent->handle->getAvailableCartridgeTypes($this->PrinterType);
    return($ret);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
