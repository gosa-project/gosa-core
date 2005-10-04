<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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
require "tabs_terminal.inc";
require "tabs_workstation.inc";
require "tabs_printers.inc";
require "tabs_phone.inc";
require "tabs_server.inc";
require "tabs_component.inc";
require "tabs_winstation.inc";


class systems extends plugin
{
  /* Definitions */
  var $plHeadline= "Systems";
  var $plDescription= "This does something";
  var $departments=array();

  /* Dialog attributes */
  var $systab= NULL;
  var $terminals= array();
  var $ui= NULL;
  var $acl= "";

  function systems ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("terminalfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $terminalfilter= array("workstations" => "checked",
          "thins" => "checked",
          "winstations" => "checked",
          "servers" => "checked",
          "printers" => "checked",
          "phones" => "checked",
          "netdev" => "checked",
          "user" => "*",
          "depselect" => $base,
          "regex" => "*");
      register_global("terminalfilter", $terminalfilter);
    }
  }

  function execute()
  {
    /* Save data */
    $terminalfilter= get_global("terminalfilter");
    foreach( array("depselect", "user", "regex") as $type){
      if (isset($_POST[$type])){
        $terminalfilter[$type]= $_POST[$type];
      }
    }
    if (isset($_POST['depselect'])){
      foreach( array("workstations", "thins", "winstations", "printers", "phones", "servers", "netdev") as $type){

        if (isset($_POST[$type])) {
          $terminalfilter[$type]= "checked";
        } else {
          $terminalfilter[$type]= "";
        }
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $terminalfilter['regex']= $s;
    }

    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty       = get_smarty();

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
            $s_action="open";
            $s_entry = base64_decode($_GET['dep_id']);
            $terminalfilter['depselect']= "".$this->config->departments[trim($s_entry)];
            $this->reload();
    }

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
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/user_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/user_setpwd_.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_setpwd_/i","",$key);
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      }elseif(preg_match("/newsystem_.*/i",$key)){
        $s_action="newsystem";
        $s_entry  = preg_replace("/newsystem_/i","",$key);
      }
    }

     if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Department changed? */
    if(isset($_POST['depselect']) && $_POST['depselect']){
      $terminalfilter['depselect']= $_POST['depselect'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $terminalfilter['depselect']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $terminalfilter['depselect']=(preg_replace("/^[^,]+,/","",$terminalfilter['depselect']));
    }

    if($s_action=="root"){
      $terminalfilter['depselect']=($this->config->current['BASE']);
    }


    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$terminalfilter['depselect']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $terminalfilter['depselect']= $this->config->departments[trim($base_back)];
      }else{
        $terminalfilter['depselect']= $this->config->departments["/"];
      }
    }

    register_global("terminalfilter", $terminalfilter);
    $this->reload();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Try to get informations about what kind of system to create */
    if ($s_action=="new") {
      return ($smarty->fetch(get_template_path('chooser.tpl', TRUE)));
    }

    /* Create new default terminal */
    if (isset($_POST['create_system'])||$s_action=="newsystem") {
      $this->dn= "new";
      $this->acl= array(":all");

      if(isset($_POST['system'])){
        $sw = $_POST['system'];
      }else{
        $sw = $s_entry;
      }

      switch ($sw){
        case 'terminal':
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['termgeneric']->cn= "default";
          $this->systab->by_object['termservice']->cn= "default";
          break;

        case 'workstation':
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['workgeneric']->cn= "wdefault";
          $this->systab->by_object['termservice']->cn= "default";
          break;

        case 'server':
          $this->systab= new servtabs($this->config,
              $this->config->data['TABS']['SERVTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'printer':
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'phone':
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'component':
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;
      }
    }

    /* User wants to edit data? */
    if ($s_action == "edit"){

      /* Get 'dn' from posted 'cn', must be unique */
      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Find out more about the object type */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn);
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs['objectClass']);

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);

      switch ($type){
        case "terminal":
          /* Register systab to trigger edit dialog */
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "server":
          /* Register systab to trigger edit dialog */
          $this->systab= new servtabs($this->config,
              $this->config->data['TABS']['SERVTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "workstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "printer":
          /* Register systab to trigger edit dialog */
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "phone":
          /* Register systab to trigger edit dialog */
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "component":
          /* Register systab to trigger edit dialog */
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;
       
        case "winstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new wintabs($this->config,
              $this->config->data['TABS']['WINTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;


        default:
          print_red (_("You can't edit this object type yet!"));
          del_lock($this->dn);
          break;
      }
    }

    /* Set terminals root password */
    if ($s_action=="change_pw"){
      $this->dn= $this->terminals[$s_entry]['dn'];
      $_SESSION['objectinfo']= $this->dn;
      return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
    }

    /* Correctly specified? */
    if (isset($_POST['password_finish'])){
      if ($_POST['new_password'] != $_POST['repeated_password']){
        print_red (_("Passwords entered as new and repeated do not match!"));
        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }
    }

    /* Change terminal password */
    if (isset($_POST['password_finish']) && 
        $_POST['new_password'] == $_POST['repeated_password']){

      /* Check if user is allowed to set password */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($acl, "password") != ""){
        print_red (_("You are not allowed to set this systems password!"));
      } else {
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);

        $attrs= array();
        if ($_POST['new_password'] == ""){
          $attrs['gotoRootPasswd']= array();
        } else {
          $attrs['gotoRootPasswd']= crypt($_POST['new_password'],
              substr(session_id(),0,2));
        }
        $ldap->modify($attrs);
        gosa_log ("Password for '".$this->dn."' has been changed");
      }
      unset($_SESSION['objectinfo']);
    }

    /* Delete terminal canceled? */
    if (isset($_POST['delete_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Remove terminal was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted termlinst */
      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this component!"));
      }
    }

    /* Confirmation for deletion has been passed. Terminal should be deleted. */
    if (isset($_POST['delete_terminal_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Find out more about the object type */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn);
        $attrs= $ldap->fetch();
        $type= $this->get_system_type($attrs['objectClass']);

        switch ($type){
          case "terminal":
            $tabtype= "termtabs";
          break;

          case "workstation":
            $tabtype= "worktabs";
          break;
          
          case "phone":
            $tabtype= "phonetabs";
          break;

          case "server":
            $tabtype= "servtabs";
          break;

          default:
#print_red (_("You can't remove this object type yet!"));
#del_lock($this->dn);
#return;
          $tabtype= "termtabs";
          break;
        }

        /* Delete request is permitted, perform LDAP action */
        if($tabtype=="phonetabs"){
          $this->systab= new $tabtype($this->config,
             $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl(array($this->acl));
          $this->systab->by_object['phonegeneric']->remove_from_parent ();
        }else{  
          $this->systab= new $tabtype($this->config,
             $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl(array($this->acl));
          $this->systab->by_object['termgeneric']->remove_from_parent ();
        }
        unset ($this->systab);
        gosa_log ("System object'".$this->dn."' has been removed");
        $this->systab= NULL;

        /* Terminal list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this component!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick system ".
            "deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }

    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if (isset($_POST['edit_finish'])){

      /* Check tabs, will feed message array */
      $message= $this->systab->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save terminal data to ldap */
        gosa_log ("System object'".$this->dn."' has been saved");
        $this->systab->save();

        /* Terminal has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new terminals at
           this point. */
        $this->reload ();
        unset ($this->systab);
        $this->systab= NULL;
        unset($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }

    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      if (isset($this->systab)){
        del_lock ($this->systab->dn);
        unset ($this->systab);
      }
      $this->systab= NULL;
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if ($this->systab){
      $display= $this->systab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->systab->by_object[$this->systab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }






    /* Prepare departments */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
            if ($terminalfilter['depselect'] == $key){
                    $options.= "<option selected value='$key'>$value</option>";
            } else {
                    $options.= "<option value='$key'>$value</option>";
            }
    }


  /* NEW LIST MANAGMENT */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
    " <input type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
    " <input type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
    " <input type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."'                     name='dep_home'>&nbsp;".
    " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
    " <input type='image' align='middle' src='images/select_new_terminal.png'   name='newsystem_terminal'    alt='"._("New Terminal")."' title='"._("New Terminal")."'>".
    " <input type='image' align='middle' src='images/select_new_workstation.png' name='newsystem_workstation' alt='"._("New Workstation")."' title='"._("New Workstation")."'>".
    " <input type='image' align='middle' src='images/select_new_server.png'     name='newsystem_server'      alt='"._("New Server")."' title='"._("New Server")."'>".
    " <input type='image' align='middle' src='images/select_new_printer.png'    name='newsystem_printer'     alt='"._("New Printer")."' title='"._("New Printer")."'>".
    " <input type='image' align='middle' src='images/select_new_phone.png'      name='newsystem_phone'       alt='"._("New Phone")."' title='"._("New Phone")."'>".
    " <input type='image' align='middle' src='images/select_new_component.png'  name='newsystem_component'   alt='"._("New Component")."' title='"._("New Component")."'>".
    " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
    _("Current base")."&nbsp;<select name='depselect' onChange='mainform.submit()'>$options</select>".
    " <input type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;".
    "</div>";



    $action= "<input type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit system")."'>";
    $action.= "<input type='image' src='images/edittrash.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete system")."'>";


    $divlist = new divlist("systemstab");
    $divlist->SetHeader(array(
          array("string" => "&nbsp;"),
          array("string" => _("System")." / "._("Department")),
          array("string" => _("Actions"), "attach" => "style='border:none'" )));


    $divlist->SetSummary(_("This table displays all systems, in the selected tree."));
    $divlist->SetEntriesPerPage(20);

    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    foreach($this->departments as $key=> $val){

      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/".$keys."\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val));
      $field3 = array("string" => "&nbsp;", "attach" => "style='border:none'");
      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    // Space
    $empty    ="&nbsp;";

    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    // Pictures for Extensions
    $img1  = "<img src='images/printer.png'            alt='C' title='"._("Cups Server")  ."'>";
    $img2  = "<img src='images/scanner.png'            alt='L' title='"._("Log Db") ."'>";
    $img3  = "<img src='images/select_terminal.png'    alt='L' title='"._("Syslog Server") ."'>";
    $img4  = "<img src='images/mailto.png'             alt='M' title='"._("Mail Server")  ."'>";
    $img5  = "<img src='images/select_phone.png'       alt='I' title='"._("Imap Server") ."'>";
    $img6  = "<img src='images/fax_small.png'          alt='F' title='"._("Nfs Server")   ."'>";
    $img7  = "<img src='images/select_winstation.png'  alt='K' title='"._("Kerberos Server") ."'>";
    $img8  = "<img src='images/select_phone.png'       alt='A' title='"._("Asterisk Server") ."'>";
    $img9  = "<img src='images/fax_small.png'          alt='F' title='"._("Fax Server") ."'>";
    $img10 = "<img src='images/save.png'               alt='L' title='"._("Ldap Server") ."'>";

    // Test Every Entry and generate divlist Array
    foreach($this->terminals as $key => $val){
      // Specify Pics for Extensions
      if(in_array("goCupsServer"    ,$val['objectClass'])) $cups    = $img1;   else $cups  =$empty;
      if(in_array("goLogDBServer"   ,$val['objectClass'])) $logdb   = $img2;   else $logdb =$empty;
      if(in_array("goSyslogServer"  ,$val['objectClass'])) $syslog  = $img3;   else $syslog=$empty;
      if(in_array("goImapServer"    ,$val['objectClass'])) $imap    = $img4;   else $imap  =$empty;
      if(in_array("sambaSamAccount" ,$val['objectClass'])) $samba   = $img5;   else $samba =$empty;
      if(in_array("goShareServer"   ,$val['objectClass'])) $nfs     = $img6;   else $nfs   =$empty;
      if(in_array("goKrbServer"     ,$val['objectClass'])) $krb     = $img7;   else $krb   =$empty;
      if(in_array("goFonServer"     ,$val['objectClass'])) $fon     = $img8;   else $fon   =$empty;
      if(in_array("goFaxServer"     ,$val['objectClass'])) $fax     = $img9;   else $fax   =$empty;
      if(in_array("goLdapServer"    ,$val['objectClass'])) $ldap     = $img10;   else $ldap   =$empty;
  
      $pics = $cups.$logdb.$syslog.$imap.$samba.$nfs.$krb.$fon.$fax.$ldap;
      $pics = "";
      // Generate Array to Add
      if((isset($val['is_new']))&&(!empty($val['is_new']))){
        $display= "".$val["cn"][0]." ".$val['is_new'];
      }else{
        $display= "".$val["cn"][0]."";
      }

      if(in_array("gotoTerminal",$val['objectClass'])){
        $action2 = "<input type='image' src='images/certs.png' alt='"._("Password")."'   name='user_setpwd_%KEY%' title='"._("Set root password")."'>";
      }else{
        $action2 = "";
      }


      if(isset($val['message'])){
        $display.= "  (".$val['message']." '".$this->config->idepartments[$terminalfilter['depselect']]."'  )";
      }

      $img=$this->convert_list($val);

      $field1 = array("string" => sprintf($img['img'],$val['dn']),"attach"=>"style='width:20px;align:middle;'");
      $field2 = array("string" => sprintf($editlink,$key,$display),"attach"=>"title='".$val['dn']."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action2.$action),"attach" => "style='border:none; text-align:right;width:48px'");
      $divlist->AddEntry( array($field1,$field2,$field3));
    }





    /* Show main page */
    $smarty->assign("terminalshead", $listhead);
    $smarty->assign("terminals", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    foreach( array("depselect", "user", "regex", "workstations", "thins", "servers",
          "winstations", "printers", "phones", "netdev") as $type){

      $smarty->assign("$type", $terminalfilter[$type]);
    }
    $smarty->assign("deplist", $this->config->idepartments);

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(	
        "D" => array("select_default.png",_("Template")),
        "T" => array("select_terminal.png",_("Terminal")),
        "L" => array("select_workstation.png",_("Workstation")),
        "F" => array("select_phone.png",_("Phone")),
        "S" => array("select_server.png",_("Server")),
        "NT"=> array("select_new_terminal.png",_("New Terminal")),
        "NL"=> array("select_new_workstation.png",_("New Workstation")),
        "W" => array("select_winstation.png",_("Winstation")),
        "C" => array("select_component.png",_("Network Device")),
        "P" => array("select_printer.png",_("Printer")));

   if((isset($input['is_new']))&&(!empty($input['is_new']))){
     $input['type']="N".$input['type'];
   }
   foreach ($conv  as $key => $value){
      if($input['type']==$key){
        $tmp['img'] ="<img src='images/".$value[0]."' alt='".$key."' title='".$value['1']."'>";
        $tmp['class']=$key;
        return $tmp;
      }
    }
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  }


  /* Check values */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
  }

  function adapt_from_template($dn)
  {
  }

  function password_change_needed()
  {
  }

  function show_header($button_text, $text, $disabled= FALSE)
  {
  }

  function reload()
  {
    /* Load terminal shortcuts */
    $responsible= array();
    foreach ($this->config->departments as $key => $value){
      if (get_module_permission(get_permissions ($value, $this->ui->subtreeACL),
            "terminal", $value) == "#all#"){
        $responsible[$key]= $value;
      }
    }

    /* Get config */
    $terminalfilter= get_global('terminalfilter');
    $filter= "";

    /* Set base for all searches */
    $base= $terminalfilter['depselect'];

    /* Regex filter? */
    if ($terminalfilter['regex'] != ""){
      $regex= $terminalfilter['regex'];
    } else {
      $regex= "*";
    }

    /* Get list of terminals to be shown */
    if ($terminalfilter['thins'] == "checked"){
      $termfilter= "(&(objectClass=gotoTerminal)(cn=$regex))";
    } else {
      $termfilter= "";
    }
    if ($terminalfilter['workstations'] == "checked"){
      $workfilter= "(&(objectClass=gotoWorkstation)(cn=$regex))";
    } else {
      $workfilter= "";
    }
    if ($terminalfilter['winstations'] == "checked"){
      if ($this->config->current['SAMBAVERSION'] == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
      $winfilter= "(&(objectClass=$samba)(cn=$regex\$))";
    } else {
      $winfilter= "";
    }
    if ($terminalfilter['printers'] == "checked"){
      $printfilter= "(&(objectClass=gotoPrinter)(cn=$regex))";
    } else {
      $printfilter= "";
    }
    if ($terminalfilter['phones'] == "checked"){
      $phonefilter= "(&(objectClass=goFonHardware)(cn=$regex))";
    } else {
      $phonefilter= "";
    }
    if ($terminalfilter['netdev'] == "checked"){
      $netfilter= "(&(objectClass=ieee802Device)(cn=$regex))";
    } else {
      $netfilter= "";
    }
    if ($terminalfilter['servers'] == "checked"){
      $serverfilter= "(&(objectClass=goServer)(cn=$regex))";
    } else {
      $serverfilter= "";
    }

    /* User filter? */
    if ($terminalfilter['user'] != "" && $terminalfilter['user'] != "*"){
      $filter.= "(gotoLastUser=".$terminalfilter['user'].")";
    }

    /* Get all gotoTerminal's */
    $this->terminals= array();

    $res= get_list($this->ui->subtreeACL, "(|$termfilter)", FALSE, "ou=terminals,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE);

    $res= array_merge($res,get_list($this->ui->subtreeACL, "(|$termfilter)", FALSE, "ou=incoming,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$serverfilter)", FALSE, "ou=servers,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$phonefilter)", FALSE, "ou=phones,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$netfilter)", FALSE, "ou=netdevices,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$printfilter)", FALSE, "ou=printers,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$workfilter)", FALSE, "ou=workstations,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$workfilter)", FALSE, "ou=incoming,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

    $res= array_merge($res,
          get_list($this->ui->subtreeACL, "(|$winfilter)", FALSE, "ou=winstations,ou=systems,".$base, 
        array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), FALSE));

/* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */
    $base2 = preg_replace("/ou=people,/i","",$base);

    $res3 =  get_list2($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
                              TRUE, $base2, array("ou", "description"), TRUE);

    $this->departments= array();
    $tmp = array();
    foreach ($res3 as $value){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if($value["description"][0]!=".."){
        $this->departments[$value['dn']]=convert_department_dn2($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]=$value["description"][0];
      }
    }

    /* END NEW LIST MANAGMENT
     */


    foreach ($res as $value){

      /* Look for new terminals and mark them with '+' */
      $tmp= "";
      if (preg_match('/,ou=terminals,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=terminals,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=workstations,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=workstations,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=servers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=servers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,'.get_winstations_ou().'/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*".get_winstations_ou()."/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=printers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=printers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=phones,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=phones,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=netdevices,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=netdevices,ou=systems,/i", "", $value["dn"]);
      }
      if ($tmp == ""){
        $tmp= $value["dn"];
      }
      if (preg_match ("/,ou=incoming,/i", $tmp)){
        if (in_array('gotoTerminal', $value['objectClass'])){
          $add= "- "._("New terminal");
        }
        if (in_array('gotoWorkstation', $value['objectClass'])){
          $add= "- "._("New workstation");
        }
      } else {
        $add= "";
      }

      $terminal = array();
  
      if (in_array ($tmp, $responsible) || $add != ""){
        if (in_array('gotoTerminal', $value["objectClass"])){
          if (isset($value["macAddress"][0]) && $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "T";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['message']  = _("Terminal template for");
            $terminal['location'] = array_search($tmp, $this->config->departments); 
          }
        } elseif (in_array('gotoWorkstation', $value["objectClass"])){
          if (isset($value["macAddress"][0]) &&  $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "L";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['location'] = array_search($tmp, $this->config->departments);
            $terminal['message']  = _("Workstation template for");
          }
        } elseif (in_array('gotoPrinter', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "P";
        } elseif (in_array('goServer', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "S";
        } elseif (in_array('goFonHardware', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "F";
        } elseif (in_array('ieee802Device', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "C";
        } else{
          $name= preg_replace('/\$$/', '', $value['cn'][0]);
          if (isset($value['sambaDomainName'])){
            $domain= " [".$value['sambaDomainName'][0]."]";
          } else {
            $domain= "";
          }
            $terminal=$value;
            $terminal['type']     ="W";
            $terminal['domain']   = $name.$domain;
        }
      $this->terminals[]=$terminal;
      }
    }

    $tmp=array();
    foreach($this->terminals as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['dn']]=$val;
    }
    ksort($tmp);
    $this->terminals=array();
    foreach($tmp as $val){
      $this->terminals[]=$val;
    }
    reset ($this->terminals);
  }

  function remove_lock()
  {
    if (isset($this->systab->dn)){
      del_lock ($this->systab->dn);
    }
  }


  function get_system_type($classes)
  {
    $type= "";

    if (in_array('ieee802Device', $classes)){
      $type= "component";
    }
    if (in_array('gotoTerminal', $classes)){
      $type= "terminal";
    }
    if (in_array('gotoWorkstation', $classes)){
      $type= "workstation";
    }
    if (in_array('gotoPrinter', $classes)){
      $type= "printer";
    }
    if (in_array('goFonHardware', $classes)){
      $type= "phone";
    }
    if (in_array('goServer', $classes)){
      $type= "server";
    }
    if (in_array('sambaAccount', $classes) ||
        in_array('sambaSamAccount', $classes)){
      $type= "winstation";
    }

    return ($type);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
