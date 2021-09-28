<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007 - Fabian Hickert <hickert@gonicus.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/* The sieve management class displays a list of sieve 
 *  scripts for the given mail account. 
 * The account is identified by the parents uid attribute. 
 *
 *  $config       The config object
 *  $dn           The object edited 
 *  $parent       The parent object that provides the uid attribute 
 */
class sieveManagement extends plugin
{
  var $parent = NULL;
  var $scripts= array();  
  var $uattrib = "uid";
  var $current_script  = -1;
  var $current_handler = NULL;
  var $script_to_delete =-1;
  var $sieve_handle = NULL; 
  var $Script_Error = "";
  var $Sieve_Error = "";
  var $create_script = FALSE;

  /* To add new elements we need to know 
   *  Where to add the element              -> add_new_id
   *  Whould we add above or below this id  -> add_above_below
   *  What kind of element should we add    -> add_element_type
   */
  var $add_new_element    = FALSE;
  var $add_new_id         = -1;
  var $add_above_below    = "below";
  var $add_element_type   = "sieve_comment";

  /* If this variable is TRUE, this indicates that we have the 
   *  import dialog opened. 
   */
  var $Import_Script = FALSE;

  var $scriptList = NULL;

  /* Initialize the class and load all sieve scripts 
   *  try to parse them and display errors 
   */ 
  function __construct(&$config,$dn,&$parent,$uattrib)
  {
    /* Check given parameter */
    if(!isset($parent->$uattrib)){
      trigger_error("Sieve Management implementation error. Parameter 4 (".$uattrib.") must be part of the given parent element (".get_class($parent).").");
    }

    $this->uattrib = $uattrib;
    $this->parent = &$parent;
    plugin::__construct($config,$dn);

    // Prepare lists
    $this->scriptList = new sortableListing();
    $this->scriptList->setDeleteable(true);
    $this->scriptList->setEditable(true);
    $this->scriptList->setWidth("100%");
    $this->scriptList->setHeight("400px");
    $this->scriptList->setColspecs(array('20px','*','*','40px','20px','40px'));
    $this->scriptList->setHeader(
        array(
            _("Status"),
            _("Name"),
            _("Information"),
            _("Length"),
            "!"));
    $this->scriptList->setDefaultSortColumn(1);
    $this->scriptList->setAcl('rwcdm'); // All ACLs, we filter on our own here.

    /* Get sieve, if this fail abort class initialization */
    if(!$this->sieve_handle = $this->get_sieve()){
      return;
    }

    /* Get all sieve scripts names */
    if($this->sieve_handle->sieve_listscripts()){
      if (is_array($this->sieve_handle->response)){
        foreach($this->sieve_handle->response as $key => $name){

          $data = array();
          $data['NAME'] = $name;

          if($key == "ACTIVE" && $key === "ACTIVE"){
            $data['ACTIVE'] = TRUE;
          }else{
            $data['ACTIVE'] = FALSE;
          }
          $this->scripts[] = $data;          
        }
      } 
    }

    /* Get script contents */
    foreach($this->scripts as $key => $script){
      $p = new My_Parser($this);
      $this->sieve_handle->sieve_getscript($script['NAME']);

      $script = "";
      foreach($this->sieve_handle->response as $line){
        $script.=$line;
      }

      $this->scripts[$key]['IS_NEW'] = FALSE;;
      $this->scripts[$key]['SCRIPT'] = $script;
      $this->scripts[$key]['ORIG_SCRIPT'] = $script;
      $this->scripts[$key]['MSG']   = "";
      $ret = $p->parse($script);
      if(!$ret){
        $this->scripts[$key]['STATUS']   = FALSE;
        $this->scripts[$key]['MODE']    = "Source";
        $this->scripts[$key]['MSG'] = _("Parse failed")."<font color='red'>".$p->status_text."</font>";
      }else{
        $this->scripts[$key]['STATUS']   = TRUE;
        $this->scripts[$key]['MODE']    = "Structured";
        $this->scripts[$key]['MSG'] = _("Parse successful");
      }
      $this->scripts[$key]['PARSER'] = $p;
      $this->scripts[$key]['EDITED'] = FALSE;
    }
    $this->sieve_handle = $this->sieve_handle;

  }


