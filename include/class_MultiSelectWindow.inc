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

define ("SEPERATOR", 1002);

class MultiSelectWindow{

	var $bool_DisplayAlpahabet	= false;
	var $bool_DisplayCloseButton= true;	
	var $bool_DisplaySaveButton = true;	

	var $SaveButtonString		= "";
	var $CloseButtonString	= "";

	var $string_Title			  = "";
	var $string_ListHeader	= "";
	var $string_ListDropDown= "";
	var $string_Summary			= "";
	var $string_Information	= "";

	var $array_Header			  = array();
	var $array_Elements			= array();	
	var $array_Checkboxes		= array();	
	var $array_Regexes			= array();	

	var $config					    = array("");
	var $is_closed				  = false;
	var $is_saved				    = false;
	var $is_headpage			  = false;	// if true the design changes
	var $filterName				  = "Liste";
	var $DepartmentsAdded		= false;
	var $Added_Departments  = array();
	var $selectedBase       = "";

	var $departments= array();

  var $DivHeight          = "";

  var $HideFilterPart     = false;
  var $List_Bottom_Info   = "";
  var $SaveAdditionalVars = array();  // Additional Post vars to store 
  var $module= "";

  var $base_selection_regex = "*";

  var $IgnoreAccount = TRUE;
  var $footer = "";
  var $post_id    = "1 2 3";

	function ClearElementsList()
	{
    $ui =get_userinfo();
    $deps = $ui->get_module_departments($this->module);
    if(!in_array($this->selectedBase, $deps)){
      $this->selectedBase = array_shift($deps);
    }
		$this->array_Elements = array();
	}

  function HideFilterPart($bool = true)
  {
    $this->HideFilterPart = $bool;
  }

  function SetHeight($height)
  { 
    $this->DivHeight=$height;
  }

	/* Adds a regex input field to the current dialog */
	function AddRegex($name,$string,$value,$conn,$image="images/lists/search.png")
	{
		$arr = array();

		/* Check if the given input field name was already used 
		   for this type of dialog */	
    $MultiDialogFilters = session::get('MultiDialogFilters');
		if(isset($MultiDialogFilters[$this->filterName][$name])){
			$arr['value']	= $MultiDialogFilters[$this->filterName][$name];
			$this->$name = $arr['value'];
		}else{
			$arr['value']	= $value;	
		}

		$arr['name'] 		= $name;
		$arr['string']		= $string;
		$arr['image']		= $image;	
		$arr['connAlpha']	= $conn;		// Connect with alphabet select 
		$this->array_Regexes[] = $arr;
	}


  function SetDropDownHeaderMenu($str)
  {
    $this->string_ListDropDown = $str;
  }

  function GetDropDownHeaderMenu()
  {
    if(!empty($this->string_ListDropDown)){
      $mid = new LayersMenu(6, 7, 2, 1);
      $mid->setImgwww("./images/layer_menu/");
      $mid->setIcondir("./images/layer_menu/");
      $mid->setDirroot("../include/utils/layer-menu/");
      $mid->setHorizontalMenuTpl("../ihtml/".get_template_path("",FALSE).'GOsa_MultiSelectHeader.ihtml');
      $mid->setSubMenuTpl("../ihtml/".get_template_path("",FALSE).'GOsa_MultiSelectHeaderSubEntry.ihtml');
      $mid->setMenuStructureString($this->string_ListDropDown);
      $mid->parseStructureForMenu('menu');
      $mid->newHorizontalMenu('menu');
      $s = $mid->getHeader();
      $s.= $mid->getMenu('menu');
      $s.= $mid->getFooter('menu');
      return($s);
    }else{
      return("");
    }
  }


	/* Contrucktion */
	function __construct(&$config, $filterName, $module)
	{
		$this->config = &$config;
		$this->module = $module;
		$this->SaveButtonString		= _("Save");
		$this->CloseButtonString	= _("Close");
		$this->filterName			= $filterName;
    $this->ui = get_userinfo();
    $this->post_id = preg_replace("/[^0-9]/","",microtime(TRUE));

    /* Check default values for SaveAdditionalVars */
    $MultiDialogFilters = session::get('MultiDialogFilters');
    foreach($this->SaveAdditionalVars as $name){
      if(isset($MultiDialogFilters[$this->filterName][$name])){
        $this->$name = $MultiDialogFilters[$this->filterName][$name];
      }
    }
    
	}

