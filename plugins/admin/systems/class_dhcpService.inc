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

class dhcpService extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $dhcpPrimaryDN= "";
  var $orig_dhcpPrimaryDN= "";
  var $options= array();
  var $statements= array();
  var $default_lease_time= 600;
  var $max_lease_time= 1700;
  var $authoritative= TRUE;
  var $get_lease_hostnames= TRUE;
  var $ddns_update_style= "none";
  var $ddns_styles= array('none', 'interim', 'ad-hoc');

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();


  function dhcpService($attrs)
  {
    /* Load statements / options */
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->new= FALSE;

      /* Load attributes */
      foreach (array("cn", "dhcpPrimaryDN") as $attr){
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
    $this->advanced->setAutoStatements(array("default-lease-time", "max-lease-time", "authoritative", "server-name", "get-lease-hostnames", "server-identifier", "ddns-update-style"));

    /* Save for later action */
    $this->orig_dhcpPrimaryDN= $this->dhcpPrimaryDN;
    $this->orig_cn= $this->cn;
  }

  function execute()
  {
    /* Show main page */
    $smarty= get_smarty();

    $smarty->assign('ddns_styles', $this->ddns_styles);

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_service.tpl', TRUE)).$this->network->execute();

    /* Merge arrays for advanced view */
    foreach (array("options", "statements") as $type){
      $tmp= array_merge($this->$type, $this->network->$type);
      $this->advanced->$type= $tmp;
    }

    $display.= $this->advanced->execute();

    /* Merge back for removals */
    foreach (array("options", "statements") as $type){
      $this->$type= $this->advanced->$type;
      $this->network->$type= $this->advanced->$type;
    }

    /* Add footer */
    $display.= "<div style='width:100%;text-align:right;margin-top:5px;'><input type=submit name='save_dhcp' value='"._("Save")."'>".
      "&nbsp;<input type=submit name='cancel_dhcp' value='"._("Cancel")."'></div>";


    return ($display);

  }

  function remove_from_parent()
  {
        /* Just remove the dn from the ldap, then we're done. Host
           entries do not have any entries below themselfes. */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->recursive_remove();
        show_ldap_error($ldap->get_error());

	$ldap->cat($this->dhcpPrimaryDN);
	$attrs= $ldap->fetch();
	$objectclasses= array();
	for($i= 0; $i<$attrs['objectClass']['count']; $i++){
		if ($attrs['objectClass'][$i] != "dhcpServer"){
			$objectclasses[]= $attrs['objectClass'][$i];
		}
	}
	$attrs= array();
	$attrs['dhcpServiceDN']= array();
	$attrs['objectClass']= $objectclasses;
	$ldap->cd($this->dhcpPrimaryDN);
	$ldap->modify($attrs);
	show_ldap_error($ldap->get_error());


	/* Optionally execute a command after we're done */
	$this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  	plugin::save_object();

	/* Save base */
	if (isset($_POST['base'])){
		$this->base= $_POST['base'];
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
        if ($this->dhcpPrimaryDN == ""){
                $message[]= _("No server specified to host dhcp service!");
        }

	return $message;
  }


  /* Save to LDAP */
  function save()
  {
echo "server-name";
echo "server-identifier";
        plugin::save();

        /* Get ldap mode */
        if ($this->dn == "new"){
                $mode= "add";
		$this->dn= "cn=".$this->cn.",ou=dhcp,ou=configs,ou=systems,".$this->base;
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

	/* Modify server entry to keep our 'dn' */
	$ldap->cat($this->dhcpPrimaryDN);
	$attrs= $ldap->fetch();
	$objectclasses= array();
	for($i= 0; $i<$attrs['objectClass']['count']; $i++){
		if ($attrs['objectClass'][$i] != "dhcpServer"){
			$objectclasses[]= $attrs['objectClass'][$i];
		}
	}
	$objectclasses[]= "dhcpServer";
	$attrs= array();
	$attrs['dhcpServiceDN']= $this->dn;
	$attrs['objectClass']= $objectclasses;
	$ldap->cd($this->dhcpPrimaryDN);
	$ldap->modify($attrs);
	show_ldap_error($ldap->get_error());

	/* Modify old server entry */
	if ( ($this->orig_dhcpPrimaryDN != "")
		&& ($this->orig_dhcpPrimaryDN != $this->dhcpPrimaryDN)){

		$ldap->cat($this->orig_dhcpPrimaryDN);
		$attrs= $ldap->fetch();
		if (isset($attrs['dhcpServiceDN'])){
			$objectclasses= array();
			for($i= 0; $i<$attrs['objectClass']['count']; $i++){
				if ($attrs['objectClass'][$i] != "dhcpServer"){
					$objectclasses[]= $attrs['objectClass'][$i];
				}
			}
			$attrs= array();
			$attrs['dhcpServiceDN']= array();
			$attrs['objectClass']= $objectclasses;
			$ldap->cd($this->orig_dhcpPrimaryDN);
			$ldap->modify($attrs);
			show_ldap_error($ldap->get_error());
		}
	}
  }
  
}

?>
