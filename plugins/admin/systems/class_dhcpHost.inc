<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003-2007 Cajus Pollmeier

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

class dhcpHost extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $dhcpHWAddress= "";
  var $options= array();
  var $statements= array();

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpHost($attrs)
  {
    /* Load statements / options */
    if ($attrs != NULL){

      /* Load attributes */
      foreach (array("cn", "dhcpHWAddress") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
      }

      /* Load options */
      if (isset($attrs['dhcpOption'])){
        for($i= 0; $i<$attrs['dhcpOption']['count']; $i++){
          $tmp= $attrs['dhcpOption'][$i];
          $idx= preg_replace('/\s.+$/', '', $tmp);
          $value= preg_replace('/^[^\s]+\s/', '', $tmp);
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        for($i= 0; $i<$attrs['dhcpStatements']['count']; $i++){
          $tmp= $attrs['dhcpStatements'][$i];
          $idx= preg_replace('/\s.+$/', '', $tmp);
          $value= preg_replace('/^[^\s]+\s/', '', $tmp);
          $this->statements[$idx]= $value;
        }
      }

    }
  }

  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("dhcpHWAddress", preg_replace('/^[^ ]+ /', '', $this->dhcpHWAddress));

    /* Create fixed address */
    if (isset($this->statements['fixed-address'])){
      $smarty->assign("fixedaddr", $this->statements['fixed-address']);
    } else {
      $smarty->assign("fixedaddr", "");
    }

    /* Prepare hw type selector */
    $hwtype= preg_replace('/\s.*$/', '', $this->dhcpHWAddress);
    $smarty->assign("hwtype", $hwtype);
    $smarty->assign("hwtypes", array("ethernet" => _("Ethernet"),
          "fddi" => _("FDDI"),
          "token-ring" => _("Token Ring")));

    /* Prepare mac address */
    if ($this->dhcpHWAddress != ""){
      list($hw0, $hw1, $hw2, $hw3, $hw4, $hw5)= 
        split (":", preg_replace('/^[^\s]+\s/', '', $this->dhcpHWAddress));
      for ($i= 0; $i<6; $i++){
        $name= "hw$i";
        $smarty->assign("$name", $$name);
      }
    }

    /* Show main page */
    return($smarty->fetch(get_template_path('dhcphost.tpl', TRUE)));
  }


#-------------------

  function remove_from_parent()
  {
	/* Just remove the dn from the ldap, then we're done. Host
	   entries do not have any entries below themselfes. */
	$ldap= $this->config->get_ldap_link();
	$ldap->rmDir($this->orig_dn);
	show_ldap_error($ldap->get_error());

	/* Optionally execute a command after we're done */
	$this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  	plugin::save_object();

	/* Save remaining attributes */
	if (isset($_POST['hwtype'])){

		/* Assemble hwAddress */
		$addr= $_POST['hwtype']." ";
		for ($i= 0; $i<6; $i++){
			$addr.= $_POST["hw$i"].":";
		}
		$this->dhcpHWAddress= preg_replace('/:$/', '', $addr);

		/* Save fixed address */
		if ($_POST['fixedaddr'] != ""){
			$this->statements['fixed-address']= $_POST['fixedaddr'];
		} else {
			unset ($this->statements['fixed-address']);
		}
	}
  }


  /* Check values */
  function check()
  {
	$message= array();

	/* All required fields are set? */
	if ($this->cn == ""){
		$message[]= _("Required field 'Name' is not filled.");
	}

	/* cn already used? */
	if ($this->dn != "new"){
		$ldap= $this->config->get_ldap_link();
		$ldap->cd($this->config->current['BASE']);
		$ldap->search("(&(objectClass=dhcpHost)(cn=".$this->cn."))");
		if ($ldap->count() >= 1){
			while ($attrs= $ldap->fetch()){
				if ($ldap->getDN() != $this->dn){
					$message[]= _("The name for this host section is already used!");
					break;
				}

			}
		}
		$ldap->fetch();
	}

	/* Check syntax of MAC address */
	$check= preg_replace('/^[^\s]*\s/', '', $this->dhcpHWAddress);
	if (!preg_match('/^([0-9a-fA-F]{1,2}:){5}[0-9a-fA-F]{1,2}$/', $check)){
		$message[]= _("The hardware address specified by you is not valid!");
	}

	return $message;
  }


  /* Save to LDAP */
  function save()
  {
	plugin::save();

	/* Generate new dn */
	if ($this->parent->parent != NULL){
		$this->dn= "cn=".$this->cn.",".$this->parent->parent;
	} else {
		$tmp= preg_replace('/^cn=[^,]+/', '', $this->dn);
		$this->dn= "cn=".$this->cn.$tmp;
	}
	
	/* Get ldap mode */
	if ($this->orig_dn != $this->dn){
		$mode= "add";
	} else {
		$mode= "modify";
	}

	/* Assemble new entry - options */
	if (isset ($this->options) && count ($this->options)){
		$this->attrs['dhcpOption']= array();
		foreach ($this->options as $key => $val){
			$this->attrs['dhcpOption'][]= "$key $val";
		}
	} else {
		if ($mode == "modify"){
			$this->attrs['dhcpOption']= array();
		}
	}
	
	/* Assemble new entry - statements */
	if (isset ($this->statements) && count ($this->statements)){
		$this->attrs['dhcpStatements']= array();
		foreach ($this->statements as $key => $val){
			$this->attrs['dhcpStatements'][]= "$key $val";
		}
	} else {
		if ($mode == "modify"){
			$this->attrs['dhcpStatements']= array();
		}
	}

	/* Do LDAP action */
	$ldap= $this->config->get_ldap_link();
        if ($mode == "add"){
		$ldap->cd($this->config->current['BASE']);
                $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }
	$ldap->cd($this->dn);
	$ldap->$mode($this->attrs);
	show_ldap_error($ldap->get_error());
	
	/* Name changed? Remove orig? */
	if ($this->orig_dn != $this->dn && $this->orig_dn != "new"){
		echo "Remove old<br>";
		$ldap->rmdir($this->orig_dn);
		show_ldap_error($ldap->get_error());
	}
  }
  
}

?>