  /* Return a sieve class handle,
   *  false if login fails
   */
  function get_sieve()
  {
    
    /* Connect to sieve class and try to get all available sieve scripts */
    if(isset($this->config->data['SERVERS']['IMAP'][$this->parent->gosaMailServer])){
      $cfg=  $this->config->data['SERVERS']['IMAP'][$this->parent->gosaMailServer];
      $this->Sieve_Error = "";

      $uattrib = $this->uattrib;

      /* Log into the mail server */
      $this->sieve_handle= new sieve(
          $cfg["sieve_server"], 
          $cfg["sieve_port"], 
          $this->parent->$uattrib, 
          $cfg["password"], 
          $cfg["admin"],
          $cfg["sieve_option"]);

      /* Try to login */
      if (!@$this->sieve_handle->sieve_login()){
        $this->Sieve_Error = $this->sieve_handle->error_raw;
        return(FALSE);
      }
      return($this->sieve_handle);
    }else{
      $this->Sieve_Error = sprintf(_("The specified mail server '%s' does not exist within the GOsa configuration."),
        $this->parent->gosaMailServer);
      return(FALSE);
    }
  }


  /* Handle sieve list 
   */
  function execute()
  {
    plugin::execute();
    /***************
     * Create a new Script 
     ***************/

    /* Force opening the add script dialog */
    if(isset($_POST['create_new_script'])){
      $this->create_script = TRUE;
    }

    /* Close add script dialog, without adding a new one */
    if(isset($_POST['create_script_cancel'])){
      $this->create_script = FALSE;
    }

    /* Display create script dialog 
     *  handle posts, display warnings if specified 
     *  name is not useable. 
     * Create a new script with given name
     */
    if($this->create_script){
    
      /* Set initial name or used posted name if available */
      $name = "";
      if(isset($_POST['NewScriptName'])){
        $name = trim($_POST['NewScriptName']);
      }
 
      /* Check given name */ 
      $err = false;

      /* Is given name in lower case characters ? */
      if(isset($_POST['create_script_save'])){
        if(!strlen($name)){
          $err =true;
          msg_dialog::display(_("Error"), _("No script name specified!"), ERROR_DIALOG);
        }
        /* Is given name in lower case characters ? */
        if($name != strtolower($name)){
          $err =true;
          msg_dialog::display(_("Error"), _("Please use only lowercase script names!"), ERROR_DIALOG);
        }

        /* Only chars are allowed here */
        if(preg_match("/[^a-z]/i",$name)){
          $err =true;
          msg_dialog::display(_("Error"), _("Please use only alphabetical characters in script names!"), ERROR_DIALOG);
        }

        $tmp = $this->get_used_script_names();
        if(in_array_ics($name,$tmp)){
          $err =true;
          msg_dialog::display(_("Error"), _("Script name already in use!"), ERROR_DIALOG);
        }
      }

      /* Create script if everything is ok */
      if($this->create_script && isset($_POST['create_script_save']) && !$err){

        /* Close dialog */
        $this->create_script = FALSE;

        /* Script contents to use */
        $script = "/*New script */".
                  "stop;";

        /* Create a new parser and initialize default values */
        $p = new My_Parser($this);
        $ret = $p->parse($script);
        $sc['SCRIPT'] = $script;
        $sc['ORIG_SCRIPT'] = $script;
        $sc['IS_NEW'] = TRUE;
        $sc['MSG']   = "";
        if(!$ret){
          $sc['STATUS']   = FALSE;
          $sc['MODE']    = "Source";
          $sc['MSG'] = _("Parse failed")."<font color='red'>".$p->status_text."</font>";
        }else{
          $sc['STATUS']   = TRUE;
          $sc['MODE']    = "Structured";
          $sc['MSG'] = _("Parse successful");
        }
        $sc['PARSER'] = $p;
        $sc['EDITED'] = TRUE;
        $sc['ACTIVE'] = FALSE;
        $sc['NAME']   = $name;
      
        /* Add script */
        $this->scripts[$name] = $sc;
      }else{
      
        /* Display dialog to enter new script name */
        $smarty = get_smarty();
        $smarty->assign("NewScriptName",$name);
        return($smarty->fetch(get_template_path("templates/create_script.tpl",TRUE,dirname(__FILE__))));
      }
    }


    /*************
     * Handle several posts 
     *************/

    $this->scriptList->save_object();
    $action = $this->scriptList->getAction();
    if($action['action'] == 'edit'){
        $id = $this->scriptList->getKey($action['targets'][0]);
        $this->current_script = $id;
        $this->current_handler = $this->scripts[$id]['PARSER'];
        $this->scripts[$id]['SCRIPT_BACKUP'] = $this->scripts[$id]['SCRIPT'];
    }
    if($action['action'] == 'delete'){
        $id = $this->scriptList->getKey($action['targets'][0]);
        $this->script_to_delete = $id;
    }

    $once = TRUE;
    foreach($_POST as $name => $value){

      /* Activate script */
      if($this->parent->acl_is_writeable("sieveManagement") && preg_match("/^active_script_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^active_script_/","",$name);
        $once = FALSE;

        /* We can only activate existing scripts */
        if(!$this->scripts[$script]['IS_NEW']){

          /* Get sieve */
          if(!$this->sieve_handle = $this->get_sieve()){
            msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot log into SIEVE server: %s"), '<br><br><i>'.to_string($this->Sieve_Error).'</i>'), ERROR_DIALOG);
          }

          /* Try to activate the given script and update 
           *  class script array. 
           */
          if(!$this->sieve_handle){
            // No sieve handler here... - A warning message will already appear.            
          }else{
            if(!$this->sieve_handle->sieve_setactivescript($this->scripts[$script]['NAME'])){
              msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot retrieve SIEVE script: %s"), '<br><br><i>'.to_string($this->sieve_handler->error_raw).'</i>'), ERROR_DIALOG);
            }else{
              foreach($this->scripts as $key => $data){
                if($key == $script){
                  $this->scripts[$key]['ACTIVE'] = TRUE;
                }else{
                  $this->scripts[$key]['ACTIVE'] = FALSE;
                }
              }
            }
          }
        }
      }
    }

    
    /*************
     * Remove script handling 
     *************/

    /* Remove aborted */
    if(isset($_POST['delete_script_cancel'])){
      $this->script_to_delete = -1;
    }

    /* Remove confirmed */
    if($this->parent->acl_is_writeable("sieveManagement") && isset($_POST['delete_script_confirm'])){

      $script = $this->scripts[$this->script_to_delete];

      /* Just remove from array if it is a new script */
      if($script['IS_NEW']){
        unset($this->scripts[$this->script_to_delete]);
      }else{

        /* Get sieve */
        if(!$this->sieve_handle = $this->get_sieve()){
          msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot log into SIEVE server: %s"), '<br><br><i>'.to_string($this->Sieve_Error).'</i>'), ERROR_DIALOG);
        }

        if(!$this->sieve_handle->sieve_deletescript($this->scripts[$this->script_to_delete]['NAME'])){
          msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot remove SIEVE script: %s"), '<br><br><i>'.to_string($this->sieve_handler->error_raw).'</i>'), ERROR_DIALOG);
        }else{
          unset($this->scripts[$this->script_to_delete]);
        }
      }
      $this->script_to_delete = -1;
    }

    /* Display confirm dialog */
    if($this->script_to_delete != -1){
      $smarty = get_smarty();
      $smarty->assign("Warning",msgPool::deleteInfo($this->scripts[$this->script_to_delete]['NAME']));
      return($smarty->fetch(get_template_path("templates/remove_script.tpl",TRUE,dirname(__FILE__))));
    }


    /**************
     * Save script changes 
     **************/

    /* Abort saving */
    if(isset($_POST['cancel_sieve_changes'])){
      $tmp = $this->scripts[$this->current_script]['SCRIPT_BACKUP'];
      $this->scripts[$this->current_script]['SCRIPT'] = $tmp;
      $this->scripts[$this->current_script]['PARSER']->parse($tmp);
      $this->current_handler = NULL;
    }

    /* Save currently edited sieve script. */
    if($this->parent->acl_is_writeable("sieveManagement") && 
       isset($_POST['save_sieve_changes']) && 
       is_object($this->current_handler)){
      $chk = $this->current_handler->check();
      if(!count($chk)){

        $sc = $this->scripts[$this->current_script]['SCRIPT'];
        $p = new My_Parser($this);
        if($p -> parse($sc)){

          if($this->scripts[$this->current_script]['MODE'] == "Source-Only"){
            $this->scripts[$this->current_script]['MODE'] = "Source";
          }
  
          $this->scripts[$this->current_script]['PARSER'] = $p;
          $this->scripts[$this->current_script]['EDITED'] = TRUE;
          $this->scripts[$this->current_script]['STATUS'] = TRUE;
          $this->scripts[$this->current_script]['MSG'] = _("Edited");
          $this->current_handler = NULL;
        }else{
          msg_dialog::display(_("SIEVE error"), $p->status_text, ERROR_DIALOG);
        }
      }else{
        foreach($chk as $msgs){
          msg_dialog::display(_("SIEVE error"), $msgs, ERROR_DIALOG);
        }
      }
    }


    /*************
     * Display edit dialog 
     *************/

    /* Display edit dialog, depending on Mode display different uis
     */
    if($this->current_handler){

        if(isset($_POST['Import_Script'])){
          $this->Import_Script = TRUE;
        }

        if(isset($_POST['Import_Script_Cancel'])){
          $this->Import_Script = FALSE;
        }

        if(isset($_POST['Import_Script_Save']) && isset($_FILES['Script_To_Import'])){

          $file     = $_FILES['Script_To_Import'];

          $filename = gosa_file_name($file['tmp_name']);

          if($file['size'] == 0){
            msg_dialog::display(_("Error"), _("Uploaded script is empty!"), ERROR_DIALOG);
          }elseif(!file_exists($filename)){
            msg_dialog::display(_("Internal error"), sprintf(_("Cannot access temporary file '%s'!"), $filename), ERROR_DIALOG);
          }elseif(!is_readable ($filename)){
            msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot open temporary file '%s'!"), $filename), ERROR_DIALOG);
          }else{
            
            
 
            $contents = file_get_contents($filename);
           
            $this->scripts[$this->current_script]['SCRIPT'] = $contents;
            if(!$this->current_handler->parse($contents)){
              $this->scripts[$this->current_script]['MODE'] = "Source";
            }else{
              $this->scripts[$this->current_script]['MODE'] = "Structured";
            }
            $this->Script_Error = "";
            $this->Import_Script = FALSE;
          }
        }

        if($this->Import_Script){
          $smarty = get_smarty();
          $str = $smarty->fetch(get_template_path("templates/import_script.tpl",TRUE,dirname(__FILE__)));
          return($str);
        }
  

        /* Create dump of current sieve script */
        if(isset($_POST['Save_Copy'])){

            /* force download dialog */
            header("Content-type: application/tiff\n");
            if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
                    preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
                header('Content-Disposition: filename="dump.script"');
            } else {
                header('Content-Disposition: attachment; filename="dump.script"');
            }
            header("Content-transfer-encoding: binary\n");
            header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
            header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
            header("Cache-Control: no-cache");
            header("Pragma: no-cache");
            header("Cache-Control: post-check=0, pre-check=0");
            echo $this->scripts[$this->current_script]['SCRIPT'];
            exit();
        }


      /****
       * Add new element to ui
       ****/

      /* Abort add dialog */ 
      if(isset($_POST['select_new_element_type_cancel'])){
        $this->add_new_element = FALSE;
      }

      /* Add a new element */
      if($this->add_new_element){

        $element_types= array(
            "sieve_keep"      => _("Keep"),
            "sieve_comment"   => _("Comment"),
            "sieve_fileinto"  => _("File into"),
            "sieve_keep"      => _("Keep"),
            "sieve_discard"   => _("Discard"),
            "sieve_redirect"  => _("Redirect"),
            "sieve_reject"    => _("Reject"),
            "sieve_require"   => _("Require"),
            "sieve_stop"      => _("Stop"),
            "sieve_vacation"  => _("Vacation message"),
            "sieve_if"        => _("If"));


        /* Element selected */
        if(isset($_POST['element_type']) && isset($element_types[$_POST['element_type']]) 
           || isset($_POST['element_type']) &&in_array_strict($_POST['element_type'],array("sieve_else","sieve_elsif"))){
          $this->add_element_type = $_POST['element_type'];
        }

        /* Create new element and add it to
         *  the selected position 
         */
        if(isset($_POST['select_new_element_type'])){
          if($this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below)){
            $this->add_new_element = FALSE;
          }else{
            msg_dialog::display(_("SIEVE error"), _("Cannot add new element!") , ERROR_DIALOG);
          }
        }
      }