	/* Enables the headpage mode, which changes the list look */
	function SetHeadpageMode()
	{
		$this->is_headpage = true;
	}

	/* Sets the List internal name (not displayed anywhere) 
	   it is used to identify every single list
	 */	
	function SetTitle($str)
	{
		$this->string_Title = $str;
	}

	/* Set the list header string  */
	function SetListHeader($str)
	{
		$this->string_ListHeader = $str;
	}

	/* This sets the list description which is the first gray bar on top of the list */
	function SetSummary($str)
	{
		$this->string_Summary = $str;
	}

	/* If the save button is enabled, you can change its caption with this function */	
	function SetSaveButtonString($str)
	{
		$this->SaveButtonString = $str;
	}

	/* If the close button is enabled, you can change its caption with this function */	
	function SetCloseButtonString($str)
	{
		$this->CloseButtonString = $str;
	}

	/* With this function you can change the text of the information box */
	function SetInformation($str)
	{
		$this->string_Information = $str;
	}

	/* Display the alphabet selection box*/
	function EnableAplhabet($bool)
	{
		$this->bool_DisplayAlpahabet = $bool;
	}

	/* Add additional header col */
	function AddHeader($arr)
	{
		$this->array_Header[] = $arr;
	}

	/* add additional List element */
	function AddElement($arr)
	{
		$this->array_Elements[] = $arr;
	}


  /* Return default header part. With back, home and root icons and department selection */
  function get_default_header($seperator= TRUE)
  {
    $enable_back = TRUE;
    $enable_root = TRUE;
    $enable_home = TRUE;

    $ui = get_userinfo();

    /* Check if selectedBase = first available base */
    $deps = $ui->get_module_departments($this->module);

    if(!count($deps) || $deps[0] == $this->selectedBase){
      $enable_back = FALSE;
      $enable_root = FALSE;
    }
  
    $listhead ="";
 
    /* Check if we are in users home  department */ 
    if(!count($deps) ||$this->selectedBase == get_base_from_people($ui->dn)){
      $enable_home = FALSE;
    }

    /* Draw root button */
    if($enable_root){
      $listhead .= " <input class='center' type='image' src='images/lists/root.png' align='middle'
        title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;";
    }else{
      $listhead .= " <img src='images/lists/root_grey.png' class='center' alt='"._("Root")."'>&nbsp;";
    }

    /* Draw back button */
    if($enable_back){
      $listhead .= " <input class='center' type='image' align='middle' src='images/lists/back.png'
        title='"._("Go up one department")."' alt='"._("Up")."'                name='dep_back'>&nbsp;";
    }else{
      $listhead .= " <img src='images/lists/back_grey.png' class='center' alt='"._("Up")."'>&nbsp;";
    }

    /* Draw home button */
    if($enable_home){
      $listhead .= " <input class='center' type='image' align='middle' src='images/lists/home.png'
        title='"._("Go to users department")."' alt='"._("Home")."'            name='dep_home'>&nbsp;";
    }else{
      $listhead .= " <img src='images/lists/home_grey.png' class='center' alt='"._("Home")."'>&nbsp;";
    }
   
    /* And at least draw reload button, this button is enabled everytime */ 
    $listhead .=  " <input class='center' type='image' src='images/lists/reload.png' align='middle'
      title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";
    if ($seperator){
      $listhead.= " <img   class='center' src='images/lists/seperator.png' align='middle' alt='-' height='16' width='1'>&nbsp;";
    }
    return ($listhead);
  }

	/* Add a checkbox to the filter element,
	   the name specifies an existing class var to store the 'selection' */
	function AddCheckBox($name,$value="Unset",$string="Unset",$default=false)
	{
		$arr = array();

		if($name == SEPERATOR){
			$arr['name'] = SEPERATOR;
		}else{
			/* Check if there was already a variable 
			   for this dialog which we should use instead of the default*/
      $MultiDialogFilters = session::get('MultiDialogFilters');
			if(isset($MultiDialogFilters[$this->filterName][$name])){
				$arr['default']	= $MultiDialogFilters[$this->filterName][$name];
				$this->$name = $arr['default'];
			}else{
				$arr['default']	= $default; 
        $this->$name = $default;
			}
			$arr['name'] 	      = $name;
			$arr['string']	    = $string;
			$arr['value']	      = $value;
			$arr['enabled']	    = true;

		}
		$this->array_Checkboxes[] = $arr;
	}


