<?php

class opengwAccount extends plugin
{
  /* Definitions */
  var $plHeadline       = "Opengroupware account";
  var $plDescription    = "This does something";

  /* CLI vars */
  var $cli_summary      = "Manage users webdav account";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("gosaWebdavAccount");

  var $handle   = NULL;

  function opengwAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->handle = new ogw("ogo","","vserver-18","ogo");//username,$password,$host,$db
    $info['login'] = $this->uid;
    if($this->handle->CheckExistence()){
      $this->info = $this->handle->GetInfo();
    }else{
      /* Generic attributes */
      $info['salutation']         = $this->attrs['vocation'];
      $info['name']               = $this->attrs['sn'];
      $info['firstname']          = $this->attrs['givenName'];
      $info['login']              = $this->attrs['uid'];
      $info['description']        = $info['firstname']." ".$info['name'];
      $info['degree']             = $this->attrs['academicTitle'];
      $info['birthday']           = $this->attrs['dateOfBirth'];
      
      if($this->attrs['gender'] == "F"){
        $info['sex']                = "female";
      }else{
        $info['sex']                = "male";
      }

      /* address settings */
      $info['street']             = $this->attrs['street'];//"iweg 2";
      $info['zip']                = $this->attrs['postalCode'];//"59821";
//      $info['country']            = $this->attrs[''];//"Arnbserg";
      $info['zipcity']            = $info['zip']." ".$info['country'];
//      $info['state']              = $this->attrs[''];//"Germany";

      /* the email address*/
      $info['value_string']       = $this->attrs['mail'];

      /* telephone number */
      $info['number']             = $this->attrs['telephoneNumber'];

      /* Selectable in GOsa */
      $info['template_user_id']   = 10130;
      $info['is_locked']          = 0;
      $info['LocationTeamID']     = "";
      $info['TeamIDis']           = array();
    }
    $this->info = $info;
    $this->handle->SetInfos($info);
  }

  function execute()
  {
    /* Show tab dialog headers */
    $display= "";

    print_a($this->info);

    /* Show main page */
    $smarty= get_smarty();

    if ($this->is_account){
      $smarty->assign("is_account", "true");
    } else {
      $smarty->assign("is_account", "false");
    }
    $display.= $smarty->fetch (get_template_path('opengw.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if(chkacl($this->acl,"webdavAccount")==""){
      /* Cancel if there's nothing to do here */
      if (!$this->initially_was_account){
        return;
      }

      plugin::remove_from_parent();
      $ldap= $this->config->get_ldap_link();

      $ldap->cd($this->dn);
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $ldap->modify($this->attrs);
      show_ldap_error($ldap->get_error());

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove');
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['webdav'])){
        if (!$this->is_account && $_POST['webdav'] == "B"){
          $this->is_account= TRUE;
        }
      } else {
        $this->is_account= FALSE;
      }
    }

    plugin::save_object();
    if (isset($_POST["WEBDAVStatus"])){
      $this->WEBDAVStatus = "disabled";
    } else {
      $this->WEBDAVStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
    if(chkacl($this->acl,"webdavAccount")==""){
      plugin::save();

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
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
