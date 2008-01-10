<?php
class kolabAccount extends plugin
{
  /* Definitions */
  var $plHeadline       = "Kolab";
  var $plDescription    = "This does something";

  /* Kolab attributes */
  var $kolabInvitationPolicy  = array();
  var $kolabFreeBusyFuture    = 60;
  var $unrestrictedMailSize   = 0;
  var $calFBURL               = "";
  var $kolabDelegate          = array();

  /* attribute list for save action */
  var $attributes     = array("kolabFreeBusyFuture", "unrestrictedMailSize", "calFBURL","kolabDelegate","kolabInvitationPolicy");
  var $objectclasses  = array("kolabInetOrgPerson");

  /* Helper */
  var $imapping     = array();
  var $mail_Account = false;
  var $ReadOnly     = false;
  var $view_logged  = FALSE;
  var $uid          = "";

  var $multiple_support = TRUE;


  /*! \brief Initialize Plugin 
      @param  $config   GOsa configuration object
      @param  $dn       DN of the currently edited object
      @param  $parent   The parent object, used to fetch Attributes.
   */
  function kolabAccount (&$config, $dn= NULL,$parent = NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Pull arrays */
    foreach(array("kolabDelegate", "kolabInvitationPolicy") as $attr){
      if (isset($this->attrs["$attr"]["count"])){
        $tmp = array();
        for ($i= 0; $i<$this->attrs["$attr"]["count"]; $i++){
          $tmp[]=$this->attrs["$attr"][$i];
        }
        $this->$attr = $tmp;
      }
    }

    /* If this one is empty, preset with ACT_MANUAL for anonymous users */
    if (count ($this->kolabInvitationPolicy) == 0){
       $this->kolabInvitationPolicy= array("ACT_MANUAL");
    }

    /* Check is account state */
    $this->is_account = false;
    if(count($this->kolabDelegate)){
      $this->is_account = true;
    }
    foreach(array("calFBURL","unrestrictedMailSize") as $attr){
      if(!empty($this->$attr)){
        $this->is_account = true;
      }
    } 

    /* Transfer account states for this union */
    if (isset($this->parent->by_object['mailAccount']) && $this->parent->by_object['mailAccount']->is_account){
      $this->mail_Account = true;
     }elseif( isset($this->attrs) && isset($this->attrs['kolabHomeServer'])){
       $this->mail_Account= true;
     }else{
       $this->is_account  = false;
      $this->mail_Account = false;
    }
  }

    
  /*! \brief Create thml output 
   */
  function execute()
  {
	  /* Call parent execute */
  	plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $acl => $description){
      $smarty->assign($acl."ACL",$this->getacl($acl,$this->ReadOnly));
      $smarty->assign($acl."_W", $this->acl_is_writeable($acl,$this->ReadOnly));
    }
    $smarty->assign("is_account" ,  $this->is_account); 

    if($this->ReadOnly){
      $smarty->assign("is_removeable",false); 
      $smarty->assign("is_createable",false); 
    }else{
      $smarty->assign("is_removeable",$this->acl_is_removeable()); 
      $smarty->assign("is_createable",$this->acl_is_createable()); 
    }

    /* Check for invitation action */
    $nr= 0;
  
    foreach($_POST as $name => $value){
      if(preg_match("/add_inv_/",$name)){
        $this->kolabInvitationPolicy[]= ": ACT_MANUAL";
      }
    }
    foreach($_POST as $name => $value){
      if(preg_match("/del_inv_/",$name)){

        $id = preg_replace("/del_inv_/","",$name);
        $id = preg_replace("/_(x|y)$/","",$id);
  
        $new= array();
        foreach ($this->kolabInvitationPolicy as $entry){
          if (!preg_match("/^".$this->imapping[$id].":/", $entry)){
            $new[]= $entry;
          }
        }
        $this->kolabInvitationPolicy= $new;
      }
    }

    /* Unify addresses */
    $new= array();
    foreach($this->kolabInvitationPolicy as $value){
      $address= preg_replace('/^([^:]+:).*$/', '\1', $value);
      $new[$address]= $value;
    }
    $this->kolabInvitationPolicy= array();
    foreach($new as $value){
      $this->kolabInvitationPolicy[]= $value;
    }

