<?php
class appgroup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage application groups";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Appgroup attributes */
  var $gosaMemberApplication= array();

  /* Helpers */
  var $department= "";
  var $apps= array();
  var $used_apps= array();
  var $opt_edit= FALSE;
  var $option_name= array();
  var $option_value= array();
  var $appoption= array();
  var $table= "";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("gosaApplicationGroup");

  function appgroup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Load member applications */
    if (isset ($this->attrs["gosaMemberApplication"][0])){
      for ($i= 0; $i<$this->attrs["gosaMemberApplication"]["count"]; $i++){
        $this->gosaMemberApplication[]=
          $this->attrs["gosaMemberApplication"][$i];
      }
    }

    /* Load application options */
    if (isset($this->attrs['gosaApplicationParameter'])){
      for ($i= 0; $i<$this->attrs['gosaApplicationParameter']['count']; $i++){
        $option= preg_replace('/^[^:]+:/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $name= preg_replace('/:.*$/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $this->appoption[$name]= $option;
      }
    }

  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no appgroup.")."</b>";
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove applications"),
            _("This group has application features enabled. You can disable them by clicking below."));
      } else {
        $display.= $this->show_header(_("Create applications"),
            _("This group has application features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Check sorting variable */
    $this->reload();

    /* Delete app from group */
    if (isset($_POST['del_app']) && isset ($_POST['used_apps'])){
      foreach ($_POST['used_apps'] as $value){
        unset ($this->used_apps["$value"]);
        $this->removeApp($value);
      }
    }

    /* Add app to group */
    if (isset($_POST['add_app']) && isset($_POST['apps'])){
      foreach ($_POST['apps'] as $value){
        $this->used_apps["$value"]= $this->apps[$value];
        asort($this->used_apps);
        $this->addApp($value);
      }
    }

    /* Edit application options? */
    if (isset($_POST['edit_options']) && isset($_POST['used_apps'])){
      $appname= $_POST['used_apps'][0];

      /* We've got the appname, get parameters from ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gosaApplication)(cn=$appname))");
      if ($ldap->count() != 1){
        print_red (_("The selected application name is not uniq. Please check your LDAP."));
      } else {
        $attrs= $ldap->fetch();
        if(isset($attrs['gosaApplicationParameter'])){
          $this->dialog= TRUE;

          /* Fill name and value arrays */
          for ($i= 0; $i<$attrs['gosaApplicationParameter']['count']; $i++){
            $option= preg_replace('/^[^:]+:/', '',
                $attrs['gosaApplicationParameter'][$i]);
            $name= preg_replace('/:.*$/', '', 
                $attrs['gosaApplicationParameter'][$i]);
            $this->option_name[$i]= $name;

            /* Fill with values from application, default should be
               loaded by the external scripts */
            if (isset($this->appoption[$name])){
              $this->option_value[$i]= $this->appoption[$name];
            }
          }

          /* Create edit field */
          $table= "<table summary=\"\">";
          for ($i= 0; $i < count($this->option_name); $i++){
            if (isset($this->option_value[$i])){
              $value= $this->option_value[$i];
            } else {
              $value= "";
            }
            $table.="<tr><td>".$this->option_name[$i]."</td><td>".
              "<input name=\"value$i\" size=60 maxlength=250 ".
              "value=\"".$value."\"><br></td></tr>";
          }
          $table.= "</table>";
          $this->table= $table;
        } else {
          print_red (_("The selected application has no options."));
        }
      }
    }

    /* Cancel edit options? */
    if (isset($_POST['edit_options_cancel'])){
      $this->dialog= FALSE;
    }

    /* Finish edit options? */
    if (isset($_POST['edit_options_finish'])){
      $this->dialog= FALSE;

      /* Save informations passed by the user */
      $this->option_value= array();
      for ($i= 0; $i<count($this->option_name); $i++){
        $this->appoption[$this->option_name[$i]]= $_POST["value$i"];
        $this->is_modified= TRUE;
      }
    }

    /* Prepare templating stuff */
    $smarty= get_smarty();
    $smarty->assign("used_apps", $this->used_apps);
    $apps= array();
    foreach ($this->apps as $key => $value){
      if (!array_key_exists($key, $this->used_apps)){
        $apps["$key"]= "$value";
      }
    }
    $smarty->assign("apps", $apps);

    /* Show main page */
    if ($this->dialog){
      $smarty->assign("table", $this->table);
      $display.= $smarty->fetch (get_template_path('application_options.tpl', TRUE));
    } else {
      $display.= $smarty->fetch (get_template_path('application.tpl', TRUE));
    }

    return ($display);
  }


  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $this->attrs["gosaMemberApplication"]= array();

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
#  function save_object()
#  {
#    plugin::save_object();
#  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Copy members */
    $this->attrs["gosaMemberApplication"]= array();
    foreach ($this->gosaMemberApplication as $val){
      $this->attrs["gosaMemberApplication"][]= stripslashes($val);
    }

    /* Are there application parameters to be saved */
    $this->attrs['gosaApplicationParameter']= array();
    foreach($this->appoption as $name => $value){
      if ($value != ""){
        $this->attrs['gosaApplicationParameter'][]= "$name:$value";
      }
    }

    /* Write back to LDAP */
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

  function check()
  {
    $message= array();
    return ($message);
  }


  function reload()
  {
    /* Generate applist */
    $this->apps= array();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(objectClass=gosaApplication)");
    while ($attrs= $ldap->fetch()){
      if (isset($attrs["description"][0])){
        $this->apps[$attrs["cn"][0]]=
          $attrs["cn"][0]." (".
          $attrs["description"][0].")";
      } else {
        $this->apps[$attrs["cn"][0]]=
          $attrs["cn"][0];
      }
    }
    natcasesort ($this->apps);
    reset ($this->apps);

    $this->used_apps= array();
  
    foreach ($this->gosaMemberApplication as $value){
      if(!isset($this->apps[$value])){
        print_red(sprintf(_("The application named %s is no longer available and has been removed."),$value));
        $test = $this->gosaMemberApplication;
        $test = array_flip($test);
        unset($test[$value]);
        $this->gosaMemberApplication = array_flip($test);
      }else{
        $this->used_apps[$value]= $this->apps[$value];
      }
    }
  }

  function addApp($cn)
  {
    $this->gosaMemberApplication[]= $cn;
    $this->gosaMemberApplication= array_unique($this->gosaMemberApplication);
    $this->is_modified= TRUE;
  }


  function removeApp($cn)
  {
    $temp= array();
    foreach ($this->gosaMemberApplication as $value){
      if ($value != $cn){
        $temp[]= $value;
      }
    }
    $this->gosaMemberApplication= $temp;
    $this->is_modified= TRUE;
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
