<?php

define("LDAP_DUMP_PATH","/tmp/gosa");

class CopyPasteHandler {

  var $config;
  var $current;

  /* This array contains all dns of the currently copyied objects */
  var $queue       = array(); 

  /* Attributes that should be overwritten */
  var $setvar_array= array();

  /* The dn of the last edited object */
  var $lastdn      = "";


  /* Create CP handler  */
  function CopyPasteHandler(&$config)
  {
    $this->config = &$config;	
    $this->current= NULL;
    $this->queue  = array();
    $this->setvar_array = array();
  }


  /* Entry entry to Copy & Paste queue.
   * A Queue entry is represented as follows.
   *  array['file_name']  - Position on hdd 
   *  array['method']     - copy/cut
   *  array['dn']         - the dn of the object added to the queue 
   *  array['tab_class']  - Tab object that should be used to initialize the new object
   *  array['tab_object'] - Tab object name used to initialize correct object Type like USERTABS
   */
  function add_to_queue($dn,$action,$tab_class,$tab_object,$tab_acl_category)
  {
    if(!class_exists($tab_class)){
      trigger_error(sprintf("Specified class object '%s' does not exists.",$tab_class));
      return(FALSE);
    }

    if(!isset($this->config->data['TABS'][$tab_object])){
      trigger_error(sprintf("Specified tab object '%s' does not exists.",$tab_object));
      return(FALSE);
    }

    if(!in_array($action,array("cut","copy"))){
      trigger_error(sprintf("Specified action '%s' does not exists for copy & paste.",$action));
      return(FALSE);
    } 

    if($file_name = $this->save_dn_attributes_to_hdd($dn)){
      $tmp = array();
      $tmp['file_name'] = $file_name;
      $tmp['method']    = $action;  
      $tmp['dn']        = $dn;
      $tmp['tab_class'] = $tab_class;
      $tmp['tab_object']= $tab_object;
      $tmp['tab_acl_category']= $tab_acl_category;
      $this->queue[]    = $tmp; 
    }
  }


  /* This removes all objects from queue.
   * Remove hdd dumps of current entries too.
   * Remove entries older than 24 hours.
   */
  function cleanup_queue()
  {
    $this->current = FALSE;
    $this->setvar_array = array();

    /* Remove all entries from queue */  
    foreach($this->queue as $key => $entry){
      @rmdir($entry['file_name']);  
      unset($this->queue[$key]);
    }

    /* Create patch if it doesn't exists */
    if(!is_dir(LDAP_DUMP_PATH)){
      @mkdir(LDAP_DUMP_PATH);
    }    
    
    /* check if we are able to create a new file the given directory */
    if(!is_writeable(LDAP_DUMP_PATH)){
      $msg= sprintf(_("Cannot cleanup copy & paste queue: no write permission inside '%s'!"),LDAP_DUMP_PATH);
      msg_dialog::display(_("Configuration error"), $msg, ERROR_DIALOG);
      new log("copy","all/all","copy & paste, event queue.",array(), $msg);
      return(FALSE);
    }

    /* Remove entries from hdd that are older than24 hours */
    $fp = opendir(LDAP_DUMP_PATH);
    while($file = readdir($fp)){
      if(is_file(LDAP_DUMP_PATH."/".$file) && !preg_match("/^\./",$file)){
        $file_time = fileatime(LDAP_DUMP_PATH."/".$file);
        if($file_time < (time() - (24* 60 *60))){
          @unlink(LDAP_DUMP_PATH."/".$file);
        }
      }
    }
  }


