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

  /* Internal variables */
  var $printerList= array();
  var $has_mailAccount= FALSE;
  var $locals_dialog= FALSE;
  var $in_blocklist_dialog= FALSE;
  var $out_blocklist_dialog= FALSE;
  var $current_blocklist= array();

  /* attribute list for save action */
  var $attributes= array("goFaxDeliveryMode", "goFaxIsEnabled",
      "goFaxPrinter", "goFaxDivertNumber", "goFaxLanguage", "goFaxFormat", "mail");
  var $objectclasses= array("goFaxAccount");

  function gofaxAccount ($config, $dn= NULL)
  {
    /* General initialization */
    plugin::plugin ($config, $dn);

    if ($dn != "new"){
      /* Get arrays */
      foreach (array("goFaxRBlocklist", "goFaxRBlockgroups", "goFaxSBlocklist",
            "goFaxSBlockgroups", "facsimileAlternateTelephoneNumber") as $val){

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
      if (isset($this->attrs["facsimileTelephoneNumber"][0])){
        $this->facsimileTelephoneNumber= $this->attrs["facsimileTelephoneNumber"][0];
      }
    }

    /* Load printer list */
    if (isset($this->config->data['SERVERS']['CUPS'])){
      $this->printerList= get_printer_list ($this->config->data['SERVERS']['CUPS']);
      asort ($this->printerList);
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
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no fax extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove fax account"),
            _("This account has fax features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_header(_("Create fax account"),
            _("This account has fax features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Trigger Add local fax alternatives dialog */
    if (isset($_POST['add_local_alternate'])){
      $this->locals_dialog= TRUE;	
      $this->dialog= TRUE;	
    }

    /* Add alternatives from dialog */
    if (isset($_POST['add_locals_finish'])){
      if (isset($_POST['local_list']) &&
          chkacl ($this->acl, "facsimileAlternateTelephoneNumber") == ""){

        foreach ($_POST['local_list'] as $val){
          $this->addAlternate($val);
          $this->is_modified= TRUE;
        }
      }
    }

    /* Add alternatives */
    if (isset($_POST['add_alternate'])){
      if ($_POST['forward_address'] != "" &&
          is_phone_nr($_POST['forward_address']) &&
          chkacl ($this->acl, "facsimileAlternateTelephoneNumber") == ""){

        $this->addAlternate($_POST['forward_address']);
      }
    }

    /* Delete alternate fax number */
    if (isset($_POST['delete_alternate'])){
      if (isset($_POST['alternate_list']) && count($_POST['alternate_list']) &&
          chkacl ($this->acl, "facsimileAlternateTelephoneNumber") == ""){

        $this->delAlternate ($_POST['alternate_list']);
      }
    }


    /* Edit incoming blocklists */
    if (isset($_POST['edit_incoming'])){
      $this->current_blocklist= array_merge($this->goFaxRBlocklist,
          $this->goFaxRBlockgroups);
      sort($this->current_blocklist);
      reset($this->current_blocklist);

      $this->in_blocklist_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Edit outgoing blocklists */
    if (isset($_POST['edit_outgoing'])){
      $this->current_blocklist= array_merge($this->goFaxSBlocklist,
          $this->goFaxSBlockgroups);
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

    /* Add list to blocklist */
    if (isset($_POST['add_blocklist']) && isset($_POST['predefined_list'])){
      $this->current_blocklist= array_merge($this->current_blocklist,
          $_POST['predefined_list']);
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
        $destlist="goFaxRBlocklist";
        $destgroup="goFaxRBlockgroups";
      } else {
        $destlist="goFaxSBlocklist";
        $destgroup="goFaxSBlockgroups";
      }

      /* Transfer values to ourself */
      $this->$destlist= array();
      $this->$destgroup= array();

      foreach ($this->current_blocklist as $val){
        if (is_phone_nr($val)){
          array_push($this->$destlist, $val);
        } else {
          array_push($this->$destgroups, $val);
        }
      }

      $this->is_modified= TRUE;
    }

    /* Set departments */
    if ($this->locals_dialog ||
        $this->in_blocklist_dialog ||
        $this->out_blocklist_dialog){

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
      register_global("faxfilter", $faxfilter);

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
      $acl= array($this->config->current['BASE'] => ":all");
      $res= get_list($acl, "(&(objectClass=goFaxAccount)$filter)", TRUE, $base, array("sn", "givenName", "facsimileTelephoneNumber"), TRUE);

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
      $ldap->search ("(objectClass=goFaxRBlock)");
      while ($attrs= $ldap->fetch()){
        $list[$attrs['cn'][0]]=
          $attrs['description'][0].
          " [".$attrs['cn'][0]."]";
      }

      /* Show dialog */
      $smarty->assign("cblocklist", $this->current_blocklist);
      $smarty->assign("goFaxBlockListACL", chkacl($this->acl, "goFaxBlockList"));
      $smarty->assign("departments", $this->config->idepartments);
      $smarty->assign("list", $list);
      
      if (isset($_POST['department'])){
        $smarty->assign("department", $_POST['department']);
      } else {
        $smarty->assign("department", "");
      }



      $display.= $smarty->fetch (get_template_path('lists.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Manage outgoing blocklists */
    if ($this->out_blocklist_dialog){
      $ldap->search ("(objectClass=goFaxSBlock)");
      while ($attrs= $ldap->fetch()){
        if (isset($attrs['description'][0])){
          $list[$attrs['cn'][0]]= $attrs['cn'][0].
            " [".$attrs['description'][0]."]";
        } else {
          $list[$attrs['cn'][0]]= $attrs['cn'][0];
        }
      }

      /* Show dialog */
      $smarty->assign("cblocklist", $this->current_blocklist);
      $smarty->assign("goFaxBlockListACL", chkacl($this->acl, "goFaxBlockList"));
      $smarty->assign("departments", $this->config->idepartments);
      $smarty->assign("list", $list);
      if (isset($_POST['department'])){
        $smarty->assign("department", $_POST['department']);
      } else {
        $smarty->assign("department", "");
      }
 

      $display.= $smarty->fetch (get_template_path('lists.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }


    /* Show main page */
    $smarty->assign("languages", $this->config->data['MAIN']['LANGUAGES']);
    $smarty->assign("formats", $this->config->data['MAIN']['FAXFORMATS']);
    $smarty->assign("printers", $this->printerList);

    /* Load attributes */
    foreach(array("goFaxIsEnabled", "goFaxDeliveryMode", "facsimileTelephoneNumber",
          "goFaxPrinter", "goFaxLanguage", "goFaxFormat", 
          "facsimileAlternateTelephoneNumber", "mail") as $val){

      $smarty->assign("$val", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }
    $smarty->assign("goFaxRBlockgroupsACL", chkacl($this->acl, "goFaxRBlockgroups"));
    $smarty->assign("goFaxSBlockgroupsACL", chkacl($this->acl, "goFaxSBlockgroups"));

    /* Load checkboxes */
    if ($this->goFaxIsEnabled == "1"){
      $smarty->assign("goFaxIsEnabled", "");
    } else {
      $smarty->assign("goFaxIsEnabled", "checked");
    }
    /* goFaxAccount has "mail" as must! Block if no mailaddress is specified... */
    if ($this->goFaxDeliveryMode & 32) {
      $smarty->assign("fax_to_mail", "checked");
    } else {
      $smarty->assign("fax_to_mail", "");
    }
    if ($this->goFaxDeliveryMode & 64) {
      $smarty->assign("fax_to_printer", "checked");
    } else {
      $smarty->assign("fax_to_printer", "");
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

    /* Zero out arrays */
    foreach (array("goFaxRBlocklist", "goFaxRBlockgroups", "goFaxSBlocklist",
          "goFaxSBlockgroups", "facsimileAlternateTelephoneNumber") as $val){

      $this->attrs[$val]= array();
    }

    /* Adapt mail settings if needed */
    if ($this->parent->by_object['mailAccount']->is_account){
      unset($this->attrs['mail']);
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  /* Check formular input */
  function check()
  {
    /* Reset message array */
    $message= array();

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
        $this->goFaxDeliveryMode && 32){
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
    if (isset($_POST['faxTab'])){
      plugin::save_object();

      /* Adapt combobox values */
      $tmp= 0;
      if (isset($_POST["fax_to_mail"]) && $_POST["fax_to_mail"] == 1){
        $tmp+= 32;
      }
      if (isset($_POST["fax_to_printer"]) && $_POST["fax_to_printer"] == 1){
        $tmp+= 64;
      }
      if (chkacl ($this->acl, "goFaxIsEnabled") == ""){
        if (isset($_POST["goFaxIsEnabled"]) && $_POST["goFaxIsEnabled"] == "1"){
          $this->goFaxIsEnabled= "0";
        } else {
          $this->goFaxIsEnabled= "1";
        }
      }

      if (isset($_POST['facsimileTelephoneNumber'])){
        if ($_POST['facsimileTelephoneNumber'] != $this->facsimileTelephoneNumber){
          $this->is_modified= TRUE;
        }
        $this->facsimileTelephoneNumber= $_POST['facsimileTelephoneNumber'];
      }

      if (isset($_POST['mail'])){
        if ($this->mail != $_POST['mail']){
          $this->is_modified= TRUE;
        }
        $this->mail= $_POST['mail'];
      }

      /* Write to object */
      if (chkacl ($this->acl, "goFaxDeliveryMode") == ""){
        if ($tmp != $this->goFaxDeliveryMode){
            $this->is_modified= TRUE;
        }
        $this->goFaxDeliveryMode= "$tmp";
      }

      /* Check if mail account is active and correct the internal
         reference to represent the current status. */
      if ($this->parent->by_object['mailAccount']->is_account){
        $this->has_mailAccount= TRUE;
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

    /* Adapt mail settings if needed */
    unset($this->attrs['mail']);
    if (!$this->has_mailAccount && $this->goFaxDeliveryMode && 32){
      $this->attrs['mail']= $this->mail;
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
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


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
