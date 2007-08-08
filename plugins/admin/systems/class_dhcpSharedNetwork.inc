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

class dhcpSharedNetwork extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $options= array();
  var $statements= array();

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpSharedNetwork($attrs)
  {
    /* Load statements / options */
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->cn= $attrs['cn'][0];
      $this->new= FALSE;

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

    /* Save CN for later reference */
    $this->orig_cn= $this->cn;
  }


  function execute()
  {
    $smarty= get_smarty();

    /* Fill template */
    $smarty->assign ("cn", $this->cn);
    foreach (array("server-identifier", "default-lease-time", 
          "max-lease-time", "min-lease-time") as $attr){
      if (isset($this->statements[$attr])){
        $smarty->assign(preg_replace('/-/', '_', $attr), $this->statements[$attr]);
      }
    }
    if ($this->dn != 'new'){
      $smarty->assign("mode", "readonly");
    }
    if (isset($this->statements["authoritative"])){
      $smarty->assign("authoritative", "checked");
    }
    if (!isset($this->statements["deny-unknown-clients"])){
      $smarty->assign("allow_unknown_state", "checked");
    }
    if (!isset($this->statements["deny-bootp"])){
      $smarty->assign("allow_bootp_state", "checked");
    }
    if (!isset($this->statements["deny-booting"])){
      $smarty->assign("allow_booting_state", "checked");
    }

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_sharedNetwork.tpl', TRUE)).$this->network->execute();

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

	/* Optionally execute a command after we're done */
	$this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  	if (isset($_POST['cn'])){
	  	plugin::save_object();

		foreach (array("server-identifier", "default-lease-time",
	                        "max-lease-time", "min-lease-time") as $attr){
			if (isset($_POST[$attr]) && $_POST[$attr] != ""){
				$this->statements[$attr]= $_POST[$attr];
			} else {
				unset($this->statements[$attr]);
			}
		}
	
		if (isset($_POST["authoritative"])){
			$this->statements["authoritative"]= "";
		} else {
			unset ($this->statements["authoritative"]);
		}
	
		foreach(array("unknown-clients", "bootp", "booting") as $name){
			if (isset($_POST[$name])){
				$this->statements["allow-$name"]= "";
				unset($this->statements["deny-$name"]);
			} else {
				$this->statements["deny-$name"]= "";
				unset($this->statements["allow-$name"]);
			}
		}
	}
  }


  /* Check values */
  function check()
  {
	$message= array();

	/* Check lease times */
	foreach (array("default-lease-time" => _("Default lease time"),
			"max-lease-time" => _("Max. lease time"),
			"min-lease-time" => _("Min. lease time")) as $key => $val){
		if (isset($this->statements[$key]) && $this->statements[$key] != "" && 
			!is_id($this->statements[$key])){
			$message[]= sprintf(_("The value specified as '%s' is no valid integer!"), $val);
		}
	}

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
			if (preg_match('/^allow-/', $key) || preg_match('/^deny-/', $key)){
				$val= preg_replace('/^[^-]+-/', '', $key);
				$key= preg_replace('/^([^-]+)-.*$/', '\\1', $key);
			}
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
