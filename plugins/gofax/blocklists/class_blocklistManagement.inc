<?php
class blocklist extends plugin
{
  /* Definitions */
  var $plHeadline= "FAX Blocklists";
  var $plDescription= "This does something";

  var $blocklists       = array();
  var $DivListBlocklist = NULL;
  var $CopyPasteHandler = NULL;

  var $start_pasting_copied_objects = FALSE;
  
  function blocklist (&$config, $ui)
  {
    $this->ui     = $ui;
    $this->dn     = "";
    $this->config = &$config;
    $this->base = $_SESSION['CurrentMainBase']; 
    $this->DivListBlocklist = new divListBlocklist($this->config,$this);

    /* Initialize copy&paste queue */
    if ($this->config->boolValueIsTrue("main", "enableCopyPaste")){
      $this->CopyPasteHandler= new CopyPasteHandler($this->config);
    }
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^goFaxBlocklist_/","/^act$/","/^id$/","/^remove_multiple_blocklists/","/^item_selected/");

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
      if(preg_match("/^goFaxBlocklist_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/^goFaxBlocklist_del_/i","",$key);
        // Post for edit
      }elseif(preg_match("/^goFaxBlocklist_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/^goFaxBlocklist_edit_/i","",$key);
        // Post for new
      }elseif(preg_match("/^goFaxBlocklist_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^remove_multiple_blocklists/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^copy/",$key)){
        $s_action = "copy";
        $s_entry= preg_replace("/^copy_/i", "", $key);
      }elseif(preg_match("/^cut/",$key)){
        $s_action = "cut";
        $s_entry= preg_replace("/^cut_/i", "", $key);
      }

    }
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
      $s_action = "copy_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
      $s_action = "cut_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^editPaste/",$_POST['menu_action'])){
      $s_action = "editPaste";
    }


    /* Create options */
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "goFaxBlocklist_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }


    /***************
      Handle copy & paste  
     ***************/

    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }


    /***************
      Cancel some dialogs  
     ***************/

    /* Cancel dialog */
    if (isset($_POST['edit_cancel']) || 
        isset($_POST['delete_blocklist_cancel']) ||
        isset($_POST['delete_lock'])){

      del_lock ($this->dn);
      $this->dialog = NULL;
      unset($_SESSION['objectinfo']);
    }


    /***************
      Save blocklist 
     ***************/

    /* What about finish? */
    if (isset($_POST['edit_finish']) || isset($_POST['edit_apply'])){

      /* No errors, save object */
      $this->dialog->save_object();
      $message= $this->check();
      if (count ($message) == 0){

        $this->dialog->save ();

        if (!isset($_POST['edit_apply'])){
          del_lock ($this->dn);
          $this->dn= "";
          $this->dialog = NULL;
          unset($_SESSION['objectinfo']);
        }
      } else {
        /* Errors found, show message */
        show_errors ($message);
      }
    }


    /***************
      Create a new blocklist (dialog)
     ***************/

    /* Create new blocklist? */
    if ($s_action=="new"){
      $this->dn= "new";
      $this->dialog = new faxblocktabs($this->config,$this->config->data['TABS']['FAXBLOCKTABS'],$this->dn);
      $this->dialog->set_acl_base($this->base);
    }


    /***************
      Edit blocklist 
     ***************/

    if ($s_action=="edit" && !is_object($this->dialog)){

      $this->dn=$this->blocklists[$s_entry]['dn'];

      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message($user, $this->dn));
      }else{
        add_lock ($this->dn, $this->ui->dn);
      }
      $_SESSION['objectinfo']= trim($this->dn);
      $this->dialog = new faxblocktabs($this->config,$this->config->data['TABS']['FAXBLOCKTABS'],$this->dn);
      $this->dialog->set_acl_base($this->dn);
    }    


    /***************
      Display dialog  
     ***************/

    if(is_object($this->dialog) && ($this->dialog instanceof faxblocktabs)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){

      $this->dns = array();
      $ids = $this->list_get_selected_items();

      if(count($ids)){
        foreach($ids as $id){
          $dn = $this->blocklists[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn;
        }

        $dns_names = "<br><pre>";
        foreach($this->dns as $dn){
          add_lock ($dn, $this->ui->dn);
          $dns_names .= $dn."\n";
        }
        $dns_names .="</pre>";

        /* Lock the current entry, so nobody will edit it during deletion */
        $smarty->assign("info",     sprintf(_("You're about to delete the following blocklists(s) %s"), @LDAP::fix($dns_names)));
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. */
    if (isset($_POST['delete_multiple_blocklist_confirm'])){

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){
        if (preg_match("/d/",$this->ui->get_permissions($this->dn,"gofaxlist/blocklistGeneric"))){
          $this->dn = $dn;
          $this->dialog = new faxblocktabs($this->config,$this->config->data['TABS']['FAXBLOCKTABS'],$this->dn);
          $this->dialog->delete();
          $this->dialog = NULL;
        } else {
          print_red (sprintf(_("You are not allowed to delete the blocklist '%s'!"),$dn));
        }
        /* Remove lock file after successfull deletion */
        del_lock ($dn);
        unset($this->dns[$key]);
      }
      $this->reload();
    }


    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
    if(isset($_POST['delete_multiple_blocklist_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
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

        // Lock this dn for editing
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the blocklist '%s'."), $this->dn));
        $smarty->assign("multiple", false);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /***************
      Remove blocklist confirmed
     ***************/

    /* Finally delete blocklist */
    if (isset($_POST['delete_blocklist_confirm'])){
      if (preg_match("/d/",$this->ui->get_permissions($this->dn,"gofaxlist/blocklistGeneric"))){
        $this->dialog = new faxblocktabs($this->config,$this->config->data['TABS']['FAXBLOCKTABS'],$this->dn);
        $this->dialog->delete();
        $this->dialog = NULL;
        del_lock ($this->dn);
      } else {
        print_red (_("You have no permission to remove this blocklist."));
      }
    }

    /***************
      Divlist dialog 
     ***************/

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListBlocklist->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListBlocklist->parent = $this;
    $this->DivListBlocklist->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListBlocklist->SubSearch){
      $this->DivListBlocklist->AddDepartments($this->DivListBlocklist->selectedBase,3,1);
    }
    $this->reload();
    $this->DivListBlocklist->setEntries($this->blocklists);
    return($this->DivListBlocklist->Draw());
  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array("ou=gofax,ou=systems,". $this->DivListBlocklist->selectedBase));
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
      $res= get_list($filter, "gofaxlist", $base,array("*"), $Flags);
    }
    if ($ShowReceiveBlocklists){
      $filter2= "(&(objectClass=goFaxRBlock)(|(cn=".$Regex.")(goFaxRBlocklist=".$Regex.")))";
      $res2= get_list($filter2, "gofaxlist", $base,array("*"), $Flags);
    }

    $this->blocklists = array_merge($res,$res2);

    /* appen && sort */
    $tmp=array();
    foreach($this->blocklists as $tkey => $val ){

      $acl = $this->ui ->get_permissions($base,"gofaxlist/blocklist");
      if(preg_match("/r/",$acl)){
        $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
      }
    }
    ksort($tmp);
    $this->blocklists=array();
    foreach($tmp as $val){
      $this->blocklists[]=$val;
    }
    reset ($this->blocklists);
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListBlocklist->save_object();
  }





  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){
      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn = $this->blocklists[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"faxblocktabs","FAXBLOCKTABS","gofaxlist");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->blocklists[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","faxblocktabs","FAXBLOCKTABS","gofaxlist");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","faxblocktabs","FAXBLOCKTABS","gofaxlist");
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }

    /* Return C&P dialog */
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){

      /* Load entry from queue and set base */
      $this->CopyPasteHandler->load_entry_from_queue();
      $this->CopyPasteHandler->SetVar("base",$this->DivListBlocklist->selectedBase);

      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
