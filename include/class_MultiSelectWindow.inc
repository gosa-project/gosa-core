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

	var $config					= "";

	var $is_closed				= false;
	var $is_saved				= false;

	function ClearElementsList()
	{
		$this->array_Elements = array();
	}

	function AddRegex($name,$string,$value,$conn)
	{
		$arr = array();
		$arr['name'] 		= $name;
		$arr['string']		= $string;
		$arr['value']		= $value;	
		$arr['connAlpha']	= $conn;		// Connect with alphabet select 
		$this->array_Regexes[] = $arr;
	}

	function MultiSelectWindow($config){
		$this->config = $config;
		$this->SaveButtonString		= _("Save");
		$this->CloseButtonString	= _("Close");
	}

	function SetTitle($str){
		$this->string_Title = $str;
	}

	function SetListHeader($str){
		$this->string_ListHeader = $str;
	}

	function SetSummary($str){
		$this->string_Summary = $str;
	}

	function SetSaveButtonString($str){
		$this->SaveButtonString = $str;
	}

	function SetCloseButtonString($str){
		$this->CloseButtonString = $str;
	}

	function SetInformation($str){
		$this->string_Information = $str;
	}

	function EnableAplhabet($bool){
		$this->bool_DisplayAlpahabet = $bool;
	}

	function AddHeader($arr){
		$this->array_Header[] = $arr;
	}

	function AddElement($arr){
		$this->array_Elements[] = $arr;
	}

	function AddCheckBox($name,$value,$string,$default){
		$arr = array();
		$arr['name'] 	= $name;
		$arr['string']	= $string;
		$arr['value']	= $value;
		$arr['default']	= $default;
		$this->array_Checkboxes[] = $arr;
	}

	function isClosed()
	{
		return($this->is_closed);
	}

	function EnableCloseButton($bool)
	{
		$this->bool_DisplayCloseButton = $bool;
	}

	function EnableSaveButton ($bool)
	{
		$this->bool_DisplaySaveButton = $bool;
	}

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
				$boxes .="<input type='checkbox' name='".$box['name']."' value='".$box['value']."' checked ".$boxClick.">&nbsp;".$box['string']."<br>";
			}else{
				$boxes .="<input type='checkbox' name='".$box['name']."' value='".$box['value']."'".$boxClick.";>&nbsp;".$box['string']."<br>";
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

		$smarty->assign("search_image", 	get_template_path('images/search.png'));
		$smarty->assign("searchu_image", 	get_template_path('images/search_user.png'));
		$smarty->assign("tree_image", 		get_template_path('images/tree.png'));
		$smarty->assign("infoimage", 		get_template_path('images/info_small.png'));
		$smarty->assign("launchimage", 		get_template_path('images/rocket.png'));
		$smarty->assign("apply", 			apply_filter());

		/* Button handling */
		$smarty->assign("SaveButtonString" ,$this->SaveButtonString);
		$smarty->assign("CloseButtonString",$this->CloseButtonString);
	
		$smarty->assign("Display_Close",	$this->bool_DisplayCloseButton);
		$smarty->assign("Display_Save" ,	$this->bool_DisplaySaveButton);

		$display = $smarty->fetch(get_template_path("MultiSelectWindow.tpl"));
		return($display);
	}

	function Close()
	{
		$this->is_closed = true;
	}

	function Save()
	{
		$this->is_saved = true;
	}

	function save_object()
	{
		if(isset($_POST['MultiSelectWindow'])){

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
			}

			/* Check regex posts */
			foreach($this->array_Regexes as $key => $box){
				$this->array_Regexes[$key]['value'] = $_POST[$box['name']];
				$this->$box['name'] = $_POST[$box['name']];
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
}

?>
