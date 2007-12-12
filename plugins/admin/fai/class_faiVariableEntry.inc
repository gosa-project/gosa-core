<?php

class faiVariableEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description","FAIvariableContent");
  var $objectclasses= array();

  var $orig_cn              = "";
  var $dn            = "";
  var $cn            = "";
  var $FAIvariableContent   = "";
  var $description   = "";
  var $status        = "new";
  var $parent        = NULL;
  var $FAIstate      = "";
  
  function faiVariableEntry (&$config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if((isset($object['cn'])) && (!empty($object['cn']))){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = $name;
        $this->$oname=addslashes($value);
      }

      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }

    }else{
      $this->status = "new";
      $this->orig_cn       = false;
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

     /* Magic quotes GPC, escapes every ' " \, to solve some security risks
     * If we post the escaped strings they will be escaped again
     */
    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
        $smarty->assign($attrs,htmlentities (stripslashes(utf8_decode($this->$attrs))));
      }else{
        $smarty->assign($attrs,htmlentities (utf8_decode($this->$attrs)));
      }
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $acl = $this->getacl($name);
      if($this->FAIstate == "freezed"){
        $acl = preg_replace("/w/","",$acl);
      }
      $smarty->assign($name."ACL",$acl);
    }



    $display.= $smarty->fetch(get_template_path('faiVariableEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
     if((isset($_POST['SubObjectFormSubmitted'])) && ($this->FAIstate != "freeze")){
      foreach($this->attributes as $attrs){
        if($this->acl_is_writeable($attrs)){
          if(isset($_POST[$attrs])){
            $this->$attrs = $_POST[$attrs];
          }else{
            $this->$attrs = "";
          }
        }
      }
    }
  }

  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(isset($this->parent->SubObjects[$this->cn]) && $this->cn != $this->orig_cn){
      $message[] =_("There is already a variable with the given name.");
    }
    
    if(empty($this->FAIvariableContent)) {
      $message[]=_("Please specify a value for the attribute 'content'."); 
    }
   
    if(empty($this->cn)){
      $message[] = _("Please enter a name.");
    }

    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $tmp[$attrs] = stripslashes( $this->$attrs);
    }

    if(($this->orig_cn)&&($tmp['cn']!=$this->orig_cn)){
      $tmp['remove']['from']  = $this->orig_cn;
      $tmp['remove']['to']    = $tmp['cn'];
    }
  
    $tmp['dn']      = $this->dn;  
    $tmp['status']  = $this->status;  
    return($tmp);
  }

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName" => _("Variable entry"),
          "plDescription" => _("FAI variable entry "),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 23,
          "plSection"     => array("administration"),
          "plCategory"    => array("fai"),
          "plProvidedAcls" => array(
            "cn"                => _("Name"),
            "description"       => _("Description"),
            "FAIvariableContent"=> _("Variable content") )
          ));
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