	/* Hides or unhides the checkbox with the given name */
	function DisableCheckBox($name,$HideShow = false)
	{
		foreach($this->array_Checkboxes as $key => $chkbox){
			if($chkbox['name'] == $name){
				$this->array_Checkboxes[$key]['enabled'] = $HideShow;
			}
		}
	}


	/* Returns true if the close button was pressed */
	function isClosed()
	{
		return($this->is_closed);
	}

  
  function SetListFooter($str){
    $this->footer = $str;
  }


	/* Enable the close button */
	function EnableCloseButton($bool)
	{
		$this->bool_DisplayCloseButton = $bool;
	}

	/* Enable the save button on the bottom of the list*/
	function EnableSaveButton ($bool)
	{
		$this->bool_DisplaySaveButton = $bool;
	}

  /* Add a list specific filter object to position 
      1 on top of Information 
      2 Between Information && Filter
      3 Below the Filter Part */
  function AddUserBoxToFilter($position)
  {
    return("");
  }

  function EnableJSLeaveMsg($ignore = TRUE)
  {
    $this->IgnoreAccount = !$ignore;
  }

	/* Draw the list with all list elements and filters */
	function Draw()
	{

		/* Check for exeeded sizelimit */
		if (($message= check_sizelimit()) != ""){
			return($message);
		}

		$smarty = get_smarty();
		$smarty->assign("UserBox1",$this->AddUserBoxToFilter(1));
		$smarty->assign("UserBox2",$this->AddUserBoxToFilter(2));
		$smarty->assign("UserBox3",$this->AddUserBoxToFilter(3));

		$divlist = new divlist($this->string_Title);
		$divlist->SetSummary($this->string_Summary);
		$divlist->SetEntriesPerPage(0); // 0 for scrollable list

    /* Display list footer with summary of all listed entries */
    if ($this->config->get_cfg_value("listSummary") == "true"){
      $divlist->SetFooter($this->get_List_Bottom_Info());
    }
  
    if($this->DivHeight != ""){
      $divlist->SetHeight($this->DivHeight);
    }

		/* set Header informations 
		 */
		$header = array();
		foreach($this->array_Header as $head){
			$header[] = $head;
		}
		$divlist->SetHeader($header);

		/* set Entries 
		 */
		$elements = array();
		foreach($this->array_Elements as $element){
			$divlist->AddEntry($element);
		}

		/* Create checkboxes fields 
		 */
		$boxes = "";
		$boxClick = " onClick='document.mainform.submit();' ";
		foreach($this->array_Checkboxes as $box){

			if($box['name'] == SEPERATOR){
				$boxes .= "</td></tr></table><table style='width:100%;border-top:1px solid #B0B0B0;'><tr><td>";
				continue;
			}

			/* Skip disabled boxes */
			if(!$box['enabled']) continue;

			/* Check if box is checked */
			if($box['default'] == true){
				$boxes .="<input type='checkbox' name=\"".$box['name']."\" value='1' title=\"".htmlspecialchars($box['value'])."\" checked ".$boxClick.">&nbsp;".$box['string']."<br>";
			}else{
				$boxes .="<input type='checkbox' name=\"".$box['name']."\" value='1' title=\"".htmlspecialchars($box['value'])."\" ".$boxClick.">&nbsp;".$box['string']."<br>";
			}
		}
		$smarty->assign("CheckBoxes", $boxes);

		/* Assign regex fields 
		 */
		$regexes = "";
		foreach($this->array_Regexes as $regex){
			$regexes.="<table summary=\"\" style=\"width:100%;border-top:1px solid #B0B0B0;\">
				<tr>
				<td>
				<label for=\"".$regex['name']."\">
				<img alt=\"".$regex['string']."\" src=\"".$regex['image']."\" align=middle>
				</label>
				</td>
				<td width=\"99%\">
				<input id=\"".$regex['name']."\" type=\"text\" style='width:99%' name=\"".$regex['name']."\" maxlength='20'
				value=\"".htmlspecialchars($regex['value'])."\" title=\"".htmlspecialchars($regex['string'])."\"> 
				</td>
				</tr>
				</table>";
		}
		$smarty->assign("regexes"			, $regexes );

    /* Hide Filter Part if Requested or empty */
    if((empty($boxes)) && (empty($regexes)) || ($this->HideFilterPart)){
		  $smarty->assign("Skip_Filter_Part",	true);
    }else{ 
		  $smarty->assign("Skip_Filter_Part",	false);
    }

		/* Assign alphabet and display it 
		 */	
		$smarty->assign("Display_alphabet",	$this->bool_DisplayAlpahabet);
		$smarty->assign("alphabet", 		generate_alphabet());
		$smarty->assign("Header"			, $this->string_ListHeader);
		$smarty->assign("HeaderDropDown", $this->GetDropDownHeaderMenu());
		$smarty->assign("Summary"			, $this->string_Summary);
		$smarty->assign("Title"				, $this->string_Title);
		$smarty->assign("Information"		, $this->string_Information);
		$smarty->assign("IgnoreAccount"		, $this->IgnoreAccount);
    $smarty->assign("POST_ID", $this->post_id);

		/* Check for exeeded sizelimit */
		$smarty->assign("hint"				, print_sizelimit_warning());
		$smarty->assign("DivList"			, $divlist->DrawList()."\n\n\n\n\n".$this->footer."\n\n\n\n\n");

		if($this->is_headpage){
			$smarty->assign("tree_image", 		get_template_path('images/lists/search-subtree.png'));
			$smarty->assign("infoimage", 		get_template_path('images/info.png'));
			$smarty->assign("launchimage", 		get_template_path('images/launch.png'));
			$smarty->assign("apply", 			apply_filter());
		}else{
			$smarty->assign("tree_image", 		get_template_path('images/lists/search-subtree.png'));
			$smarty->assign("infoimage", 		get_template_path('images/info_small.png'));
			$smarty->assign("launchimage", 		get_template_path('images/rocket.png'));
			$smarty->assign("apply", 			apply_filter());
		}

		/* Button handling */
		$smarty->assign("SaveButtonString" ,$this->SaveButtonString);
		$smarty->assign("CloseButtonString",$this->CloseButtonString);

		$smarty->assign("Display_Close",	$this->bool_DisplayCloseButton);
		$smarty->assign("Display_Save" ,	$this->bool_DisplaySaveButton);

		$smarty->assign("filterName"	,	$this->filterName);
		$smarty->assign("is_headpage"	,	$this->is_headpage);

		$display = $smarty->fetch(get_template_path("MultiSelectWindow.tpl"));
		return($display);
	}

