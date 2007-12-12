<?php

class gofaxAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "FAX";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users fax account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");


  /* Department list shown in the divSelectList*/
  var $departments;

  /* Fax attributes */
  var $goFaxDeliveryMode= "";
  var $facsimileTelephoneNumber= "";
  var $goFaxIsEnabled= 1;
  var $goFaxPrinter= "";
  var $goFaxDivertNumber= "";
  var $goFaxLanguage= "";
  var $goFaxFormat= "";
  var $goFaxRBlocklist = array();
  var $goFaxRBlockgroups= array();
  var $goFaxSBlocklist= array();
  var $goFaxSBlockgroups= array();
  var $mail= "";
  var $facsimileAlternateTelephoneNumber= array();
  var $fax_formats = array("pdf","ps","png","mtiff","tiff");  

  /* Internal variables */
  var $printerList= array();
  var $has_mailAccount= FALSE;
  var $locals_dialog= FALSE;
  var $in_blocklist_dialog= FALSE;
  var $out_blocklist_dialog= FALSE;
  var $current_blocklist= array();
  var $view_logged = FALSE;

  /* Copy & paste variables */
  var $CopyPasteVars=array("facsimileTelephoneNumber");//,"goFaxRBlocklist","goFaxRBlockgroups","goFaxSBlocklist","goFaxSBlockgroups");

  /* attribute list for save action */
  var $attributes= array("goFaxDeliveryMode", "goFaxIsEnabled","facsimileAlternateTelephoneNumber","goFaxRBlocklist","goFaxRBlockgroups","goFaxSBlocklist","goFaxSBlockgroups","goFaxPrinter", "goFaxDivertNumber", "goFaxLanguage", "goFaxFormat", "mail","facsimileTelephoneNumber");

  var $uid ="";

  var $objectclasses= array("goFaxAccount");

  function gofaxAccount (&$config, $dn= NULL)
  {
    /* General initialization */
    plugin::plugin ($config, $dn);

    /* Set uid, it is used in handle_post_events */
    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Hickert : 11.11.05
     * Added to be able to handle department selection in divSelelect
     */
    if(!isset($_SESSION["Fax_Filter"])){
      $_SESSION['Fax_Filter']['depselect'] = $this->config->current['BASE'];
    }

    if ($dn != "new"){
      /* Get arrays */
      foreach (array("goFaxRBlocklist", "goFaxRBlockgroups", "goFaxSBlocklist",
            "goFaxSBlockgroups", "facsimileAlternateTelephoneNumber") as $val){
        $this->$val =array();
        if (isset($this->attrs["$val"]["count"])){
          for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }

      /* Set up has_mailAccount */
      if (in_array("gosaMailAccount", $this->attrs['objectClass'])){
        $this->has_mailAccount= TRUE;
      }
    }

    /* Load printer list */
    if (isset($this->config->data['SERVERS']['CUPS'])){
      $this->printerList= get_printer_list ($this->config->data['SERVERS']['CUPS']);
      asort ($this->printerList);
    }

    /* Check if the currently selected printer is still available. 
       If not, append current printer to list of available.
       It could be possible, that we are not allowed to view printers and so the list is empty ...  */
    if(!empty($this->goFaxPrinter) && !isset($this->printerList[$this->goFaxPrinter])) {
      $this->printerList[$this->goFaxPrinter] = "[".$this->goFaxPrinter."]";
    }

    /* Get global filter config */
    if (!is_global("faxfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $faxfilter= array( "depselect"       => $base,
          "fuser"            => "*",
          "regex"           => "");
      register_global("faxfilter", $faxfilter);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Hickert : 11.11.05
     * Added to be able to handle department selection in divSelelect
     */
    if((isset($_GET['act']))&&($_GET['act']=="dep_open")){

      if(strlen(base64_decode($_GET['dep_id']))>= strlen($this->config->current['BASE'])){
        $_SESSION['Fax_Filter']['depselect']= base64_decode($_GET['dep_id']);
      }else{
        $_SESSION['Fax_Filter']['depselect']= $this->config->current['BASE'];
      }
    }

    /* Edit mode specifies if we are editing from my accout */
    $edit_mode = (!is_object($this->parent) && !isset($_SESSION['edit']));

    /* Load smarty stuff */
    $smarty= get_smarty();

    /* Check if mail account is active. We'll display an additional
       mail address field if this is not the case. Some people may
       want goFax, but have a mailserver which is not managed with
       GOsa */
    if (!@isset($this->parent->by_object['mailAccount'])) {
      $smarty->assign("has_mailaccount", $this->has_mailAccount?"true":"false");
    } elseif ( !$this->parent->by_object['mailAccount']->is_account){
      $smarty->assign("has_mailaccount", "false");
      $this->has_mailAccount= false;
    } else {
      $smarty->assign("has_mailaccount", "true");
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no fax extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display= $this->show_disable_header(_("Remove fax account"),
            _("This account has fax features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_enable_header(_("Create fax account"),
            _("This account has fax features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Trigger Add local fax alternatives dialog */
    if (isset($_POST['add_local_alternate'])){
      if($this->acl_is_writeable("facsimileAlternateTelephoneNumber",$edit_mode)){
        $this->locals_dialog= TRUE;	
        $this->dialog= TRUE;	
      }
    }

    /* Add alternatives from dialog */
    if (isset($_POST['add_locals_finish']) && isset($_POST['local_list'])){
      if($this->acl_is_writeable("facsimileAlternateTelephoneNumber",$edit_mode)){
        foreach ($_POST['local_list'] as $val){
          $this->addAlternate($val);
          $this->is_modified= TRUE;
        }
      }
    }

    /* Add alternatives */
    if (isset($_POST['add_alternate']) && !empty($_POST['forward_address']) && is_phone_nr($_POST['forward_address'])){ 
      if($this->acl_is_writeable("facsimileAlternateTelephoneNumber",$edit_mode)){
        $this->addAlternate($_POST['forward_address']);
      }
    }

    /* Delete alternate fax number */
    if (isset($_POST['delete_alternate']) && isset($_POST['alternate_list']) && count($_POST['alternate_list'])){ 
      if($this->acl_is_writeable("facsimileAlternateTelephoneNumber",$edit_mode)){
        $this->delAlternate ($_POST['alternate_list']);
      }
    }


    /* Edit incoming blocklists */
    if (isset($_POST['edit_incoming'])){
      if($this->acl_is_writeable("goFaxRBlocklist",$edit_mode))  {
        $this->current_blocklist= array_merge($this->goFaxRBlocklist,$this->goFaxRBlockgroups);
        sort($this->current_blocklist);
        reset($this->current_blocklist);

        $this->in_blocklist_dialog= TRUE;
        $this->dialog= TRUE;
      }
    }

    /* Edit outgoing blocklists */
    if (isset($_POST['edit_outgoing'])){
      $this->current_blocklist= array_merge($this->goFaxSBlocklist,$this->goFaxSBlockgroups);
      sort($this->current_blocklist);
      reset($this->current_blocklist);

      $this->out_blocklist_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Add number to blocklist (dialog) */
    if (isset($_POST['add_blocklist_number']) && $_POST['block_number'] != ""){
      if (!is_phone_nr($_POST['block_number'])){
        print_red (_("You're trying to add an invalid phone number."));
      } else {
        array_push($this->current_blocklist, $_POST['block_number']);
        $this->current_blocklist= array_unique($this->current_blocklist);
        sort($this->current_blocklist);
        reset($this->current_blocklist);
      }
    }

    /* Hickert : 11.11.05
     * Add selection from divSelelect to our Blocklist 
     */
    /* Add list to blocklist */
    if (isset($_POST['add_blocklist'])){
      foreach($_POST as $name => $value){
        if(preg_match("/ADDLIST_/i",$name)){
          $this->current_blocklist= array_merge($this->current_blocklist, array(base64_decode($value)));

        }
      } 
      $this->current_blocklist= array_unique($this->current_blocklist);
      sort($this->current_blocklist);
      reset($this->current_blocklist);
    }

    /* Add list to blocklist */
    if (isset($_GET['add'])){
      if(!is_array($this->current_blocklist)) $this->current_blocklist=array();
      $this->current_blocklist= array_merge($this->current_blocklist, array( base64_decode($_GET['add'])));
      $this->current_blocklist= array_unique($this->current_blocklist);
      sort($this->current_blocklist);
      reset($this->current_blocklist);
    }

    /* Delete from blocklist */
    if (isset($_POST['delete_blocklist_number']) && isset($_POST['block_list'])){
      $tmp= array();
      foreach($this->current_blocklist as $val){
        if (!in_array($val, $_POST['block_list'])){
          $tmp[]= $val;
        }
      }
      $this->current_blocklist= $tmp;
    }


    /* Blocklist edit finished */
    if (isset($_POST['edit_blocklists_finish'])){

      /* Incoming or outgoing? */
      if ($this->in_blocklist_dialog){
        $this->goFaxRBlocklist = array();
        $this->goFaxRBlockgroups = array();

        foreach ($this->current_blocklist as $val){
          if (is_phone_nr($val)){
            $this->goFaxRBlocklist[]=$val;
          } else {
            $this->goFaxRBlockgroups[]= $val;
          }
        }
      } else {
        $this->goFaxSBlocklist = array();
        $this->goFaxSBlockgroups = array();

        /* Transfer values to ourself */
        foreach ($this->current_blocklist as $val){
          if (is_phone_nr($val)){
            $this->goFaxSBlocklist[]=$val;
          } else {
            $this->goFaxSBlockgroups[]= $val;
          }
        }
      }
      $this->is_modified= TRUE;
    }


    /* Set departments */
    if ($this->locals_dialog || $this->in_blocklist_dialog || $this->out_blocklist_dialog){
      
      $list= array ();
      $ldap= $this->config->get_ldap_link();
      if (isset ($_POST['department'])){
        $ldap->cd ($_POST['department']);
      } else {
        $ldap->cd ($this->config->current['BASE']);
      }
    }

    /* Cancel  dialogs */
    if (isset($_POST['add_locals_cancel']) || isset($_POST['edit_blocklists_finish']) ||
        isset($_POST['edit_blocklists_cancel']) || isset($_POST['add_locals_finish'])){

      $this->locals_dialog= FALSE;
      $this->in_blocklist_dialog= FALSE;
      $this->out_blocklist_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Manage locals dialog */
    if ($this->locals_dialog){

      /* Save data */
      $faxfilter= get_global("faxfilter");
      foreach( array("depselect", "fuser", "regex") as $type){
        if (isset($_POST[$type])){
          $faxfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $faxfilter['fuser']= $s;
      }

      if ($faxfilter['regex'] != '*' && $faxfilter['regex'] != ""){
        $regex= $faxfilter['regex'];
        $filter= "(facimileTelephoneNumber=$regex)";
      } else {
        $filter= "";
      }
      if ($faxfilter['fuser'] != ""){
        $user= $faxfilter['fuser'];
        $filter= "$filter(|(uid=$user)(cn=$user)(givenName=$user)(sn=$user))";
      }

      $base= $faxfilter['depselect'];
      $res= get_list("(&(objectClass=goFaxAccount)$filter)", "gofax", $base, 
                     array("sn", "givenName", "facsimileTelephoneNumber"), GL_SIZELIMIT | GL_SUBSEARCH);

      foreach ($res as $attrs){
        $list[$attrs['facsimileTelephoneNumber'][0]]=
          $attrs['sn'][0].", ".
          $attrs['givenName'][0]." [".
          $attrs['facsimileTelephoneNumber'][0]."]";
      }

      /* Show dialog */
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("usearch_image", get_template_path('images/search_user.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("infoimage", get_template_path('images/info.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("departments", $this->config->idepartments);
      $smarty->assign("list", $list);
      if (isset($_POST['depselect'])){
        $smarty->assign("depselect", $_POST['depselect']);
      }
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("apply", apply_filter());
      $smarty->assign("alphabet", generate_alphabet());
      $smarty->assign("hint", print_sizelimit_warning());
      foreach( array("depselect", "fuser", "regex") as $type){
        $smarty->assign("$type", $faxfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());

      $display.= $smarty->fetch (get_template_path('locals.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }


    /* Manage incoming blocklists */
    if ($this->in_blocklist_dialog){

      /* This handles the divSelectBox */

      /* The base specifies the current position in the ldap tree 
       * The current base was specified by $_GET['dep_id'] before. Or contains the default value.
       */
      $base = $_SESSION['Fax_Filter']['depselect'];
      $ldap->cd($base);

      /* Ge all Blocklists */
      $ldap->search ("(objectClass=goFaxRBlock)",array("cn","description"));
      while ($attrs= $ldap->fetch()){

        /* Generate list depending on description */
        if(isset($attrs['description'][0])){
          $list[$attrs['cn'][0]]=
            $attrs['description'][0].
            " [".$attrs['cn'][0]."]";
        }else{
          $list[$attrs['cn'][0]]= $attrs['cn'][0];
        }
      }

      /* Create our divselect box */
      $divSel = new divSelectBox("divSelectPredefined");
      $divSel->setHeight(296);

      /* NEW LIST MANAGMENT
       * We also need to search for the departments
       * So we are able to navigate like in konquerer
       */
      $this->departments= array();
   
      /* Get all departments within the current department */ 
      $base = $_SESSION['Fax_Filter']['depselect'];
      $ldap->ls("(objectClass=gosaDepartment)",$base);

      /* Base back is used to go one department up in our ldap tree */
      $base_back = preg_replace("/^[^,]+,/","",$base);

      /* Only show base_back if it is needed */
      if((strlen($base_back)>= strlen($this->config->current['BASE']))&&($base!=$this->config->current['BASE'])){
        $this->departments[preg_replace("/^[^,]+,/","",$base)] = ".. - ["._("back")."]"; 
      }

      /* Fetch all returned departments an add them to our divlist */
      while($value = $ldap->fetch()){
        if($value["description"][0]!=".."){
          $this->departments[$value['dn']]=@LDAP::fix(convert_department_dn($value['dn'])." - [".$value["description"][0]."]");
        }else{
          $this->departments[$value['dn']]=$value["description"][0];
        }
      }
      /* END NEW LIST MANAGMENT
       */

      // This links specifies the department open link 
      $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

      /* Insert departments in divsel */
      foreach($this->departments as $key=> $val){
        if(!isset($this->config->departments[trim($key)])){
          $this->config->departments[trim($key)]="";
        }
        $field1 = array("string" => "<img src='images/folder.png' alt='department'>","attach"=>"style='width:16px;text-align:center;'");
        $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val),"attach"=>" style='border:none'");
        $divSel->AddEntry(array($field1,$field2));
      }

      /* Append predefined Blocklists */
      foreach($list as $entry => $value){
        $divSel->AddEntry(array(
              array("string"=>"<input type='checkbox' value='".base64_encode($entry)."' name='ADDLIST_".base64_encode($entry)."'>"),
              array("string"=>$value,"attach"=>"style='border:0px;'")
              ));
      }

      /* Show dialog */
      $smarty->assign("cblocklist", $this->current_blocklist);
      $smarty->assign("departments", $this->config->idepartments);
      $smarty->assign("divSelectPredefined", $divSel->DrawList());
      $display.= $smarty->fetch (get_template_path('lists.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Manage outgoing blocklists */

    if ($this->out_blocklist_dialog){

      /* This handles the divSelectBox */

      /* The base specifies the current position in the ldap tree
       * The current base was specified by $_GET['dep_id'] before. Or contains the default value.
       */
      $base = $_SESSION['Fax_Filter']['depselect'];
      $ldap->cd($base);

      /* Ge all Blocklists */ 
      $ldap->search ("(objectClass=goFaxSBlock)",array("cn","description"));
      while ($attrs= $ldap->fetch()){
        if(isset($attrs['description'][0])){
          $list[$attrs['cn'][0]]=
            $attrs['description'][0].
            " [".$attrs['cn'][0]."]";
        }else{
          $list[$attrs['cn'][0]]= $attrs['cn'][0];
        }
      }

      /*Create DivSel*/
      $divSel = new divSelectBox("divSelectPredefined");
      $divSel->setHeight(296);

      /* NEW LIST MANAGMENT
       * We also need to search for the departments
       * So we are able to navigate like in konquerer
       */
      $this->departments= array();
      $ldap->ls("(objectClass=gosaDepartment)",$base);

      /* Generate Back url, and append if it is needed */
      $base_back = preg_replace("/^[^,]+,/","",$base);
      if((strlen($base_back)>= strlen($this->config->current['BASE']))&&($base!=$this->config->current['BASE'])){
        $this->departments[preg_replace("/^[^,]+,/","",$base)] = ".. - ["._("back")."]";
      }

      /* Get all departments */
      while($value = $ldap->fetch()){
        if(isset($value["description"][0])){
          $this->departments[$value['dn']]=@LDAP::fix(convert_department_dn($value['dn'])." - [".$value["description"][0]."]");
        }else{
          $this->departments[$value['dn']]=$value["description"][0];
        }
      }
      /* END NEW LIST MANAGMENT
       */

      // Defining Links
      $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

      /* Insert departments in divlist*/
      foreach($this->departments as $key=> $val){
        if(!isset($this->config->departments[trim($key)])){
          $this->config->departments[trim($key)]="";
        }

        $field1 = array("string" => "<img src='images/folder.png' alt='department'>","attach"=>"style='width:16px;text-align:center;'");
        $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val),"attach"=>" style='border:none'");
        $divSel->AddEntry(array($field1,$field2));
      }

      /* Append Blocklits */
      foreach($list as $entry => $value){
        $divSel->AddEntry(array(
              array("string"=>"<input type='checkbox' value='".base64_encode($entry)."' name='ADDLIST_".base64_encode($entry)."'>"),
              array("string"=>$value,"attach"=>"style='border:0px;'")
              ));
      }

      /* Show dialog */
      $smarty->assign("cblocklist", $this->current_blocklist);
      $smarty->assign("departments", $this->config->idepartments);
      $smarty->assign("divSelectPredefined", $divSel->DrawList());
      $display.= $smarty->fetch (get_template_path('lists.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }


    /* Show main page */
    $smarty->assign("languages", get_languages(TRUE));

    $smarty->assign("formats",  $this->fax_formats);
    $smarty->assign("printers", $this->printerList);

    /* Load attributes */
    foreach(array("goFaxIsEnabled", "goFaxDeliveryMode", "facsimileTelephoneNumber",
          "goFaxPrinter", "goFaxLanguage", "goFaxFormat", 
          "facsimileAlternateTelephoneNumber", "mail") as $val){

      $smarty->assign("$val", $this->$val);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $acl => $desc){
      $smarty->assign($acl."ACL",$this->getacl($acl,$edit_mode));
    }

    /* Load checkboxes */
    if ($this->goFaxIsEnabled == "1"){
      $smarty->assign("goFaxIsEnabled", "");
    } else {
      $smarty->assign("goFaxIsEnabled", "checked");
    }
    /* goFaxAccount has "mail" as must! Block if no mailaddress is specified... */
    if ($this->goFaxDeliveryMode & 32) {
      $smarty->assign("faxtomail", "checked");
    } else {
      $smarty->assign("faxtomail", "");
    }
    if ($this->goFaxDeliveryMode & 64) {
      $smarty->assign("faxtoprinter", "checked");
    } else {
      $smarty->assign("faxtoprinter", "");
    }


    $display.= $smarty->fetch (get_template_path('generic.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    plugin::remove_from_parent();

    /* Adapt mail settings if needed */
    if ((isset($this->parent->by_object['mailAccount']->is_account)) && ($this->parent->by_object['mailAccount']->is_account)){
      unset($this->attrs['mail']);
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/fax account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove',array("uid"=> $this->uid));
  }


  /* Check formular input */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* must: facsimileTelephoneNumber */
    if ($this->facsimileTelephoneNumber == ""){
      $message[]= _("The required field 'Fax' is not set.");
    }

    if (!is_phone_nr($this->facsimileTelephoneNumber)){
      $message[]= _("Please enter a valid telephone number in the 'Fax' field.");
    }

    /* IF mail is specified (which is only the case if there's no mail account
       present), check if it's valid..  */
    if (@isset($this->parent->by_object['mailAccount']) &&
        $this->goFaxDeliveryMode & 32){
      if ($this->mail == ""){
        $message[]= _("Mail delivery is checked, but no address has been specified.");
      } elseif (!is_email($this->mail)){
        $message[]= _("The mail address you've entered is invalid.");
      }
    }

    // IE Fix, IE lets you choose disabled option, stupid browser ... 
    if((empty($this->goFaxPrinter))&&($this->goFaxDeliveryMode & 64)){
      $message[]= _("Deliver fax to printer, is only possible if valid printer is given. Please correct your choice.");
    }

    return ($message);
  }

  /* Save data to object */
  function save_object()
  {
    $edit_mode = (!is_object($this->parent) && !isset($_SESSION['edit']));
    if (isset($_POST['faxTab'])){
      plugin::save_object();


      $tmp = 0+$this->goFaxDeliveryMode;

      if($this->acl_is_writeable("faxtomail",$edit_mode)){
        if (isset($_POST["faxtomail"]) && $_POST["faxtomail"] == 1){  
          $tmp |= 32;
        }elseif($tmp & 32){
          $tmp &= (!32);
        }
      }
      if($this->acl_is_writeable("faxtoprinter",$edit_mode)){
        if (isset($_POST["faxtoprinter"]) && $_POST["faxtoprinter"] == 1){  
          $tmp |= 64;
        }elseif($tmp & 64){
          $tmp &= !64;
        }
      }
      $this->goFaxDeliveryMode = $tmp;

      if($this->acl_is_writeable("goFaxIsEnabled",$edit_mode)){
        if (isset($_POST["goFaxIsEnabled"]) && $_POST["goFaxIsEnabled"] == "1"){
          $this->goFaxIsEnabled= "0";
        } else {
          $this->goFaxIsEnabled= "1";
        }
      }

    
      if (isset($_POST['mail']) && $this->acl_is_writeable("faxtomail",$edit_mode)){
        $this->mail= $_POST['mail'];
      }

      /* Check if mail account is active and correct the internal
         reference to represent the current status. */
      if(isset($this->parent)){
        if (isset($this->parent->by_object['mailAccount']->is_account)&&($this->parent->by_object['mailAccount']->is_account)){
          $this->has_mailAccount= TRUE;
        }
      }
    }

  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Save arrays */
    foreach (array("goFaxRBlocklist", "goFaxRBlockgroups", "goFaxSBlocklist",
          "goFaxSBlockgroups", "facsimileAlternateTelephoneNumber") as $val){

      $this->attrs[$val]= $this->$val;
    }

    if(!$this->attrs['goFaxDeliveryMode']){
      $this->attrs['goFaxDeliveryMode'] = 0;
    }
  
    /* Do not save mail address ... it was possibly changed by mail plugin */
    /* Adapt mail settings if needed */
    if ((isset($this->parent->by_object['mailAccount']->is_account)) && ($this->parent->by_object['mailAccount']->is_account)){
      unset($this->attrs['mail']);
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    /* Log last action */
    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/fax account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofidy",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }

  }


  /* Adapt from template, using 'dn' */
  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);

    foreach (array("goFaxRBlocklist", "goFaxRBlockgroups", "goFaxSBlocklist",
          "goFaxSBlockgroups", "facsimileAlternateTelephoneNumber") as $val){

      if (isset($this->attrs[$val])){
        $this->$val= $this->attrs[$val];
      }
    }
  }



  /* Add alternate fax recipient */
  function addAlternate($number)
  {
    $this->facsimileAlternateTelephoneNumber[]= "$number";
    $this->facsimileAlternateTelephoneNumber=
      array_unique ($this->facsimileAlternateTelephoneNumber);

    sort ($this->facsimileAlternateTelephoneNumber);
    reset ($this->facsimileAlternateTelephoneNumber);
  }

  function delAlternate($numbers)
  {
    $this->facsimileAlternateTelephoneNumber= array_remove_entries ($numbers,
        $this->facsimileAlternateTelephoneNumber);
  }

  function getCopyDialog()
  {
    $str    = "";
    $smarty = get_smarty();
    $smarty->assign("facsimileTelephoneNumber", $this->facsimileTelephoneNumber);
    $str['string']   = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));
    $str['status'] ="";
    return($str);
  }

  function SaveCopyDialog()
  {
    if(isset($_POST['facsimileTelephoneNumber'])){
      $this->facsimileTelephoneNumber = $_POST['facsimileTelephoneNumber'];
    }
  }


  /* Return plugin informations for acl handling 
      #FIXME some attributes are still missing in this plugin acls */
  static function plInfo()
  {
    return (array(  
          "plShortName"     => _("Fax"),
          "plDescription"   => _("Fax account settings"),
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 6,             
          "plSection"       => "personal",  
          "plCategory"      => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "goFaxIsEnabled"                    => _("Enable/Disable fax"),
            "goFaxRBlocklist"                   => _("Receive blocklist"),
            "goFaxSBlocklist"                   => _("Send blocklist"),
            "facsimileTelephoneNumber"          => _("Fax number"), // goFaxDeliveryMode
            "facsimileAlternateTelephoneNumber" => _("Alternate fax number"), // goFaxDeliveryMode
            "faxtomail"                         => _("Deliver fax as mail"),
            "faxtoprinter"                      => _("Deliver fax to printer"),
            "goFaxFormat"                       => _("Delivery format"),
            "goFaxLanguage"                     => _("Language"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
