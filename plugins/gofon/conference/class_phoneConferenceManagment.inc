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
require "tabs_conference.inc";

class phoneConferenceManagment extends plugin
{
	/* Definitions */
	var $plHeadline				  = "Phone Conference Rooms";
	var $plDescription			= "Managment ";

	/* CLI vars */
	var $cli_summary			  = "Handling of LDAP subtrees";
	var $cli_description		= "Some longer text\nfor help";
	var $cli_parameters			= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

	/* Headpage attributes */
	var $conferences			  = array();

	/* attribute list for save action */
	var $attributes				  = array();
	var $objectclasses			= array();
  var $conftab            = false;

	/* Initialise Class */
	function phoneConferenceManagment ($config, $ui)
	{
		$this->ui			= $ui;
		$this->dn			= "";
		$this->config		= $config;
	}

	/* Execute class and display something */
	function execute()
	{
		/* Reload departments */
		$this->config->departments	= get_departments();
		$smarty						= get_smarty();
		$display 					= "";
		$s_action 					= "";  // Will contain an action, like del or edit
		$s_entry  					= "";  // The entry name for edit delete -...

    foreach($_POST as $post => $key){
      if(preg_match("/.*new.*/i",$post)){
        $s_action = "new";
      }
    }
      
    if($s_action == "new"){
      $this->dn= "new";
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn);
      $this->conftab->set_acl(array(':all'));
    }
    
    /* if edit or new, show dialog */
		if($this->conftab){
      $display= $this->conftab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->conftab->by_object[$this->conftab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    $this->reload();
		$listhead = "<div style='background:#F0F0F9;padding:5px;'>  
                  <input type='image' align='middle' src='images/list_new_department.png' 
                      align='middle' alt='"._("Create new department")."' name='dep_new'>&nbsp;
			          </div>";
    $actions = "<input type='image' src='images/edit.png' 		 alt='"._("edit")."' 	  name='dep_edit_%KEY%' title='"._("Edit this entry")."'>";
		$actions.= "<input type='image' src='images/edittrash.png' alt='"._("delete")."' 	name='dep_del_%KEY%'  title='"._("Delete this entry")."'>";
		$linkopen= "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    /* Set Header */
		$divlist  = new divlist("conferenceTabs");
		$divlist->SetSummary(_("This table displays all available conference rooms."));
		$divlist->SetEntriesPerPage(20);
		$divlist->SetHeader(array(
          array("string" => "&nbsp;"),            
					array("string" =>_("Name / nummer")),
					array("string" =>_("Actions"), "attach" => "style='text-align:  right;border:none'")));

    foreach($this->conferences as $conference ){
      $field1 = array("string"=>"");
      $field2 = array("string"=>$conference['cn'][0]);
      $field3 = array("string"=>"");
      $divlist->AddEntry(array($field1,$field2,$field3));
    }

		$smarty->assign("search_image",     get_template_path('images/search.png'));
		$smarty->assign("tree_image",       get_template_path('images/tree.png'));
		$smarty->assign("infoimage",        get_template_path('images/info.png'));
		$smarty->assign("launchimage",      get_template_path('images/launch.png'));
    $smarty->assign("conferences",      $divlist->DrawList());
    $smarty->assign("conferencehead",  $listhead);
		$smarty->assign("deplist",          $this->config->idepartments);
		$smarty->assign("apply",            apply_filter());
		$smarty->assign("alphabet",         generate_alphabet());
		$smarty->assign("hint",             print_sizelimit_warning());

		$display= $smarty->fetch(get_template_path('headpage.tpl', TRUE));
		return($display);
	}


	function reload()
	{
    $base = "dc=gonicus,dc=de";
		$this->conferences= get_list($this->ui->subtreeACL, "(objectClass=goFonAccount)",TRUE, $base, array("*"), TRUE);
	  
  }

	function remove_from_parent()
	{
		$ldap= $this->config->get_ldap_link();
		$ldap->cd ($this->dn);
		$ldap->recursive_remove();

		/* Optionally execute a command after we're done */
		$this->postremove();

		/* Delete references to object groups */
		$ldap->cd ($this->config->current['BASE']);
		$ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
		while ($ldap->fetch()){
			$og= new conftab($this->config, $ldap->getDN());
			unset($og->member[$this->dn]);
			$og->save ();
		}

	}

	function remove_lock()
	{
		if (isset($this->dn)){
			del_lock ($this->dn);
		}
	}

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