	/* Set the close var, which simulates the close button is pressed */
	function Close()
	{
		$this->is_closed = true;
	}

	function Save()
	{
		$this->is_saved = true;
	}

	/* Store all checkboxes/ regexes ... 
	   Store data also into a session var, to keep the checkboxes check after reload  */
	function save_object()
	{

    /* Ensure that we do not handle posts for other dialogs 
     */
    if((isset($_POST['POST_ID']) && $_POST['POST_ID'] != $this->post_id) || 
       (isset($_GET['post_id']) && $_GET['post_id'] != $this->post_id)){
      return;
    }

    /* Get up to date config */
    if(isset($this->parent->config)){
      $this->config = $this->parent->config;
    }

    /* Update current base */
    $s_action ="";
    foreach($_POST as $key => $value){
      if(preg_match("/^dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/^dep_root.*/",$key)){
        $s_action="root";
      }elseif(preg_match("/^dep_home.*/i",$key)){
        $s_action="home";
      }
    }

    /* Save base selection from headpage selectbox */
    if(isset($_POST['CurrentMainBase'])){
      $this->selectedBase = get_post('CurrentMainBase');
      session::global_set("CurrentMainBase", $this->selectedBase);
    } else {
      $this->selectedBase= session::global_get("CurrentMainBase");
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $ui= get_userinfo();
      $base = get_base_from_people($ui->dn);
      $this->selectedBase= $base;
    }

    /* Open selected department
       this is posted by the parent class MultiSelectWindow */
    if(isset($_GET['act'])&& ($_GET['act'] == "dep_open")){
      $s_entry = $_GET['dep_id'];
      if (!isset($this->departments[$s_entry])){

        msg_dialog::display(_("LDAP error"), sprintf(_("Inconsistent DN encoding detected: '%s'"), LDAP::fix($s_entry)), ERROR_DIALOG);
      } else {
        $this->selectedBase = $this->departments[$s_entry]['dn'];
      }
    }

    /* back to the roots ^^ */
    if($s_action=="root"){
      $this->selectedBase=($this->config->current['BASE']);
      $dep_id = $this->ui->get_module_departments($this->module);
      if(isset($dep_id[key($dep_id)])){
        $this->selectedBase = $dep_id[key($dep_id)];
      }
    }

    /* If Back-button is pressed, move back one step in DN */
    if($s_action=="back"){


      /* Get parent deprtment and check if we are allowed to step in it */
      $base_back= preg_replace("/^[^,]+,/", "", $this->selectedBase);
      $dep_id = $this->ui->get_module_departments($this->module);
      if(in_array_ics($base_back,$dep_id)){
        if(in_array($base_back,$this->config->departments)){
          $this->selectedBase = $base_back;
        }
      }
    }

		if(isset($_POST['MultiSelectWindow'.$this->filterName])){

      /* Save some additional vars */
      $MultiDialogFilters = session::get('MultiDialogFilters');
      foreach($this->SaveAdditionalVars as $name){
        if(isset($_POST[$name])){
          if(isset($this->$name)){
            $this->$name = $_POST[$name];
            $MultiDialogFilters[$this->filterName][$name] = $_POST[$name];
          }
        }
      }
      session::set('MultiDialogFilters',$MultiDialogFilters);

			/* Check posts from checkboxes 
			 */
			foreach($this->array_Checkboxes as $key => $box){
				if(isset($_POST[$box['name']])){
					$this->array_Checkboxes[$key]['default'] = true;
					$this->$box['name'] = true;
				}else{
					$this->array_Checkboxes[$key]['default'] = false;
					$this->$box['name'] = false;
				}

				/* Save settings in out session */
        $MultiDialogFilters = session::get('MultiDialogFilters');
				$MultiDialogFilters[$this->filterName][$box['name']] = $this->$box['name'];
        session::set('MultiDialogFilters',$MultiDialogFilters);
			}

			/* Check regex posts */
			foreach($this->array_Regexes as $key => $box){
				$this->array_Regexes[$key]['value'] = $_POST[$box['name']];
				$this->$box['name'] = $_POST[$box['name']];
        $MultiDialogFilters = session::get('MultiDialogFilters');
				$MultiDialogFilters[$this->filterName][$box['name']] = $this->$box['name'];
        session::set('MultiDialogFilters',$MultiDialogFilters);
			}

			/* call close/save if buttons are pressed */
			if(isset($_POST['CloseMultiSelectWindow'])){
				$this->Close();
			}

			if(isset($_POST['SaveMultiSelectWindow'])){
				$this->Save();
			}
		}

		/* check for alphabet selection
		   Check which regexes are connected to the alphabet 
		 */	
		if(isset($_GET['search'])){
			foreach($this->array_Regexes as $key => $box){
				/* check if this regex is connected to the alphabet selection */
				if(($box['connAlpha'])&&(isset($_GET['search']))){
					$val =  $_GET['search']."*";
					$val = preg_replace("/\*\**/","*",$val);
					$this->array_Regexes[$key]['value'] = $val;
					$this->$box['name'] = $val;
          $MultiDialogFilters = session::get('MultiDialogFilters');
          $MultiDialogFilters[$this->filterName][$box['name']] =  $val;
          session::set('MultiDialogFilters',$MultiDialogFilters);
				}
			}
		}

    /* Save currenlty selected base in session */
    session::global_set("CurrentMainBase",$this->selectedBase);
	}