  /* To increase performance we save the ldap dump on hdd 
   * This function automatically creates the dumps and returns 
   *  the name of the dumpfile we created 
   */
  function save_dn_attributes_to_hdd($dn)
  {
    $filename = "Should not be returned";
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $res  = $ldap->cat($dn);

    /* Check if given dn is valid and ldap search was succesfull */ 
    if(!$res){
      $msg= sprintf(_("Copy and paste failed: object '%s' is not a valid ldap object!"), LDAP::fix($dn));
      msg_dialog::display(_("Internal error"), $msg, ERROR_DIALOG);
      new log("copy","all/all",$dn,array(), $msg);
      return(FALSE);
    }

    /* Create data to save given ldap dump on the hdd */
    $filename = "gosa_copy-paste_dump_".preg_replace("/[^0-9]/","",microtime());
    $path     = LDAP_DUMP_PATH;

    /* Create patch if it doesn't exists */
    if(!is_dir($path)){
      @mkdir($path);
    }    

    /* check if we are able to create a new file the given directory */
    if(!is_writeable($path)){
      $msg= sprintf(_("Cannot save LDAP dump: no write permission inside '%s'!"),LDAP_DUMP_PATH);
      msg_dialog::display(_("Configuration error"), $msg, ERROR_DIALOG);
      new log("copy","all/all",$dn,array(), $msg);
      return(FALSE);
    }  

    /* Create file handle */
    $fp = @fopen($path."/".$filename,"w+");
    if(!$fp){
      $msg= sprintf(_("Cannot save LDAP dump: no write permission to '%s/%s'!"),$path,$filename);
      msg_dialog::display(_("Configuration error"), $msg, ERROR_DIALOG);
      new log("copy","all/all",$dn,array(), $msg);
      return(FALSE);
    }    

    $data = serialize($ldap->fetch());
    fwrite($fp,$data,strlen($data));
    fclose($fp);

    /* Only the webserver should be able to read those files */
    @chmod($path."/".$filename,0600); 
    return($path."/".$filename);
  }


  /* Check if there are still entries the object queue */
  function entries_queued()
  {
    return( count($this->queue) >=1 || $this->current != FALSE);
  }


  /* Paste one entry from queue */
  function load_entry_from_queue()
  {

    /* Save posted variables, handle dialog posts like 'cancel' */
    $this->save_object();

    /* If there is currently no object pasted 
     *  create new object and prepare object to be pasted
     */
    if(!$this->current && $this->entries_queued()){
      $key    = key($this->queue);
      $entry  = $this->queue[$key];
      $tab_c = $entry['tab_class'];
      $tab_o = $entry['tab_object'];
      $tab_a = $entry['tab_acl_category'];

      if($entry['method'] == "copy"){
        $entry['object']      = new $tab_c($this->config,$this->config->data['TABS'][$tab_o],"new",$tab_a);
      }else{
        $entry['object']      = new $tab_c($this->config,$this->config->data['TABS'][$tab_o],$entry['dn'],$tab_a);
      }

      $entry['source_data'] = $this->load_attributes_from_hdd($entry['file_name']);

      if($entry['method'] == "copy"){

        /* Prepare each plugin of this tab object to be posted */
        foreach($entry['object']->by_object as $name => $obj){

          /* Prepare every single class, to be copied  */
          $entry['object']->by_object[$name]->PrepareForCopyPaste($entry['source_data']);

          /* handle some special vars */
          foreach(array("is_account") as $attr){
            if(isset($entry['source_data'][$attr])){
              $entry['object']->by_object[$name]->$attr = $entry['source_data'][$attr];
            }
          }
        }
      }

      /* Assign created object as current */
      $this->current = $entry;
      unset($this->queue[$key]);
    }
  }


  /* Load dumped ldap entry specified by $filename and 
   *  return data an unserailized data array
   */
  function load_attributes_from_hdd($filename)
  {
    $fp = @fopen($filename,"r");
    if(is_file($filename) && is_readable($filename) && $fp){
      $data = "";
      while($str = fgets($fp,512)){
        $data .= $str;
      }
      return(unserialize($data));
    }else{
      print_red(sprintf(_("Could not load dumped file '%s', from hard disk drive."),$filename));
      new log("copy","all/all",$dn,array(), 
          sprintf(sprintf("Could not load dumped file '%s', from hard disk drive.",$filename)));
      return(FALSE);
    }
  }


