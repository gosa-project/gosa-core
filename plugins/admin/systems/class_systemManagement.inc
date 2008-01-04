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

class systems extends plugin
{
  /* Definitions */
  var $plHeadline     = "Systems";
  var $plDescription  = "This does something";
  var $departments    = array();

  /* Dialog attributes */
  var $systab   = NULL;
  var $terminals= array();
  var $ui       = NULL;
  var $DivListSystem;
  var $start_pasting_copied_objects = FALSE;
  var $CopyPasteHandler   = NULL;

  /* Arp hanlding activated */
  var $arp_handling_active = FALSE;

  function systems (&$config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Creat dialog object */
    $this->DivListSystem = new divListSystem($this->config,$this);

    /* Copy & Paste enabled ?*/
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE']))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Check whether the arp handling active or not */
    if($this->config->search("ArpNewDevice","CLASS",array('tabs')) != ""){
      $this->arp_handling_active = TRUE;
    }

  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^system_edit_/i","/^system_del_/","/^act/","/^id/","/^item_selected/","/^remove_multiple_systems/");

    /********************
      Check for functional posts, edit|delete|add|... system devices 
     ********************/
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty       = get_smarty();

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/system_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/system_del_/i","",$key);
        // Post for edit
      }elseif(preg_match("/system_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/system_edit_/i","",$key);
        // Post for new
      }elseif(preg_match("/system_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/system_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/system_setpwd_.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/system_setpwd_/i","",$key);
      }elseif(preg_match("/gen_cd_.*/i",$key)){
        $s_action="gen_cd";
        $s_entry  = preg_replace("/gen_cd_/i","",$key);
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }
    }
   
    /* Incoming handling  
     * If someone made a systemtype and ogroup selection 
     * Display the new requested entry type ... servtab  in case of server and so on.
     */
    if(isset($_POST['SystemTypeChoosen'])){
      $s_action = "SelectedSystemType";
      $s_entry  = $_POST['SystemType'];
      $_SESSION['SelectedSystemType']['ogroup'] = $_POST['ObjectGroup'];
      $this->systab = NULL;
    }

    /* remove image tags from posted entry  (posts looks like this 'name_x')*/
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Edit was requested by pressing the name(link) of an item */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }
    /* Create options */
    if(isset($_POST['menu_action']) && preg_match("/^newsystem_/",$_POST['menu_action'])){
      $s_action = "newsystem";
      $s_entry  = preg_replace("/^newsystem_/","",$_POST['menu_action']);
    }

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

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }
    

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Try to get informations about what kind of system to create */
    if ($s_action=="new") {
      return ($smarty->fetch(get_template_path('chooser.tpl', TRUE)));
    }


    /********************
      Copy & Paste Handling  ...
     ********************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }
  

    /********************
      Create FAI CD ...   
     ********************/
    if ($s_action=="gen_cd"){
      $this->dn= $this->terminals[$s_entry]['dn'];
      $_SESSION['objectinfo']= $this->dn;
      return ($smarty->fetch(get_template_path('gencd.tpl', TRUE)));
    }


    /* Start CD-Creation */
    if ((isset($_POST["cd_create"])) && !empty($this->dn)){
      $smarty->assign("src", "?plug=".$_GET['plug']."&amp;PerformIsoCreation");
      return ($smarty->fetch(get_template_path('gencd_frame.tpl', TRUE)));
    }


    if ($this->dn != "" && isset($_GET['PerformIsoCreation'])){

      $return_button   = "<form method='get' action='main.php' target='_parent'>
        <input type='submit' value='"._("Back")."'>
        <input type='hidden' name='plug' value='".$_GET['plug']."'/>
        </form>";

      $dsc             = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));

      /* Get and check command */
      $command= $this->config->search("workgeneric", "ISOCMD",array('tabs'));
      
      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");

        /* Print out html introduction */
        echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
          <html>
          <head>
          <title></title>
          <style type="text/css">@import url("themes/default/style.css");</style>
          <script language="javascript" src="include/focus.js" type="text/javascript"></script>
          </head>
          <body style="background: none; margin:4px;" id="body" >
          <pre>';

        /* Open process handle and check if it is a valid process */
        $process= proc_open($command." '".$this->dn."'", $dsc, $pipes);
        if (is_resource($process)) {
          fclose($pipes[0]);

          /* Print out returned lines && write JS to scroll down each line */
          while (!feof($pipes[1])){
            $cur_dat = fgets($pipes[1], 1024);
            echo $cur_dat;
            echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
            flush();
          }
        }

        /* Get error string && close streams */
        $buffer= stream_get_contents($pipes[2]);

        fclose($pipes[1]);
        fclose($pipes[2]);
        echo "</pre>";

        /* Check return code */
        $ret= proc_close($process);
        if ($ret != 0){
          echo "<h1 style='color:red'>"._("Creating the image failed. Please see the report below.")."</h1>";
          echo "<pre style='color:red'>$buffer</pre>";
        }



        echo $return_button."<br>";

      } else {
        $tmp= "<h1 style='color:red'>".sprintf(_("Command '%s', specified for ISO creation doesn't seem to exist."), $command)."</h1>";
        echo $tmp;
      }

      /* Scroll down completly */
      echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
      echo '</body></html>';
      flush();
      exit;
    }


    /********************
      Create new system ...   
     ********************/
    /* Create new default terminal 
     * Or create specified object of selected system type, from given incoming object  
     */

    $save_object_directly = false;
    if(($s_action == "SelectedSystemType") && (isset($_POST['ObjectGroup']) && ($_POST['ObjectGroup'] != "none"))){
      $save_object_directly = true;
    }

    if (isset($_POST['create_system'])||$s_action=="newsystem"||$s_action == "SelectedSystemType") {

      /* If the current entry is an incoming object 
       * $sw = System type as posted in new incoming handling dialog 
       */ 
      if($s_action == "SelectedSystemType") {
        $sw         = $s_entry;
      }else{
        if(isset($_POST['system'])){
          $sw = $_POST['system'];
        }else{
          $sw = $s_entry;
        }
        $this->dn= "new";
      }

      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABNAME"=>"termgeneric",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABNAME"=>"workgeneric",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABNAME"=>"servgeneric",     "TABCLASS" =>"servtabs",      "ACL"=> "server"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABNAME"=>"printgeneric",    "TABCLASS" =>"printtabs",     "ACL"=> "printer"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABNAME"=>"phoneGeneric",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABNAME"=>"componentGeneric","TABCLASS" =>"componenttabs", "ACL"=> "component"));

      if(isset($tabs[$sw])){
        $class    = $tabs[$sw]["CLASS"];
        $tabname  = $tabs[$sw]["TABNAME"];
        $tabclass = $tabs[$sw]["TABCLASS"];
        $acl_cat  = $tabs[$sw]["ACL"];

        /* Load permissions for selected 'dn' and check if
           we're allowed to remove this 'dn' */
        $ui       = get_userinfo();
        $tabacl   = $ui->get_permissions($this->DivListSystem->selectedBase,$acl_cat."/".$tabname);
        if(preg_match("/c/",$tabacl)){
          $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn,$sw);
          $this->systab->set_acl_base($this->DivListSystem->selectedBase);
          $this->systab->by_object[$tabname]->base = $this->DivListSystem->selectedBase;
          $this->systab->base = $this->DivListSystem->selectedBase;
        }else{
          print_red(_("You are not allowed to create a new object of this type."));
        }
      }
    }

    /********************
      Edit system ...   
     ********************/

    /* User wants to edit data? */
    if (($s_action == "edit") && (!isset($this->systab->config))){

      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Find out more about the object type */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('objectClass','gotoMode'));
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs);

      /* Lock the current entry, so everyone will get the
         above dialog */
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs",      "ACL"=> "server"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs",     "ACL"=> "printer"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs",       "ACL"=> "winworkstation"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs", "ACL"=> "component"));


      if($type == "ArpNewDevice"){
        $this->systab = new ArpNewDeviceTabs($this->config,$this->config->data['TABS']['ARPNEWDEVICETABS'],$this->dn);
      }elseif($type == "NewDevice"){
        $this->systab = new SelectDeviceType($this->config,$this->dn) ;
      }elseif(isset($tabs[$type])){

        $class    = $tabs[$type]["CLASS"];
        $acl_cat  = $tabs[$type]["ACL"];
        $tabclass = $tabs[$type]["TABCLASS"];

        $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn,$acl_cat);
        $this->systab->set_acl_base($this->dn);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
      }else{ 
        print_red (_("You can't edit this object type yet!"));
        del_lock($this->dn);
      }
    }


    /********************
      Change password ...   
     ********************/

    /* Set terminals root password */
    if ($s_action=="change_pw"){
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABNAME"=>"termgeneric",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABNAME"=>"workgeneric",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation"));

      $type = $this->get_system_type($this->terminals[$s_entry]);
      $class    = $tabs[$type]["CLASS"];
      $tabname  = $tabs[$type]["TABNAME"];
      $acl_cat  = $tabs[$type]["ACL"];
      $tabclass = $tabs[$type]["TABCLASS"];
      $ui       = get_userinfo();
      $tabacl   = $ui->get_permissions($this->DivListSystem->selectedBase,$acl_cat."/".$tabname,"gotoRootPasswd");
      if(preg_match("/w/",$tabacl)){
        $this->dn= $this->terminals[$s_entry]['dn'];
        $_SESSION['objectinfo']= $this->dn;
        return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }else{
        print_red(_("You are not allowed to change the password for this object."));
      }
    }


    /********************
      Password change finish, but check if entered data is ok 
     ********************/

    /* Correctly specified? */
    if (isset($_POST['password_finish'])){
      if ($_POST['new_password'] != $_POST['repeated_password']){
        print_red (_("Passwords entered as new and repeated do not match!"));
        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }
    }

    /********************
      Password change finish
     ********************/

    /* Change terminal password */
    if (isset($_POST['password_finish']) && 
        $_POST['new_password'] == $_POST['repeated_password']){

      /* Check if user is allowed to set password */
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABNAME"=>"termgeneric",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABNAME"=>"workgeneric",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation"));

      /* Detect object type */
      $type = "";
      foreach($this->terminals as $terminal){
        if($terminal['dn'] == $this->dn){
          $type = $this->get_system_type($terminal);
          break;
        } 
      }

      /* Type detected */
      if(!empty($type)){

        /* Get infos */
        $class    = $tabs[$type]["CLASS"];
        $tabname  = $tabs[$type]["TABNAME"];
        $acl_cat  = $tabs[$type]["ACL"];
        $tabclass = $tabs[$type]["TABCLASS"];
    
        /* Get acls */
        $ui       = get_userinfo();
        $tabacl   = $ui->get_permissions($this->DivListSystem->selectedBase,$acl_cat."/".$tabname,"gotoRootPasswd");

        /* Check acls */
        if(preg_match("/w/",$tabacl)){
          $ldap = $this->config->get_ldap_link();
          $ldap->cd($this->dn);

          $attrs= array();
          if ($_POST['new_password'] == ""){
            $attrs['gotoRootPasswd']= array();
          } else {
            $attrs['gotoRootPasswd']= crypt($_POST['new_password'],substr(session_id(),0,2));
          }
          $ldap->modify($attrs);
          new log("security","systems/".get_class($this),$this->dn,array_keys($attrs),$ldap->get_error());
        }else{
          print_red(_("You are not allowed to change the password for this object."));
        }
      }else{
        print_red(_("Can't detect object to change password."));
      }
      session::un_set('objectinfo');
    }


    /********************
      Delete system cancel
     ********************/

    /* Delete terminal canceled? */
    if (isset($_POST['delete_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $this->dns = array();
      $ids = $this->list_get_selected_items();

      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->terminals[$id]['dn'];
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
        if (count($this->dns) == 1){
          $smarty->assign("warning",     sprintf(_("You're about to delete the following entry %s"), @LDAP::fix($dns_names)));
        } else {
          $smarty->assign("warning",     sprintf(_("You're about to delete the following entries %s"), @LDAP::fix($dns_names)));
        }
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_system_confirm'])){

      $ui = get_userinfo();
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal/termgeneric"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation/workgeneric"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs",      "ACL"=> "server/servgeneric"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs",     "ACL"=> "printer/printgeneric"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone/phoneGeneric"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs",       "ACL"=> "winworkstation/wingeneric"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs", "ACL"=> "component/componentGeneric"));


      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        /* Get 'dn' from posted termlinst */
        $attrs    = $this->terminals[$key];
        $type= $this->get_system_type($attrs);

        /* get object type */
        $tabtype  = "termtabs";
        $tabobj   = "TERMTABS";
        $tabacl   = "";
        if(isset($tabs[$type])){
          $tabtype = $tabs[$type]['TABCLASS'];
          $tabobj  = $tabs[$type]['CLASS'];
          $tabacl  = $ui->get_permissions($dn,$tabs[$type]['ACL']);

          /* Load permissions for selected 'dn' and check if
             we're allowed to remove this 'dn' */
          if(preg_match("/d/",$tabacl)){ 

            /* Delete request is permitted, perform LDAP action */
            if($tabtype=="phonetabs"){
              $this->systab= new $tabtype($this->config, $this->config->data['TABS'][$tabobj], $dn,$type);
              $this->systab->set_acl_base($dn);
              $this->systab->by_object['phoneGeneric']->remove_from_parent ();
            }else{
              $this->systab= new $tabtype($this->config,$this->config->data['TABS'][$tabobj], $dn,$type);
              $this->systab->set_acl_base($dn);
              $this->systab->delete();
            }
            unset ($this->systab);
            $this->systab= NULL;

          } else {
            /* Normally this shouldn't be reached, send some extra
               logs to notify the administrator */
            print_red (_("You are not allowed to delete this component!"));
            new log("security","systems/".get_class($this),$dn,array(),"Tried to trick deletion.");
          }
          /* Remove lock file after successfull deletion */
          del_lock ($dn);
          unset($this->dns[$key]);
        }
      }
    }

    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
   if(isset($_POST['delete_multiple_system_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /********************
      Delete system, confirm dialog
     ********************/

    /* Remove terminal was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted termlinst */
      $this->dn = $this->terminals[$s_entry]['dn'];
      $attrs    = $this->terminals[$s_entry];

      $type= $this->get_system_type($attrs);
      $ui = get_userinfo();
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal/termgeneric"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation/workgeneric"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs",      "ACL"=> "server/servgeneric"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs",     "ACL"=> "printer/printgeneric"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone/phoneGeneric"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs",       "ACL"=> "winworkstation/wingeneric"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs", "ACL"=> "component/componentGeneric"));


      /* get object type */
      $tabtype  = "termtabs";
      $tabobj   = "TERMTABS";
      $tabacl   = "";
      if(isset($tabs[$type])){
        $tabtype = $tabs[$type]['TABCLASS'];
        $tabobj  = $tabs[$type]['CLASS'];
        $tabacl  = $ui->get_permissions($this->dn,$tabs[$type]['ACL']);
      }

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      if(preg_match("/d/",$tabacl)){ 

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), @LDAP::fix($this->dn)));
        $smarty->assign("multiple", false);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this component!"));
      }
    }


    /********************
      Delete system, confirmed
     ********************/
    /* Confirmation for deletion has been passed. Terminal should be deleted. */
    if (isset($_POST['delete_terminal_confirm'])){

      /* Find out more about the object type */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('objectClass'));
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs);

      $ui = get_userinfo();

      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal/termgeneric"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation/workgeneric"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs",      "ACL"=> "server/servgeneric"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs",     "ACL"=> "printer/printgeneric"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone/phoneGeneric"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs",       "ACL"=> "winworkstation/wingeneric"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs", "ACL"=> "component/componentGeneric"));

      /* get object type */
      $tabtype  = "termtabs";
      $tabobj   = "TERMTABS";
      $tabacl   = "";
      if(isset($tabs[$type])){
        $tabtype = $tabs[$type]['TABCLASS'];
        $tabobj  = $tabs[$type]['CLASS'];
        $tabacl  = $ui->get_permissions($this->dn,$tabs[$type]['ACL']);
      }

      /* Check if we are allowed to remove this object */
      if(preg_match("/d/",$tabacl)){

        /* Delete request is permitted, perform LDAP action */
        if($tabtype=="phonetabs"){
          $this->systab= new $tabtype($this->config, $this->config->data['TABS'][$tabobj], $this->dn,$type);
          $this->systab->set_acl_base($this->dn);
          $this->systab->by_object['phoneGeneric']->remove_from_parent ();
        }else{  
          $this->systab= new $tabtype($this->config,$this->config->data['TABS'][$tabobj], $this->dn,$type);
          $this->systab->set_acl_base($this->dn);
          $this->systab->delete();
        }
        unset ($this->systab);
        $this->systab= NULL;

        /* Terminal list has changed, reload it. */
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this component!"));
        new log("security","systems/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }









   /********************
      Edit system type finished, check if everything went ok
     ********************/
    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->systab->config)) || $save_object_directly){

      /* Check tabs, will feed message array */
      $message = array();
      if(!$save_object_directly){
        $message = $this->systab->check();
      }else{
        $found = false;

        /* Set gotoMode to active if we there was an ogroup selected . (save_object_directly) */
        foreach(array("workgeneric"=>"active","servgeneric"=>"active","termgeneric"=>"graphic") as $tab => $value){
          if(isset($this->systab->by_object[$tab]->gotoMode)) {
            $found = true;
            $this->systab->by_object[$tab]->gotoMode = $value;
          }
        }
        if(!$found){
          print_red(sprintf(_("Can't set gotoMode to status 'active', the current object couldn't be identified.")));
        }

      }
      /* Save, or display error message? */
      if (count($message) == 0){

        /* Incoming behavior; you can select a system type and an ogroup membership.
         * If this object is an Incoming object, $_SESSION['SelectedSystemType'] isset.
         * Check if we must add the new object to an object group.
         *
         * If this is done, delete the old incoming entry... it is still there, because this is a new
         * entry and not an edited one, so we will delete it.
         *
         */

        if(isset($_SESSION['SelectedSystemType'])){
          $SelectedSystemType= $_SESSION['SelectedSystemType'];
          if($SelectedSystemType['ogroup'] != "none"){
            $og = new ogroup($this->config,$SelectedSystemType['ogroup']);
            if($og){
              $og->AddDelMembership($this->systab->dn);
              $og->save();
            }
          }
        }

        /* Save terminal data to ldap */
        if(isset($_SESSION['SelectedSystemType']['ogroup']) && $_SESSION['SelectedSystemType']['ogroup'] != 'none'){
          foreach (array("workservice", "termservice") as $cls){
            if (isset($this->systab->by_object[$cls])){
              $this->systab->by_object[$cls]->gotoXMouseport= "";
              $this->systab->by_object[$cls]->gotoXMouseType= "";
              $this->systab->by_object[$cls]->gotoXResolution= "";
              $this->systab->by_object[$cls]->gotoXColordepth= "";
            }
          }
        }

        $this->systab->save();

        if(isset($_SESSION['SelectedSystemType'])){
          unset($_SESSION['SelectedSystemType']);
          if(!isset($ldap)){
            $ldap = $this->config->get_ldap_link();
          }
          $ldap->cd ($this->dn);
          $ldap->cat($this->dn, array('dn'));
          if(count($ldap->fetch())){
            $ldap->cd($this->dn);
            $ldap->rmDir($this->dn);
          }
          $ldap->cd($this->config->current['BASE']);
        }

        /* Terminal has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        unset ($this->systab);
        $this->systab= NULL;
        session::un_set('objectinfo');
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /********************
      Edit system was canceled 
     ********************/
    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel']) || isset($_POST['SystemTypeAborted'])){
      if (isset($this->systab)){
        del_lock ($this->systab->dn);
        unset ($this->systab);
      }
      $this->systab= NULL;
      session::un_set('objectinfo');

      /* Remove ogroup selection, which was set while editing a new incoming entry */
      if(isset($_SESSION['SelectedSystemType'])){
        unset($_SESSION['SelectedSystemType']);
      }
    }

    /********************
      Display edit dialog, or some other
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->systab->config)){
      $display= $this->systab->execute();

      /* Don't show buttons if tab dialog requests this */

      $dialog     = FALSE;
      $hide_apply = $this->dn == "new";
      if(is_object($this->systab) && !isset($this->systab->by_object)){
        $dialog = TRUE;
        $hide_apply = TRUE;
      }elseif(isset($this->systab->by_object[$this->systab->current]->dialog)){
        $dia = $this->systab->by_object[$this->systab->current]->dialog;
        if($dia === TRUE || is_object($dia)){
          $dialog = TRUE;
        }  
      }
      if(isset($this->systab->by_object[$this->systab->current]->netConfigDNS) && 
        $this->systab->by_object[$this->systab->current]->netConfigDNS->dialog){
        $dialog = TRUE;
      }

      if (!$dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
        $display.= "&nbsp;\n";
        if (!$hide_apply){
          $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListSystem->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListSystem->parent = $this;
    $this->DivListSystem->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListSystem->SubSearch){
      $this->DivListSystem->AddDepartments($this->DivListSystem->selectedBase,3,1);
    }
    $this->reload();
    $this->DivListSystem->setEntries($this->terminals);
    return($this->DivListSystem->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    $tmp = array();

    /* Check acls, if we are not allowed to create and write each plugin tab, skip this object */

    $tabs = array(
        "terminal"        => get_ou('terminalou'),
        "workstation"     => get_ou('workstationou'),
        "incoming"        => get_ou('incomingou'),
        "server"          => get_ou('serverou'),
        "printer"         => get_ou('printerou'),
        "phone"           => get_ou('phoneou'),
        "winworkstation"  => get_winstations_ou(),
        "component"       => get_ou('componentou')
        ); 

    foreach($tabs as $acl_cat => $dn){

      $acl_all = $this->ui->has_complete_category_acls($dn.$this->DivListSystem->selectedBase,$acl_cat);
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $tmp[] = $dn.$this->DivListSystem->selectedBase;
      }
    }
    return($tmp); 
  }


  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListSystem->save_object();
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

  function reload()
  {
    /* some var init */
    $ui = get_userinfo();
    $res              = array();
    $this->terminals  = array();
    $userregex        = "";

    /* Set base for all searches */
    $base=  $this->DivListSystem->selectedBase;

    /* Prepare samba class name */
    $samba  ="";
    if ($this->DivListSystem->ShowWinWorkstations){
      if ($this->config->current['SAMBAVERSION'] == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
    }

    /* This array represents the combination between checkboxes and search filters */
    $objs = array( 
        "ShowServers"        => array("CLASS" => "goServer"        ,"TREE" => get_ou('serverou')),
        "ShowTerminals"      => array("CLASS" => "gotoTerminal"    ,"TREE" => get_ou('terminalou')),
        "ShowPrinters"       => array("CLASS" => "gotoPrinter"     ,"TREE" => get_ou('printerou')),
        "ShowDevices"        => array("CLASS" => "ieee802Device"   ,"TREE" => get_ou('componentou')),
        "ShowPhones"         => array("CLASS" => "goFonHardware"   ,"TREE" => get_ou('phoneou')),
        "ShowWorkstations"   => array("CLASS" => "gotoWorkstation" ,"TREE" => get_ou('workstationou')),
        "ShowWinWorkstations"=> array("CLASS" => $samba            ,"TREE" => get_winstations_ou() ));

    /* Include the 'Display Systems of user' attribute */ 
    if ((!empty($this->DivListSystem->UserRegex)) && ($this->DivListSystem->UserRegex!= "*")){
      $userregex = "(gotoLastUser=".$this->DivListSystem->UserRegex.")";
    }

    /* Attributes to fetch */
    $sys_attrs        = array("cn", "description", "macAddress", "objectClass", "sambaDomainName","gotoMode");
    $sys_categories   = array("terminal", "workstation", "server", "phone" ,"printer");

    /* Add FAIstate to attributes if FAI is activated */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $sys_attrs[] = "FAIstate";
    }    

    /* Walk through all possible search combinations, and search for some objects if the checkbox is enabled  */
    foreach($objs as $checkBox => $oc){
      if($this->DivListSystem->$checkBox){
        if($this->DivListSystem->SubSearch){
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $new_res = get_sub_list($filter, $sys_categories ,preg_replace("/,.*$/","",$base), $base,$sys_attrs, GL_NONE | GL_SUBSEARCH | GL_SIZELIMIT);
      
            /* Remove all objects that are not in the expected sub department */
            foreach($new_res as $key => $obj){
              if(preg_match("/^[^,]+,".normalizePreg($oc['TREE'])."/",$obj['dn'])){
                $res[$obj['dn']] = $obj;
              }
            }
          }
        }else{
          /* User filter? */
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $res = array_merge($res,get_sub_list($filter,$sys_categories,preg_replace("/,.*$/","",$oc['TREE']),$oc['TREE'].$base, $sys_attrs, GL_NONE | GL_SIZELIMIT));
          }
        }
      } 
    }

    /* Search for incoming objects */ 
    $filter = "(|(&".$userregex."(objectClass=goHard)(cn=".$this->DivListSystem->Regex.")))";
    $res = array_merge($res,get_list($filter,$sys_categories, get_ou('incomingou'),get_ou('incomingou').$base,$sys_attrs, GL_NONE | GL_SIZELIMIT));

    /* Get all gotoTerminal's */
    foreach ($res as $value){

      $tmp= $value['dn'];
      $add= "";

      /* Extract base */
      foreach($objs as $obj){
        if(preg_match("/,".$obj['TREE']."/i",$value['dn'])){
          $tmp = trim( preg_replace("/^[^,]+,[^o]*".$obj['TREE']."/i","",$value['dn']));
        }
      }

      /* Create a string containing the last part of the department. */
      $dn_name = preg_replace("#^([^/]+/)*#","",convert_department_dn(@LDAP::fix($tmp)));
      if(empty($dn_name)){
        $dn_name = "/";
      }

      /* check if current object is a new one */
      if (preg_match ("/,".get_ou('incomingou')."/i", $tmp)){
        if (in_array_ics('gotoTerminal', $value['objectClass'])){
          $add= "- "._("New terminal");
        }elseif (in_array_ics('gotoWorkstation', $value['objectClass'])){
          $add= "- "._("New workstation");
        }elseif (in_array_ics('GOhard', $value['objectClass']) && !isset($value['gotoMode'])){
          $add= "- "._("Unknown device");
        }elseif (in_array_ics('GOhard', $value['objectClass'])){
          $add= "- "._("New Device");
        }
      } 

      /* Detect type of object and create an entry for $this->terminals */
      $terminal = array();

      if (in_array_ics('gotoTerminal', $value["objectClass"])){

        /* check acl */
        $acl = $ui->get_permissions($value['dn'],"terminal/termgeneric");
        if($add != "" || preg_match("/r/",$acl)) {
          if (isset($value["macAddress"][0]) && $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "T";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['message']  = _("Terminal template for")."&nbsp;'".$dn_name."'&nbsp;";
            $terminal['location'] = array_search($tmp, $this->config->departments); 
          }
        }
      } elseif (in_array_ics('gotoWorkstation', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"workstation/workgeneric");
        if($add != "" || preg_match("/r/",$acl)) {
          if (isset($value["macAddress"][0]) &&  $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "L";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['location'] = array_search($tmp, $this->config->departments);
            $terminal['message']  = _("Workstation template for")."&nbsp;'".$dn_name."'&nbsp;";
          }
          if (isset($value["FAIstate"][0])){
            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
          }
        }
      } elseif (in_array_ics('gotoPrinter', $value["objectClass"])){
       
   
        $acl = $ui->get_permissions($value['dn'],"printer/printgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "P";
        }
      } elseif (in_array_ics('goServer', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"server/servgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "S";
          if (isset($value["FAIstate"][0])){
            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
          }
        }
      } elseif (in_array_ics('goFonHardware', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"phone/phoneGeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "F";
        }
      }elseif (in_array_ics("GOhard",$value['objectClass'])){

        $acl =  $ui->get_permissions($value['dn'],"server/servgeneric"). 
                $ui->get_permissions($value['dn'],"terminal/termgeneric").
                $ui->get_permissions($value['dn'],"workstation/workgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal = $value;
          $terminal['type']   = "Q";
          $terminal['is_new'] = $add;
        }
      } elseif (in_array_ics('ieee802Device', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"component/componentGeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "C";
        }
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

      if(count($terminal)){
        $this->terminals[]=$terminal;
      }
    }

    $tmp  =array();
    $tmp2 =array();
    foreach($this->terminals as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['dn']]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['dn']] = strtolower($val['cn'][0]).$val['dn'];
    }
    natcasesort($tmp2);
    $this->terminals=array();
    foreach($tmp2 as $val){
      $this->terminals[]=$tmp[$val];
    }
    reset ($this->terminals);
  }

  function remove_lock()
  {
    if (isset($this->systab->dn)){
      del_lock ($this->systab->dn);
    }
  }


  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }


    $tabs = array(
        "terminal"    => array( "CLASS"     =>"TERMTABS",     "TABNAME" =>"termgeneric",     
          "TABCLASS"  =>"termtabs",     "ACL"     =>"terminal"),
        "workstation" => array( "CLASS"     =>"WORKTABS",     "TABNAME" =>"workgeneric",     
          "TABCLASS"  =>"worktabs",     "ACL"     =>"workstation"),
        "server"      => array( "CLASS"     =>"SERVTABS",     "TABNAME" =>"servgeneric",     
          "TABCLASS"  =>"servtabs",     "ACL"     =>"server"),
        "printer"     => array( "CLASS"     =>"PRINTTABS",    "TABNAME" =>"printgeneric",    
          "TABCLASS"  =>"printtabs",    "ACL"     =>"printer"),
        "phone"       => array( "CLASS"     =>"PHONETABS",    "TABNAME" =>"phoneGeneric",    
          "TABCLASS"  =>"phonetabs",    "ACL"     =>"phone"),
        "component"   => array( "CLASS"     =>"COMPONENTTABS","TABNAME" =>"componentGeneric",
          "TABCLASS"  =>"componenttabs","ACL"     =>"component"));

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn     = $this->terminals[$s_entry]['dn'];
      $oc     = $this->terminals[$s_entry]['objectClass'];
      $type   = $this->get_system_type($this->terminals[$s_entry]);

      $tab_o  = $tabs[$type]['CLASS'];
      $tab_c  = $tabs[$type]['TABCLASS'];
      $acl    = $tabs[$type]['ACL'];

      $this->CopyPasteHandler->add_to_queue($dn,$s_action,$tab_c,$tab_o,$acl);
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->terminals[$id]['dn'];
        $oc = $this->terminals[$id]['objectClass']; 
        $type = $this->get_system_type($this->terminals[$id]);

        if(isset($tabs[$type])){
          $tab_o  = $tabs[$type]['CLASS'];
          $tab_c  = $tabs[$type]['TABCLASS'];
          $acl    = $tabs[$type]['ACL'];

          if($s_action == "copy_multiple"){
            $this->CopyPasteHandler->add_to_queue($dn,"copy",$tab_c,$tab_o,$acl);
          }
          if($s_action == "cut_multiple"){
            $this->CopyPasteHandler->add_to_queue($dn,"cut",$tab_c,$tab_o,$acl);
          }
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
      $this->CopyPasteHandler->SetVar("base",$this->DivListSystem->selectedBase);

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


  function get_system_type($attrs)
  {
    $classes = $attrs['objectClass'];

    $type= "";
    if (in_array_ics('ieee802Device', $classes)){
      $type= "component";
    }elseif (in_array_ics('gotoTerminal', $classes)){
      $type= "terminal";
    }elseif (in_array_ics('gotoWorkstation', $classes)){
      $type= "workstation";
    }elseif (in_array_ics('gotoPrinter', $classes)){
      $type= "printer";
    }elseif (in_array_ics('goFonHardware', $classes)){
      $type= "phone";
    }elseif (in_array_ics('goServer', $classes)){
      $type= "server";
    }elseif (in_array_ics('GOhard', $classes) && !isset($attrs['gotoMode']) && $this->arp_handling_active){
      $type= "ArpNewDevice";
    }elseif (in_array_ics('GOhard', $classes)){
      $type= "NewDevice";
    }elseif (in_array_ics('sambaAccount', $classes) ||
        in_array_ics('sambaSamAccount', $classes)){
      $type= "winstation";
    }
    return ($type);
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(	
        "NQ" => array("select_newsystem.png",_("New System from incoming")),
        "D" => array("select_default.png",_("Template")),
        "T" => array("select_terminal.png",_("Terminal")),
        "L" => array("select_workstation.png",_("Workstation")),
        "GL" => array("select_workstation_green.png",_("Workstation is installing")),
        "YL" => array("select_workstation_yellow.png",_("Workstation is waiting for action")),
        "RL" => array("select_workstation_red.png",_("Workstation installation failed")),
        "F" => array("select_phone.png",_("Phone")),
        "S" => array("select_server.png",_("Server")),
        "GS" => array("select_server_green.png",_("Server is installing")),
        "YS" => array("select_server_yellow.png",_("Server is waiting for action")),
        "RS" => array("select_server_red.png",_("Server installation failed")),
        "W" => array("select_winstation.png",_("Winstation")),
        "C" => array("select_component.png",_("Network Device")),
        "NT"=> array("select_new_terminal.png",_("New Terminal")),
        "NL"=> array("select_new_workstation.png",_("New Workstation")),
        "P" => array("select_printer.png",_("Printer")));

    if((isset($input['is_new']))&&(!empty($input['is_new']))){
      $input['type']="N".$input['type'];
    }
    foreach ($conv  as $key => $value){
      if($input['type']==$key){
        $tmp['img'] ="<img class='center' src='images/".$value[0]."' alt='".$key."' title='".$value['1']."'>";
        $tmp['class']=$key;
        return $tmp;
      }
    }
  }

  
  function getState($type, $state)
  {
    switch (preg_replace('/:.*$/', '', $state)) {
      case 'installing':
                $type= 'G'.$type;
                break;
      case 'error':
                $type= 'R'.$type;
                break;
      case 'install':
      case 'sysinfo':
      case 'softupdate':
      case 'scheduledupdate':
                $type= 'Y'.$type;
                break;
    }


    return ($type);
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


  /* !! Incoming dummy acls, required to defined acls for incoming objects
   */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Incoming objects"),
          "plDescription" => _("Incoming objects"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 99,
          "plSection"     => array("administration"),
          "plCategory"    => array("incoming"   => array( "description"  => _("Incoming"),
                                                          "objectClass"  => "")),
          "plProvidedAcls"=> array()
            
          ));
  }


  

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