      /* Only display select dialog if it is necessary */
      if($this->add_new_element){
        $smarty = get_smarty();
    
        $add_else_elsif = FALSE;

        /* Check if we should add else/elsif to the select box 
         *  or not. We can't add else twice!.
         */
        if($this->add_above_below == "below"){

          /* Get posistion of the current element 
           */
          foreach($this->current_handler->tree_->pap as $key => $obj){
        
            if($obj->object_id == $this->add_new_id && in_array_strict(get_class($obj),array("sieve_if","sieve_elsif"))){
  
              /* Get block start/end */
              $end_id = $this->current_handler->tree_->get_block_end($key);
              $else_found = FALSE;
              $elsif_found = FALSE;
          
              /* Check if there is already an else in this block 
               */
              for($i =  $key ; $i < $end_id ; $i ++){
                if(get_class($this->current_handler->tree_->pap[$i]) == "sieve_else"){
                  $else_found = TRUE;
                }
                if(get_class($this->current_handler->tree_->pap[$i]) == "sieve_elsif"){
                  $elsif_found = TRUE;
                }
              }
  
              /* Only allow adding 'else' if there is currently 
               *  no 'else' statement. And don't allow adding 
               *  'else' before 'elseif'
               */ 
              if(!$else_found && (!(get_class($obj) == "sieve_if" && $elsif_found))){
                $element_types['sieve_else'] = _("Else");
              }
              $element_types['sieve_elsif'] = _("Else If");
            }
          }
        }

        $smarty->assign("element_types",$element_types );
        $smarty->assign("element_type",$this->add_element_type);
        $str = $smarty->fetch(get_template_path("templates/add_element.tpl",TRUE,dirname(__FILE__)));
        return($str);
      }