	/* this function adds the sub-departments of the current tree to the list */
	function AddDepartments($base = false,$numtabs = 3,$empty_tabs_in_front = 0)
	{
		$this->DepartmentsAdded = true;
    $this->Added_Departments = array();
    $this->departments = array();

    if(isset($this->Regex)){
      $this->base_selection_regex = $this->Regex;
    }

		$linkopen = "<a href='?plug=".$_GET['plug']."&amp;post_id=".$this->post_id."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $types = departmentManagement::get_support_departments();
    $ui = get_userinfo();
    $module_deps = $ui->get_module_departments($this->module);
    $found_deps = array();

		/* check for a valid base */
		if(!$base){
      $base = $this->selectedBase;
		}

		/* Create ldap obj and switch into base*/
		$ldap = $this->config->get_ldap_link();
		$ldap->cd($base);

		/* Get all departments within this subtree */
    $s_filter = "";
    $s_attrs  = array("description","objectClass");
    $s_acls   = $this->module;
    foreach($types as $name => $data){
      $s_filter.= "(&(objectClass=gosaDepartment)(objectClass=".$data['OC'].")(".$data['ATTR']."=".$this->base_selection_regex."))";
      $s_attrs[]= $data['ATTR']; 
    }

    $tmp = get_list("(|".$s_filter.")",$s_acls,$this->selectedBase,$s_attrs,GL_NONE);
    foreach($tmp as $attrs){
      foreach($types as $name => $data){
        if(in_array($data['OC'],$attrs['objectClass']) && isset($attrs[$data['ATTR']][0])){
          $attrs['NAME'] = $attrs[$data['ATTR']][0];
          $attrs['TYPE'] = $data;
          $name = $attrs['NAME']." ".$attrs['dn'];
          $departments[$name] = $attrs;
          $found_deps[$name] = $attrs['dn'];
          break;
        }
      }
    }

    /* Detect allowed departments 
     */
    $deps = array_intersect($found_deps,$module_deps);
    uksort($deps, 'strnatcasecmp');
    $key = 0;
    foreach($deps as $name => $dn){
      $key ++;
      $val = $departments[$name];
      $this->departments[$key] = $val;
      $img = "<image src='".$val['TYPE']['IMG']."' class='center'>";

			/* Add spacer cols to divlist 
       */
			$row = array();
      if($empty_tabs_in_front){
        for($i = 0; $i < $empty_tabs_in_front ; $i ++){
          $row[] = array("string"=>"&nbsp;", "attach" => "style='text-align:center;width:20px;'");
        }
      }

      /* Create entry name 
       */
      $name = $val['NAME'];
      if(isset($val['description'])){
        $name .=  " - [".$val["description"][0]."]";
      }

      /* Add departments
       */
			$row[]=array("string"=> $img,"attach"=>"style='text-align:center;width:20px;'");
			$row[]=array("string"=>sprintf($linkopen,$key,$name), "attach" => "style=''");

      /* Add spacer tabs 
       */
			if($numtabs > 2){	
				for($i = 2 ; $i < $numtabs;$i++){
          if(isset( $this->array_Header[$i + $empty_tabs_in_front]['attach'])){
            $row[] = array("string"=>"&nbsp;","attach" => $this->array_Header[$i + $empty_tabs_in_front]['attach']);
          }else{
            $row[] = array("string"=>"&nbsp;");
          }
				}
			}
			$this->AddElement($row);
      $this->Added_Departments[] = $row;
		}
	}


  function create_department_list($modules)
  { 
    $departments = array();
    $ui= get_userinfo();
    $first = "";
    $found = FALSE;
    $options ="";

    $ids = $this->config->idepartments;
    $d   = $ui->get_module_departments($modules);
    $k_ids = array_keys($ids);
    $deps = array_intersect($d,$k_ids);
    foreach($k_ids as $department){
      $departments[$department] = $ids[$department];
    }

    foreach($departments as $value => $name){

      /* Keep first base dn in mind, we could need this
       *  info if no valid base was found
       */
      if(empty($first)) {
        $first = $value;
      }


      $v_value = htmlentities($value,ENT_QUOTES);
      if ($this->selectedBase == $value){
        $found = TRUE;
        $options.= "<option selected='selected' value=\"".$v_value."\">$name</option>";
      } else {
        $options.= "<option value=\"".$v_value."\">$name</option>";
      }
    }

    /* The currently used base is not visible with your acl setup.
     * Set base to first useable base.
     */
    if(!$found){
      $this->selectedBase = $first;
    }

    return($options);
  }

  function set_List_Bottom_Info($str)
  {
    $this->List_Bottom_Info = $str;
  }

  function get_List_Bottom_Info()
  {
    return($this->List_Bottom_Info); 
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
