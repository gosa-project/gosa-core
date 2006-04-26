<?php

class MultiSelectWindow{

	var $bool_DisplayAlpahabet	= false;
	var $bool_DisplayCloseButton= true;	
	var $bool_DisplaySaveButton = true;	

	var $SaveButtonString		= "";
	var $CloseButtonString		= "";

	var $string_Title			= "";
	var $string_ListHeader		= "";
	var $string_Summary			= "";
	var $string_Information		= "";

	var $array_Header			= array();
	var $array_Elements			= array();	
	var $array_Checkboxes		= array();	
	var $array_Regexes			= array();	

	var $config					= array("");

	var $is_closed				= false;
	var $is_saved				= false;

	var $is_headpage			= false;	// if true the design changes

	var $filterName				= "Liste";

	function ClearElementsList()
	{
		$this->array_Elements = array();
	}

	/* Adds a regex input field to the current dialog */
	function AddRegex($name,$string,$value,$conn,$image="images/search.png")
	{
		$arr = array();
	
		/* Check if the given input field name was already used 
            for this type of dialog */	
		if(isset($_SESSION['MultiDialogFilters'][$this->filterName][$name])){
			$arr['value']	= $_SESSION['MultiDialogFilters'][$this->filterName][$name];
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

	/* Contrucktion */
	function MultiSelectWindow($config,$filterName)
	{
		$this->config = $config;
		$this->SaveButtonString		= _("Save");
		$this->CloseButtonString	= _("Close");
		$this->filterName			= $filterName;
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

	/* Add a checkbox to the filter element,
    	the name specifies an existing class var to store the 'selection' */
	function AddCheckBox($name,$value,$string,$default)
	{
		$arr = array();
	
		/* Check if there was already a variable 
			for this dialog which we should use instead of the default*/
		if(isset($_SESSION['MultiDialogFilters'][$this->filterName][$name])){
			$arr['default']	= $_SESSION['MultiDialogFilters'][$this->filterName][$name];
			$this->$name = $arr['default'];
		}else{
			$arr['default']	= $default;
		}

		$arr['name'] 	= $name;
		$arr['string']	= $string;
		$arr['value']	= $value;
		$this->array_Checkboxes[] = $arr;
	}

	/* Returns true if the close button was pressed */
	function isClosed()
	{
		return($this->is_closed);
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

	/* Draw the list with all list elements and filters */
	function Draw()
	{
		$smarty = get_smarty();

		$divlist = new divlist($this->string_Title);
		$divlist->SetSummary($this->string_Summary);
		$divlist->SetEntriesPerPage(0); // 0 for scrollable list 

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
			if($box['default'] == true){
				$boxes .="<input type='checkbox' name='".$box['name']."' value='1' title='".$box['value']."' checked ".$boxClick.">&nbsp;".$box['string']."<br>";
			}else{
				$boxes .="<input type='checkbox' name='".$box['name']."' value='1' title='".$box['value']."'".$boxClick.";>&nbsp;".$box['string']."<br>";
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
				<img alt=\"".$regex['string']."\" src=\"images/search.png\" align=middle>
				</label>
				</td>
				<td width=\"99%\">
				<input type='text' style='width:99%' name='".$regex['name']."' maxlength='20'
				value='".$regex['value']."' title=\"".$regex['string']."\"> 
				</td>
				</tr>
				</table>";
		}
		$smarty->assign("regexes"			, $regexes );
		
		/* Assign alphabet and display it 
         */	
		$smarty->assign("Display_alphabet",	$this->bool_DisplayAlpahabet);
		$smarty->assign("alphabet", 		generate_alphabet());

		$smarty->assign("Header"			, $this->string_ListHeader );
		$smarty->assign("Summary"			, $this->string_Summary);
		$smarty->assign("Title"				, $this->string_Title);
		$smarty->assign("Information"		, $this->string_Information);

		$smarty->assign("DivList"			, $divlist->DrawList());


		if($this->is_headpage){
			$smarty->assign("tree_image", 		get_template_path('images/tree.png'));
			$smarty->assign("infoimage", 		get_template_path('images/info.png'));
			$smarty->assign("launchimage", 		get_template_path('images/launch.png'));
			$smarty->assign("apply", 			apply_filter());
		}else{
			$smarty->assign("tree_image", 		get_template_path('images/tree.png'));
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
		if(isset($_POST['MultiSelectWindow'.$this->filterName])){

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
				$_SESSION['MultiDialogFilters'][$this->filterName][$box['name']] = $this->$box['name'];
			}

			/* Check regex posts */
			foreach($this->array_Regexes as $key => $box){
				$this->array_Regexes[$key]['value'] = $_POST[$box['name']];
				$this->$box['name'] = $_POST[$box['name']];
				$_SESSION['MultiDialogFilters'][$this->filterName][$box['name']] = $this->$box['name'];
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
				}
			}
		}

	}

	/* this function adds the sub-departments of the current tree to the list */
	function AddDepartments($base = false)
	{
		
		#FIXME the num of header cols must match the num of entry cols .	
	
		/* check for a valid base */
		if(!$base){
			if(!isset($_SESSION['CurrentMainBase'])){
				$_SESSION['CurrentMainBase'] = $this->config->current['BASE'];
			}
			$base = $_SESSION['CurrentMainBase'];
		}

		/* Create ldap obj and switch into base*/
		$ldap = $this->config->get_ldap_link();
		$ldap->cd($base);

		/* reset current deps */
		$this->departments = array();

		/* Get all departments within this subtree */
		$deps= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->ui->subtreeACL,
				$base, array("ou", "description"), GL_SIZELIMIT | GL_CONVERT);

		/* Edit delete link for system types
		 */
		$linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

		/* Create an array with all visible (in the list) departments */
		$departments = array();
		foreach($deps as $value){
			if(isset($value['description'][0])){
				$this->departments[$value['dn']]= get_sub_department($value['dn'])." - [".$value["description"][0]."]";
			}else{
				$this->departments[$value['dn']]= get_sub_department($value['dn']);
			}
		}
		natcasesort($this->departments);

		/* Add deps to this dialog object list */
		foreach($this->departments as $key=> $val){
			/* Add missing entries ... */
			if(!isset($this->config->departments[trim($key)])){
				$this->config->departments[trim($key)]="";
			}

			/* check if this department contains sub-departments
			   Display different image in this case
			 */
			$non_empty="";
			$nkey= normalizePreg($key);
			foreach($this->config->departments as $keyd=>$vald ){
				if(preg_match("/$nkey\/.*/",$keyd)){
					$non_empty="full";
				}
			}

			/* Add to divlist */
			$field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
			$field2 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style=''");
			$field3 = array("string" => "&nbsp;", "attach" => "style='width:60px;border-right:0px;text-align:right;'");
			$this->AddElement(array($field1,$field2,$field3));
		}
	}
}

?>
