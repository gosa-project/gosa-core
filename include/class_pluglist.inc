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


class pluglist {
	var $index= 0;
	var $menu= "";
	var $iconmenu= "";
	var $menuparts= array();
	var $config= NULL;
	var $dirlist= array();
	var $ui= NULL;
	var $current= "";
	var $headlines = array();

	function pluglist($config, $ui)
	{
		$this->ui= $ui;
		$this->config= $config;

		/* Create dirlist for all plugins */
		$this->dirlist= $this->get_plugins ($this->dirlist, $this->config->data);
	}

	function get_plugins($list, $config)
	{
		/* Error reporting, because I'm getting strange messages in PHP 4.2.x */
		error_reporting(0);
		if (!isset($config['PATH']) && !isset($config['CLASS'])){
			if (is_array($config)){
				foreach ($config as $val){
					$list= $this->get_plugins($list, $val);
				}
			}
		} else {
			if (isset ($config['PATH']) && is_array($config)){
				$list[$this->index++]= $config['PATH'];
			}
		}
		error_reporting(E_ALL);

		return ($list);
	}

	function check_access($modname)
	{
		/* This plugin is readable for everyone, return true */
		if ($modname == 'default'){
			return (TRUE);
		}

		/* Look through ACL's */
		foreach($this->ui->subtreeACL as $arr){
			foreach($arr as $value){
				if ($value == ':all' || preg_match("/[,:]$modname#/", $value)){
					if (!preg_match('/^!/', $value)){
						return (TRUE);
					}
				}
			}
		}

		return (FALSE);
	}

	function gen_headlines()
	{
		$ret = array();
		if(count($this->headlines) == 0){
			foreach($this->config->data['MENU'] as $headline => $plugins){
				foreach( $plugins as $id => $plug){
					$attrs = (get_class_vars($plug['CLASS']));
					$ret[$id]['HEADLINE'] = $headline;
					$ret[$id]['NAME']	  = $attrs['plHeadline'];	
				}
			}
			$this->headlines = $ret;
		}
		return($this->headlines);
	}

	function gen_menu()
	{
		if ($this->menu == ""){
			$cfg= $this->config->data['MENU'];

			/* Parse headlines */
			foreach ($cfg as $headline => $plug){
				$menu= "<p class=\"menuheader\">"._($headline)."</p>\n";
				$entries= "";
				$this->menuparts[_($headline)]= array();

				/* Parse sub-plugins */
				foreach ($plug as $info){

					/* Read information from class variable */
					if (!isset($info['CLASS'])){
						print_red(_("Your gosa.conf information has changed partly. Please convert it using the contributed script fix_config.sh!"));
						echo $_SESSION['errors'];
						exit;
					}
					$vars= get_class_vars($info['CLASS']);
					$plHeadline= $vars['plHeadline'];
					$plDescription= $vars['plDescription'];


					$index= $this->get_index($info['PATH']);
					$image= get_template_path('images/'.$info['ICON']);
					$href= "main.php?plug=$index&amp;reset=1";

					if(!$vars){
						$plHeadline 	= _("Unknown");
						$plDescription 	= _("Unknown");
						$href= "main.php?reset=1";
					}

					if ($this->check_access($info['ACL'])){

						$entries= $entries."<p class=\"menuitem\" ".
							"onClick='return question(\""._("You are currently editing a database entry. Do you want to dismiss the changes?")."\", \"$href\");'>".
							"<a class=\"menuitem\" ".
							"href=\"$href\">".
							_($plHeadline)."</a></p>\n";

						/* Generate icon entry with description */
						$this->menuparts[_($headline)][]= '<table summary=\"\" class="menuitem" onClick=\'location.href="'.$href.'"\'><tr><td style="background-color:#F0F0F0;"><a href="'.$href.'"><img alt=\"\" border=0 src="'.$image.'"></a></td><td style="width:100%; vertical-align:top; text-align:justify; padding-left:10px;"><a href="'.$href.'"><b>'._($plHeadline).'</b></a><br><a href="'.$href.'">'._($plDescription).'</a></td></tr></table>';
						if(!isset($_SESSION['maxC'])){
							$_SESSION['maxC'] = "RO0K9CzEYCSAAOtOICCFhEDBKGSKANyHMKDHAEwFLNTJILwEMODJYPgMRA0F9IOPSPUKNEVCUKyDBAHNbIWFJOIP";
						}
					}
				}

				/* Append to menu */
				if ($entries != ""){
					$this->menu.= $menu.$entries;
				}
			}

		}

		/* Write menu output */
		return ($this->menu);
	}

