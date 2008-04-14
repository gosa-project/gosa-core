<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class divListUsers extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes */
  var $ShowTemplates ;
  var $ShowFunctionalUsers;
  var $ShowUnixUsers;
  var $ShowMailUsers;
  var $ShowSambaUsers;
  var $ShowProxyUsers;

  /* Subsearch checkbox */
  var $SubSearch              = false;

  var $parent             ;
  var $ui                 ;

  function divListUsers (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Users", "users");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of users"));
    $this->SetSummary(_("List of users"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 122;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' 
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=> "&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Username")." / "._("Department")));
    $this->AddHeader(array("string"=>_("Properties"),   "attach" => "style='width:166px;'"));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));
    
    $this->AddCheckBox("ShowTemplates"      ,msgPool::selectToView(_("Template"))             ,sprintf(_("Show %s"),_("templates")), false);
    $this->AddCheckBox("ShowFunctionalUsers",msgPool::selectToView(_("GOsa object"),"enabled"),sprintf(_("Show %s"),_("functional users")), true);
    $this->AddCheckBox("ShowUnixUsers"      ,msgPool::selectToView(_("Posix"),"enabled")      ,sprintf(_("Show %s"),_("POSIX users")), true);
    $this->AddCheckBox("ShowMailUsers"      ,msgPool::selectToView(_("Mail"),"enabled")       ,sprintf(_("Show %s"),_("mail users")), true);
    $this->AddCheckBox("ShowSambaUsers"     ,msgPool::selectToView(_("Samba"),"enabled")      ,sprintf(_("Show %s"),_("samba users")), true);
    $this->AddCheckBox("ShowProxyUsers"     ,msgPool::selectToView(_("Proxy"),"enabled")      ,sprintf(_("Show %s"),_("proxy users")), true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
  }


  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */ 
    $base = $this->config->current['BASE'];

    /* Add base */
    $tmp = array();
    $tmp[] = array("dn"=>$this->config->current['BASE']);
    $tmp=  array_merge($tmp,get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH));
    
    $deps = array();
    foreach($tmp as $tm){
      $deps[$tm['dn']] = $tm['dn'];
    }    


    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("users");
    $ids = $this->config->idepartments;
    $first = "";
    $found = FALSE;
    foreach($ids as $dep => $name){
      if(isset($deps[$dep]) && in_array_ics($dep, $tdeps)){

        /* Keep first base dn in mind, we could need this
         *  info if no valid base was found
         */
        if(empty($first)) {
          $first = $dep['dn'];
        }

        $value = $ids[$dep]; 
        if ($this->selectedBase == $dep){
          $found = TRUE;
          $options.= "<option selected='selected' value='".$dep."'>$value</option>";
        } else {
          $options.= "<option value='".$dep."'>$value</option>";
        }
      }
    }

    /* The currently used base is not visible with your acl setup.
     * Set base to first useable base. 
     */
    if(!$found){
      $this->selectedBase = $first;
    }

    /* Get copy & paste icon */
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"users") ;
    $acl      = $ui->get_permissions($this->selectedBase,"users/user");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead .=
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;";


    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s.= "...|<input class='center' type='image' src='images/list_new_user.png' alt=''>".
        "&nbsp;"._("User")."|user_new|\n";
      $s.= "...|<input class='center' type='image' src='images/list_new.png' alt=''>".
        "&nbsp;"._("Template")."|user_tplnew|\n";
    }

    /* Multiple options */ 
    $s.= "..|---|\n";
    $s.= "..|<img src='images/edit.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Edit")."|"."multiple_edit|\n";
    $s.= "..|<img src='images/list_password.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Change password")."|"."multiple_password_change|\n";
    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";
    $s.= "..|<img src='images/wizard.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Apply template")."|"."templatize_multiple|\n";

    /* Add event tag */
    if(class_available("DaemonEvent")){
      $events  = DaemonEvent::get_event_types(USER_EVENT);
      if(count($events['BY_CLASS'])){
        $s.= "..|---|\n";
        foreach($events['BY_CLASS'] as $name => $event){
          $s.= "..|".$event['MenuImage']."&nbsp;".$event['s_Menu_Name']."|event_".$name."\n";
        }
      }
    }

    /* Add multiple copy & cut icons */
    if(is_object($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s.= "..|---|\n";
      $s.= "..|<img src='images/lists/copy.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Copy")."|"."multiple_copy_systems|\n";
      $s.= "..|<img src='images/lists/cut.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Cut")."|"."multiple_cut_systems|\n";
      
      if($this->parent->CopyPasteHandler->entries_queued()){
        $img = "<img border='0' class='center' src='images/lists/paste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."|editPaste|\n";
      }else{
        $img = "<img border='0' class='center' src='images/lists/paste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."\n";
      }
    }

    /* Add snapshot icons */
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s .= "..|---|\n";
      $s .= $this->get_snapshot_header(TRUE);
    }

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    /********************
      Variable init
     ********************/
    
    $num_users      = 0;
    $num_templates  = 0;

    /* Variable initialation */
    $enviro     = $posix = $maila = $faxac = $samba = $netatalk = "";
    $empty      = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt='-'>";
    $userimg    = "<img class='center' src='images/select_user.png' alt='User' title='%s'>";
    $tplimg     = "<img class='center' src='images/select_template.png' alt='Template' title='%s'>";
    $editlink   = "<a href='?plug=".validate($_GET['plug'])."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 122;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Possilbe objectClass image combinations */  
    $possibleAccounts = array(
            "posixAccount"    => array("VAR"=>"posix"     ,"IMG"=>"posiximg"),
            "gotoEnvironment" => array("VAR"=>"enviro"    ,"IMG"=>"eviroimg"),
            "gosaMailAccount" => array("VAR"=>"maila"     ,"IMG"=>"mailimg"),
            "goFaxAccount"    => array("VAR"=>"faxac"     ,"IMG"=>"faximg"),
            "sambaSamAccount" => array("VAR"=>"samba"     ,"IMG"=>"sambaimg"),
            "apple-user"      => array("VAR"=>"netatalk"  ,"IMG"=>"netatalkimg"));

     /* Pictures for Extensions */
    $usrimg   ="<input class='center' type='image' src='images/select_user.png' alt='"._("GOsa")."'
                    name='user_edit_%KEY%-user' title='"._("Edit generic properties")."'>";
    $posiximg = "<input class='center' type='image' src='images/penguin.png' alt='"._("Posix")."'
                    name='user_edit_%KEY%-posixAccount' title='"._("Edit UNIX properties")."'>";
    $eviroimg = "<input class='center' type='image' src='images/smallenv.png' alt='"._("Environment")."'
                    name='user_edit_%KEY%-environment' title='"._("Edit environment properties")."'>";
    $mailimg  = "<input class='center' type='image' src='images/mailto.png' alt='"._("Mail")."'
                    name='user_edit_%KEY%-mailAccount' title='"._("Edit mail properties")."'>";
    $fonimg   = "<input class='center' type='image' src='images/%image%' alt='"._("Phone")."'
                    name='user_edit_%KEY%-phoneAccount' title='"._("Edit phone properties")."%title%'>";
    $faximg   = "<input class='center' type='image' src='images/fax_small.png' alt='"._("Fax")."'
                    name='user_edit_%KEY%-gofaxAccount' title='"._("Edit fax properies")."'>";
    $sambaimg = "<input class='center' type='image' src='images/select_winstation.png' alt='"._("Samba")."'
                    name='user_edit_%KEY%-sambaAccount' title='"._("Edit samba properties")."'>";
    $netatalkimg = "<input class='center' type='image' src='images/select_netatalk.png' alt='"._("Netatalk")."'
                    name='user_edit_%KEY%-netatalk' title='"._("Edit netatalk properties")."'>";
    $tplcreateuserimg  = "<input type='image' class='center' src='images/list_new.png' alt='"._("Create user from template")."'
                    name='userfrom_tpl_%KEY%' title='"._("Create user with this template")."'>";

    /********************
      END :: Variable init
     ********************/

 
    /********************
      Append entries to divlist 
     ********************/
 
    $ui = get_userinfo(); 
  
    // Test Every Entry and generate divlist Array
    foreach($list as $key => $val){
  
      /* Create action icons */
      $action= "";

      /* Add copy & cut icons */     
      $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"users") ;
      $acl      = $ui->get_permissions($val['dn'],"users/user");
      if(preg_match("/(c.*w|w.*c)/",$acl_all) && $this->parent->CopyPasteHandler){
        $action .= "<input class='center' type='image'
          src='images/lists/cut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $action.= "<input class='center' type='image'
          src='images/lists/copy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      /* Add deactivate status */
      $lock_status = "";

      if(isset($val['userPassword'][0]) && preg_match("/^\{[^\}]/",$val['userPassword'][0])){
        if(preg_match("/^[^\}]*+\}!/",$val['userPassword'][0])){
          $lock_status = "<input class='center' type='image' src='images/lists/locked.png'
            alt='"._("inactive")."' name='toggle_lock_status_%KEY%'>";
        }else{
          $lock_status = "<input class='center' type='image' src='images/lists/unlocked.png'
            alt='"._("active")."' name='toggle_lock_status_%KEY%'>";
        }
      }else{
        $lock_status = "<img src='images/empty.png' alt=' ' class='center'>";
      }

      /* Generate edit icon */
      $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'
        name='user_edit_%KEY%' title='".msgPool::editButton(_("user"))."'>";

      /* Add deactivate status if we are allowed to */
      if (preg_match("/w/",$acl_all)){
        $action.= $lock_status;
      }

      /* Add Password change icon, if we are able to write users/password */
      if(!in_array_ics("gosaUserTemplate",$val['objectClass']) && preg_match("/w/",$ui->get_permissions($val['dn'],"users/password"))) {
        $action.= "<input class='center' type='image' src='images/list_password.png' alt='"._("password")."'
          name='user_chgpw_%KEY%' title='"._("Change password")."'>";
      }else{
        $action.= "<img class='center' src='images/empty.png' alt='&nbsp;' 
          title='"._("Not allowed")."'>";
      }

      /* Add snapshot icon */
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $action.= $this->GetSnapShotActions($val['dn']);
      }

      /* Add remove icon, if we are allowed to remove the current user */
      if(preg_match("/d/",$ui->get_permissions($val['dn'],"users/user"))) {
        $action.= "<input class='center' type='image' src='images/lists/trash.png' alt='"._("delete")."'
          name='user_del_%KEY%' title='"._("Delete user")."'>";
      }else{
        $action.= "<img class='center' src='images/empty.png' alt='&nbsp;'  
          title='"._("Not allowed")."'>";
      }

      /* Create phonaccopunt informationm, if conencted && is phoneAccount */ 
      if((in_array("goFonAccount"    ,$val['objectClass']))){

        /* Set defaults */
        $fonac = preg_replace("/%image%/", "select_phone.png", $fonimg);
        $fonac = preg_replace("/%KEY%/", "$key", $fonac);
        $fonac = preg_replace("/%title%/", "", $fonac);

      }else{
        $fonac=$empty;
      }

      /* Set images for different types of objectClasses */
      foreach($possibleAccounts as $objectClass => $Settigns){ 
        if(in_array($objectClass ,$val['objectClass'])){
          $$Settigns['VAR'] = preg_replace("/%KEY%/", "$key", $$Settigns['IMG']);
        }else{
          $$Settigns['VAR'] = $empty;
        }
      }

      /* Create userimg */
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpl                          = preg_replace("/%KEY%/", "$key", $tplimg);
        $s_img_create_from_template   = preg_replace("/%KEY%/", "$key", $tplcreateuserimg);
        $num_templates ++;
      }else{
        $s_img_create_from_template   = "";
        $tpl                          = $userimg;
        $num_users ++;
      }

      /* Insert key into userimg */
      $usrimg2 = preg_replace("/%KEY%/", "$key", $usrimg);

      // Generate caption for rows
      if (isset($val["sn"]) && isset($val["givenName"])){
        $display= $val["sn"][0].", ".$val["givenName"][0]." [".$val["uid"][0]."]";
      } else {
        $display= "[".$val["uid"][0]."]";
      }

      /* Connect all images */
      $UseImg = $usrimg2."&nbsp;".$posix."&nbsp;".$enviro."&nbsp;".$maila."&nbsp;".$fonac."&nbsp;".$faxac."&nbsp;".$samba."&nbsp;".$netatalk;

      /* Cutted objects should be displayed in light grey */ 
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $val['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");
      $field1 = array("string" => sprintf($tpl,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => $UseImg, "attach" => "style='width:166px;'");
      $field4 = array("string" => $s_img_create_from_template.preg_replace("/%KEY%/", "$key", $action),
                      "attach" => "style='width:".$action_col_size."px;border-right:0px;    text-align:right;'");
      /* Add to list */
      $add = array($field0,$field1,$field2,$field3,$field4);
      $this->AddElement($add);

      // Template or User
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpls[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }else{
        $users[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }
    }


    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_users = count($list);

    $num_user_str = _("Number of listed users");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='images/select_user.png'
              title='".$num_user_str."' alt='".$num_user_str."'>&nbsp;".$num_users."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/folder.png'
              title='".$num_dep_str."' alt='".$num_dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";

    $this->set_List_Bottom_Info($str);
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