    /* Add delegation */
    if (isset($_POST['add_delegation'])){
      if ($_POST['delegate_address'] != ""){

        /* Valid email address specified? */
        $address= $_POST['delegate_address'];
        $valid= FALSE;
        if (!tests::is_email($address)){
          if (!tests::is_email($address, TRUE)){
              msg_dialog::display(_("Error"), _("Cannot add invalid mail address!"), ERROR_DIALOG);
          }
        } else {

          $ldap= $this->config->get_ldap_link();
          $ldap->cd ($this->config->current['BASE']);
          $ldap->search('(mail='.$address.')',array("mail"));
          if ($ldap->count() == 0){
            msg_dialog::display(_("Error"), _("Primary mail address already in use by another user!"), ERROR_DIALOG);
          } else {
            $valid= TRUE;
          }
        }

        if ($valid){
          /* Add it */
          if ($this->acl_is_writeable("kolabDelegate")){
            $this->addDelegate ($address);
            $this->is_modified= TRUE;
          }

        }
      }
    }

    /* Delete forward email addresses */
    if ((isset($_POST['delete_delegation'])) && (isset($_POST['delegate_list']))){
      if (count($_POST['delegate_list']) && $this->acl_is_writeable("kolabDelegate")){
        $this->delDelegate ($_POST['delegate_list']);
      }
    }

    /* Assemble policies */
    $policies= array( 'ACT_ALWAYS_ACCEPT'       => _("Always accept"),
                      'ACT_ALWAYS_REJECT'       => _("Always reject"),
                      'ACT_REJECT_IF_CONFLICTS' => _("Reject if conflicts"),
                      'ACT_MANUAL_IF_CONFLICTS' => _("Manual if conflicts"),
                      'ACT_MANUAL'              => _("Manual"));
    $smarty->assign('policies', $policies);

    /* Adjust checkbox */
    if ($this->unrestrictedMailSize){
      $smarty->assign('unrestrictedMailSizeState', "checked");
    } else {
      $smarty->assign('unrestrictedMailSizeState', "");
    }

    /* Transfer delegation list */
    if (!count($this->kolabDelegate)){
      /* Smarty will produce <option value=""></option> and tidy don't like that, so tell smarty to create no option (array();)*/
      $smarty->assign("kolabDelegate", array());
    } else {
      $smarty->assign("kolabDelegate", $this->kolabDelegate);
    }

    $smarty->assign("mail_account",$this->mail_Account);

    /* Create InvitationPolicy table */
    $invitation= "";
    $this->imapping= array();
    $nr= 0;
    $changeState  = "";
    foreach ($this->kolabInvitationPolicy as $entry){

      if($this->acl_is_writeable("kolabInvitationPolicy")){
        $changeState .= "changeState('address".$nr."'); \n changeState('policy".$nr."'); \n
          changeState('add".$nr."'); \n changeState('remove".$nr."'); \n";
      }

      if(!$this->acl_is_writeable("kolabInvitationPolicy") && !$this->multiple_support_active){
        $dis = " disabled ";
      }else{
        if($this->is_account || $this->multiple_support_active){
          $dis = " ";
        }else{
          $dis = " disabled ";
        }
      }
      $invitation.= "<tr><td>";

      if(!$this->acl_is_readable("kolabInvitationPolicy"))    {
      }

      /* The default entry does not have colons... */
      if (!preg_match('/:/', $entry)){
        $invitation.= _("Anonymous");
        $name= "";
        $mode= $entry;
      } else {
        $name= preg_replace('/:.*$/', '', $entry);
        $mode= preg_replace('/^[^:]*: */', '', $entry);

        if(!$this->acl_is_readable("kolabInvitationPolicy")){
          $name='';
        }
        $invitation.= "<input name=\"address$nr\" size=16 maxlength=60  value=\"$name\" id='address".$nr."' ".$dis.">";
      }
      $invitation.= "</td>";

      /* Add mode switch */
      $invitation.= "<td><select size=\"1\" name=\"policy$nr\" id='policy".$nr."' ".$dis.">";
      foreach($policies as $key => $value){
        if ($key == $mode){
          $invitation.= "<option value=\"$key\" selected>$value</option>";
        } else {
          $invitation.= "<option value=\"$key\">$value</option>";
        }
      }
      
      /* Assign buttons */
      $button= "";
      if ($nr == count($this->kolabInvitationPolicy)-1){
        $button= "<input type='image' name=\"add_inv_".$nr."\" src='images/list_new.png' id='add".$nr."' ".$dis.">";
      }
      if ($nr != 0) {
        $button.= "<input type='image' name=\"del_inv_".$nr."\" src='images/edittrash.png' id='remove".$nr."' ".$dis.">";
      }
      
      $invitation.= "</select>&nbsp;</td>
                    <td>$button</td>
                  </tr>\n";
      $this->imapping[$nr]= $name;
      $nr++;
    }
    $smarty->assign("invitation", $invitation);
    $smarty->assign("changeState", $changeState);
    $smarty->assign("kolabState",$this->is_account);

  
    $smarty->assign("multiple_support",$this->multiple_support_active);
    foreach($this->attributes as $attr){
      $smarty->assign("use_".$attr,in_array($attr,$this->multi_boxes));
    }
    foreach(array("kolabState") as $attr){
      $smarty->assign("use_".$attr,in_array($attr,$this->multi_boxes));
    }

