<?php
/*
 * This code is part of GOsa (https://gosa.gonicus.de)
 * Copyright (C) 2003  Cajus Pollmeier
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

class xmlParse  {

	/* XML parser */
	var $parser;

	/* Selected connection */
	var $current	= array();
	var $currentPos = 0;
	var $depth		= array();
	var $data		= array();

	function xmlParse()
	{
		$this->parser = xml_parser_create();
		xml_set_object($this->parser, $this);
		xml_set_element_handler($this->parser, "tag_open", "tag_close");
	}

	function parseMenu($file)
	{
		if (!($fp = @fopen($file, "r"))) {
			print_red(sprintf(_("Could not open XML input '%s'."),$file));
			$this->data = array();
			return(false);
		}

		$data = fread($fp, filesize($file));

		$t = split("\n",$data);
		$s = preg_replace("/<!DOCTYPE.*>/","<Input>",$data."</Input>");
		$data = $t[0]."\n".$s;	
		
		fclose($fp);
		xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, 1);
		xml_parser_set_option($this->parser, XML_OPTION_SKIP_WHITE, 0);
		xml_parse_into_struct($this->parser, $data, $vals, $index);

		$params = array();
		$level = array();
		
		$this->data = array();	
		$this->depth = array();	
		$i = 0 ; 
		$current ="";
		$dir = "";
		foreach($vals as $ele){
			if($ele['tag'] =="INPUT") continue;
			if($ele['tag'] =="INCLUDE") continue;

			if(!in_array($ele['tag'],array("MENU","CATEGORY","DIRECTORY"))) continue;

			if($ele['type'] == "open"){
				$tag = $ele['tag'].$i;
		
				if($ele['tag'] != "MENU"){
					$i++;
					$this->currentPos ++ ;
					$this->depth[$this->currentPos] = $tag;
				}
				$current = &$this->data;
				foreach($this->depth as $name){
					$current = &$current[$name];	
				}
			}elseif($ele['type']=="close"){
				unset($this->depth[($this->currentPos)]);
				$this->currentPos --;
				if($ele['tag'] == "MENU")
				$dir ="";
			}
			if(($ele['tag'] == "CATEGORY")&&(!empty($ele['value']))&&(isset($ele['value']))){
				$current[preg_replace("/\/$/","",$dir)]['name'] = $ele['value'];
			}
			if(isset($current['type'])){
				unset($current['type']);
			}
			if($ele['tag'] == "DIRECTORY"){
				$dir .= preg_replace("/\.directory/","",$ele['value'])."/";
			}
			if(($ele['tag'] == "CATEGORY")&&(!empty($ele['value']))&&(isset($ele['value']))){
				$current[preg_replace("/\/$/","",$dir)] = $ele['value'];
			}
			$curback = $current;	
		}
		
	}


	function parse($filename)
	{
		if(!file_exists($filename)){
			return(false);
		}else{ 
			$fh= fopen($filename, "r"); 
			$xmldata= fread($fh, 100000);
			fclose($fh); 
			if(!xml_parse($this->parser, chop($xmldata))){
				print(sprintf(_("XML error in %s : %s at line %d"),$filename,
							xml_error_string(xml_get_error_code($this->parser)),
							xml_get_current_line_number($this->parser)));
			}
		}
		return(true);
	}

	function tag_open($parser, $tag, $attrs)
	{
		$this->currentPos ++ ;
		$this->depth[$this->currentPos] = $tag;

		$current = &$this->data;
		foreach($this->depth as $name){
			$current = &$current[$name];	
		}
		$current = $attrs;
	}

	function tag_close($parser, $tag)
	{
		unset($this->depth[($this->currentPos)]);
		$this->currentPos --; 
	}

	function GetData()
	{
		return ($this->data);
	}

}
?>
