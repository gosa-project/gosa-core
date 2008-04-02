<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_sudo.inc 9975 2008-03-25 14:09:30Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class sudo extends plugin
{
  /* Group attributes */
  var $cn= "";
  var $description= "";

  var $sudoUser   = array();
  var $sudoCommand= array();
  var $sudoOption = array();
  var $sudoHost   = array();
  var $sudoRunas  = array();

  var $objectclasses = array("top","sudoRole");
  var $attributes    = array("cn","description","sudoUser","sudoCommand","sudoOption","sudoHost","sudoRunas");

  function sudo(&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    foreach(array("sudoUser","sudoCommand","sudoOption","sudoHost","sudoRunas") as $attr){
      $this->$attr = array();
      if(isset($this->attrs[$attr])){
        $tmp = array();
        for($i = 0 ; $i < $this->attrs[$attr]['count']; $i++){
          $tmp[] = $this->attrs[$attr][$i];
        }
        $this->$attr = $tmp;
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
  
    
  
  
  








    $smarty = get_smarty();

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    $divlist_sudoUser = new divSelectBox("divlist_sudoUser");
    $divlist_sudoUser->SetHeight("90");
    $divlist_sudoHost = new divSelectBox("divlist_sudoHost");
    $divlist_sudoHost->Setheight("90");
    $divlist_sudoRunas = new divSelectBox("divlist_sudoRunas");
    $divlist_sudoRunas->Setheight("90");
    $divlist_sudoOption = new divSelectBox("divlist_sudoOption");
    $divlist_sudoOption->Setheight("90");
    $divlist_sudoCommand = new divSelectBox("divlist_sudoCommand");
    $divlist_sudoCommand->Setheight("90");

    $neg_img= "<img src='images/negate.png' alt='!' class='center'>"; 
    $option = "<input type='image' src='images/negate.png'     name='del_%ATTR%_%KEY%' class='center'>"; 
    $option.= "<input type='image' src='images/edittrash.png'  name='neg_%ATTR%_%KEY%' class='center'>"; 
    foreach(array("sudoUser","sudoCommand","sudoOption","sudoHost","sudoRunas") as $attr){
      foreach($this->$attr as $key => $entry){
        $entry = preg_replace("/^!/",$neg_img,$entry);
        
        $list_name = "divlist_".$attr;
        $$list_name->AddEntry(
          array(
            array("string" => $entry),
            array("string" => preg_replace(array("/%KEY%/","/%ATTR%/"),array($key,$attr),$option),
              "attach" => "style='width:40px; border-right: 0px;'")));
      }
    }

    $smarty->assign("divlist_sudoUser",$divlist_sudoUser->DrawList());
    $smarty->assign("divlist_sudoHost",$divlist_sudoHost->DrawList());
    $smarty->assign("divlist_sudoRunas",$divlist_sudoRunas->DrawList());
    $smarty->assign("divlist_sudoOption",$divlist_sudoOption->DrawList());
    $smarty->assign("divlist_sudoCommand",$divlist_sudoCommand->DrawList());
    
    return($smarty->fetch(get_template_path('generic.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();

    /* Send signal to the world that we've done */
    $this->handle_post_events("remove");
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }

  /* Save to LDAP */
  function save()
  {
    plugin::save(); 
  }

  function check()
  {
    $message = plugin::check();
    return ($message);
  }

  function getCopyDialog()
  {
    echo "NIY";
  }

  function saveCopyDialog()
  {
    echo "NIY";
  }
  
  /* Return plugin informations for acl handling  */ 
  static function plInfo()
  {
    return (array(  
          "plShortName" => _("Sudo"),
          "plDescription" => _("Sudo role"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("admin"),
          "plCategory"    => array("sudo" => array("objectClass" => "sudoRole", "description" => _("Sudo role"))),

          "plProvidedAcls"    => array(
            "cn"                => _("Name"),
            "description"       => _("Description"),
            "sudoUser"          => _("Users"),
            "sudoHost"          => _("Host"),
            "sudoCommand"       => _("Command"),
            "sudoRunas"         => _("Run as user"),
            "sudoOption"        => _("Flags"))
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
