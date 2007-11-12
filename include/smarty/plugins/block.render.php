<?php

function smarty_block_render($params, $text, &$smarty)
{
	/* Skip closing tag </render> */	
	if(empty($text)) {
		return("");
	}

	/* Get acl parameter */
	if (isset($params['acl'])) {
		$acl = $params['acl'];
		unset($params['acl']);
	}

	if (isset($_SESSION['DEBUGLEVEL']) && $_SESSION['DEBUGLEVEL'] & DEBUG_ACL ){
		echo "<font color='blue' size='2'>&nbsp;".$acl."</font>";
	}

	/* Read / Write*/
	if(preg_match("/w/i",$acl)){
		return ($text);
	}

	$text = preg_replace ("/\n/","GOSA_LINE_BREAK",$text);

	/* Disable objects, but keep those active that have mode=read_active */
	if(!(isset($params['mode']) && $params['mode']=='read_active')){

		/* Disable options && greyout divlists */
		$from 	= array("/class=['\"]list1nohighlight['\"]/i",
				"/class=['\"]list0['\"]/i",
				"/class=['\"]list1['\"]/i");
		$to 	= array("class='list1nohighlightdisabled'",
				"class='list1nohighlightdisabled'",
				"class='list1nohighlightdisabled'");
				
		if(!preg_match("/ disabled /",$text)){
			$from [] = "/name=/i" ;
			$to   [] = "disabled name=";
		}

		$text 	= preg_replace($from,$to,$text);

		/* Replace picture if object is disabled */
		if(isset($params['disable_picture'])){
			$syn = "/src=['\"][^\"']*['\"]/i";
			$new = "src=\"".$params['disable_picture']."\"";
			$text = preg_replace($syn,$new,$text);
		}
	}		

	/* Read only */
	if(preg_match("/r/i",$acl)){
		return(preg_replace("/GOSA_LINE_BREAK/","\n",$text));	
	}

	/* No acls */	
	if(preg_match("/type['\"= ].*submit/",$text)){
		$text = preg_replace("/submit/","button",$text);
	}else{
		$text = preg_replace("/value=['\"][^\"']*['\"]/","",$text);
	}

	/* Remove select options */
	$from 	= array("#<option.*<\/option>#i",
			"/(<textarea.*>).*(<\/textarea>)/i",
			"/^(.*<input.*)checked(.*>.*)$/i");

	$to 	= array(" ",
			"\\1\\2",
			"\\1 \\2");
	$text 	= preg_replace($from,$to,$text);
	$text = preg_replace("/GOSA_LINE_BREAK/","\n",$text);

	return $text;
}

?>
