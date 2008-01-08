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
  var $acl = "";

  /* Initialize the class and load all sieve scripts 
   *  try to parse them and display errors 
   */ 
  function sieveManagement($config,$dn,$parent,$uattrib)
  {
    /* Use mail Acls here */    
    $ui = get_userinfo();
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "mailAccount", $ui->dn);

    /* Check given parameter */
    if(!isset($parent->$uattrib)){
      trigger_error("Sieve Management implementation error. Parameter 4 must be part of the given parent element.");
    }

    $this->uattrib = $uattrib;
    $this->parent = $parent;
    plugin::plugin($config,$dn);

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
          $cfg["admin"]);

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
          print_red(_("You should specify a name for your new script."));
        }
        /* Is given name in lower case characters ? */
        if($name != strtolower($name)){
          $err =true;
          print_red(_("Only lower case names are allowed."));
        }

        /* Only chars are allowed here */
        if(preg_match("/[^a-z]/i",$name)){
          $err =true;
          print_red(_("Only alphabetical characters are allowed in script names."));
        }

        $tmp = $this->get_used_script_names();
        if(in_array_ics($name,$tmp)){
          $err =true;
          print_red(_("The specified name is already in use."));
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

    $once = TRUE;
    foreach($_POST as $name => $value){

      /* Edit script requested */
      if(preg_match("/^editscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^editscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        $this->current_script = $script;
        $this->current_handler = $this->scripts[$script]['PARSER'];
        $this->scripts[$script]['SCRIPT_BACKUP'] = $this->scripts[$script]['SCRIPT'];
      }

      /* remove script requested */
      if(chkacl($this->acl,"sieveManagement") == "" && preg_match("/^delscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^delscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;
        $this->script_to_delete = $script;  
      }

      /* Activate script */
      if(chkacl($this->acl,"sieveManagement") == "" && preg_match("/^active_script_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^active_script_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        /* We can only activate existing scripts */
        if(!$this->scripts[$script]['IS_NEW']){

          /* Get sieve */
          if(!$this->sieve_handle = $this->get_sieve()){
            print_red(
                sprintf(
                  _("Can't log into SIEVE server. Server says '%s'."),
                  to_string($this->Sieve_Error)));
          }

          /* Try to activate the given script and update 
           *  class script array. 
           */
          if(!$this->sieve_handle->sieve_setactivescript($this->scripts[$script]['NAME'])){
            print_red(sprintf(_("Can't activate sieve script on server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
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

    
    /*************
     * Remove script handling 
     *************/

    /* Remove aborted */
    if(isset($_POST['delete_cancel'])){
      $this->script_to_delete = -1;
    }

    /* Remove confirmed */
    if(chkacl($this->acl,"sieveManagement") == "" && isset($_POST['delete_script_confirm'])){

      $script = $this->scripts[$this->script_to_delete];

      /* Just remove from array if it is a new script */
      if($script['IS_NEW']){
        unset($this->scripts[$this->script_to_delete]);
      }else{

        /* Get sieve */
        if(!$this->sieve_handle = $this->get_sieve()){
          print_red(
              sprintf(
                _("Can't log into SIEVE server. Server says '%s'."),
                to_string($this->Sieve_Error)));
        }

        if(!$this->sieve_handle->sieve_deletescript($this->scripts[$this->script_to_delete]['NAME'])){
          print_red(sprintf(_("Can't remove sieve script from server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
        }else{
          unset($this->scripts[$this->script_to_delete]);
        }
      }
      $this->script_to_delete = -1;
    }

    /* Display confirm dialog */
    if($this->script_to_delete != -1){
      $smarty = get_smarty();
      $smarty->assign("Warning",
          sprintf(_("You are going to remove the sieve script '%s' from your mail server."),
            $this->scripts[$this->script_to_delete]['NAME']));
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
    if(chkacl($this->acl,"sieveManagement") == "" && 
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
          print_red($p->status_text);;
        }
      }else{
        foreach($chk as $msgs){
          print_red(sprintf(_("Please fix all errors before saving. Last error was: %s"),$msgs));
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

          if($file['size'] == 0){
            print_red(_("Specified file seems to be empty."));
          }elseif(!file_exists($file['tmp_name'])){
            print_red(_("Upload failed. The temporary file can't be accessed."));
          }elseif(!is_readable ($file['tmp_name'])){
            print_red(sprintf(_("Can't open file '%s'."),$file['tmp_name']));
          }else{
            
            
 
            $contents = file_get_contents($file['tmp_name']);
           
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
           || isset($_POST['element_type']) &&in_array($_POST['element_type'],array("sieve_else","sieve_elsif"))){
          $this->add_element_type = $_POST['element_type'];
        }

        /* Create new element and add it to
         *  the selected position 
         */
        if(isset($_POST['select_new_element_type'])){
          if($this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below)){
            $this->add_new_element = FALSE;
          }else{
            print_red(_("Failed to add new element."));
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
        
            if($obj->object_id == $this->add_new_id && in_array(get_class($obj),array("sieve_if","sieve_elsif"))){
  
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
          $name = preg_replace("/_(x|y)$/","",$name);

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
    $List = new divSelectBox("sieveManagement");
    foreach($this->scripts as $key => $script){
  
      $edited =  $script['EDITED'];
      $active =  $script['ACTIVE'];
      
      $field1 = array("string" => "&nbsp;",
                      "attach" => "style='width:20px;'");  
      if($active){
        $field1 = array("string" => "<img src='images/true.png' alt='"._("Active")."' 
                                      title='"._("This script is marked as active")."'>",
                        "attach" => "style='width:20px;'");  
      }
      $field2 = array("string" => $script['NAME']);  
      $field3 = array("string" => $script['MSG']);
      $field4 = array("string" => _("Script length").":&nbsp;".strlen($script['SCRIPT']));

      if(chkacl($this->acl,"sieveManagement") == ""){
        $del = "<input type='image' name='delscript_".$key."' src='images/edittrash.png'
                  title='"._("Remove script")."'>";
      }else{
        $del = "<img src='images/empty.png' alt=' '>";
      }

      if($active || $script['IS_NEW'] || chkacl($this->acl,"sieveManagement") != ""){
        $activate = "<img src='images/empty.png' alt=' '>";
      }else{
        $activate = "<input type='image' name='active_script_".$key."' src='images/true.png'
                       title='"._("Activate script")."'>";
      }

      $field6 = array("string" => $activate."<input type='image' name='editscript_".$key."' src='images/edit.png'
                        title='"._("Edit script")."'>".$del,
                      "attach" => "style='border-right:0px; width:70px;'");
      $List->AddEntry(array($field1,$field2,$field3,$field4,$field6)); 
    }

    $List->SetHeight(400);
 
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
    $smarty->assign("List",$List->DrawList());
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
    $tmp = get_declared_classes();  
    if(!in_array($type,$tmp)){
      if(!empty($type)){
        trigger_error("Can't add new element, given \$class=".$class." does not exists.");
      }
      return(FALSE);
    }
  
    if(!is_object($this->current_handler) || !preg_match("/My_Parser/i",get_class($this->current_handler))){
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

      if($obj->object_id == $id && in_array(get_class($obj),array("sieve_if","sieve_elsif"))){

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
          if(in_array(get_class($obj),array("sieve_elsif"))){
            $element_types['sieve_elsif'] = _("Else If");
          }
        }
      }
    }

    if(!isset($element_types[$type])){
      print_red(sprintf(_("Can't add the specified element at the given position.")));
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

    /* PHP 4 automatic requirement add */
    if(isset($_SESSION['add_require']) && count($_SESSION['add_require'])){
      foreach($_SESSION['add_require'] as $reg){
        $this->current_handler->tree_->add_require($reg,true);
      }
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
          if(isset($obj->object_id) && preg_match("/^Add_Object_Top_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->add_element_type   =  $_POST['element_type_'.$obj->object_id];
            $this->add_new_element    = FALSE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "above";
            $this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below);
          }
          if(isset($obj->object_id) && preg_match("/^Add_Object_Bottom_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->add_element_type   =  $_POST['element_type_'.$obj->object_id];
            $this->add_new_element    = FALSE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "below";
            $this->add_new_element_to_current_script($this->add_element_type,$this->add_new_id,$this->add_above_below);
          }
        
          if(isset($obj->object_id) && preg_match("/^Remove_Object_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->current_handler->tree_->remove_object($key);
          }
          if(isset($obj->object_id) && preg_match("/^Move_Up_Object_".$obj->object_id."_/",$name) && $once){
            $this->current_handler->tree_->move_up_down($key,"up");
            $once = FALSE;
          }
          if(isset($obj->object_id) && preg_match("/^Move_Down_Object_".$obj->object_id."_/",$name) && $once){
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
      if(in_array($Mode,array("Source-Only","Source"))){
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
      if(in_array($Mode,array("Structured"))){
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
      print_red(
          sprintf(
            _("Can't log into SIEVE server. Server says '%s'."),
            to_string($this->Sieve_Error)));
      return;
    }

    $everything_went_fine = TRUE;

    foreach($this->scripts as $key => $script){
      if($script['EDITED']){
        $data = $this->scripts[$key]['SCRIPT'];
        if(!$this->sieve_handle->sieve_sendscript($script['NAME'], addcslashes ($data,"\\"))){
          gosa_log("Failed to save sieve script named '".$script['NAME']."': ".to_string($this->sieve_handle->error_raw));
          $everything_went_fine = FALSE;
          print_red(to_string($this->sieve_handle->error_raw));
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
