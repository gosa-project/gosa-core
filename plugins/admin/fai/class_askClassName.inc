<?php

class askClassName extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account     = TRUE;
  var $attributes         = array();
  var $objectclasses      = array("whatever");

  var $objectClass        = false;
  var $forbidden          = array();
  var $ClassName          = "";  
  var $ClassAlreadyExists = false;

  function askClassName (&$config,$dn,$ui,$objectClass)
  {
    $this->ui = $ui;
    $this->objectClass = $objectClass;
    plugin::plugin ($config, $dn);
    if(!isset($_SESSION['CurrentMainBase'])){
      session::set('CurrentMainBase',$this->config->current['BASE']);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty = get_smarty();
    $display= "";

    /* The faifilter contais 
     * The base for all fai objects 
     */
    $faifilter = $_SESSION['faifilter'];

    /* First search for every ou, in given fai base
     * With a second search detect all object that belong to the different ous. 
     */

    $base = get_ou('faiou').$_SESSION['CurrentMainBase'];
    if($faifilter['branch'] != "main"){
      $base = $faifilter['branch'];
    }
    $ldap = $this->config->get_ldap_link();
    
    $res= get_list("(&(objectClass=organizationalUnit)(!(objectClass=FAIbranch)))", "fai", $base,
                   array("cn","description","objectClass"), GL_SIZELIMIT );

    $delete = array();
    $used   = array();
    foreach($res as $objecttypes){
      $res2= get_list("(objectClass=*)", "fai", $objecttypes['dn'],
                      array("cn","description","objectClass","FAIclass","FAIstate"), GL_SIZELIMIT | GL_CONVERT );
      foreach($res2 as $object){

        /* skip class names with this name */ 
        if(in_array($this->objectClass,$object['objectClass']) || in_array("FAIprofile",$object['objectClass'])){
          if(isset($object['FAIstate'][0]) && preg_match("/removed$/",$object['FAIstate'][0])){
            continue;
          }
          $delete[] = $object['cn'][0];
        }
    
        /* Skip profiles */
        if(!in_array("FAIprofile",$object['objectClass'])){
          if(isset($object['cn'])){
            $used[$object['cn'][0]]= $object['cn'][0];
          }
        }
      }
    }

    /* Create headline
     * Depending on the object type we want to create, a different headline will be shown
     */ 
    switch($this->objectClass) {
      case "FAIpartitionTable":  $str =_("Create new FAI object - partition table.");break;
      case "FAIpackageList"   :  $str =_("Create new FAI object - package bundle.");break;
      case "FAIscript"        :  $str =_("Create new FAI object - script bundle.");break;
      case "FAIvariable"      :  $str =_("Create new FAI object - variable bundle.");break;
      case "FAIhook"          :  $str =_("Create new FAI object - hook bundle.");break;
      case "FAIprofile"       :  $str =_("Create new FAI object - profile.");break;
      case "FAItemplate"      :  $str =_("Create new FAI object - template.");break;
      default                 :  $str =_("Create new FAI object");break;
    }
    $smarty->assign("headline",$str);
    
    /* Save forbidden class names
     */
    $this->forbidden = $delete;

    /* Delete all class names which already have this type of object 
     */
    foreach($delete as $del){
      unset($used[$del]);
    }
   
    /* if there is no class name which is missing for this type 
     *  of objectClass, we only can create a new one, disable selectBox 
     */
    if(count ($used)==0){
      $smarty->assign("ClassNamesAvailable", " disabled ");
      $smarty->assign("grey", 'style="color:#C0C0C0"');
    }else{
      $smarty->assign("ClassNamesAvailable", "");
      $smarty->assign("grey", "");
    }
    ksort($used);
    $smarty->assign("ClassNames", $used);
    $smarty->assign("ClassName",  $this->ClassName);
    $display.= $smarty->fetch(get_template_path('askClassName.tpl', TRUE));
    return($display);
  }

  /* Get posts and set class name 
   */ 
  function save_object()
  {
    if(isset($_POST['classSelector']) && $_POST['classSelector'] == 1 
       && isset($_POST['edit_continue'])){
      $this->ClassName          = $_POST['UseTextInputName'];
      $this->ClassAlreadyExists = true;
    }
  
    if(isset($_POST['classSelector']) && $_POST['classSelector'] == 2 
       && isset($_POST['edit_continue'])){
      $this->ClassAlreadyExists = false;
      $this->ClassName          = $_POST['SelectedClass'];
    }
  }

  /* Check given class name 
   */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
  
    if($this->ClassName != preg_replace("/ /","",trim($this->ClassName))){
      $message[] = _("Spaces are not allowed within class names.");
    }

    if(empty($this->ClassName)){
      $message[]=_("The given class name is empty.");
    }

    if(in_array($this->ClassName,$this->forbidden)){
      $message[]=_("The specified class name is already in use for this object type.");
    }

    return ($message);
  }


  /* Return the class name */
  function save()
  {
    return($this->ClassName);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
