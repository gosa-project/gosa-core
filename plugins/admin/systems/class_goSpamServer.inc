<?php

class gospamserver extends plugin{

  /* CLI vars */
  var $cli_summary= "Manage server base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goSpamServer");
  var $attributes       = array("saRewriteHeader","saTrustedNetworks","saRequiredScore","saFlags","saRule");
  var $StatusFlag       = "saStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goSpamServer");
  var $Flags            = array("B","b","C","R","D","P");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn               = "";
  var $saStatus         = "";

  var $saRewriteHeader  = "";
  var $saTrustedNetworks= array();
  var $TrustedNetworks  = array();
  var $saRequiredScore  = 0;
  var $saFlags          = "";
  var $Rules            = array();
  var $saRule           = array();

  var $saFlagsB         = false;
  var $saFlagsb         = false;
  var $saFlagsC         = false;
  var $saFlagsR         = false;
  var $saFlagsD         = false;
  var $saFlagsP         = false;
 
  var $dialog           = NULL;
  var $ui               = NULL;
  var $acl              = NULL;

  function gospamserver($config,$dn, $parent= NULL)
  {
    /* Init class */
    plugin::plugin($config,$dn, $parent);
    $this->DisplayName = _("Spamassassin");

    /* Get userinfo & acls */
    $this->ui = get_userinfo();

    /* Set up the users ACL's for this 'dn' */
    $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goSpamServer", $this->ui->dn);

    /* Get Flags */
    foreach($this->Flags as $flag){
      $var = "saFlags".$flag;
      if(preg_match("/".$flag."/",$this->saFlags)){
        $this->$var = TRUE;
      }
    }
    
    /* Get trusted networks */
    $this->TrustedNetworks = array();
    if(isset($this->attrs['saTrustedNetworks']) && is_array($this->attrs['saTrustedNetworks'])){
      $var = $this->attrs['saTrustedNetworks'];
      for($i = 0 ; $i < $var['count'] ; $i ++ ){
        $var2 = $this->attrs['saTrustedNetworks'][$i];
        $this->TrustedNetworks[ $var2 ] = $var2; 
      }
    }

    /* Get rules */
    $this->Rules = array();
    if(isset($this->attrs['saRule']) && is_array($this->attrs['saRule'])){  
      $var = $this->attrs['saRule'];
      for($i = 0 ; $i < $var['count'] ; $i ++ ){
        $var2 = $this->attrs['saRule'][$i];
        $name = preg_replace("/:.*$/","",$var2);
        $value= base64_decode(preg_replace("/^.*:/","",$var2));
        $this->Rules[ $name ] = $value;
      }
    }
  }


  function execute()
  {
    $smarty = get_smarty(); 
    if(get_class($this->parent) == "servtabs"){

      $smarty->assign("servtabs",true);
      /* Do we need to flip is_account state? */
      if (isset($_POST['modify_state'])) {
        $this->is_account = !$this->is_account;
      }

      /* Show tab dialog headers */
      if ($this->is_account) {
        /* call Add Acoount to add account */
        $display = $this->show_header(_("Remove spamassassin extension"), 
            _("This server has spamassassin features enabled. You can disable them by clicking below."));
      } else {
        /* call remove Account */
        $display = $this->show_header(_("Add spamassassin service"), 
            _("This server has spamassassin features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }else{
      $this->is_account =true;
      $display ="";
      $smarty->assign("servtabs",false);
    }
  
    /* Add new trusted network */
    if(isset($_POST['AddNewTrust'])){
      $this->AddTrust($_POST['NewTrustName']);
    }
  
    /* Delete selected trusted network */
    if(isset($_POST['DelTrust'])){
      $this->DelTrust($_POST['TrustedNetworks']);
    }

    /* Add a new rule */
    if(isset($_POST['AddRule'])){
      $this->dialog = new goSpamServerRule($this->config,$this->dn);
    }
  
    /* Cancel adding/editing specified rule */
    if(isset($_POST['CancelRule'])){
      $this->dialog = NULL;
    }

    /* Handle post to delete rules */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^editRule_/",$name) && $once ){
        $once = false;
        $entry = preg_replace("/^editRule_/","",$name);
        $entry = preg_replace("/_(x|y)$/","",$entry);
        $rule = $this->Rules[$entry];
        $name = $entry;
        $this->dialog = new goSpamServerRule($this->config,$this->dn,$name,$rule);
      }
      if(preg_match("/^delRule_/",$name) && $once ){
        $once = false;
        $entry = preg_replace("/^delRule_/","",$name);
        $entry = preg_replace("/_(x|y)$/","",$entry);
        unset($this->Rules[$entry]);
      }
    }

    /* Save rules */
    if(isset($_POST['SaveRule'])){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $ret = $this->dialog->save();
        if((!empty($ret['orig_name'])) && isset($this->Rules[$ret['orig_name']])){
          unset($this->Rules[$ret['orig_name']]);
        }
        $this->Rules[$ret['name']] = $ret['rule'];
        $this->dialog = NULL;
      }
    }
   
    /* Display dialog if available */ 
    if($this->dialog && $this->dialog->config){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    /* Assign smarty vars */
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Assign checkbox states */
    foreach($this->Flags as $Flag){
      $var = "saFlags".$Flag;
      $smarty->assign("saFlags".$Flag."ACL",chkacl($this->acl,$Flag));
      if($this->$var){
        $smarty->assign("saFlags".$Flag."CHK"," checked " );
      }else{
        $smarty->assign("saFlags".$Flag."CHK","");
      }
    }

    /* Create divlist */
    $DivRules = new divSelectBox("SpamRules");
    $DivRules->SetHeight(130);

    if(preg_match("/disabled/",chkacl($this->acl,"saTrustedNetworks"))){
      $actions = "";
    }else{
      $actions = "<input type='image' src='images/edit.png'      name='editRule_%s'>";
      $actions.= "<input type='image' src='images/edittrash.png' name='delRule_%s'>";
    }

    foreach($this->Rules as $key => $net){
      $field1 = array("string" => $key );
      $field2 = array("string" => sprintf($actions,$key,$key) , "attach" => "style='border-right:0px;width:36px;'");
      $DivRules->AddEntry(array($field1,$field2));
    }
    $smarty->assign("divRules",$DivRules->DrawList()); 
    $smarty->assign("TrustedNetworks",$this->TrustedNetworks); 

    /* Create Spam score select box entries */
    $tmp = array();
    for($i = 0 ; $i <= 20 ; $i ++ ){
      $tmp[$i] = $i;
    }
    $smarty->assign("SpamScore",$tmp);

    return($display.$smarty->fetch(get_template_path("goSpamServer.tpl",TRUE,dirname(__FILE__))));
  }


  /* Add $post to list of configured trusted */
  function AddTrust($post)
  {
    if(!empty($post)){
      if(is_ip($post) || is_domain($post) || (is_ip_with_subnetmask($post))){
        $this->TrustedNetworks[$post] = $post;
      }else{
        print_red(_("Specified value is not a valid 'trusted network' value."));
      }
    }
  }


  /* Delete trusted network */
  function DelTrust($posts)
  {
    foreach($posts as $post){
      if(isset($this->TrustedNetworks[$post])){
        unset($this->TrustedNetworks[$post]);     
      }
    }
  }


  /* remove this extension */
  function remove_from_parent()
  {

    if(!$this->is_account && $this->initially_was_account){

      plugin::remove_from_parent();

      /* Remove status flag, it is not a memeber of
         this->attributes, so ensure that it is deleted too */
      if(!empty($this->StatusFlag)){
        $this->attrs[$this->StatusFlag] = array();
      }

      /* Check if this is a new entry ... add/modify */
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($this->dn,array("objectClass"));
      if($ldap->count()){
        $ldap->cd($this->dn);
        $ldap->modify($this->attrs);
      }else{
        $ldap->cd($this->dn);
        $ldap->add($this->attrs);
      }
      show_ldap_error($ldap->get_error(), sprintf(_("Removing of server services/spamassassin with dn '%s' failed."),$this->dn));
      $this->handle_post_events("remove");
    }
  }


  function save()
  {
    if(!$this->is_account) return;
    plugin::save();

    /* Create Flags */     
    $this->attrs['saFlags'] = array();
    foreach($this->Flags as $flag){
      $var = "saFlags".$flag;
      if($this->$var){
        $this->attrs['saFlags'].=$flag;
      }
    }

    /* Create trusted network entries */
    $this->attrs['saTrustedNetworks'] = array();
    foreach($this->TrustedNetworks as $net){
      $this->attrs['saTrustedNetworks'][] = $net; 
    }    

    /* Rules */
    $this->attrs['saRule'] = array();
    foreach($this->Rules as $name => $rule){
      $this->attrs['saRule'][] = $name.":".base64_encode($rule);
    }

    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    if($this->initially_was_account){
      $this->handle_post_events("modify");
    }else{
      $this->handle_post_events("add");
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of server services/spamassassin with dn '%s' failed."),$this->dn));
  }

  function check()
  { 
    $message = plugin::check();

    /* Check if required score is numeric */
    if(!is_numeric($this->saRequiredScore)){
      $message[] = _("Required score must be a numeric value.");
    }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goSpamServer'])){

      plugin::save_object();

      /* Check flags */
      foreach($this->Flags as $flag){
        $var = "saFlags".$flag;
        if(isset($_POST[$var])){
          $this->$var = TRUE;
        }else{
          $this->$var = FALSE;
        }
      }
    }    
  }  

  
  /* Return plugin informations for acl handling 
  function plInfo()
  {
    return (array(
          "plShortName"   => _("Spamassassin"),
          "plDescription" => _("Spamassassin service"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),
          "plProvidedAcls"=> array(

            "saRewriteHeader"   => _("Rewrite header"),
            "saTrustedNetworks" => _("Trusted networks"),
            "saRequiredScore"   => _("Required score"),
            "saRule"            => _("Rules"),

            "saFlagB"           => _("Enable use of bayes filtering"),
            "saFlagb"           => _("Enabled bayes auto learning"),
            "saFlagC"           => _("Enable RBL checks"),
            "saFlagR"           => _("Enable use of Razor"),
            "saFlagD"           => _("Enable use of DDC"),
            "saFlagP"           => _("Enable use of Pyzor"))
          ));
  }
  */

  /* For newer service management dialogs */
  function getListEntry()
  {
    $this->updateStatusState();
    $flag                   = $this->StatusFlag;
    $fields['Status']       = $this->$flag;
    $fields['Message']      = _("Spamassassin");
    $fields['AllowStart']   = true;
    $fields['AllowStop']    = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']  = true;
    $fields['AllowEdit']    = true;
    return($fields);
  }

  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }
  function action_hook($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "ACTION_HOOK");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "ACTION_HOOK");
    }
    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);
      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      /* If there are still some %.. in our command, try to fill these with some other class vars */
      if(preg_match("/%/",$command)){
        $attrs = get_object_vars($this);
        foreach($attrs as $name => $value){
          if(!is_string($value)) continue;
          $command= preg_replace("/%$name/", $value, $command);
        }
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as ACTION_HOOK for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;
    if(!$this->initially_was_account) return;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for server services/spamassassin with dn '%s' failed."),$this->dn));
      $this->action_hook();
    }
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
