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

  var $last= "";
  var $current= "";
  var $disabled= "";
  var $by_name= array();
  var $by_object= array();
  var $SubDialog = false;

  function tabs($config, $data, $dn)
  {
	/* Save dn */
	$this->dn= $dn;
	$this->config= $config;

	foreach ($data as $tab){
		$this->by_name[$tab['CLASS']]= $tab['NAME'];
		$this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn);
		$this->by_object[$tab['CLASS']]->parent= &$this;

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
	foreach ($this->by_object as $class => $obj){
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
	$display.= "<table summary=\"\" cellpadding=4 cellspacing=0 border=0 style=\"width:100%; background-color:#F0F0F0; border-style:solid; border-color:black; border-top-width:0px; border-bottom-width:1px; border-left-width:1px; border-right-width:1px;\">\n";
	$display.= "<tr><td>\n";

	$obj= $this->by_object[$this->current];
	$display.= $obj->execute();
	$this->by_object[$this->current]= $obj;

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

		$obj= $this->by_object[$this->last];
		$obj->save_object ();
		$this->by_object[$this->last]= $obj;
	}

	/* Skip if curent and last are the same object */
	if ($this->last == $this->current){
		return;
	}

	$obj= $this->by_object[$this->current];
	$this->disabled= $obj->parent->disabled;

	if ($save_current){
        	@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
        	        $this->current, "Saving (current)");

		$obj->save_object ();
		$this->by_object[$this->current]= $obj;
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
		$display.= "<td style=\"vertical-align:bottom; width:100px;\">";

		/* Shorten string if its too long for the tab headers*/
		$title= _($name);
		if (mb_strlen($title, 'UTF-8') > 14){
			$title= mb_substr($title,0, 12, 'UTF-8')."...";
		}
		
		if ($_SESSION['js']==FALSE){	
			$display.= "<div class=\"$style[$index]\"><input type=\"submit\" name=\"$class\"".
		                   " class=\"$style[$index]\" value=\"$title\"";
		} else {			 
			$display.= "<div class=\"$style[$index]\"><a class=\"$style[$index]\" onclick=\"return true;\" href=\"javascript:document.mainform.arg.value='$class';document.mainform.submit();\">$title</a";
		}
		$display.= "></div></td>";
	}
	$display.= "<td style=\"vertical-align:bottom;\">\n";
	$display.= "<div class=\"tab_border\">&nbsp;</div></td></tr></table>";

	return($display);
  }


  function set_acl($acl)
  {
	/* Set local acl */
	$this->acl= $acl;

	/* Setup for all plugins */
	foreach ($this->by_object as $key => $obj){
		$sacl= get_module_permission($acl, "$key", $this->dn);
		$obj->acl= $sacl;
		$this->by_object[$key]= $obj;
	}
  }

  function delete()
  {
	/* Check if all plugins will ACK for deletion */
	foreach (array_reverse($this->by_object) as $key => $obj){
		$reason= $obj->allow_remove();
		if ($reason != ""){
			print_red(sprintf(_("Delete process has been canceled by plugin '%s': %s"), $key, $reason));
			return;
		}
	}

	/* Delete for all plugins */
	foreach (array_reverse($this->by_object) as $key => $obj){
		$obj->remove_from_parent();
	}
  }

  function password_change_needed()
  {
	/* Ask all plugins for needed password changes */
	foreach ($this->by_object as $key => $obj){
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

	/* Check all plugins */
	foreach ($this->by_object as $key => $obj){
		if ($obj->is_account || $ignore_account || $obj->ignore_account){
	        	@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
	        	        $key, "Checking");

			$messages= $obj->check();
			if (count($messages)){
				$this->current= $key;
				break;
			}
		}
	}

	return ($messages);
  }

  function save($ignore_account= FALSE)
  {
	/* Save all plugins */
	foreach ($this->by_object as $key => $obj){
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
	  foreach ($this->by_object as $key => $obj){
		  @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
				  $key, "Adapting");
		  $obj->parent= &$this;
		  $obj->adapt_from_template($dn);
		  $this->by_object[$key]= $obj;
	  }
  }

	
  /* Save attributes posted by copy & paste dialog
   */
  function saveCopyDialog()
  {
	  foreach ($this->by_object as $key => $obj){
		  if($obj->is_account){
			  $this->by_object[$key]->saveCopyDialog();
		  }
	  }
  }


  /* return copy & paste dialog
   */
  function getCopyDialog()
  {
	  $ret = "";
	  $this->SubDialog = false;
	  foreach ($this->by_object as $key => $obj){
		  if($obj->is_account){
			  $tmp = $this->by_object[$key]->getCopyDialog();
			  if($tmp['status'] == "SubDialog"){
				  $this->SubDialog = true;
	              return($tmp['string']);
	     	  }else{
                 $ret .= $tmp['string'];
			  }
		  }
	  }
	  return($ret);
  }

}

?>
