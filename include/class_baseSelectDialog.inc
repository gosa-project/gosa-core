<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class baseSelectDialog extends MultiSelectWindow
{
  var $selectedBase        = ""; // Used for navigation 
  var $base_selection_regex = "*"; // Used regex ... 
  var $BaseToUse         = false;  // used to specify the selected base, 
                                      // false if none is selected

  var $allowedBases         = array();
  var $parent               = NULL;
 
  function baseSelectDialog (&$config,$parent,$onlyAllowThisBases = array())
  {
    $module= "department";
    MultiSelectWindow::MultiSelectWindow($config, "BASEselectWindow", $module);
   
    $this->parent = $parent;
 
    $this->selectedBase = $config->current['BASE'];
    $this->allowedBases  = $onlyAllowThisBases;

    $this->SetTitle("Base");
    $this->SetSummary(_("Choose a base"));
    $this->SetListHeader("<div style='background:#F0F0F9;padding:5px;'>".
        " <input class='center' type='image' src='images/lists/root.png' align='middle'
        title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
        " <input class='center' type='image' align='middle' src='images/lists/back.png'
        title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
        " <input class='center' type='image' align='middle' src='images/lists/home.png'
        title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
        " <input class='center' type='image' src='images/lists/reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
        "</div>");

    $this->SetInformation(_("Step in the prefered tree and click save to use the current subtree as base. Or click the image at the end of each entry."));

    $this->EnableAplhabet   (true);
    $this->EnableCloseButton(true);
    $this->EnableSaveButton (true);

    $this->SetSaveButtonString(_("Use"));
    $this->SetCloseButtonString(_("Cancel"));

    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Base")));

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("base_selection_regex",_("Filter entries with this syntax"),"*"      , true);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->AddDepartments($this->selectedBase,2);
    return($this->Draw());
  }

  function setEntries()
  {
    // Do nothing here 
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
    $this->BaseToUse = $this->selectedBase;
  }

  function isSelected() 
  {
    return($this->BaseToUse);
  }

  function setCurrentBase($base)
  {
    $this->selectedBase = $base;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
