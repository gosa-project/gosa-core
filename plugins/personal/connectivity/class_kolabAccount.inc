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
  var $kolabInvitationPolicy= "ACT_MANUAL";
  var $kolabFreeBusyFuture= 60;
  var $unrestrictedMailSize= 0;
  var $calFBURL= "";
  var $kolabDelegate= array();

  /* attribute list for save action */
  var $attributes= array( "kolabInvitationPolicy", "kolabFreeBusyFuture",
                          "unrestrictedMailSize", "calFBURL");
  var $objectclasses= array();

  function kolabAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Pull delegation array */
    if (isset($this->attrs["kolabDelegate"]["count"])){
      for ($i= 0; $i<$this->attrs["kolabDelegate"]["count"]; $i++){
        array_push($this->kolabDelegate, $this->attrs["kolabDelegate"][$i]);
      }
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
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Transfer delegation array */
    $this->attrs['kolabDelegate']= $this->kolabDelegate;

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
