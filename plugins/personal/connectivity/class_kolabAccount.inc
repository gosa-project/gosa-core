<?php
class kolabAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Kolab";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users Kolab account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Kolab attributes */
  var $kolabInvitationPolicy= array();
  var $kolabFreeBusyFuture= 60;
  var $unrestrictedMailSize= 0;
  var $calFBURL= "";
  var $kolabDelegate= array();

  /* attribute list for save action */
  var $attributes= array( "kolabFreeBusyFuture", "unrestrictedMailSize", "calFBURL");
  var $objectclasses= array();

  /* Helper */
  var $imapping= array();

  function kolabAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Pull arrays */
    foreach(array("kolabDelegate", "kolabInvitationPolicy") as $attr){
      if (isset($this->attrs["$attr"]["count"])){
        for ($i= 0; $i<$this->attrs["$attr"]["count"]; $i++){
          array_push($this->$attr, $this->attrs["$attr"][$i]);
        }
      }
    }

    /* If this one is empty, preset with ACT_MANUAL for anonymous users */
    if (count ($this->kolabInvitationPolicy) == 0){
       $this->kolabInvitationPolicy= array("ACT_MANUAL");
    }

  }

  function execute()
  {
    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }

    /* Check for invitation action */
    $nr= 0;
    while (isset($_POST["policy$nr"])){
      if (isset($_POST["add$nr"])){
        $this->kolabInvitationPolicy[]= ": ACT_MANUAL";
      }
      if (isset($_POST["remove$nr"])){
        $new= array();
        foreach ($this->kolabInvitationPolicy as $entry){
          if (!preg_match("/^".$this->imapping[$nr].":/", $entry)){
            $new[]= $entry;
          }
        }
        $this->kolabInvitationPolicy= $new;
      }
      $nr++;
    }

    /* Unify addresses */
    $new= array();
    foreach($this->kolabInvitationPolicy as $value){
      if (preg_match('/^:/', $value)){
        continue;
      }
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
        if (!is_email($address)){
          if (!is_email($address, TRUE)){
              print_red (_("You're trying to add an invalid email address to the list of delegations."));
          }
        } else {

          $ldap= $this->config->get_ldap_link();
          $ldap->cd ($this->config->current['BASE']);
          $ldap->search('(mail='.$address.')');
          if ($ldap->count() == 0){
            print_red (_("The mail address you're trying to add is no primary mail address of an existing user."));
          } else {
            $valid= TRUE;
          }
        }

        if ($valid){
          /* Add it */
          if (chkacl ($this->acl, "kolabDelegate") == ""){
            $this->addDelegate ($address);
            $this->is_modified= TRUE;
          }

        }
      }
    }

    /* Delete forward email addresses */
    if (isset($_POST['delete_delegation'])){
      if (count($_POST['delegate_list'])
          && chkacl ($this->acl, "kolabDelegate") == ""){

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

    /* Transfer account states for this union */
    if ($this->parent->by_object['mailAccount']->is_account){
      $smarty->assign('is_account', 'true');
    } else {
      $smarty->assign('is_account', '');
    }

    /* Transfer delegation list */
    $smarty->assign("kolabDelegate", $this->kolabDelegate);
    $smarty->assign("kolabDelegateACL", chkacl($this->acl, $this->kolabDelegate));

    /* Create InvitationPolicy table */
    $invitation= "";
    $this->imapping= array();
    $nr= 0;
    $acl= chkacl($this->acl, "kolabInvitationPolicy");
    foreach ($this->kolabInvitationPolicy as $entry){
      $invitation.= "<tr><td>";

      /* The default entry does not have colons... */
      if (!preg_match('/:/', $entry)){
        $invitation.= _("Anonymous");
        $name= "";
        $mode= $entry;
      } else {
        $name= preg_replace('/:.*$/', '', $entry);
        $mode= preg_replace('/^[^:]*: */', '', $entry);
        $invitation.= "<input name=\"address$nr\" size=16 maxlength=60 $acl value=\"$name\">";
      }
      $invitation.= "</td>";

      /* Add mode switch */
      $invitation.= "<td><select size=\"1\" name=\"policy$nr\" $acl>";
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
        $button= "<input type=submit name=\"add$nr\" value=\""._("Add")."\">";
      }
      if ($nr != 0) {
        $button.= "<input type=submit name=\"remove$nr\" value=\""._("Remove")."\">";
      }
      
      $invitation.= "</select>&nbsp;$button</td></tr>\n";
      $this->imapping[$nr]= $name;
      $nr++;
    }
    $smarty->assign("invitation", $invitation);

    $display.= $smarty->fetch (get_template_path('kolab.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  function check()
  {
    $message= array();

    /* FBFuture is in days... */
    if ($this->kolabFreeBusyFuture != "" && !preg_match('/^[0-9]+$/', $this->kolabFreeBusyFuture)){
      $message[]= _("The value specified as Free Busy future needs to be an integer.");
    }

    /* Check for URL scheme... */
    if ($this->calFBURL != "" && !preg_match('/^[^:/]+://[a-z0-9_/.+~-]+$/i', $this->calFBURL)){
      $message[]= _("The value specified as Free Busy Information URL is invalid.");
    }

    /* Check invitation policy for existing mail addresses */
    foreach($this->kolabInvitationPolicy as $policy){
      
      /* Ignore anonymous string */
      if (!preg_match('/:/', $policy)){
        continue;
      }
      
      $address= preg_replace('/^([^:]+).*$/', '\1', $policy);
      if (!is_email($address)){
        if (!is_email($address, TRUE)){
          $message[]= sprintf(_("The invitation policy entry for address '%s' is not valid."), $address);
        }
      } else {

        $ldap= $this->config->get_ldap_link();
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search('(mail='.$address.')');
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
      if (chkacl('unrestrictedMailSize', $this->acl == "")){
        if (isset($_POST['unrestrictedMailSize']) && $_POST['unrestrictedMailSize'] == 1){
          $this->unrestrictedMailSize= 1;
        } else {
          $this->unrestrictedMailSize= 0;
        }
      }
    }

    plugin::save_object();

    /* Save changes done in invitation policies */
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


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Transfer arrays */
    $this->attrs['kolabDelegate']= $this->kolabDelegate;
    $this->attrs['kolabInvitationPolicy']= $this->kolabInvitationPolicy;

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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