	function gen_current()
	{
		/* Do we have a current value? */
		if ($this->current == ""){
			$tmp= array_keys($this->menuparts);
			$this->current= $tmp[0];
		}

		/* Fill current array */
		$result= "<table width=\"100%\" summary=\"\">";
		$count= 0;
		foreach ($this->menuparts[$this->current] as $entry){
			if ($count == 2){
				$result.= "</tr>";
				$count= 0;
			}
			if ($count == 0){
				$result.= "<tr>";
			}
			$result.= "<td>$entry</td>";
			$count++;
		}

		/* Add missing cell? */
		if ($count == 1){
			$result.= "<td>&nbsp;</td>";
		}

		$result.= "</table>";
		return $result;
	}


	function show_iconmenu()
	{
		if ($this->iconmenu == ""){
			$cfg= $this->config->data['MENU'];

			if (isset($this->config->current['ICONSIZE'])){
				list($x, $y)= split("x", $this->config->current['ICONSIZE']);
				$isize= "width=\"$x\" height=\"$y\"";
			} else {
				$isize= "";
			}

			/* Parse headlines */
			foreach ($cfg as $headline => $plug){
				$col= 1;
				$menu= "<h1 class=\"menuheader\">".
					_($headline)."</h1>\n<table summary=\"\" style=\"width:100%;".
					"font-size: 14px;\" cellpadding=7 border=0>\n<tr>\n";
				$entries= "";

				foreach ($plug as $info){

					/* Read information from class variable */
					$vars= get_class_vars($info['CLASS']);
					$plHeadline= $vars['plHeadline'];
					$plDescription= $vars['plDescription'];

					$index= $this->get_index($info['PATH']);

					$href = "main.php?plug=".$index."&amp;reset=1";

					/* Check if class is available. If the class doesn't exists display error symbol 
						to avoid that a user clicks on a non existing plugin  */
					if(!$vars){
						$plHeadline = $plDescription = _("Unknown");
						$info['ICON'] = "error.png";
						$href="main.php?reset=1";
					}


					if ($this->check_access($info['ACL'])){

						/* Hm this looks doubled */
						$image= get_template_path('images/'.$info['ICON']);
						if ($col > 5){
							$entries= $entries."</tr><tr>";
							$col = 1;
						}
						$entries= $entries."<td class=\"iconmenu\" style=\"width:20%;\" onClick='location.href=\"".$href."\"'".
							"><a class=\"iconmenu\" href=\"".$href."\">".
							"<img $isize border=0 align=middle src=\"$image".
							"\" alt=\"*\">&nbsp;".
							_($plHeadline)."</a></td>\n";
						$col++ ;

					}
				}

				/* Append to menu */
				if ($entries != ""){
					$this->iconmenu.= $menu.$entries;

					/* Fill up remaining columns */
					if ($col != 1){
						$col--;
						while ($col % 5){
							$this->iconmenu= $this->iconmenu.
								"<td style=\"width:20%\">&nbsp;</td>\n";
							$col++;
						}
					}

					/* close table */
					$this->iconmenu= $this->iconmenu."</tr>\n</table>\n";
				}
			}

		}

		/* Write menu output */
		return ($this->iconmenu);
	}

	function get_path($index)
	{
		if(!isset($this->dirlist[$index])){
			return ("");
		}
		return ("../".$this->dirlist[$index]);
	}

	function get_index($path)
	{
		return (array_search($path, $this->dirlist));
	}
}
?>
