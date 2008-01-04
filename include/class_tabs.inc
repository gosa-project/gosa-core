<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003  Cajus Pollmeier

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

class tabs
{
  var $dn;
  var $config;
  var $acl;
  var $is_template;
  var $is_new= FALSE;

  var $last= "";
  var $current= "";
  var $disabled= "";
  var $by_name= array();
  var $by_object= array();
  var $SubDialog = false;

  var $multiple_support_active = FALSE;

  function tabs(&$config, $data, $dn, $acl_category= "")
  {
    /* Save dn */
    $this->dn= $dn;
    $this->config= &$config;

    $baseobject= NULL;

    foreach ($data as &$tab){
      $this->by_name[$tab['CLASS']]= $tab['NAME'];

      if ($baseobject === NULL){
        $baseobject= new $tab['CLASS']($this->config, $this->dn);
        $baseobject->enable_CSN_check();
        $this->by_object[$tab['CLASS']]= $baseobject;
      } else {
        $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn, $baseobject);
      }

      $this->by_object[$tab['CLASS']]->parent= &$this;
      $this->by_object[$tab['CLASS']]->set_acl_category($acl_category);

      /* Initialize current */
      if ($this->current == ""){
        $this->current= $tab['CLASS'];
      }
    }
  }


  function execute()
  {
    /* Rotate current to last */
    $this->last= $this->current;

    /* Look for pressed tab button */
    foreach ($this->by_object as $class => &$obj){
      if (isset($_POST[$class]) || (isset($_POST['arg']) && $_POST['arg'] == "$class")){
        $this->current= $class;
        break;
      }
    }

    /* Save last tab object */
    if ($this->last == $this->current){
      $this->save_object(TRUE);
    } else {
      $this->save_object(FALSE);
    }

    /* Build tab line */
    $display= $this->gen_tabs();

    /* Show object */
    $display.= "<table summary=\"\" cellpadding=4 cellspacing=0 border=0 style=\"width:100%; background-color:#F8F8F8; border-style:solid; border-color:#AAA; border-top-width:0px; border-bottom-width:1px; border-left-width:1px; border-right-width:1px;\">\n";
    $display.= "<tr><td>\n";

    /* If multiple edit is enabled for this tab, 
       we have tho display different templates */
    if(!$this->multiple_support_active){
      $display.= $this->by_object[$this->current]->execute();
    }else{
      $display.= $this->by_object[$this->current]->multiple_execute();
    }

    /* Footer for tabbed dialog */
    $display.= "</td></tr></table>";

    return ($display);
  }

  function save_object($save_current= FALSE)
  {
    /* Save last tab */
    if ($this->last != ""){
      @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
          $this->last, "Saving");

      if(!$this->multiple_support_active){
        $this->by_object[$this->last]->save_object ();
      }else{
        $this->by_object[$this->last]->multiple_save_object();
      }
    }

    /* Skip if curent and last are the same object */
    if ($this->last == $this->current){
      return;
    }

    $obj= @$this->by_object[$this->current];
    $this->disabled= $obj->parent->disabled;

    if ($save_current){
      @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
          $this->current, "Saving (current)");

      if(!$this->multiple_support_active){
        $obj->save_object();
      }else{
        $obj->multiple_save_object();
      }
    }
  }

  function gen_tabs()
  {
    $display= "<input type=\"hidden\" name=\"arg\" value=\"\">";
    $display.= "<table summary=\"\" cellpadding=0 cellspacing=0 border=0 style=\"width:100%;\"><tr>";
    $index= 0;
    $style= array("tab_left", "tab_active", "tab_near_active", "tab_right");
    foreach ($this->by_name as $class => $name){

      /* Activate right tabs with style "tab_right"
         Activate near current with style "tab_near_active" */
      if ($index == 2 || $index == 1){
        $index++;
      }

      /* Activate current tab with style "tab_active " */
      if ($class == $this->current){
        $index++;
      }

      /* Paint tab */
      $display.= "<td style=\"vertical-align:bottom;width:1px;white-space:nowrap;\">";

      /* Shorten string if its too long for the tab headers*/
      $title= _($name);
      if (mb_strlen($title, 'UTF-8') > 28){
        $title= mb_substr($title,0, 25, 'UTF-8')."...";
      }

      /* nobr causes w3c warnings so we use &nbsp; to keep the tab name in one line */
      $title= preg_replace("/ /","&nbsp;",$title);

      /* Take care about notifications */
      $obj = $this->by_object[$class];
      if ( $this->by_object[$class]->pl_notify && ($obj->is_account || $obj->ignore_account)){
        $notify= "id=\"notify\"";
      } else {
        $notify= "";
      }

      if (session::get('js')==FALSE){	
        $display.= "<div ".$notify." class=\"$style[$index]\"><input type=\"submit\" name=\"$class\"".
          " class=\"$style[$index]\" value=\"$title\"";
      } else {			 
        $display.= "<div ".$notify." class=\"$style[$index]\"><a class=\"$style[$index]\" onclick=\"return true;\" href=\"javascript:document.mainform.arg.value='$class';document.mainform.submit();\">$title</a";
      }
      $display.= "></div></td>";
    }
    $display.= "<td style=\"vertical-align:bottom;\">\n";
    $display.= "<div class=\"tab_border\">&nbsp;</div></td></tr></table>";

    return($display);
  }


  function set_acl($acl)
  {
  	/* Look for attribute in ACL */
	  trigger_error("Don't use tabs::set_acl() its obsolete.");
  }

  function delete()
  {
    /* Check if all plugins will ACK for deletion */
    foreach (array_reverse($this->by_object) as $key => $obj){
      $reason= $obj->allow_remove();
      if ($reason != ""){
        msg_dialog::display(_("Warning"), sprintf(_("Delete process has been canceled by plugin '%s': %s"), $key, $reason), WARNING_DIALOG);
        return;
      }
    }

    /* Delete for all plugins */
    foreach (array_reverse($this->by_object) as $obj){
      $obj->remove_from_parent();
    }
  }

  function password_change_needed()
  {
    /* Ask all plugins for needed password changes */
    foreach ($this->by_object as &$obj){
      if ($obj->password_change_needed()){
        return TRUE;
      }
    }

    return FALSE;
  }

  function check($ignore_account= FALSE)
  {
    $this->save_object(TRUE);
    $messages= array();

    $current_set = FALSE;

    /* Check all plugins */
    foreach ($this->by_object as $key => &$obj){
      if ($obj->is_account || $ignore_account || $obj->ignore_account){
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$key, "Checking");

        if(!$this->multiple_support_active){
          $msg = $obj->check();
        }else{
          $msg = $obj->multiple_check();
        }

        if (count($msg)){
          $obj->pl_notify= TRUE;
          if(!$current_set){
            $current_set = TRUE;
            $this->current= $key;
            $messages = $msg;
          }
        }else{
          $obj->pl_notify= FALSE;
        }
      }else{
        $obj->pl_notify= FALSE;
      }
    }
    return ($messages);
  }

  function save($ignore_account= FALSE)
  {
    /* Save all plugins */
    foreach ($this->by_object as $key => &$obj){
      @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
          $key, "Saving");

      $obj->dn= $this->dn;

      if ($obj->is_account || $ignore_account || $obj->ignore_account){
        if ($obj->save() == 1){
          return (1);
        }
      } else {
        $obj->remove_from_parent();
      }
    }
    return (0);
  }

  function adapt_from_template($dn)
  {
    foreach ($this->by_object as $key => &$obj){
      @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
          $key, "Adapting");
      $obj->parent= &$this;
      $obj->adapt_from_template($dn);
    }
  }

	
  /* Save attributes posted by copy & paste dialog
   */
  function saveCopyDialog()
  {
	  foreach ($this->by_object as &$obj){
		  if($obj->is_account || $obj->ignore_account){
			  $obj->saveCopyDialog();
		  }
	  }
  }


  /* return copy & paste dialog
   */
  function getCopyDialog()
  {
    $ret = "";
    $this->SubDialog = false;
    foreach ($this->by_object as &$obj){
      if($obj->is_account || $obj->ignore_account){
        $tmp = $obj->getCopyDialog();
        if($tmp['status'] == "SubDialog"){
          $this->SubDialog = true;
          return($tmp['string']);
        }else{
          if(!empty($tmp['string'])){
            $ret .= $tmp['string'];
            $ret .= "<p class='seperator'>&nbsp;</p>";
          }
        }
      }
    }
    return($ret);
  }


  function addSpecialTabs()
  {
    $this->by_name['acl']= _("ACL");
    $this->by_object['acl']= new acl($this->config, $this, $this->dn);
    $this->by_object['acl']->parent= &$this;
    $this->by_name['reference']= _("References");
    $this->by_object['reference']= new reference($this->config, $this->dn);
    $this->by_object['reference']->parent= &$this;
  }


  function set_acl_base($base= "")
  {
    /* Update reference, transfer variables */
    $first= ($base == "");
    foreach ($this->by_object as &$obj){
      if ($first){
        $first= FALSE;
        $base= $obj->acl_base;
      } else {
        $obj->set_acl_base($base);
      }
    }
  }

 
  /*!   \brief    Checks if one of the used tab plugins supports multiple edit.
        @param    boolean Returns TRUE if at least one plugins supports multiple edit. 
  */
  function multiple_support_available()
  {
    foreach($this->by_object as $name => $obj){
      if($obj->multiple_support){
        return(TRUE);
      }
    }
    return(FALSE);
  }  


  /*!   \brief    Enables multiple edit support for the given tab.
                  All unsupported plugins will be disabled.
        @param    boolean Returns TRUE if at least one plugin supports multiple edit 
  */
  function enable_multiple_support()
  {
    if(!$this->multiple_support_available()){
      return(FALSE);
    }else{
      $this->multiple_support_active = TRUE;
      foreach($this->by_object as $name => $obj){
        if($obj->multiple_support){
          $this->by_object[$name]->enable_multiple_support();
        }else{
          unset($this->by_object[$name]);
          unset($this->by_name[$name]);
        }
      }
    }
    return(TRUE);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
