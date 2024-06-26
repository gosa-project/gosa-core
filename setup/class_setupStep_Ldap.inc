<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

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


class Step_Ldap extends setup_step
{
  var $connection = "ldap://localhost:389";
  var $location   = "default";
  var $admin      = "";
  var $password   = "";
  var $base       = "";
  var $append_base_to_admin_dn = FALSE;
  var $admin_given = "";

  var $connect_id = FALSE;
  var $bind_id    = FALSE;

  var $resolve_filter = "*";
  var $resolve_user   = FALSE;
  var $tls            = FALSE;

  var $rfc2307bis             = FALSE;
  var $attributes = array("connection", "location", "admin", "password", "base", "admin_given", "append_base_to_admin_dn", "tls", "rfc2307bis");

  var $header_image = "images/setup/ldap.png";

  var $b_displayCheckbutton = TRUE;

  function __construct()
  {
    $this->update_strings();
  }

  
  function update_strings()
  {
    $this->s_title      = _("LDAP setup");
    $this->s_title_long = _("LDAP connection setup");
    $this->s_info       = _("This dialog performs the basic configuration of the LDAP connectivity for GOsa.");
  }
  
  
  function execute()
  {
    $smarty = get_smarty();
    foreach ($this->attributes as $attr) {
      $smarty->assign($attr, htmlentities($this->$attr, ENT_QUOTES, "UTF-8"));
    }

    /* Assign connection status */
    $smarty->assign("connection_status", $this->get_connection_status());

    /* Handle namingContext detection */
    $attr = @LDAP::get_naming_contexts($this->connection);
    unset($attr['count']);
    $smarty->assign("namingContexts", $attr);
    $smarty->assign("namingContextsCount", count($attr));
    $smarty->assign("bool", array(FALSE => _("No"), TRUE => _("Yes")));

    /* Addign resolved users */
    $smarty->assign("resolve_user", $this->resolve_user);
    if ($this->resolve_user) {
      $tmp = $this->resolve_user();
      $smarty->assign("resolved_users", $tmp);
      $smarty->assign("resolved_users_count", count($tmp));
      $smarty->assign("resolve_filter", $this->resolve_filter);
    }

    $base_to_append = $this->base;
    if (strlen($base_to_append) > 20) {
      $base_to_append = substr($base_to_append, 0, 17) . "...";
    }
    $smarty->assign("base_to_append", $base_to_append);
    return ($smarty->fetch(get_template_path("../setup/" . $GLOBALS['theme'] . "/setup_ldap.tpl")));
  }

  function get_connection_status()
  {
    $this->bind_id    = FALSE;

    @ldap_set_option(NULL, LDAP_OPT_DEBUG_LEVEL, 7);
    $connect_id = ldap_connect($this->connection) or die( "Der LDAP-URI konnte nicht geparst werden");

    if ($this->tls) {
      if (@ldap_set_option($connect_id, LDAP_OPT_REFERRALS, 0))
        if (@ldap_start_tls($connect_id))
          $this->bind_id = @ldap_bind($connect_id, $this->admin, $this->password);
      @ldap_set_option($connect_id, LDAP_OPT_PROTOCOL_VERSION, 3);
    } else {
      @ldap_set_option($connect_id, LDAP_OPT_PROTOCOL_VERSION, 3);
      $this->bind_id = @ldap_bind($connect_id, $this->admin, $this->password);
    }

    if (!$this->bind_id) {
      $str = sprintf(_("Anonymous bind to server '%s' failed!"), $this->connection);
      if (!empty($this->admin)) {
        $str = sprintf(_("Bind as user '%s' failed!"), $this->admin, $this->connection);
      }
      return ("<p style='color:red'>" . $str . "</p>");
    } else {
      if (empty($this->admin)) {
        $str = sprintf(_("Anonymous bind to server '%s' succeeded."), $this->connection);
        return ("<p><span style='color:blue'>" . $str . "</span><br><span style='color:red'>" . _("Please specify user and password!") . "</span></p>");
      } else {
        $str = sprintf(_("Bind as user '%s' to server '%s' succeeded!"), $this->admin, $this->connection);
        return ("<p style='color:green'>" . $str . "</p>");
      }
    }
  }

  
  function resolve_user()
  {
    $filter  = $this->resolve_filter;

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP("", "", $this->connection, FALSE, $this->tls);
    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($this->base);
    $ldap->search("(&(objectClass=person)(|(uid=" . $filter . ")(cn=" . $filter . ")))");
    $tmp = array();
    while ($attrs = $ldap->fetch()) {
      $tmp[base64_encode($attrs['dn'])] = LDAP::fix($attrs['dn']);
      natcasesort($tmp);
    }
    return ($tmp);
  }

  function save_object()
  {
    $reset = FALSE;
    foreach ($this->attributes as $attr) {
      if (isset($_POST[$attr])) {
        if (in_array_strict($attr, array("base", "connection")) && $this->$attr != get_post($attr)) {
          $reset = TRUE;
        }
        $this->$attr = get_post($attr);
      }
    }

    if ($reset) {
      $this->parent->disable_steps_from(($this->parent->step_name_to_id(get_class($this))) + 1);
      $attr = @LDAP::get_naming_contexts($this->connection);
      if (is_array($attr) && !in_array_strict(get_post("base"), $attr)) {
        if (isset($attr[0])) {
          $this->base = $attr[0];
        }
      }
    }

    if (isset($_POST['resolve_user_x'])) {
      $this->resolve_user = !$this->resolve_user;
    }
    if (isset($_POST['resolve_user'])) {
      $this->resolve_user = !$this->resolve_user;
    }
  
    /* Hide backward forward button*/
    $this->dialog = $this->resolve_user;
    if (isset($_POST['resolve_filter'])) {
      $this->resolve_filter = get_post('resolve_filter');
    }

    if (isset($_POST['use_selected_user'])) {

      if (isset($_POST['admin_to_use'])) {
        $this->admin = base64_decode(get_post('admin_to_use'));
        $this->resolve_user = false;
      }
    }

    if (isset($_POST['append_base_to_admin_dn'])) {
      $this->append_base_to_admin_dn = TRUE;
    } else {
      $this->append_base_to_admin_dn = FALSE;
    }
    if ($this->append_base_to_admin_dn) {
      $base = $this->base;
      if (!preg_match("/,$/", $this->admin_given)) {
        $base = "," . $base;
      }
      $this->admin = $this->admin_given . $base;
    } else {
      $this->admin = $this->admin_given;
    }

    $this->get_connection_status();
    if ($this->bind_id && !empty($this->admin) && !empty($this->base)) {
      $this->is_completed = TRUE;
    } else {
      $this->is_completed = FALSE;
    }
  }
}