  /* Displays a dialog which allows the user to fix all dependencies of this object.
     Create unique names, ids, or what ever */
  function execute()
  {
    $ui = get_userinfo();
    $type = $this->current['method'];
    if($type == "cut"){
      if(isset($_POST['PerformCopyPaste'])){
        while($this->entries_queued()){
          $this->load_entry_from_queue();      
          $this->_update_vars();

          $msgs = $this->check();
          $acl = $ui->get_category_permissions($this->current['dn'], $this->current['tab_acl_category']);
 
          /* Check permissions */ 
          if(!preg_match("/((c|w)|(w|c))/",$acl)){
            print_red(sprintf(_("You are not allowed to cut and paste the following object '%s'."),$this->current['dn']));
          }elseif(count ($msgs) ){
            foreach( $msgs as $msg){
              print_red($msg);
            }
          }else{

            /* Load next queue entry */
            $this->lastdn = $this->current['object']->dn;
            $this->current['object']->save();
          }

          $this->current =FALSE;
        }
      }
      if($this->current){

        $dns = $this->current['dn']."\n";
        foreach($this->queue as $entry){
          $dns .= $entry['dn']."\n";
        }

        $smarty = get_smarty();
        $smarty->assign("type","cut");
        $smarty->assign("Complete",false);
        $smarty->assign("AttributesToFix","&nbsp;");
        $smarty->assign("SubDialog",$this->current['object']->SubDialog);
        $smarty->assign("objectDN"     ,$dns);
        $smarty->assign("message", sprintf(_("You are going to paste the cutted entry '%s'."), "<pre>".$dns."</pre>"));
        return($smarty->fetch(get_template_path("copyPasteDialog.tpl",FALSE)));
      }
    }
    if($type == "copy"){
      if(isset($_POST['PerformCopyPaste'])){
        $this->_update_vars();
        $msgs = $this->check();

        $acl = $ui->get_category_permissions($this->current['dn'], $this->current['tab_acl_category']);
 
        /* Check permissions */ 
        if(!preg_match("/((c|w)|(w|c))/",$acl)){
          print_red(sprintf(_("You are not allowed to copy and paste the following object '%s'."),$this->current['dn']));
        }elseif(count ($msgs) ){
          foreach( $msgs as $msg){
            print_red($msg);
          }
        }else{
          $this->current['object']->save();
          $this->lastdn = $this->current['object']->dn;
          $this->current =FALSE;

          /* Load next queue entry */
          $this->load_entry_from_queue();
        }
      }
      if($this->current){
        $smarty = get_smarty();
        $smarty->assign("type","copy");
        $smarty->assign("Complete",false);
        $smarty->assign("AttributesToFix",$this->generateAttributesToFix());
        $smarty->assign("SubDialog",$this->current['object']->SubDialog);
        $smarty->assign("objectDN"     ,$this->current['source_data']['dn']);
        $smarty->assign("message", sprintf(_("You are going to paste the copied entry '%s'."), $this->current['source_data']['dn']));
        return($smarty->fetch(get_template_path("copyPasteDialog.tpl",FALSE)));
      }
    }
  }


  /* Return the dn of the last edited entry */
  function last_entry()
  {
    return($this->lastdn);
  }


  /* Save new values posted by copy & paste dialog */
  function save_object()
  {
    if(isset($_POST['abort_current_cut-copy_operation'])){
      $this->current = FALSE;
    }

    if(isset($_POST['abort_all_cut-copy_operations'])){
      $this->cleanup_queue();
      $this->current = FALSE;
    }

    /* Assign posted var to all tabs
     */
    if($this->current){
      $this->current['object']->saveCopyDialog();
    }
  }


  /* Create dialog which asks unique attributes/values ... 
   *  call tabs -> getCopyDialog() 
   *    which calls tab -> getCopyDialog()  */
  function generateAttributesToFix()
  {
    if($this->current){
      return($this->current['object']->getCopyDialog());  
    }
  }


  /* Set a single attribute to specified value
   *  example :   ("base", $newBase );    */
  function SetVar($name,$value)
  {
    $this->setvar_array[$name]=$value; 
  }


  /* Update current object attributes, collected via SetVar */
  function _update_vars()
  {
    if($this->current){

      /* Update all attributes specified with SetVar */
      foreach($this->setvar_array as $name => $value){
        if(isset($this->current['object']->$name)){
          $this->current['object']->$name = $value;
        }
      }

      /* Walk through tabs */
      foreach($this->current['object']->by_object as $key => $obj){

        /* Update all attributes specified with SetVar */
        foreach($this->setvar_array as $name => $value){
          if(isset($this->current['object']->by_object[$key]->$name)){
            $this->current['object']->by_object[$key]->$name = $value;
          }
        }
      }
    }
  }


  /* Returns errors from including tabs. */
  function check()
  {
    $ret = array();
    foreach($this->current['object']->by_object as $obj){
      if($obj->is_account || $obj->ignore_account){
        $ret = array_merge($ret , $obj->check());
      }
    }
    return($ret);
  }


  /* returns the paste icon for headpages */ 
  function generatePasteIcon()
  {
    $Copy_Paste= "&nbsp;<img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    if($this->entries_queued()){
      $img= "images/copypaste.png";
      $Copy_Paste.= "<input type='image' name='editPaste' class='center'
        src='".$img."' alt='"._("Paste")."'>&nbsp;";
    }else{
      $Copy_Paste.= "<img class='center' src='images/cant_editpaste.png' alt=\""._("Can't paste")."\">&nbsp;";
    }
    return ($Copy_Paste);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
