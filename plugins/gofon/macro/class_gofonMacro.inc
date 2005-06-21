<?php
class macro extends plugin
{
  /* CLI vars */
  var $cli_summary= "Handling of GOsa's application object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* application attributes */
  var $cn= "";
  var $description= "";
  var $base= "";
  var $displayName = "";
  var $goFonMacroContent;
  var $goFonMacroVisible=0;

  /* attribute list for save action */
  var $attributes= array("cn","base", "description","displayName","goFonMacroContent","goFonMacroVisible");
  var $objectclasses= array("top", "goFonMacro");

  function macro ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $ldap= $config->get_ldap_link();

    $this->dn = $dn;

    /* This is always an account */
    $this->is_account= TRUE;

    /* Edit or new one ?*/
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
    } else {
      $this->base= dn2base($this->dn);
    }
  }

  function execute()
  {
    /* Variables */
    $vars 	= "";
    $tmp  	= array();
    $number = 0; 

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no application.")."</b>";
      return ($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign all vars to Smarty */
    foreach($this->attributes as $ar){
      $smarty->assign($ar, $this->$ar);
    }
    /* Checkboxes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("vars", $vars);

    if($this->goFonMacroVisible){
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else{
      $smarty->assign("goFonMacroVisibleChecked","");
    }

    if(isset($_POST['goFonMacroVisible'])) {
      $this->goFonMacroVisible= 1 ;
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else  {
      if(isset($_POST['cn'])){
        $this->goFonMacroVisible= 0 ;
        $smarty->assign("goFonMacroVisibleChecked","");
      }
    }


    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['cn'])){
      plugin::save_object();

    }
  }


  /* Check values */
  function check()
  {
    $message = array();
    
    foreach($this->attributes as $attr){
      if(chkacl($this->acl,$attr)){
        $str =  sprintf( _("Insufficient permissions, can't change attribute '%s' in goFonMacro"),$attr) ;
        return(array($str));
        }
      }
    if(($this->cn != strtolower($this->cn))||(empty($this->cn))){
      $message[] = "The attribute 'cn' must be lowercase and a least one char.";
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Post checks */
    $this->execute();

    plugin::save();

    unset($this->attrs['base']);

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn);
    $a= $ldap->fetch();

    if (count($a)){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
      $this->handle_post_events("modify");
    } else {
      $ldap->cd($this->dn);
      $ldap->create_missing_trees( $this->dn);
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
    show_ldap_error($ldap->get_error());
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