    if($this->multiple_support_active){
    }


    $display.= $smarty->fetch (get_template_path('kolab.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if(!$this->initially_was_account){
      return;
    }
  
    /* Optionally execute a command after we're done */
    plugin::remove_from_parent();
    
    if(!in_array("kolabInetOrgPerson",$this->attrs['objectClass'])){
      $this->attrs['objectClass'][] = "kolabInetOrgPerson";
    }

    $ldap = $this->config->get_ldap_linK(); 
    $ldap->cd($this->config->current['BASE']);
    $ldap->cd ($this->dn);
    $ldap->modify($this->attrs);

    /* Log last action */
    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    $this->handle_post_events('remove',array("uid" => $this->uid));
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/kolab account with dn '%s' failed."),$this->dn));
  }


  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* FBFuture is in days... */
    if ($this->kolabFreeBusyFuture != "" && !preg_match('/^[0-9]+$/', $this->kolabFreeBusyFuture)){
      $message[]= _("The value specified as Free Busy future needs to be an integer.");
    }

    /* Check for URL scheme... */
    if(!empty($this->calFBURL) && !preg_match("/http+(s)*:\/\//",$this->calFBURL)){
      $message[]= _("The value specified as Free Busy Information URL is invalid.");
    }

    /* Check invitation policy for existing mail addresses */
    foreach($this->kolabInvitationPolicy as $policy){
      
      /* Ignore anonymous string */
      if (!preg_match('/:/', $policy)){
        continue;
      }
      
      $address= preg_replace('/^([^:]+).*$/', '\1', $policy);
      if (!tests::is_email($address)){
        if (!tests::is_email($address, TRUE)){
          $message[]= sprintf(_("The invitation policy entry for address '%s' is not valid."), $address);
        }
      } else {

        $ldap= $this->config->get_ldap_link();
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search('(mail='.$address.')',array("mail"));
        if ($ldap->count() == 0){
          $message[]= sprintf(_("There's no mail user with address '%s' for your invitation policy!"), $address);
        } else {
          $valid= TRUE;
        }
      }
    }

