<?php

class aclroletab extends tabs
{

  function aclroletab($config, $data, $dn)
  {
    $data = array(array("CLASS" => "aclrole" , "NAME" => _("ACL Templates")));
    tabs::tabs($config, $data, $dn,"acl");
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['aclrole'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $new_dn= 'cn='.$baseobject->cn.",ou=aclroles,".$baseobject->base;

    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['aclrole']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    $ret= tabs::save();

    /* Fix tagging if needed */
    $baseobject->dn= $new_dn;
    $baseobject->handle_object_tagging();

    return $ret;
  }

  function save_object($ignore_account= FALSE)
  {
    tabs::save_object();
  }
  
  function execute()
  {
    $display= tabs::execute();
    if(!$this->by_object['aclrole']->dialog){
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