      /****************
       * Handle test posts 
       ****************/

      /* handle some special posts from test elements 
       */
      foreach($_POST as $name => $value){
        if(preg_match("/^Add_Test_Object_/",$name)) {
          $name = preg_replace("/^Add_Test_Object_/","",$name);

          $test_types_to_add = array(
              "address" =>_("Address"),
              "header"  =>_("Header"),
              "envelope"=>_("Envelope"),
              "size"    =>_("Size"),
              "exists"  =>_("Exists"),
              "allof"   =>_("All of"),
              "anyof"   =>_("Any of"),
              "true"    =>_("True"),
              "false"   =>_("False"));

          $smarty = get_smarty();
          $smarty->assign("ID",$name);
          $smarty->assign("test_types_to_add",$test_types_to_add);
          $ret = $smarty->fetch(get_template_path("templates/select_test_type.tpl",TRUE,dirname(__FILE__)));
          return($ret);
        }
      }

      $current = $this->scripts[$this->current_script];

      /* Create html results */
      $smarty = get_smarty();
      $smarty->assign("Mode",$current['MODE']);
      if($current['MODE'] == "Structured"){
        $smarty->assign("Contents",$this->current_handler->tree_->execute());
      }else{
        $smarty->assign("Contents",$current['SCRIPT']);
      }
      $smarty->assign("Script_Error",$this->Script_Error);
      $ret = $smarty->fetch(get_template_path("templates/edit_frame_base.tpl",TRUE,dirname(__FILE__)));
      return($ret);
    }


    /* Create list of available sieve scripts 
     */
    $data = $lData = array();
    $this->scriptList->setAcl($this->parent->getacl("sieveManagement"));
    foreach($this->scripts as $key => $script){
      $active =  $script['ACTIVE'];
      $state = "&nbsp;";
      if($active){
        $state = image('images/true.png','',_("This script is marked as active"));
      }
      $name = $script['NAME'];
      $msg = $script['MSG'];
      $length = strlen($script['SCRIPT']);
      if($active || $script['IS_NEW']){
          $activate = "&nbsp;";
      }else{
          $activate = image('images/true.png','active_script_'.$key,_("Activate script"));
      }
      $data[$key] = $key;
      $lData[$key] = array('data' => array($state, $name, $msg, $length,$activate));
    }
    $this->scriptList->setListData($data,$lData);
    $this->scriptList->update();

    /* If the uattrib is empty   (Attribute to use for authentification with sieve)
     *  Display a message that the connection can't be established.
     */
    $uattrib = $this->uattrib;
    $smarty = get_smarty();

    if(!$this->get_sieve()){
      $smarty->assign("Sieve_Error",sprintf(
        _("Can't log into SIEVE server. Server says '%s'."),
          to_string($this->Sieve_Error)));
    }else{
      $smarty->assign("Sieve_Error","");
    }

    $smarty->assign("uattrib_empty",empty($this->parent->$uattrib));
    $smarty->assign("List",$this->scriptList->render());
    return($smarty->fetch(get_template_path("templates/management.tpl",TRUE,dirname(__FILE__))));
  }


  /* Add a new element to the currently opened script editor.
   * The insert position is specified by 
   */
  function add_new_element_to_current_script($type,$id,$position)
  {
    /* Test given data */
    if(!in_array_ics($position,array("above","below"))){
      trigger_error("Can't add new element with \$position=".$position.". Only 'above','below' are allowed here.");
      return(FALSE);
    }
    if(!is_numeric($id)){
      trigger_error("Can't add new element, given id is not numeric.");
      return(FALSE);
    }
    if(!class_available($type)){
      if(!empty($type)){
        trigger_error("Can't add new element, given \$class=".$class." does not exists.");
      }
      return(FALSE);
    }
    if(!is_object($this->current_handler) || get_class($this->current_handler) != "My_Parser"){
      trigger_error("Can't add new element, there is no valid script editor opened.");
      return(FALSE);
    }

    /* These element types are allowed to be added here */
    $element_types= array(
        "sieve_keep"      => _("Keep"),
        "sieve_comment"   => _("Comment"),
        "sieve_fileinto"  => _("File into"),
        "sieve_keep"      => _("Keep"),
        "sieve_discard"   => _("Discard"),
        "sieve_redirect"  => _("Redirect"),
        "sieve_reject"    => _("Reject"),
        "sieve_require"   => _("Require"),
        "sieve_stop"      => _("Stop"),
        "sieve_vacation"  => _("Vacation message"),
        "sieve_if"        => _("If"));

    /* Check if we should add else/elsif to the select box
     *  or not. We can't add else twice!.
     */

    /* Get posistion of the current element
     */
    foreach($this->current_handler->tree_->pap as $key => $obj){

      if($obj->object_id == $id && in_array_strict(get_class($obj),array("sieve_if","sieve_elsif"))){

        /* Get block start/end */
        $end_id = $this->current_handler->tree_->get_block_end($key);
        $else_found = FALSE;
        $elsif_found = FALSE;

        /* Check if there is already an else in this block
         */
        for($i =  $key ; $i < $end_id ; $i ++){
          if(get_class($this->current_handler->tree_->pap[$i]) == "sieve_else"){
            $else_found = TRUE;
          }
          if(get_class($this->current_handler->tree_->pap[$i]) == "sieve_elsif"){
            $elsif_found = TRUE;
          }
        }

        if($this->add_above_below == "below"){

          /* Only allow adding 'else' if there is currently
           *  no 'else' statement. And don't allow adding
           *  'else' before 'elseif'
           */
          if(!$else_found && (!(get_class($obj) == "sieve_if" && $elsif_found))){
            $element_types['sieve_else'] = _("Else");
          }
          $element_types['sieve_elsif'] = _("Else If");
        }else{
         
          /* Allow adding elsif above elsif */ 
          if(in_array_strict(get_class($obj),array("sieve_elsif"))){
            $element_types['sieve_elsif'] = _("Else If");
          }
        }
      }
    }

    if(!isset($element_types[$type])){
      msg_dialog::display(_("SIEVE error"), _("Cannot insert element at the requested position!") , ERROR_DIALOG);
      return;
    }


    /* Create elements we should add 
     * -Some element require also surrounding block elements
     */
    $parent = $this->current_handler->tree_;
    if($this->add_element_type == "sieve_if"){
      $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
    }elseif($this->add_element_type == "sieve_else"){
      $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
    }elseif($this->add_element_type == "sieve_elsif"){
      $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
      $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
    }elseif($this->add_element_type == "sieve_vacation"){

      /* Automatically add addresses to sieve alternate addresses */
      $data = NULL;
      $tmp = new $this->add_element_type($data, preg_replace("/[^0-9]/","",microtime()),$parent);
      if(isset($this->parent->gosaMailAlternateAddress)){
        $tmp->addresses = $this->parent->gosaMailAlternateAddress;
      }
      $ele[] = $tmp ;
    }else{
      $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
    }

    /* Get index of the element identified by object_id == $id; 
     */
    $index = -1;
    $data = $this->current_handler->tree_->pap;
    foreach($data as $key => $obj){
      if($obj->object_id == $id && $index==-1){
        $index = $key;
      }
    }

    /* Tell to user that we couldn't find the given object 
     *  so we can't add an element. 
     */
    if($index == -1 ){
      trigger_error("Can't add new element, specified \$id=".$id." could not be found in object tree.");
      return(FALSE);
    }

    /* We have found the specified object_id 
     *  and want to detect the next free position 
     *  to insert the new element.
     */
    if($position == "above"){
      $direction ="up";
      $next_free = $this->current_handler->tree_->_get_next_free_move_slot($index,$direction,TRUE);
    }else{
      $direction = "down";
      $next_free = $this->current_handler->tree_->_get_next_free_move_slot($index,$direction,TRUE);
    }
    /* This is extremly necessary, cause some objects 
     *  updates the tree objects ... Somehow i should change this ... 
     */
    $data = $this->current_handler->tree_->pap;
    $start = $end = array();

    if($position == "above"){
      $start = array_slice($data,0,$next_free);
      $end   = array_slice($data,$next_free);
    }else{
      $start = array_slice($data,0,$next_free+1);
      $end   = array_slice($data,$next_free+1);
    }

    $new = array();
    foreach($start as $obj){
      $new[] = $obj;
    }
    foreach($ele as $el){
      $new[] = $el;
    }
    foreach($end as $obj){
      $new[] = $obj;
    }
    $data= $new;
    $this->current_handler->tree_->pap = $data;
    return(TRUE);
  }



  function save_object()
  {
    if($this->current_handler){

      if(isset($_GET['Add_Object_Top_ID'])){
        $this->add_new_element    = TRUE;
        $this->add_new_id         = $_GET['Add_Object_Top_ID'];
        $this->add_above_below    = "above";
      }  

      if(isset($_GET['Add_Object_Bottom_ID'])){
        $this->add_new_element    = TRUE;
        $this->add_new_id         = $_GET['Add_Object_Bottom_ID'];
        $this->add_above_below    = "below";
      }  

      if(isset($_GET['Remove_Object_ID'])){
        $found_id = -1;
        foreach($this->current_handler->tree_->pap as $key => $element){
          if($element->object_id == $_GET['Remove_Object_ID']){
            $found_id = $key;
          }
        }
        if($found_id != -1 ){
          $this->current_handler->tree_->remove_object($found_id);  
        }
      }  
 
      if(isset($_GET['Move_Up_Object_ID'])){
        $found_id = -1;
        foreach($this->current_handler->tree_->pap as $key => $element){
          if($element->object_id == $_GET['Move_Up_Object_ID']){
            $found_id = $key;
          }
        }
        if($found_id != -1 ){
          $this->current_handler->tree_->move_up_down($found_id,"up");
        }
      }  
 
      if(isset($_GET['Move_Down_Object_ID'])){
        $found_id = -1;
        foreach($this->current_handler->tree_->pap as $key => $element){
          if($element->object_id == $_GET['Move_Down_Object_ID']){
            $found_id = $key;
          }
        }
        if($found_id != -1 ){
          $this->current_handler->tree_->move_up_down($found_id,"down");
        }
      }  
  

      /* Check if there is an add object requested 
       */
      $data = $this->current_handler->tree_->pap;
      $once = TRUE;
      foreach($_POST as $name => $value){
        foreach($data as $key => $obj){
          if(isset($obj->object_id) && preg_match("/^Add_Object_Top_".$obj->object_id."/",$name) && $once){
            $once = FALSE;
            $this->add_element_type   =  $_POST['element_type_'.$obj->object_id];
            $this->add_new_element    = FALSE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "above";
            $this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below);
          }
          if(isset($obj->object_id) && preg_match("/^Add_Object_Bottom_".$obj->object_id."/",$name) && $once){
            $once = FALSE;
            $this->add_element_type   =  $_POST['element_type_'.$obj->object_id];
            $this->add_new_element    = FALSE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "below";
            $this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below);
          }
        
          if(isset($obj->object_id) && preg_match("/^Remove_Object_".$obj->object_id."/",$name) && $once){
            $once = FALSE;
            $this->current_handler->tree_->remove_object($key);
          }
          if(isset($obj->object_id) && preg_match("/^Move_Up_Object_".$obj->object_id."/",$name) && $once){
            $this->current_handler->tree_->move_up_down($key,"up");
            $once = FALSE;
          }
          if(isset($obj->object_id) && preg_match("/^Move_Down_Object_".$obj->object_id."/",$name) && $once){
            $this->current_handler->tree_->move_up_down($key,"down");
            $once = FALSE;
          }
        }
      }

      /* Skip Mode changes and Parse tests 
       *  if we are currently in a subdialog 
       */

      $this->current_handler->save_object();
      $Mode = $this->scripts[$this->current_script]['MODE'];
      $skip_mode_change = false;
      if(in_array_strict($Mode,array("Source-Only","Source"))){
        if(isset($_POST['script_contents'])){
          $sc = stripslashes($_POST['script_contents']);
          $this->scripts[$this->current_script]['SCRIPT'] = $sc;
          $p = new My_Parser($this);
          if($p -> parse($sc)){
            $this->Script_Error = "";
          } else {
            $this->Script_Error = $p->status_text;
            $skip_mode_change = TRUE;
          }
        }
      }
      if(in_array_strict($Mode,array("Structured"))){
        $sc = $this->current_handler->get_sieve_script();
        $this->scripts[$this->current_script]['SCRIPT'] = $sc;
        $p = new My_Parser($this);
        if($p -> parse($sc)){
          $this->Script_Error = "";
        } else {
          $this->Script_Error = $p->status_text;
          $skip_mode_change = TRUE;
        }
      }
      if(!$skip_mode_change){
        if($this->scripts[$this->current_script]['MODE'] != "Source-Only"){
          $old_mode = $this->scripts[$this->current_script]['MODE'];
          if(isset($_POST['View_Source'])){
            $this->scripts[$this->current_script]['MODE'] = "Source";
          }
          if(isset($_POST['View_Structured'])){
            $this->scripts[$this->current_script]['MODE'] = "Structured";
          }
          $new_mode = $this->scripts[$this->current_script]['MODE'];

          if($old_mode != $new_mode){

            $sc = $this->scripts[$this->current_script]['SCRIPT'];
            $p = new My_Parser($this);

            if($p -> parse($sc)){
              $this->current_handler->parse($sc);
              $this->Script_Error = "";
            } else {
              $this->Script_Error = $p->status_text;
            }
          } 
        }
      }
    }
  }

  
  function get_used_script_names()
  {
    $ret = array();
    foreach($this->scripts as $script){
      $ret[] = $script['NAME'];
    }
    return($ret);
  }



  function save()
  {
    /* Get sieve */
    if(!$this->sieve_handle = $this->get_sieve()){
      msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot log into SIEVE server: %s"), '<br><br><i>'.to_string($this->Sieve_Error).'</i>'), ERROR_DIALOG);
      return;
    }

    $everything_went_fine = TRUE;

    foreach($this->scripts as $key => $script){
      if($script['EDITED']){
        $data = $this->scripts[$key]['SCRIPT'];
        if(!$this->sieve_handle->sieve_sendscript($script['NAME'], addcslashes ($data,"\\"))){
          new log("modify","users/mailAccount".get_class($this),$script['NAME'],"Failed to save sieve script named '".$script['NAME']."': ".to_string($this->sieve_handle->error_raw));

          $everything_went_fine = FALSE;
          msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot store SIEVE script: %s"), '<br><br><i>'.to_string($this->sieve_handle->error_raw).'</i>'), ERROR_DIALOG);
          $this->scripts[$key]['MSG'] = "<font color='red'>".
                                           _("Failed to save sieve script").": ".
                                           to_string($this->sieve_handle->error_raw).
                                           "</font>";
        }
      }
    }
    return($everything_went_fine);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
