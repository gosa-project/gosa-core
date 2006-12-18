<?php

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

  function divListUsers ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Users");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of users"));
    $this->SetSummary(_("List of users"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, edit and delete selected users. Having a great number of users, you may want to use the range selectors on top of the user list."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Username")." / "._("Department")));
    $this->AddHeader(array("string"=>_("Properties"),   "attach" => "style='width:152px;'"));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:102px;border-right:0px;text-align:right;'"));
    
    $this->AddCheckBox("ShowTemplates"      ,_("Select to see template pseudo users")               ,_("Show templates")        , false);
    $this->AddCheckBox("ShowFunctionalUsers",_("Select to see users that have only a GOsa object"),_("Show functional users") , true);
    $this->AddCheckBox("ShowUnixUsers"      ,_("Select to see users that have posix settings")      ,_("Show unix users")       , true);
    $this->AddCheckBox("ShowMailUsers"      ,_("Select to see users that have mail settings")       ,_("Show mail users")       , true);
    $this->AddCheckBox("ShowSambaUsers"     ,_("Select to see users that have samba settings")      ,_("Show samba users")      , true);
    $this->AddCheckBox("ShowProxyUsers"     ,_("Select to see users that have proxy settings")      ,_("Show proxy users")      , true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
  }

  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    /* Get copy & paste icon */ 
    if($this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
    }else{
      $Copy_Paste ="";
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    $listhead.= " <input class='center' type='image' align='middle' src='images/list_new_user.png' 
      title='"._("Create new user")."' alt='"._("New user")."'           name='user_new'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_new.png' 
      title='"._("Create new template")."' alt='"._("New template")."'        name='user_tplnew'>&nbsp;".
      $Copy_Paste.
      " <img   class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;".
      "</div>";

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

    /* Variable initialation */
    $enviro     = $posix = $maila = $faxac = $samba = $netatalk = "";
    $empty      = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";
    $userimg    = "<img class='center' src='images/select_user.png' alt='User' title='%s'>";
    $tplimg     = "<img class='center' src='images/select_template.png' alt='Template' title='%s'>";
    $editlink   = "<a href='?plug=".validate($_GET['plug'])."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Create action icons */
    $action = "";
    if($this->parent->CopyPasteHandler){
      $action .= "<input class='center' type='image'
        src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
      $action.= "<input class='center' type='image'
        src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
    }
    $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'
      name='user_edit_%KEY%' title='"._("Edit user")."'>";
    $action.= "<input class='center' type='image' src='images/list_password.png' alt='"._("password")."'
      name='user_chgpw_%KEY%' title='"._("Change password")."'>";
    $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'
      name='user_del_%KEY%' title='"._("Delete user")."'>";
    
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

    /* Get Configuration for goFon DB (if it is set), to connect to the asterisk tables.
     * Read Sip Table for specified account (for each user with phoneAccount).
     * Check the attributes ip port and regseconds.
     * If regseconds is set and >0 , the phone is logged in.
     * Else the phone is currently not logged.
     * If we can't read any Data from the DB or there is no goFon DB specified
     * show old style without status icons.
     */
    $r_db   =false;
    $r_con  =false;
    if (isset($_SESSION['config']->data['SERVERS']['FON'])){
      $a_SETUP= $_SESSION['config']->data['SERVERS']['FON'];
      $r_con = false;
      $r_db  = false;
      if(is_callable("mysql_pconnect")){
        $r_con= @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);
        // Check if we are  connected correctly
        if($r_con){
          $r_db  =  @mysql_select_db($a_SETUP['DB'],$r_con);
        }
      }
    }
   
 
    /********************
      Append entries to divlist 
     ********************/
 
    // Test Every Entry and generate divlist Array
    foreach($list as $key => $val){

      /* Create phonaccopunt informationm, if conencted && is phoneAccount */ 
      $connected  = ""; // This string represents timestamp or offline status
      $ip_port    = ""; // String that will represent ip : port of the connected phone
      if((in_array("goFonAccount"    ,$val['objectClass']))){

        /* Set defaults */
        $fonac = preg_replace("/%image%/", "select_phone.png", $fonimg);
        $fonac = preg_replace("/%KEY%/", "$key", $fonac);
        $fonac = preg_replace("/%title%/", "", $fonac);

        /* Database connection is ok ?*/
        if(($r_db)&&(is_callable("mysql_query"))){
          $res= @mysql_query("SELECT regseconds,name,port,ipaddr FROM ".$a_SETUP['SIP_TABLE']." WHERE (name='".$val['uid'][0]."')");
          $mysql_entry = @mysql_fetch_row($res);
          if(is_array($mysql_entry)){
            if((isset($mysql_entry[0]))&&($mysql_entry[0]>1)){
              $connected = " | "._("Online")." : ".gmdate("d.m.Y H:i:s",($mysql_entry[0]+(60*60)));
              $fonac = preg_replace("/%image%/", "select_phone_connected.png", $fonimg);
              $fonac = preg_replace("/%KEY%/", "$key", $fonac);
              $fonac = preg_replace("/%title%/", $connected, $fonac);
              #$ip_port= " - ".$mysql_entry[3].":".$mysql_entry[2];
            }
            if((isset($mysql_entry[0]))&&($mysql_entry[0]==0)){
              $connected = " | "._("Offline");
              $fonac = preg_replace("/%image%/", "select_phone.png", $fonimg);
              $fonac = preg_replace("/%KEY%/", "$key", $fonac);
              $fonac = preg_replace("/%title%/", $connected, $fonac);
            }
          }
        }
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
      }else{
        $s_img_create_from_template   = "";
        $tpl                          = $userimg;
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

      /* Create each field */
      $field1 = array("string" => sprintf($tpl,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display).$ip_port, "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => $UseImg, "attach" => "style='width:152px;'");
      $field4 = array("string" => $s_img_create_from_template.preg_replace("/%KEY%/", "$key", $action),
                      "attach" => "style='width:102px;border-right:0px;    text-align:right;'");
      /* Add to list */
      $add = array($field1,$field2,$field3,$field4);
      $this->AddElement($add);

      // Template or User
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpls[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }else{
        $users[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }
    }
    
    /* close database connection, if it was opened */
    if(isset($r_con)){
      if((is_callable("mysql_close"))&&($r_con)){
        @mysql_close($r_con);
      }
    }

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
