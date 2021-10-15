<?php
class applicationParameters extends plugin
{
  /* Parameters  */
  var $option_name= array();
  var $option_value= array();

  /* attribute list for save action */
  var $attributes= array("gosaApplicationParameter");

  function __construct(&$config, $dn= NULL, $parent= NULL)
  {
    plugin::__construct ($config, $dn, $parent);

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

    $theme = getThemeName();

    /* Do we need to flip is_account state? */
    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display= $this->show_disable_header(_("Remove options"),msgPool::featuresEnabled(_("parameter")));
      } else {
        $display= $this->show_enable_header(_("Create options"), msgPool::featuresDisabled(_("parameter")));
        $this->parent->by_object['application']->generateTemplate();
        return ($display);
      }
    }

    if($this->acl_is_writeable("gosaApplicationParameter")){

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
    }

    /* Generate list of attributes */
    if (count($this->option_name) == 0){
      $this->option_name[]= "";
      $this->option_value[]= "";
    }



    $acl = $this->getacl("gosaApplicationParameter");

    switch ($theme) {
      case 'classic':
        $table= "<table summary=\""._("Application parameter")."\"><tr><td>"._("Variable")."</td><td>"._("Default value")."</td><td></td></tr>";
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
              "  <input type='text' name=\"option$i\" size=25 maxlength=50 value=\"".set_post($name)."\" ".$tag.">".
              " </td>".
              " <td>".
              "  <input type='text' name=\"value$i\" size=60 maxlength=250 value=\"".set_post($value)."\" ".$tag.">".
              "  <br>".
              " </td>".
              " <td>".
              "  <button type=\"submit\" name=\"remove$i\" ".$tag.">".msgPool::delButton()."</button>".
              " </td>".
              "</tr>";
          }
        }
        $table.= "</table>";
        $table.="<button type=\"submit\" name=\"add_option\" $tag >"._("Add option")."</button>";
        break;

      default:
        $table= "<div class='parameter-content'>";
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

            $table.=  "<div class='parameter-option'>
                        <div class='input-field'>
                          <input type='text' name=\"option$i\" size=25 maxlength=50 value=\"".set_post($name)."\" ".$tag.">
                          <label for=\"option$i\">"._("Variable")."</label>
                        </div>
                        <div class='input-field'>
                          <input type='text' name=\"value$i\" size=60 maxlength=250 value=\"".set_post($value)."\" ".$tag.">
                          <label for=\"value$i\">"._("Default value")."</label>
                        </div>
                        <div class='action-wrapper'>
                          <button class='btn-small' type=\"submit\" name=\"remove$i\" ".$tag.">".msgPool::delButton()."</button>
                        </div>
                      </div>";
          }
        }
        $table.=  "</div>";
        $table.="<button class='btn-small' type=\"submit\" name=\"add_option\" $tag >"._("Add option")."</button>";
        break;
    }


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

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['option0']) && $this->acl_is_writeable("gosaApplicationParameter")){
      for ($i= 0; $i<count($this->option_name); $i++){
        $this->option_name[$i]= get_post("option$i");
        $this->option_value[$i]= "";
        if ($_POST["value$i"] != ""){
          $this->option_value[$i]= get_post("value$i");
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
        $message[]= msgPool::invalid(_("Name"), $this->option_name[$i],"/^[a-z0-9_]+$/i");
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

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events('modify');
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $source_o = new applicationParameters($this->config,$source['dn']);
    $this->option_name = $source_o->option_name;
    $this->option_value = $source_o->option_value;

    /* Set is_account state so that the save function is called properly */
    $this->is_account = TRUE;
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
