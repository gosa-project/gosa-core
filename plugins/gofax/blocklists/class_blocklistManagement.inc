<?php
class blocklist extends plugin
{
  /* Definitions */
  var $plHeadline= "FAX Blocklists";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Handling of GOfax block lists";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* blocklist attributes */
  var $cn               = "";
  var $description      = "";
  var $type             = "";
  var $base             = "";

  var $goFaxBlocklist   = array();
  var $blocklists       = array();
  var $attributes       = array("cn", "description");
  var $objectclasses    = array();
  var $DivListBlocklist = NULL;

  function blocklist ($config, $ui)
  {
    /* Init class */
    $this->ui     = $ui;
    $this->dn     = "";
    $this->config = $config;

    $ui           = get_userinfo();
    $acl          = get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl    = get_module_permission($acl, "blocklists", $ui->dn);

    $this->DivListBlocklist = new divListBlocklist($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /***************
      Init vars 
     ***************/

    /* Get global smarty instance */
    $smarty       = get_smarty();
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton


    /***************
      Fetch posts 
     ***************/

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/user_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/user_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/user_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/user_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_chgpw_/i","",$key);
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /***************
      Cancel some dialogs  
     ***************/

    /* Cancel dialog */
    if (isset($_POST['edit_cancel']) || 
        isset($_POST['delete_blocklist_cancel']) ||
        isset($_POST['delete_blocklist_confirm']) || 
        isset($_POST['delete_lock'])){

      if(!isset($_POST['delete_blocklist_confirm'])){
        $this->clear_fields();
      }
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /***************
      Cancel some dialogs  
     ***************/

    /* Create new blocklist? */
    if ($s_action=="new"){
      $this->clear_fields();
      $this->dn= "new";
      plugin::plugin ($this->config, $this->dn);
    }


    /***************
      Edit blocklist 
     ***************/

    /* Edit selected blocklist? */
    if (($s_action=="edit") && (empty($this->dn))){
      $this->clear_fields();
      $this->dn=$this->blocklists[$s_entry]['dn']; 
      if (($user= get_lock($this->dn)) != ""){
        $_SESSION['dn']= $this->dn;
        //$this->dn ="";
        return(gen_locked_message($user, $this->dn));
      } else {
# Lock this dn for editing
        add_lock ($this->dn, $this->ui->dn);

        plugin::plugin ($this->config, $this->dn);

        /* Load missing lists */
        if (in_array("goFaxSBlock",$this->attrs['objectClass'])){
          if(isset($this->attrs["goFaxSBlocklist"])){
            for ($i= 0; $i<$this->attrs["goFaxSBlocklist"]["count"]; $i++){
              $this->goFaxBlocklist[]= $this->attrs["goFaxSBlocklist"][$i];
            }
          }
          $this->type= 0;
        } elseif (in_array("goFaxRBlock",$this->attrs['objectClass'])){
          if(isset($this->attrs["goFaxRBlocklist"])){
            for ($i= 0; $i<$this->attrs["goFaxRBlocklist"]["count"]; $i++){
              $this->goFaxBlocklist[]= $this->attrs["goFaxRBlocklist"][$i];
            }
          }
          $this->type= 1;
        }
        $_SESSION['objectinfo']= trim($this->dn);
      }
    }


    /***************
      Remove blocklist
     ***************/

    /* Delete blocklist requested */
    if ($s_action=="del"){
      $this->dn=$this->blocklists[$s_entry]['dn'];
      /* Check locking */
      if (($user= get_lock($this->dn)) != ""){
        $_SESSION['dn']= $this->dn;
        return(gen_locked_message($user, $this->dn));
      } else {
# Lock this dn for editing
        add_lock ($this->dn, $this->ui->dn);

        $smarty->assign("info", sprintf(_("You're about to delete the blocklist '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /***************
      Remove blocklist confirmed
     ***************/

    /* Finally delete blocklist */
    if (isset($_POST['delete_blocklist_confirm'])){
      if (chkacl($this->acl, "delete") == ""){
        $this->remove_from_parent();
        gosa_log ("Blocklist object'".$this->dn."' has been removed");
        $this->reload ();
      } else {
        print_red (_("You have no permission to remove this blocklist."));
      }
    }


    /***************
      Add numer to blocklist
     ***************/


    /* Handle interactions: add */
    if (isset($_POST['add_number']) && $_POST['number'] != ""){
      if (is_phone_nr($_POST['number'])){
        $this->addNumber ($_POST['number']);
      } else {
        print_red (_("Please specify a valid phone number."));
      }
    }


    /***************
      Delete number from list
     ***************/

    /* Handle interactions: delete */
    if (isset($_POST['delete_number']) && count($_POST['numbers']) > 0){
      $this->delNumber ($_POST['numbers']);
    }


    /***************
      Edit finished
     ***************/

    /* What about finish? */
    if ((isset($_POST['edit_finish'])) && (!empty($this->dn))){
      $message= $this->check();
      $this->remove_lock();

      /* No errors, save object */
      if (count ($message) == 0){

        /* Perpare 'dn' in case of new entries */
        if ($this->dn == "new"){
          $this->dn= "cn=$this->cn,ou=gofax,ou=systems,".$this->base;
        }

        $this->save ();
        gosa_log ("Blocklist object'".$this->dn."' has been saved");
        $this->dn= "";
        del_lock ($this->ui->dn);
        unset($_SESSION['objectinfo']);
      } else {
        /* Errors found, show message */
        show_errors ($message);
      }
    }


    /***************
      Object currently in edit  
     ***************/

    if($this->dn){

      /* Base select dialog */
      $once = true;
      foreach($_POST as $name => $value){
        if(preg_match("/^chooseBase/",$name) && $once){
          $once = false;
          $this->dialog = new baseSelectDialog($this->config);
          $this->dialog->setCurrentBase($this->base);
        }
      }

      /* Dialog handling */
      if(is_object($this->dialog)){
        /* Must be called before save_object */
        $this->dialog->save_object();

        if($this->dialog->isClosed()){
          $this->dialog = false;
        }elseif($this->dialog->isSelected()){
          $this->base = $this->dialog->isSelected();
          $this->dialog= false;
        }else{
          return($this->dialog->execute());
        }
      }

      /* Set base */
      if ($this->base == ""){
        if ($this->dn == "new"){
          $ui= get_userinfo();
          $this->base= dn2base($ui->dn);
        } else {
          $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
        }
      }

      /* Fill templating stuff */
      $smarty->assign("bases", $this->config->idepartments);
      $smarty->assign("base_select", $this->base);
      $smarty->assign("types", array(0 => _("send"), 1 => _("receive")));
      if ($this->dn == "new" || preg_match ("/,ou=incoming,/", $this->dn)){
        $smarty->assign("selectmode", "");
        $smarty->assign("mode", "");
      } else {
        $smarty->assign("selectmode", "disabled");
        $smarty->assign("mode", "readonly");
      }
      foreach(array("cn", "description", "type", "goFaxBlocklist") as $val){
        $smarty->assign("$val", $this->$val);
        $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
      }

      /* Lock blocklist type for non new entries */
      if ($this->dn != "new"){
        $smarty->assign('typeACL', "disabled");
      }

      /* Show main page */
      return($smarty->fetch (get_template_path('generic.tpl', TRUE)));

    }else{

      /***************
        Divlist dialog 
       ***************/

      /* Display dialog with system list */
      $this->DivListBlocklist->parent = $this;
      $this->DivListBlocklist->execute();

      /* Add departments if subsearch is disabled */
      if(!$this->DivListBlocklist->SubSearch){
        $this->DivListBlocklist->AddDepartments($this->DivListBlocklist->selectedBase);
      }
      $this->reload();
      $this->DivListBlocklist->setEntries($this->blocklists);
      return($this->DivListBlocklist->Draw());
    }
  }


  /* Clear garbage from edited lists */
  function clear_fields()
  {
    $this->dn= "";
    $this->description= "";
    $this->cn= "";
    $this->base= $_SESSION['CurrentMainBase'];
    $this->goFaxBlocklist= array();
  }


  /* Reload the list of known blocklists */
  function reload()
  {
    /* Init some vars */
    $filter = $filter2      = "";
    $base                   = $this->DivListBlocklist->selectedBase;
    $Regex                  = $this->DivListBlocklist->Regex;
    $SubSearch              = $this->DivListBlocklist->SubSearch;
    $ShowSendBocklists      = $this->DivListBlocklist->ShowSendBocklists;
    $ShowReceiveBlocklists  = $this->DivListBlocklist->ShowReceiveBlocklists;
    $Flags                  = GL_SIZELIMIT;
    $res = $res2            = array();

    /* Append subsearch to Flags */
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }else{
      $base = "ou=gofax,ou=systems,".$base;
    }  

    /* Create filter */
    if ($ShowSendBocklists){
      $filter = "(&(objectClass=goFaxSBlock)(|(cn=".$Regex.")(goFaxSBlocklist=".$Regex.")))";
      $res= get_list($filter, $this->ui->subtreeACL, $base,array("*"), $Flags);
    }
    if ($ShowReceiveBlocklists){
      $filter2= "(&(objectClass=goFaxRBlock)(|(cn=".$Regex.")(goFaxRBlocklist=".$Regex.")))";
      $res2= get_list($filter2, $this->ui->subtreeACL, $base,array("*"), $Flags);
    }
   
    $this->blocklists = array_merge($res,$res2);

    /* appen && sort */
    $tmp=array();
    foreach($this->blocklists as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->blocklists=array();
    foreach($tmp as $val){
      $this->blocklists[]=$val;
    }
    reset ($this->blocklists);
  }


  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing blocklist object failed"));
    $this->clear_fields();
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListBlocklist->save_object();
    plugin::save_object();

    if(isset($_POST['base'])){
      $this->base = $_POST['base'];
    }
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }
  }


  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Permissions for that base? */
    if ($this->base != ""){
      $new_dn= "cn=".$this->cn.",ou=gofax,ou=systems,".$this->base;
    } else {
      $new_dn= $this->dn;
    }

    if (chkacl($this->acl, "create") != ""){
      $message[]= _("You have no permissions to create a blocklist on this 'Base'.");
    }


    /* check syntax: must cn */
    if ($this->cn == ""){
      $message[]= _("Required field 'Name' is not set.");
    } else {
      if (!is_uid($this->cn)){
        $message[]= _("Required field 'Name' contains invalid characters");
      }
      if ($this->dn == 'new'){
        $ldap= $this->config->get_ldap_link();
        $ldap->cd ("ou=gofax,ou=systems,".$this->config->current["BASE"]);
        $ldap->search ("(&(|(objectClass=goFaxSBlock)(objectClass=goFaxRBlock))(cn=".$this->cn."))", array("cn"));
        if ($ldap->count() != 0){
          $message[]= _("Specified name is already used.");
        }
      }
    }

    /* Is user allowed to create in 'base'? */
    if (chkacl($this->acl, "create") != ""){
      $message[]= _("No permission to create a blocklist on this base.");
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();


    /* Type selection */
    if ($this->type == 0){
      $type= "goFaxSBlock";
    } else {
      $type= "goFaxRBlock";
    }

    /* Add list */
    $this->attrs['objectClass']= $type;
    if (count($this->goFaxBlocklist)){
      $this->attrs["$type"."list"]= $this->goFaxBlocklist;
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->base);
    $ldap->search("cn=$this->cn",array("cn"));
    $ldap->cat($this->dn, array('dn'));
    $a= $ldap->fetch();

    if (count($a)){
      if (!isset($this->attrs["$type"."list"])){
        $this->attrs["$type"."list"]= array();
      }
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify($this->attrs);
      $this->handle_post_events("modify");
    } else {
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
    show_ldap_error($ldap->get_error(), _("Saving blocklist object failed"));
  }


  /* Add number */
  function addNumber($number)
  {
    if (!in_array($number, $this->goFaxBlocklist)){
      $this->goFaxBlocklist[]= $number;
      sort($this->goFaxBlocklist);
    }
  }


  /* Remove number from list */
  function delNumber($numbers)
  {
    $tmp= array();
    foreach ($this->goFaxBlocklist as $val){
      if (!in_array($val, $numbers)){
        $tmp[]= $val;
      }
    }
    $this->goFaxBlocklist= $tmp;
  }


  /* Delete lock */
  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
