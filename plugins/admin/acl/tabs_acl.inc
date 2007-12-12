<?php

class acltab extends tabs
{

  function acltab($config, $data, $dn)
  {
    $data = array(array("CLASS" => "acl" , "NAME" => _("Acl")));

    /* Save dn */
    $this->dn= $dn;
    $this->config= $config;

    $baseobject= NULL;

    foreach ($data as $tab){
      $this->by_name[$tab['CLASS']]= $tab['NAME'];

      
      if ($baseobject === NULL){
        if($tab['CLASS'] == "acl"){
          $baseobject= new $tab['CLASS']($this->config, $this, $this->dn);
        }else{
          $baseobject= new $tab['CLASS']($this->config,$this->dn);
        }
        $this->by_object[$tab['CLASS']]= $baseobject;
      } else {
        if($tab['CLASS'] == "acl"){
          $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config,$this,$this->dn, $baseobject);
        }else{
          $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn, $baseobject);
        }
      }

      $this->by_object[$tab['CLASS']]->parent= &$this;
      $this->by_object[$tab['CLASS']]->set_acl_category("acl");

      /* Initialize current */
      if ($this->current == ""){
        $this->current= $tab['CLASS'];
      }
    }
  }

  function save($ignore_account= FALSE)
  {
    $ret= tabs::save();
    return $ret;
  }

  function save_object($ignore_account= FALSE)
  {
    tabs::save_object();
  }
  
  function execute()
  {
    $display= tabs::execute();
    if(!$this->by_object['acl']->dialog){
      $display.= "<p style=\"text-align:right\">\n";
      $display.= "  <input type=submit name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
      $display.= "  &nbsp;\n";
      $display.= "  <input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
      $display.= "</p>";
    }
    return($display);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