    return ($message);
  }

  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){

      if(isset($_POST["kolabState"])){
        if($this->acl_is_createable()){
          $this->is_account = true;
        }
      }else{
        if($this->acl_is_removeable()){
          $this->is_account = false;
        }
      }
      if ($this->acl_is_writeable("unrestrictedMailSize")){
        if (isset($_POST['unrestrictedMailSize']) && $_POST['unrestrictedMailSize'] == 1){
          $this->unrestrictedMailSize= 1;
        } else {
          $this->unrestrictedMailSize= 0;
        }
      }
    }

    plugin::save_object();

    /* Save changes done in invitation policies */
    if($this->acl_is_writeable("kolabInvitationPolicy")){

      $nr= 0;
      $this->kolabInvitationPolicy= array();
      while (isset($_POST["policy$nr"])){

        /* Anonymous? */
        if (!isset($_POST["address$nr"])){
          $this->kolabInvitationPolicy[]= $_POST["policy$nr"];
        } else {
          $this->kolabInvitationPolicy[]= $_POST["address$nr"].": ".$_POST["policy$nr"];
        }

        $nr++;
      }

      /* If this one is empty, preset with ACT_MANUAL for anonymous users */
      if (count ($this->kolabInvitationPolicy) == 0){
        $this->kolabInvitationPolicy= array("ACT_MANUAL");
      }
    }
  }


  /* Save to LDAP */
  function save()
  {
    /* Check mailmethod before doing something useful */
    plugin::save();

    /* Transfer arrays */
    $this->attrs['kolabDelegate']= $this->kolabDelegate;
    $this->attrs['kolabInvitationPolicy']= $this->kolabInvitationPolicy;

    /* unrestrictedMailSize is boolean */
    if($this->attrs['unrestrictedMailSize']){
      $this->attrs['unrestrictedMailSize'] = "TRUE";
    }else{
      $this->attrs['unrestrictedMailSize'] = "FALSE";
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

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/kolab account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }
  }


  /* Add entry to delegation list */
  function addDelegate($address)
  {
    $this->kolabDelegate[]= $address;
    $this->kolabDelegate= array_unique ($this->kolabDelegate);

    sort ($this->kolabDelegate);
    reset ($this->kolabDelegate);
    $this->is_modified= TRUE;
  }

  function delDelegate($addresses)
  {
    $this->kolabDelegate= array_remove_entries ($addresses, $this->kolabDelegate);
    $this->is_modified= TRUE;
  }


  /* Return plugin informations for acl handling  */
  static function plInfo()
  {
    return (array(	
          "plShortName"     => _("Kolab"),
          "plDescription"   => _("Kolab account settings")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 20,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "kolabFreeBusyFuture"   => _("Free busy future"),
            "unrestrictedMailSize"  => _("Mail size restriction"),
            "calFBURL"              => _("Free busy information"),
            "kolabDelegate"         => _("Delegations"),
            "kolabInvitationPolicy" => _("Invitation policy"))
          ));
  }


  /*! \brief  Enable multiple edit support. \
              Also sets $this->mail_account to TRUE, which simulates \
              a valid mail extension.
   */
  public function enable_multiple_support()
  {
    plugin::enable_multiple_support();
    $this->mail_Account = TRUE;
  }


  /*! \brief  Save html POSTs in multiple edit.
   */
  public function multiple_save_object()
  {
    if (isset($_POST['connectivityTab'])){
      plugin::multiple_save_object();
      if(isset($_POST['use_kolabState'])){
        $this->multi_boxes[] = "kolabState";
      }
      $this->save_object();
    }
  }


  /*! \brief  Returns all modified values. \
              All selected an modified values will be returned \
              in an array.
      @return array   Returns an array containing all attribute modifications  
   */
  public function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    if(in_array("kolabState",$this->multi_boxes)){
      $ret['is_account'] = $this->is_account;
    }

    print_a($ret);
  
    return($ret);
  }

  /*! \brief  Sets modified attributes in mutliple edit. \
              All collected values from "get_multi_edit_values()" \
              will be applied to this plugin.
      @param  array   An array containing modified attributes returned by get_multi_edit_values();
   */
  public function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);
    if(isset($values['is_account'])){
      $this->is_account = $values['is_account'];
    }
  }


  /*! \brief  Initialize multiple edit ui for this plugin. \
              This function sets plugin defaults in multiple edit.
      @param  array   Attributes used in all object 
      @param  array   All used attributes.
   */
  public function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);
    if(isset($attrs['objectClass']) && in_array("kolabInetOrgPerson",$attrs['objectClass'])){
      $this->is_account = TRUE;
    }

    /* Pull arrays */
    foreach(array("kolabDelegate", "kolabInvitationPolicy") as $attr){
      if (isset($this->multi_attrs["$attr"]["count"])){
        $tmp = array();
        for ($i= 0; $i<$this->multi_attrs["$attr"]["count"]; $i++){
          $tmp[]=$this->multi_attrs["$attr"][$i];
        }
        $this->$attr = $tmp;
      }
    }

    /* If this one is empty, preset with ACT_MANUAL for anonymous users */
    if (count ($this->kolabInvitationPolicy) == 0){
       $this->kolabInvitationPolicy= array("ACT_MANUAL");
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
