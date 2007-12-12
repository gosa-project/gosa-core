<?php
class applicationParameters extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage application class parameters";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Parameters  */
  var $option_name= array();
  var $option_value= array();

  /* attribute list for save action */
  var $attributes= array("gosaApplicationParameter");
  var $objectclasses= array();

  var $CopyPasteVars = array("option_name","option_value");

  function applicationParameters (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $this->gosaApplicationParameter = array();

    if (isset($this->attrs['gosaApplicationParameter'])){
      $this->is_account= TRUE;
      for ($i= 0; $i<$this->attrs['gosaApplicationParameter']['count']; $i++){
        $option= preg_replace('/^[^:]+:/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $name= preg_replace('/:.*$/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $this->option_name[$i]= $name;
        $this->option_value[$i]= $option;
      }
    } else {
      $this->is_account= FALSE;
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

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display= $this->show_disable_header(_("Remove options"),
            _("This application has options. You can disable them by clicking below."));
      } else {
        $display= $this->show_enable_header(_("Create options"),
            _("This application has options disabled. You can enable them by clicking below."));
        $this->parent->by_object['application']->generateTemplate();
        return ($display);
      }
    }

    /* Add option to list */
    if (isset($_POST['add_option'])){
      $i= count($this->option_name);
      $this->option_name[$i]= "";
      $this->option_value[$i]= "";
    }

    /* Remove value from list */
    for ($i= 0; $i<count($this->option_name); $i++){
      if (isset($_POST["remove$i"])){
        $k= 0;
        $on= array();
        $ov= array();
        for ($j= 0; $j<count($this->option_name); $j++){
          if ($j != $i){
            $on[$k]= $this->option_name[$j];
            $ov[$k]= $this->option_value[$j];
            $k++;
          }
        }
        $this->option_name= $on;
        $this->option_value= $ov;
        break;
      }
    }

    /* Generate list of attributes */
    if (count($this->option_name) == 0){
      $this->option_name[]= "";
      $this->option_value[]= "";
    }


    $acl = $this->getacl("gosaApplicationParameter")	;
    $table= "<table summary=\"\"><tr><td>"._("Variable")."</td><td>"._("Default value")."</td><td></td></tr>";
    if (count ($this->option_name)){

      for ($i= 0; $i < count($this->option_name); $i++){
        $name = $this->option_name[$i];
        $value= $this->option_value[$i];

        $tag = "";
        if(!preg_match("/w/",$acl)){
          $tag = " disabled ";
        }

        if(!preg_match("/r/",$acl)){
          $name = "";
          $value= "";
        }

        $table.="<tr>".
          " <td>".
          "  <input name=\"option$i\" size=25 maxlength=50 value=\"".$name."\" ".$tag.">".
          " </td>".
          " <td>".
          "  <input name=\"value$i\" size=60 maxlength=250 value=\"".$value."\" ".$tag.">".
          "  <br>".
          " </td>".
          " <td>".
          "  <input type=\"submit\" name=\"remove$i\" value=\""._("Remove")."\" ".$tag.">".
          " </td>".
          "</tr>";
      }
    }
    $table.= "</table>";
    $table.="<input type=\"submit\" name=\"add_option\" value=\""._("Add option")."\">";

    /* Show main page */
    $smarty= get_smarty();
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $smarty->assign("table", $table);
    $display.= $smarty->fetch(get_template_path('parameters.tpl', TRUE));
    $this->parent->by_object['application']->generateTemplate();
    return ($display);
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();

    /* Zero attributes */
    $this->attrs= array();
    $this->attrs['gosaApplicationParameter']= array();

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of application parameters with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['option0'])){
      for ($i= 0; $i<count($this->option_name); $i++){
        $this->option_name[$i]= $_POST["option$i"];
        $this->option_value[$i]= "";
        if ($_POST["value$i"] != ""){
          $this->option_value[$i]= $_POST["value$i"];
        }
      }
    }
  }


  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Check for valid option names */
    for ($i= 0; $i<count($this->option_name); $i++){
      if (!preg_match ("/^[a-z0-9_]+$/i", $this->option_name[$i])){
        $message[]= sprintf(_("Value '%s' specified as option name is not valid."), 
            $this->option_name[$i]);
      }
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Generate values */
    $this->attrs= array();
    if (count($this->option_name) == 0){
      $this->attrs['gosaApplicationParameter']= array();
    } else {
      for ($i= 0; $i<count($this->option_name); $i++){
        $this->attrs['gosaApplicationParameter'][]= $this->option_name[$i].
          ":".$this->option_value[$i];
      }
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of application parameters with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events('modify');
  }

  /* Return plugin informations for acl handling
#FIXME FAIscript seams to ununsed within this class... */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Parameter"),
          "plDescription" => _("Parameter configuration"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("application"),

          "plProvidedAcls"=> array(
            "gosaApplicationParameter"      => _("Application parameter settings"))  
          ));
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
