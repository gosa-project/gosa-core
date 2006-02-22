<?php

class termgroup extends plugin
{
  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $members= array();

  function termgroup ($config, $dn= NULL)
  {
    plugin::plugin($config, $dn);
    $ldap= $config->get_ldap_link();

    
    /* We're only interested in the terminal members here, evaluate
       these... */
    if(isset($this->attrs['member'])){
      for ($i= 0; $i<$this->attrs['member']['count']; $i++){
        $member= $this->attrs['member'][$i];
        $ldap->cat($member);
        if (preg_match("/success/i", $ldap->error)){
          $attrs = $ldap->fetch();
          if (in_array("gotoTerminal", $attrs['objectClass']) ||
              in_array("gotoWorkstation", $attrs['objectClass'])){
            if (isset($attrs['macAddress'])){
              $this->members[$attrs['cn'][0]]= $attrs['macAddress'][0];
            } else {
              $this->members[$attrs['cn'][0]]= "";
            }
          }
        }
      }
    }

    /* Include config object */
    $this->config= $config;
  }

  function check()
  {
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();


    /* Watch for events */
    if (isset($_POST['action'])){
      $macaddresses="";
      $names="";
      foreach ($this->members as $cn => $macAddress){
        $macaddresses.= "$macAddress ";
        $names.= "$cn ";
      }

      switch($_POST['saction']){
        case 'wake':
          $cmd= search_config($this->config->data['TABS'], "terminfo", "WAKECMD");
          if ($cmd == ""){
            print_red(_("No WAKECMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$macaddresses, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;

        case 'reboot':
          $cmd= search_config($this->config->data['TABS'], "terminfo", "REBOOTCMD");
          if ($cmd == ""){
            print_red(_("No REBOOTCMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$names, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;

        case 'halt':
          $cmd= search_config($this->config->data['TABS'], "terminfo", "HALTCMD");
          if ($cmd == ""){
            print_red(_("No HALTCMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$names, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;
      }
    }
 

    
    /* Set government mode */
    $smarty= get_smarty();

    $smarty->assign("actions", array("wake" => _("Wake up"), "halt" => _("Switch off"), "reboot" => _("Reboot")));
    $smarty->assign("actionACL", chkacl($this->acl, 'action'));

    /* Show main page */
    return ($smarty->fetch (get_template_path('termgroup.tpl', TRUE)));
  }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
