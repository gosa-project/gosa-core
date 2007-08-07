<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003-2007  Cajus Pollmeier

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

class dhcpSubnet extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $dhcpNetMask= "";
  var $dhcpRange= "";
  var $options= array();
  var $statements= array();

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpSubnet($attrs)
  {
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->new= FALSE;

      /* Load attributes */
      foreach (array("cn", "dhcpNetMask", "dhcpRange") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
      }

      /* Load options */
      if (isset($attrs['dhcpOption'])){
        foreach ($attrs['dhcpOption'] as $opt){
          $idx= preg_replace('/\s.+$/', '', $opt);
          $value= preg_replace('/^[^\s]+\s/', '', $opt);
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        foreach ($attrs['dhcpStatements'] as $opt){
          $idx= preg_replace('/\s.+$/', '', $opt);
          $value= preg_replace('/^[^\s]+\s/', '', $opt);
          $this->statements[$idx]= $value;
        }
      }

    } else {
      /* We keep the parent dn here if it's new */
      $this->dn= $attrs;
      $this->new= TRUE;
    }

    /* Load network module */
    $this->network= new dhcpNetwork();
    $this->network->options= $this->options;
    $this->network->statements= $this->statements;
    $this->advanced= new dhcpAdvanced();
    $this->advanced->options= $this->options;
    $this->advanced->statements= $this->statements;
    $this->advanced->setAutoStatements(array("fixed-address"));

    /* Save CN for later reference */
    $this->orig_cn= $this->cn;
    $this->dhcpNetMask= normalize_netmask($this->dhcpNetMask);
  }


  function execute()
  {
	$smarty= get_smarty();
	$smarty->assign("cn", $this->cn);

	if ($this->dn != "new"){
		$smarty->assign("mode", "readonly");

		list($na0, $na1, $na2, $na3)= split('\.', $this->cn);
                for ($i= 0; $i<4; $i++){
                        $name= "na$i";
                        $smarty->assign("na$i", $$name);
                }

		/* Assign netmask */
		list($nm0, $nm1, $nm2, $nm3)= split('\.', $this->dhcpNetMask);
		for ($i= 0; $i<4; $i++){
			$name= "nm$i";
			$smarty->assign("nm$i", $$name);
		}

		/* Prepare range */
		if ($this->dhcpRange != ""){
			list($r1, $r2)= preg_split('/\s+/', $this->dhcpRange);
			list($r00, $r01, $r02, $r03)= split('\.', $r1);
			for ($i= 0; $i<4; $i++){
				$name= "r0$i";
				$smarty->assign("r0$i", $$name);
			}
			list($r10, $r11, $r12, $r13)= split('\.', $r2);
			for ($i= 0; $i<4; $i++){
				$name= "r1$i";
				$smarty->assign("r1$i", $$name);
			}
		}
	}

	/* Show main page */
	return $smarty->fetch (get_template_path('dhcp_subnet.tpl', TRUE));
  }

  function remove_from_parent()
  {
	/* Just remove the dn from the ldap, then we're done. Host
	   entries do not have any entries below themselfes. */
	$ldap= $this->config->get_ldap_link();
	$ldap->cd($this->dn);
	$ldap->recursive_remove();
	show_ldap_error($ldap->get_error());

	/* Optionally execute a command after we're done */
	$this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  	plugin::save_object();

	if (isset($_POST['na0'])){
		$this->cn= $_POST['na0'].".".$_POST['na1'].".".$_POST['na2'].".".$_POST['na3'];
		$this->dhcpNetMask= $_POST['nm0'].".".$_POST['nm1'].".".$_POST['nm2'].".".$_POST['nm3'];
		$this->dhcpRange= $_POST['r00'].".".$_POST['r01'].".".$_POST['r02'].".".$_POST['r03']." ".$_POST['r10'].".".$_POST['r11'].".".$_POST['r12'].".".$_POST['r13'];
	}
  }


  /* Check values */
  function check()
  {
	$message= array();

	/* All required fields are set? */
	if ($this->cn == ""){
		$message[]= _("Required field 'Network address' is not filled.");
	}
	if ($this->dhcpNetMask == ""){
		$message[]= _("Required field 'Netmask' is not filled.");
	}

	/* cn already used? */
	if ($this->dn != "new"){
		$ldap= $this->config->get_ldap_link();
		$ldap->cd($this->config->current['BASE']);
		$ldap->search("(&(objectClass=dhcpSubnet)(cn=".$this->cn."))");
		if ($ldap->count() >= 1){
			while ($attrs= $ldap->fetch()){
				if ($ldap->getDN() != $this->dn){
					$message[]= _("The network defined in this section is already used!");
					break;
				}

			}
		}
		$ldap->fetch();
	}

	#FIXME: There are some more things we could test -> valid netmask, range
	return $message;
  }


  /* Save to LDAP */
  function save()
  {
	plugin::save();

	/* Get ldap mode */
	if ($this->dn == "new"){
		$mode= "add";
	} else {
		$mode= "modify";
	}

	/* Generate new dn */
	if ($this->parent->parent != NULL && $this->dn == "new"){
                $this->dn= "cn=".$this->cn.",".$this->parent->parent;
        }

	/* Reconvert netmask to number of filled bits */
	$this->attrs['dhcpNetMask']= netmask_to_bits($this->dhcpNetMask);
	
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
  }
  
}

?>
