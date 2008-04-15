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

class reference extends plugin
{
  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $objectlist= "";
  var $obtypes= array();
  var $fold= array();
  var $objectCache= array();

  function reference (&$config, $dn= NULL)
  {
	/* Include config object */
	$this->config= &$config;
	$this->dn= $dn;

        /* Fill translation array */
        $this->obtypes= array(  "gosaAccount" => array(	   "text" => _("Generic"),
							   "image" => "penguin.png"),
				"posixAccount" => array(   "text" =>_("UNIX"),
							   "image" => "select_user.png"),
                                "gosaMailAccount" => array("text" => _("Mail"),
							   "image" => "mailto.png"),
                                "sambaSamAccount" => array("text" => _("Samba"),
							   "image" => "select_winstation.png"),
                                "sambaAccount" => array(   "text" => _("Samba"),
							   "image" => "select_winstation.png"),
                                "goFaxAccount" => array(   "text" => _("FAX"),
							   "image" => "fax_small.png"),
                                "gosaProxyAccount" => array("text" => _("Proxy"),
							   "image" => "select_proxy.png"),
                                "PureFTPdUser" => array(   "text" => _("FTP"),
							   "image" => "select_proxy.png"),
                                "posixGroup" => array(     "text" => _("Group"),
							   "image" => "select_groups.png"),
                                "gosaDepartment" => array( "text" => _("Department"),
							   "image" => "select_department.png"),
                                "goFonHardware" => array(   "text" => _("Phone"),
							   "image" => "select_phone.png"),
                                "gosaApplication" => array("text" => _("Application"),
							   "image" => "select_application.png"),
                                "goServer" => array(	   "text" => _("Server"),
							   "image" => "select_server.png"),
                                "gotoTerminal" => array(   "text" => _("Thin Client"),
							   "image" => "select_terminal.png"),
                                "gotoWorkstation" => array("text" => _("Workstation"),
							   "image" => "select_workstation.png"),
                                "gosaGroupOfNames" => array("text" => _("Object group"),
							   "image" => "select_ogroup.png"),
                                "gotoPrinter" => array(	   "text" => _("Printer"),
							   "image" => "select_printer.png"));
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

	/* Set government mode */
	$smarty= get_smarty();

	if (isset($_GET['show'])){
		$dn= base64_decode($_GET['show']);
		if (isset($this->fold["$dn"])){
			unset($this->fold["$dn"]);
		} else {
			$this->fold["$dn"]= "$dn";
		}
	}

	/* Fill array */
	$this->reload();
	$smarty->assign("objectList", $this->objectlist);

	/* Show main page */
	return ($smarty->fetch (get_template_path('contents.tpl', TRUE, dirname(__FILE__))));
  }

  function reload()
  {
	$ldap= $this->config->get_ldap_link();
	$ldap->cd($this->config->current['BASE']);

	$ldap->search("(|(member=".LDAP::prepare4filter($this->dn).")(memberUid=".$this->uid."))",array("memberUid","member","cn","description","objectClass"));
	
  	$cycle= array("F5F5F5", "ECECEC");
	$id= 0;
	$this->objectlist= "";
	while ($attrs= $ldap->fetch()){

		/* Add more information? */
		$addon= "";
		if (in_array($attrs['dn'], $this->fold)){
			$memberattr= "";
			if (isset($attrs['memberUid'])){
				unset($attrs['memberUid']['count']);
				$memberattr= "memberUid";
			} 
			if (isset($attrs['member'])){
				unset($attrs['member']['count']);
				$memberattr= "member";
			} 
			if ($memberattr == ""){
				continue;
			}
			foreach ($attrs[$memberattr] as $uid){
				$content= $this->getCacheObject($uid);
				if ($content === NULL) {
					continue;
				}
				$addon.= '<tr style="background-color:'.$cycle[$id&1].';"><td title="'.$attrs['cn'][0].'">&nbsp;&nbsp;&nbsp;'.$content['name'].'</td><td>'.$content['description'].'</td><td>'.$content['type'].'</td> </tr>';
			}
		}

		/* Build final entry */
		if (isset($attrs['description'])){
			$description= $attrs['description'][0];
		} else {
			$description= "-";
		}
		$content= $this->createContentString($attrs['objectClass']);
		if ($addon == ""){
			$img= "images/expand.png";
		} else {
			$img= "images/lists/sort-down.png";
		}
		$this->objectlist.= '<tr style="background-color:'.$cycle[$id&1].';"><td class="phonelist" title="'.$attrs['cn'][0].'"><a href="main.php?plug='.$_GET['plug'].'&amp;start=&amp;show='.base64_encode($attrs['dn']).'"><img alt="" src="'.$img.'" border=0>&nbsp;'.$attrs['cn'][0].'</a></td><td>'.$description.'</td><td>'.$content.'</td> </tr>'.$addon;

		$id++;
	}
	
  }

  function getCacheObject($dn)
  {
	if (!isset($this->objectCache[$dn])){
		$ldap= $this->config->get_ldap_link();
		if (preg_match('/,/', $dn)){
			$ldap->cat($dn, array("cn", "objectClass", "description","ou"));
		} else {
			$ldap->search("(uid=$dn)", array("cn", "objectClass", "description","ou"));
		}
		if ($ldap->count()){
			$attrs= $ldap->fetch();

			if(isset($attrs['ou']) && !isset($attrs['cn'])){
				$attrs['cn'] = $attrs['ou'];
			}
				
			if (isset($attrs['description'])){
				$description= $attrs['description'][0];
			} else {
				$description= "-";
			}
			$this->objectCache[$dn]= array(	'name' => $attrs['cn'][0],
					'description' => "$description",
					'type' => $this->createContentString($attrs['objectClass']));
		} else {
			return NULL;
		}
	}
	return ($this->objectCache[$dn]);
  }

  function createContentString($classes)
  {
	$result= "";
  	if(!is_null($classes)) {
		foreach ($classes as $class){
			if (isset($this->obtypes[$class])){
				$result.= "<a><img alt=\"\" border=0 src='images/".$this->obtypes[$class]['image']."'>&nbsp;".$this->obtypes[$class]['text']."</a> ";
			}
		}
	}
	return (preg_replace('/ $/', '', $result));
  }
  
}

?>
