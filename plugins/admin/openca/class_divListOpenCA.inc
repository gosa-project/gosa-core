<?php
/*
 * Created on 24-ott-2006 by "Guido Serra aka Zeph" <zeph@purotesto.it>
 * 
 * PuroTesto (freesoftware developers, Milano, IT)
 * http://purotesto.it
 * 
 * This file is part of OpenCA module for GOsa^2.
 *
 * The OpenCA module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * OpenCA module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */
 
class divListOpenCA extends MultiSelectWindow {
	
	/* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $UserRegex          = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $ShowSPKACs;
  var $ShowPKCS10s;
  var $ShowCertificates;
  var $ShowExpired;
  var $ShowRevoked;

  /* Subsearch checkbox */
  var $SubSearch;
  
	var $parent;
	var $ui;

	function divListOpenCA ($config, $parent){
		MultiSelectWindow::MultiSelectWindow($config,"System");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();
    
    /* Set list strings */
    $this->SetTitle(_("List of cryptographic objects"));
    $this->SetSummary(_("List of cryptographic objects"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to sign, " .
    		"revoke and view the status of every certificate. "));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);
    
    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("SSL Object / Department")));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:60px;border-right:0px;'"));

    /*                  Text        				,Value    																								,Name         					,Is selected */
    $this->AddCheckBox("ShowSPKACs",    		_("Select to see pending netscape signature requests"), 	_("Show pending SPKACs"),      	true);
    $this->AddCheckBox("ShowPKCS10s",      	_("Select to see penging explorer signature requests"),   _("Show pending PKCS10s") ,    	true);
    $this->AddCheckBox("ShowCertificates",  _("Select to see valid certificates"), 										_("Show valid certificates"), 	true);
    $this->AddCheckBox("ShowExpired",				_("Select to see expired certificates"), 									_("Show expired certificates"),	true);
    $this->AddCheckBox("ShowRevoked",				_("Select to see revoked certificates"),    							_("Show revoked certificates") ,true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name        ,Text                             ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display SSL objects matching"),"*" , true);
    $this->AddRegex   ("UserRegex", _("Display SSL objects of user"), "*" , false, "images/search_user.png");
    
	}
		
	function GenHeader(){
   /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
    " <input class='center' type='image' src='images/list_root.png' align='middle'
        title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/list_back.png'
        title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/list_home.png'
        title='"._("Go to users department")."' alt='"._("Home")."'                     name='dep_home'>&nbsp;".
    " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='".          _("Submit")."'>&nbsp;".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;"._("Base")."&nbsp;".
    " <select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
    " <input class='center' type='image' src='images/list_submit.png' align='middle'
        title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;".
    "</div>";
    $this->SetListHeader($listhead);
	}
	
	function execute(){    
		$this->ClearElementsList();
    $this->GenHeader();
	}
	
	function setEntries($sslObjects){		
		foreach($sslObjects as $sslObject){

			$str = $sslObject["cn"][0];
		
			$elem1 = array("string" => "");
			$elem2 = array("string" => $str);
			$elem3 = array("string" => "");		
			$this->AddElement( array($elem1,$elem2,$elem3));
		}
	}
	
	function Save(){
    MultiSelectWindow :: Save();  
	}
	
	function save_object(){
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow :: save_object();   
	}
}

?>
