<?php

class phoneAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone";
  var $plDescription= "This does something";
  var $has_mailAccount= FALSE;

  /* Attributes */
  var $telephoneNumber= "";
  var $goFonHardware= "";
  var $goFonForwarding= "";
  var $goFonFormat= "";
  var $goFonPIN= "";
  var $goFonDeliveryMode= "";
  var $phoneNumbers= array();
  var $forwarders= array();
  var $mail= "";
  var $hardware_list= array();
  var $used_hardware= array();

  /* CLI vars */
  var $cli_summary= "Manage users phone account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array("goFonDeliveryMode", "goFonForwarding", "goFonFormat",
      "goFonHardware", "goFonPIN", "telephoneNumber");
  var $objectclasses= array("goFonAccount");

  function phoneAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Set phone hardware */
    if (!isset($this->attrs['goFonHardware'])){
      $this->goFonHardware= "automatic";
    }

    /* Preset voice format */
    if (!isset($this->attrs['goFonFormat'])){
      $this->goFonFormat= "wav";
    }

    /* Assemble phone numbers */
    if (isset($this->attrs['telephoneNumber'])){
      for ($i= 0; $i<$this->attrs['telephoneNumber']['count']; $i++){
        $number= $this->attrs['telephoneNumber'][$i];
        $this->phoneNumbers[$number]= $number;
      }
    }

    /* Assemble forwarders */
    if (isset($this->attrs['goFonForwarding'])){
      for ($i= 0; $i<$this->attrs['goFonForwarding']['count']; $i++){
        list($num, $v1, $v2) =split(';', $this->attrs['goFonForwarding'][$i]);
        $this->forwarders[$num]= "$v1;$v2";
      }
    } else {
      $this->forwarders= array("");
    }

    /* Set up has_mailAccount */
    if (isset($this->attrs['objectClass'])){
      if (in_array("gosaMailAccount", $this->attrs['objectClass'])){
        $this->has_mailAccount= TRUE;
      }
    }

    /* Load hardware list */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goFonHardware)", array('cn', 'description'));
    while ($attrs= $ldap->fetch()){
      $cn= $attrs['cn'][0];
      if (isset($attrs['description'])){
        $description= " - ".$attrs['description'][0];
      } else {
        $description= "";
      }
      $this->hardware_list[$cn]= "$cn$description";
      
    }

    /* Eventually colorize phones */
    $ldap->cd($this->config->current['BASE']);
    foreach ($this->hardware_list as $cn => $desc){
      $ldap->search("(goFonHardware=$cn)", array('cn'));
      if ($ldap->count() > 0){
        $ldap->fetch();
        if ($ldap->getDN() != $this->dn){
          $this->used_hardware[$cn]= $ldap->getDN();
        }
      }
    }
    
    $this->hardware_list["automatic"]= _("automatic");
    ksort($this->hardware_list);
  }


  function execute()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no phone extensions.")."</b>";
      $display.= back_to_main();
      return($display);
    }

    $display= "";

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove phone account"),
            _("This account has phone features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_header(_("Create phone account"),
            _("This account has phone features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Add phone number */
    if (isset($_POST["add_phonenumber"]) && $_POST['phonenumber']){
      if (is_phone_nr($_POST['phonenumber'])){
        $number= $_POST["phonenumber"];
        $this->phoneNumbers[$number]= $number;
        $this->is_modified= TRUE;
      } else {
        print_red(_("Please enter a valid phone number!"));
      }
    }

    /* Remove phone number */
    if (isset($_POST["delete_phonenumber"]) && isset($_POST["phonenumber_list"])){
      foreach ($_POST['phonenumber_list'] as $number){
        unset($this->phoneNumbers[$number]);
        $this->is_modified= TRUE;
      }
    }

    /* Check for forwarding action */
    foreach ($this->forwarders as $nr => $fw){

      /* Buttons pressed? */
      if (isset($_POST["add_fw$nr"])){
        $this->forwarders= $this->insert_after("", $nr, $this->forwarders);
      }
      if (isset($_POST["remove_fw$nr"])){
        unset($this->forwarders[$nr]);
      }
    }

    /* Prepare templating */
    $smarty= get_smarty();

    /* Transfer ACL's */
    foreach($this->attributes as $val){
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }

    /* Fill arrays */
    $smarty->assign ("goFonHardware", $this->goFonHardware);
    $smarty->assign ("phoneNumbers", $this->phoneNumbers);
    $hl= "<select size=\"1\" name=\"goFonHardware\" title=\"".
         _("Choose your private phone")."\" ".chkacl($this->acl, "goFonHardware").">\n";
    foreach ($this->hardware_list as $cn => $description){
      if ($cn == $this->goFonHardware){
        $selected= "selected";
      } else {
        $selected= "";
      }
      if (isset($this->used_hardware[$cn])){
        $color= "style=\"color:#A0A0A0\"";
      } else {
        $color= "";
      }
      $hl.= "  <option $color label=\"$cn\" value=\"$cn\" $selected>$description</option>\n";
    }
    $hl.= "</select>\n";
    $smarty->assign ("hardware_list", $hl);

    /* Generate forwarder view */
    $forwarder_list="";
    $acl= chkacl($this->acl, "goFonForwaring");
    foreach ($this->forwarders as $nr => $fw){
      if ($fw == ""){
        $number= ""; $timeout= "";
      } else {
        list($number, $timeout)= split(";", $fw);
      }
      $forwarder_list.= "<tr><td>";
      $forwarder_list.= "<input name=\"fwn$nr\" size=25 align=\"middle\" maxlength=60 value=\"$number\" $acl>";
      $forwarder_list.= "</td><td>";
      $forwarder_list.= "<input name=\"fwt$nr\" size=5 align=\"middle\" maxlength=5 value=\"$timeout\" $acl>";
      $forwarder_list.= "</td><td>";
      $forwarder_list.= "<input type=\"submit\" value=\""._("Add")."\" name=\"add_fw$nr\" $acl>";
      if (count($this->forwarders) > 1){
        $forwarder_list.= "<input type=\"submit\" value=\""._("Remove")."\" name=\"remove_fw$nr\" $acl>";
      }
      $forwarder_list.= "</td></tr>";
    }
    $smarty->assign("forwarder_list", $forwarder_list);



    /* Check box */
    if ($this->parent->by_object['mailAccount'] &&
        $this->parent->by_object['mailAccount']->is_account &&
        preg_match("/M/i", $this->goFonDeliveryMode)){
      $smarty->assign("fon_to_mail", "checked");
    } else {
      $smarty->assign("fon_to_mail", "");
    }

    if (!isset($this->parent->by_object['mailAccount'])) {
      $smarty->assign("has_mailaccount", "false");
      $this->has_mailAccount= false;
    } elseif ( !$this->parent->by_object['mailAccount']->is_account){
      $smarty->assign("has_mailaccount", "false");
      $this->has_mailAccount= false;
    } else {
      $smarty->assign("has_mailaccount", "true");
    }

    /* Show main page */
    $display.= $smarty->fetch(get_template_path('generic.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }


  function save_object()
  {
    if (isset($_POST["phoneTab"])){
      plugin::save_object();

      /* Save checkbox */
      if (isset($_POST['fon_to_mail'])){
        $tmp= "[M]";
      } else {
        $tmp= "[]";
      }
      if (chkacl ($this->acl, "goFonDeliveryMode") == ""){
        if ($this->goFonDeliveryMode != $tmp){
          $this->is_modified= TRUE;
        }
        $this->goFonDeliveryMode= $tmp;
      }

      /* Save forwarding numbers and timeouts */
      if (chkacl ($this->acl, "goFonForwarder") == ""){
        foreach ($this->forwarders as $nr => $fw){
          $tmp= $_POST["fwn$nr"].";".$_POST["fwt$nr"];
          if ($this->forwarders[$nr] != $tmp){
            $this->is_modified= TRUE;
          }
          $this->forwarders[$nr]= $tmp;
        }
      }

      /* Check if mail account is active and correct the internal
         reference to represent the current status. */
      if ($this->parent->by_object['mailAccount']->is_account){
        $this->has_mailAccount= TRUE;
      }
    }

  }

  function check()
  {
    /* Reset message array */
    $message= array();

    /* We need at least one phone number */
    if (count($this->phoneNumbers) == 0){
      $message[]= sprintf(_("You need to specify at least one phone number!"));
    }

    /* Check timestamps and phonenumbers */
    foreach ($this->forwarders as $fw){

      /* Skip empty values */
      if ($fw == ";"){
        continue;
      }		

      /* Check */
      list($number, $timeout)= split(";", $fw);
      if (!is_phone_nr($number)){
        $message[]= sprintf(_("The number '%s' is no valid phone number!"), $number);
      }
      if (!is_id($timeout)){
        $message[]= sprintf(_("The timeout '%s' contains invalid characters!"), $timeout);
      }
    }

    return ($message);
  }



  function save()
  {
    plugin::save();

    /* goFonAccount has "mail" as must! Block if no mailaddress is specified... */
    if (isset($this->parent->by_object['mailAccount']) &&
        !$this->parent->by_object['mailAccount']->is_account) {

      $this->goFonDeliveryMode= preg_replace("/M/i", "", $this->goFonDeliveryMode);
    }

    /* Save arrays */
    $this->attrs['telephoneNumber']= array();
    foreach ($this->phoneNumbers as $number){
      $this->attrs['telephoneNumber'][]= $number;
    }
    $this->attrs['goFonForwarding']= array();
    foreach ($this->forwarders as $index => $number){
      $this->attrs['goFonForwarding'][]= "$index;$number";
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }


  function insert_after($entry, $nr, $list)
  {
    /* Is the entry free? No? Make it free... */
    if (isset($list[$nr])) {
      $dest= array();
      $newidx= 0;

      foreach ($list as $idx => $contents){
        $dest[$newidx++]= $contents;
        if ($idx == $nr){
          $dest[$newidx++]= $entry;
        }
      }
    } else {
      $dest= $list;
      $dest[$nr]= $entry;
    }

    return ($dest);
  }


  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);

    /* Assemble phone numbers */
    if (isset($this->attrs['telephoneNumber'])){
      for ($i= 0; $i<$this->attrs['telephoneNumber']['count']; $i++){
        $number= $this->attrs['telephoneNumber'][$i];
        $this->phoneNumbers[$number]= $number;
      }
    }

    /* Assemble forwarders */
    if (isset($this->attrs['goFonForwarding'])){
      for ($i= 0; $i<$this->attrs['goFonForwarding']['count']; $i++){
        list($num, $v1, $v2) =split(';', $this->attrs['goFonForwarding'][$i]);
        $this->forwarders[$num]= "$v1;$v2";
      }
    } else {
      $this->forwarders= array("");
    }
  }


  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }
    
    plugin::remove_from_parent();

    /* Just keep one phone number */
    if (count($this->telephoneNumber) && $this->telephoneNumber != ""){
      $this->attrs['telephoneNumber']= $this->telephoneNumber[0];
    } else {
      $this->attrs['telephoneNumber']= array();
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
