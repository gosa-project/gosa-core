<?php

class phoneAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone";
  var $plDescription= "This does something";
  var $has_mailAccount= FALSE;

  /* Attributes */
  var $telephoneNumber        = "";
  var $goFonHardware          = "";
  var $goFonForwarding        = "";
  var $goFonFormat            = "";
  var $goFonPIN               = "";
  var $goFonDeliveryMode      = "";
  var $phoneNumbers           = array();
  var $forwarders             = array();
  var $mail                   = "";
  var $hardware_list          = array();
  var $used_hardware          = array();
  var $goFonMacro             = "";
  var $macro                  = 0;              // Selected Macor
  var $macros                 = array();        // List of macros for smarty select box
  var $macroarray             = array();        // All needed macro informations
  var $macrostillavailable    = false;

  /* CLI vars */
  var $cli_summary            = "Manage users phone account";
  var $cli_description        = "Some longer text\nfor help";
  var $cli_parameters         = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes             = array("goFonDeliveryMode", "goFonForwarding", "goFonFormat",
      "goFonHardware", "goFonPIN", "telephoneNumber", "goFonMacro","macro");
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

    /* Prepare templating */
    $smarty= get_smarty();


    /* Perform search, to get Macro Parameters,Name,Dn,Displayname etc*/
    $ldap->search("(objectClass=goFonMacro)", array("*"));

    /* Add none for no macro*/
    $this->macros['none']=_("no macro");    
    $this->macro ="none";


    /* Fetch all Macros*/
    while ($attrs= $ldap->fetch()){

      /* Only visisble */
      if((isset($attrs['goFonMacroVisible'][0]))&&($attrs['goFonMacroVisible'][0] ==1)){

        /* unset Count, we don't need that here */
        unset($attrs['displayName']['count']);

        /* fill Selectfield variable with Macros */
        if(isset($attrs['displayName'][0])){
          $this->macros[$attrs['dn']] = $attrs['displayName'][0]." (".$attrs['cn'][0].")";
        }else{
          $this->macros[$attrs['dn']] = _("undefined");
        }

        /* Parse macro data, unset count for parameterarrays  */
        unset($attrs['goFonMacroParameter']['count']);

        /* Go through available parameters and parse all attributes, like parametername, type, default ...*/
        if((isset($attrs['goFonMacroParameter']))&&(is_array($attrs['goFonMacroParameter']))){

          foreach($attrs['goFonMacroParameter'] as $pkey=>$pval){

            /* Split Data in readable values, by delimiter !  */
            $data = split("!",$attrs['goFonMacroParameter'][$pkey]);

            /* Set all attrs */
            $id = $data[0];
            $this->macroarray[$attrs['dn']][$id]['var']    ="var".$id;
            $this->macroarray[$attrs['dn']][$id]['choosen']=$data[3];
            $this->macroarray[$attrs['dn']][$id]['id']     = $id;
            $this->macroarray[$attrs['dn']][$id]['name']   =$data[1];
            $this->macroarray[$attrs['dn']][$id]['type']   =$data[2];
            $this->macroarray[$attrs['dn']][$id]['default']=$data[3];
          }//foreach
        }//is_array
      }//visible = 1
    }//while

    /* Go through already saved values, for a parameter */
    $tmp = split("!",$this->goFonMacro);

    /* it is possible that nothing has been saved yet */
    if(is_array($tmp)){

      /* First value is the macroname */
      $this->macro = $tmp[0];

      /* Macroname saved, delete that index */
      unset($tmp[0]);

      /* Check if makro has been removed */
      if(!isset($this->macroarray[$this->macro])){
        $this->macrostillavailable = false;
      }else{
        $this->macrostillavailable = true;
      }

      /* for each parametervalues ( parameterID#value like 25#twentyfive) */
      foreach($tmp as $var){

        /* Split this, so we have $varar[0] = parameterID $varar[1] = SelectedValue */
        $varar = split("#",$var);

        /* Only insert if the parameter still exists */
        if(isset($this->macroarray[$this->macro][$varar[0]])){

          /* Assign value */
          $this->macroarray[$this->macro][$varar[0]]['choosen']=$varar[1];
        }
      }
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

    /* Select no macro if, state is empty, this is the case, if the selected macro is no longer available */
    if(empty($this->macro)){
      $this->macro ="none";
    }

    /* tell user that the pluging selected is no longer available*/
    if((!$this->macrostillavailable)&&($this->macro!="none")){
      print_red(_("The macro you selected in the past, is no longer available for you, please choose another one."));
    }

    /* Prepare templating */
    $smarty= get_smarty();

    /* Assing macroselectbox values  */
    $smarty->assign("macros",$this->macros);   
    $smarty->assign("macro", $this->macro);   

    /* Create parameter table, skip if no parameters given */
    if(!isset($this->macroarray[$this->macro])){
      $macrotab="";
    }else{

      $macrotab ="<table summary=\""._("Parameter")."\">";
      /* for every single parameter-> display textfile,combo, or true false switch*/
      foreach($this->macroarray[$this->macro] as $paras){

        /* get al vars */
        $var        = $paras['var'];           
        $name       = $paras['name'];           
        $default    = $paras['default'];
        $type       = $paras['type'];
        $choosen    = $paras['choosen'] ; 
        $str        = $default;

        /* in case of a combo box display a combobox with selected attr */
        $macrotab.= "<tr>";
        switch ($type){

          case "combo":
            $str= "<select name='".$var."' ".chkacl($this->acl, "goFonMacro")."  ".chkacl($this->acl, "goFonMacro").">";
            foreach(split(":",$default) as $choice){
              if($choosen==$choice){
                $str.= "\n<option value='".$choice."' selected>".$choice."&nbsp;</option>";
              }else{
                $str.= "\n<option value='".$choice."'>".$choice."&nbsp;</option>";
              }
            }
            $str.="</select>";
            $macrotab.= "<td>$name</td><td>$str";
            break;

          case "bool":
            if(!$choosen){
              $str="\n<input type='checkbox' name='".$var."' value='1' ".chkacl($this->acl, "goFonMacro")." >";
            }else{
              $str="\n<input type='checkbox' name='".$var."' value='1' checked  ".chkacl($this->acl, "goFonMacro").">";
            }
            $macrotab.= "<td colspan='2'>$str&nbsp;$name";
            break;

          case "string":
            $str="<input name='".$var."' value='".$choosen."' ".chkacl($this->acl, "goFonMacro").">";
            $macrotab.= "<td>$name</td><td>$str";
            break;

        }
        $macrotab.= "</td</tr>";

      }
      $macrotab.="</table>";
    }//is_array()

    /* Give smarty the table */
    $smarty->assign("macrotab",$macrotab);

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

    /* Transfer ACL's */
    foreach($this->attributes as $val){
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
      $smarty->assign($val,$this->$val);
    }

    /* Fill arrays */
    $smarty->assign ("goFonHardware", $this->goFonHardware);
    if (!count($this->phoneNumbers)){
      $smarty->assign ("phoneNumbers", array(""));
    } else {
      $smarty->assign ("phoneNumbers", $this->phoneNumbers);
    }
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
      $hl.= "  <option $color label=\"$cn\" value=\"$cn\" $selected>$description&nbsp;</option>\n";
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

      /* Every macro in the select box are available */
      if((isset($_POST['macro']))){
        $this->macrostillavailable=true;
      }

      /* get all Postvars */
      if(isset($this->macroarray[$this->macro])){ 
        foreach($this->macroarray[$this->macro] as $key => $paras){
          if(isset($_POST[$paras['var']])){
            $this->macroarray[$this->macro][$key]['choosen'] = $_POST[$paras['var']]; 
          }
        
          /* Checkboxes are special, they are not Posted if they are not selected, so the won't be changed with the above code 
             We need this code below to read and save checkboxes correct
          */
          if($this->macroarray[$this->macro][$key]['type']=="bool"){
            if(isset($_POST[$this->macroarray[$this->macro][$key]['var']])) {
              $this->macroarray[$this->macro][$key]['choosen']=$_POST[$paras['var']];
            }else{
              $this->macroarray[$this->macro][$key]['choosen']=false;
            }
          }
        }
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

    if(($this->goFonPIN)==""){
      $this->goFonPIN = array();
    }else{
      if(strcmp ((int)($this->goFonPIN),($this->goFonPIN))){
        $message[] = sprintf(_("The given PIN is not valid, only numbers are allowed for this type."));
      }elseif(strlen($this->goFonPIN) < 4){
        $message[] = sprintf(_("The given PIN is too short"));
      }

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
    
    /* check for ! in any parameter setting*/
    if(isset($this->macroarray[$this->macro])){
      foreach($this->macroarray[$this->macro] as $val){
        if((strstr($val['choosen'],"!"))||(strstr($val['choosen'],"#"))){
          $message[] = sprintf(_("The parameter %s contains invalid char. '!,#' is used as delimiter"),$val['name']);
        }
      }
    }
    return ($message);
  }



  function save()
  {
    plugin::save();

    /* Save arrays */
    $this->attrs['telephoneNumber']= array();
    foreach ($this->phoneNumbers as $number){
      $this->attrs['telephoneNumber'][]= $number;
    }
    $this->attrs['goFonForwarding']= array();
    foreach ($this->forwarders as $index => $number){
      $this->attrs['goFonForwarding'][]= "$index;$number";
    }

    /* Save settings, or remove goFonMacro attribute*/
    if($this->macro!="none"){    
      $this->attrs['goFonMacro']=$this->macro;
      if(isset($this->macroarray[$this->macro])){
        foreach($this->macroarray[$this->macro] as $paras)  {
          $this->attrs['goFonMacro'].="!".$paras['id']."#".$paras['choosen'];
        }
      }
    }else{
      $this->attrs['goFonMacro']=array();
    }
    unset($this->attrs['macro'])  ;

    if($this->attrs['goFonMacro']==""){
      $this->attrs['goFonMacro']=array();
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
    /* unset macro attr, it will cause an error */
    $tmp = array_flip($this->attributes);
    unset($tmp['macro']);
    $this->attributes=array_flip($tmp);
  
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
