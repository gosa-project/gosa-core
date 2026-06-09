<?php

/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 * Copyright (C) 2003 Alejandro Escanero Blanco <aescanerochaosdimension.org>
 * Copyright (C) 1998  Eric Kilfoil <ericipass.net>
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

define("ALREADY_EXISTING_ENTRY", -10001);
define("UNKNOWN_TOKEN_IN_LDIF_FILE", -10002);
define("NO_FILE_UPLOADED", 10003);
define("INSERT_OK", 10000);
define("SPECIALS_OVERRIDE", true);

class LDAP
{
    public static $characterMap = null;
    public static $characterMapRegFrom = null;
    public static $characterMapRegTo = null;
    public static $readableMapRegFrom = null;
    public static $readableMapRegTo = null;

    // connection handle
    static $cid;
    // search resources (?)
    static $sr    = [];
    // search resource pointer (?)
    var $srp      = 0;
    var $hascon   = false;
    var $reconnect = false;
    var $tls      = true;
    var $hasres   = [];
    var $re       = [];
    var $basedn   = "";
    var $start    = []; // 0 if we are fetching the first entry, otherwise 1
    var $error    = ""; // Any error messages to be returned can be put here
    var $objectClasses = []; // Information read from slapd.oc.conf
    var $binddn   = "";
    var $bindpw   = "";
    var $hostname = "";
    var $follow_referral = false;
    var $referrals = [];
    var $max_ldap_query_time = 0;   // 0, empty or negative values will disable this check

    function __construct($binddn, $bindpw, $hostname, $follow_referral = false, $tls = false)
    {
        global $config;
        $this->follow_referral = $follow_referral;
        $this->tls = $tls;
        $this->binddn = self::convert($binddn);

        $this->bindpw = $bindpw;
        $this->hostname = $hostname;

        /* Check if MAX_LDAP_QUERY_TIME is defined */
        if (is_object($config) && $config->get_cfg_value("core", "ldapMaxQueryTime") != "") {
            $str = $config->get_cfg_value("core", "ldapMaxQueryTime");
            $this->max_ldap_query_time = (float)($str);
        }

        $this->connect();
    }

    static function escapeDN($value)
    {
        return ldap_escape($value, '', LDAP_ESCAPE_DN);
    }

    /* Escape a value (NOT a dn) that is used in an LDAP search query. */
    static function escapeValue($value)
    {
        return ldap_escape($value, '', LDAP_ESCAPE_FILTER);
    }

    /* Escape a value (NOT a dn) that is used in an LDAP search query and may contain a wildcard (*). */
    static function escapeValueWithWildcard($value)
    {
        return ldap_escape($value, '*', LDAP_ESCAPE_FILTER);
    }

    /**
     * Return a search resource.
     *
     * The current search resource pointer (?) is
     * used to clear the current pointer value. Afterwards
     * increase the pointer value.
     *
     */
    function getSearchResource(): int
    {
        self::$sr[$this->srp] = null;
        $this->start[$this->srp] = 0;
        $this->hasres[$this->srp] = false;
        return $this->srp++;
    }

    /**
     *  Function to replace all problematic characters inside a DN by \001XX, where
     *  \001 is decoded to chr(1) [ctrl+a]. It is not impossible, but very unlikely
     *  that this character is inside a DN.
     *  Currently used codes:
     *  ,   => CO
     *  \2C => CO
     *  (   => OB
     *  )   => CB
     *  /   => SL
     *  \22 => DQ
     */
    static function convert($dn)
    {
        if ($dn !== null) {
            if (SPECIALS_OVERRIDE == true) {
                $tmp = preg_replace(
                    array("/\\\\,/", "/\\\\2C/", "/\(/", "/\)/", "/\//", "/\\\\22/", '/\\\\"/'),
                    array("\001CO", "\001CO", "\001OB", "\001CB", "\001SL", "\001DQ", "\001DQ"),
                    $dn
                );
                return (preg_replace('/,\s+/', ',', $tmp));
            } else {
                return ($dn);
            }
        }
    }

    /* \brief     Tests for the special-char handling of the currently used ldap database
     *             and updates the LDAP class correspondingly.
     *            This affects the self::fix function and allows us to write
     *             dns containing  , " ( )
     */
    static function updateSpecialCharHandling()
    {
        // Load results from the session if they exists.
        if (session::is_set('self::updateSpecialCharHandling')) {
            $data = session::get('self::updateSpecialCharHandling');
            $attrs = array("characterMap", "characterMapRegFrom", "characterMapRegTo", "readableMapRegFrom", "readableMapRegTo");
            foreach ($attrs as $attr) {
                self::$$attr = $data[$attr];
            }

            $attrs = array(implode(", ", self::$characterMapRegFrom), implode(", ", self::$characterMapRegTo));
            DEBUG(DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $attrs, "(Cached) Detected special-char handling for LDAP actions");
            return;
        }

        // Set a default character handling.
        self::$characterMapRegFrom = array("/\001CO/", "/\001OB/", "/\001CB/", "/\001SL/", "/\001DQ/");
        self::$characterMapRegTo = array("\,", "(", ")", "/", '\"');

        // Set a default mapping to make readable strings out of dns.
        self::$readableMapRegFrom = array(
            "/\001CO/", "/\001OB/", "/\001CB/", "/\001SL/", "/\001DQ/",
            "/\\\\,/", "/\\\\2C/", "/\(/", "/\)/", "/\//", "/\\\\22/", '/\\\\"/'
        );
        self::$readableMapRegTo = array(",", "(", ")", "/", '"', ",", ",", "(", ")", "/", '"', '"');

        if (self::$characterMap === null) {
            self::$characterMap = detectLdapSpecialCharHandling();

            // Check if character-detection was successfull, if it wasn't use a fallback.
            if (self::$characterMap !== null) {
                self::$characterMapRegFrom = [];
                self::$characterMapRegTo = [];
                self::$readableMapRegFrom = [];
                self::$readableMapRegTo = [];
                foreach (self::$characterMap as $from => $to) {
                    // Append entry character conversion
                    self::$characterMapRegFrom[] = "/" . preg_quote(self::convert($from), '/') . "/";
                    self::$characterMapRegTo[] = addslashes($to);

                    // Append entry make readable array
                    self::$readableMapRegFrom[] = "/" . preg_quote($to, '/') . "/";
                    self::$readableMapRegFrom[] = "/" . preg_quote(self::convert($to), '/') . "/";
                    self::$readableMapRegTo[] = stripslashes($from);
                    self::$readableMapRegTo[] = stripslashes($from);
                }
            } else {
                // To avoid querying a hundred times without any success, stop here.
                self::$characterMap = [];
            }
        }

        // Store results in the session.
        $attrs = array("characterMap", "characterMapRegFrom", "characterMapRegTo", "readableMapRegFrom", "readableMapRegTo");
        $data = [];
        foreach ($attrs as $attr) {
            $data[$attr] = self::$$attr;
        }
        session::set("self::updateSpecialCharHandling", $data);

        $attrs = array(implode(", ", self::$characterMapRegFrom), implode(", ", self::$characterMapRegTo));
        DEBUG(DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $attrs, "Detected special-char handling for LDAP actions");
    }


    /*!  \brief     This methods replaces non-readable characters be readable ones,
     *               depends on what were detect in 'updateSpecialCharHandling'.
     * param       String  The DN/string to cleanup
     * return      String  The readable string.
     */
    static function makeReadable($dn)
    {
        if ($dn !== null) {
            if (self::$characterMap === null) {
                self::updateSpecialCharHandling();
            }
            return (preg_replace(self::$readableMapRegFrom, self::$readableMapRegTo, $dn));
        }

        return $dn;
    }


    /* \brief   Function to fix all problematic characters inside a DN by replacing \001XX
     *           codes to their original values. See "convert" for more information.
     *          The ',' characters are always expanded to \, (not \2C), since all tested LDAP
     *           servers seem to take it the correct way.
     * param String  The DN to convert characters in.
     * param String  The converted dn.
     */
    static function fix($dn)
    {
        if (SPECIALS_OVERRIDE == true && $dn != "" && $dn !== null) {
            // Update the conversion instruction set.
            if (self::$characterMap === null) {
                self::updateSpecialCharHandling();
            }

            return (preg_replace(self::$characterMapRegFrom, self::$characterMapRegTo, $dn));
        }

        return ($dn);
    }


    /* Function to fix problematic characters in DN's that are used for search
       requests. I.e. member=....                                               */
    static function prepare4filter($dn)
    {
        $fixed = LDAP::escapeDN(str_replace('\\\\', '\\\\\\', self::fix($dn)));
        return str_replace('\\,', '\\\\,', $fixed);
    }


    function connect()
    {
        $this->hascon = false;
        $this->reconnect = false;
        if (self::$cid = ldap_connect($this->hostname)) {
            ldap_set_option(self::$cid, LDAP_OPT_PROTOCOL_VERSION, 3);
            if (function_exists("ldap_set_rebind_proc") && $this->follow_referral) {
                ldap_set_option(self::$cid, LDAP_OPT_REFERRALS, 1);
                ldap_set_rebind_proc(self::$cid, array(&$this, "rebind"));
            }
            if (function_exists("ldap_start_tls") && $this->tls) {
                ldap_start_tls(self::$cid);
            }

            $this->error = "No Error";
            if ($bid = @ldap_bind(self::$cid, self::fix($this->binddn), $this->bindpw)) {
                $this->error = "Success";
                $this->hascon = true;
            } else {
                if ($this->reconnect) {
                    if ($this->error != "Success") {
                        $this->error = "Could not rebind to " . $this->binddn;
                    }
                } else {
                    $this->error = "Could not bind to " . $this->binddn;
                }
            }
        } else {
            $this->error = "Could not connect to LDAP server";
        }
    }

    function rebind($ldap, $referral): int|null
    {
        $credentials = $this->get_credentials($referral);
        if (@ldap_bind($ldap, self::fix($credentials['ADMINDN']), $credentials['ADMINPASSWORD'])) {
            $this->error = "Success";
            $this->hascon = true;
            $this->reconnect = true;
            return 0;
        }

        $this->error = "Could not bind to " . $credentials['ADMINDN'];
        return null;
    }

    function reconnect(): void
    {
        if ($this->reconnect) {
            ldap_unbind(self::$cid);
            self::$cid = null;
        }
    }

    function unbind(): void
    {
        ldap_unbind(self::$cid);
        self::$cid = null;
    }

    function disconnect(): void
    {
        if ($this->hascon) {
            ldap_close(self::$cid);
            $this->hascon = false;
        }
    }

    function cd($dir): void
    {
        if ($dir == "..") {
            $this->basedn = $this->getParentDir();
        } else {
            $this->basedn = self::convert($dir);
        }
    }

    function getParentDir($basedn = "")
    {
        if ($basedn == "") {
            $basedn = $this->basedn;
        } else {
            $basedn = self::convert($basedn);
        }
        return (preg_replace("/[^,]*[,]*[ ]*(.*)/", "$1", $basedn));
    }


    function search($srp, $filter, $attrs = []): bool|\LDAP\Result
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }

            $start = microtime(true);
            $this->clearResult($srp);
            self::$sr[$srp] = ldap_search(self::$cid, LDAP::fix($this->basedn), $filter, $attrs);
            $this->error = ldap_error(self::$cid);
            $this->resetResult($srp);
            $this->hasres[$srp] = true;

            /* Check if query took longer as specified in max_ldap_query_time */
            if ($this->max_ldap_query_time) {
                $diff = microtime(true) - $start;
                if ($diff > $this->max_ldap_query_time) {
                    msg_dialog::display(_("Performance warning"), sprintf(_("LDAP performance is poor: last query took %.2fs!"), $diff), WARNING_DIALOG);
                }
            }

            $this->log("LDAP operation: time=" . (microtime(true) - $start) . " operation=search('" . self::fix($this->basedn) . "', '$filter')");

            // Create statistic table entry
            stats::log('ldap', get_class($this), [], __FUNCTION__, 1, microtime(true) - $start);
            return (self::$sr[$srp]);
        }

        $this->error = "Could not connect to LDAP server";
        return false;
    }

    function ls($srp, $filter = "(objectclass=*)", $basedn = "", $attrs = array("*")): bool|\LDAP\Result
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }

            $this->clearResult($srp);
            if ($basedn == "") {
                $basedn = $this->basedn;
            } else {
                $basedn = self::convert($basedn);
            }

            $start = microtime(true);
            self::$sr[$srp] = @ldap_list(self::$cid, self::fix($basedn), $filter, $attrs);
            $this->error = ldap_error(self::$cid);
            $this->resetResult($srp);
            $this->hasres[$srp] = true;

            /* Check if query took longer as specified in max_ldap_query_time */
            if ($this->max_ldap_query_time) {
                $diff = microtime(true) - $start;
                if ($diff > $this->max_ldap_query_time) {
                    msg_dialog::display(_("Performance warning"), sprintf(_("LDAP performance is poor: last query took %.2fs!"), $diff), WARNING_DIALOG);
                }
            }

            $this->log("LDAP operation: time=" . (microtime(true) - $start) . " operation=ls('" . self::fix($basedn) . "', '$filter')");

            // Create statistic table entry
            stats::log('ldap', get_class($this), [], __FUNCTION__, 1, microtime(true) - $start);

            return (self::$sr[$srp]);
        }

        $this->error = "Could not connect to LDAP server";
        return false;
    }

    function cat($srp, $dn, $attrs = array("*"), $filter = "(objectclass=*)"): bool|\LDAP\Result
    {
        if ($dn === 'new') {
            return false;
        }
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }

            $this->clearResult($srp);
            self::$sr[$srp] = ldap_read(self::$cid, self::fix($dn), $filter, $attrs);
            $this->error = ldap_error(self::$cid);
            $this->resetResult($srp);
            $this->hasres[$srp] = true;
            return (self::$sr[$srp]);
        }

        $this->error = "Could not connect to LDAP server";
        return false;
    }

    /**
     * Count entries get from LDAP with the given filter.
     *
     * \param $dn the dn where we start
     * \param $filter custom filter used for the ldap read
     * \return bool|int
     */
    function object_match_filter($dn, $filter): bool|int
    {
        // check if hascon
        if ($this->hascon) {
            // check if reconnect is set
            if ($this->reconnect) {
                $this->connect();
            }
            // read from ldap with given parameters
            // even though there might be nothing wrong
            // we get an error if $dn is not available in LDAP
            $res =  ldap_read(self::$cid, self::fix($dn), $filter, array("objectClass"));
            // check for boolean false result
            if ($res) {
                $rv =   ldap_count_entries(self::$cid, $res);
                return ($rv);
            }
        } else {
            $this->error = "Could not connect to LDAP server";
        }
        return false;
    }

    function set_size_limit($size)
    {
        /* Ignore zero settings */
        if ($size == 0) {
            ldap_set_option(self::$cid, LDAP_OPT_SIZELIMIT, 10000000);
        }
        if ($this->hascon) {
            ldap_set_option(self::$cid, LDAP_OPT_SIZELIMIT, $size);
        } else {
            $this->error = "Could not connect to LDAP server";
        }
    }

    function fetch($srp)
    {
        $att = [];
        if ($this->hascon) {
            if ($this->hasres[$srp]) {
                if ($this->start[$srp] == 0) {
                    if (self::$sr[$srp]) {
                        $this->start[$srp] = 1;
                        $this->re[$srp] = ldap_first_entry(self::$cid, self::$sr[$srp]);
                    } else {
                        return [];
                    }
                } else {
                    $this->re[$srp] = ldap_next_entry(self::$cid, $this->re[$srp]);
                }
                if ($this->re[$srp]) {
                    $att = ldap_get_attributes(self::$cid, $this->re[$srp]);
                    $att['dn'] = trim(self::convert(ldap_get_dn(self::$cid, $this->re[$srp])));
                }
                $this->error = ldap_error(self::$cid);
                if (!isset($att)) {
                    $att = [];
                }
                return ($att);
            } else {
                $this->error = "Perform a fetch with no search";
                return ("");
            }
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    function resetResult($srp)
    {
        $this->start[$srp] = 0;
    }

    /**
     * Clear search result pointer (?)
     *
     * If there is an LDAP\Result Object available
     * at the given pointer (?) release it.
     *
     * \param $srp - search result pointer that is about to be cleared
     * \return void
     */
    function clearResult($srp): void
    {
        if ($this->hasres[$srp]) {
            $this->hasres[$srp] = false;
            if (!is_null(self::$sr[$srp]) && !is_bool(self::$sr[$srp])) {
                ldap_free_result(self::$sr[$srp]);
            }
        }
    }

    function getDN($srp)
    {
        if ($this->hascon) {
            if ($this->hasres[$srp]) {
                if (!$this->re[$srp]) {
                    $this->error = "Perform a Fetch with no valid Result";
                } else {
                    $rv = ldap_get_dn(self::$cid, $this->re[$srp]);

                    $this->error = ldap_error(self::$cid);
                    return (trim(self::convert($rv)));
                }
            } else {
                $this->error = "Perform a Fetch with no Search";
                return ("");
            }
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    /**
     * This method will always return an integer.
     * On error a negative value is returned otherwise
     * the actual count is returned.
     *
     * The only exception is for the error "no such object".
     * This error may indicate that there is no technical problem
     * but just no entry available. In this case we return a 0.
     *
     * \param $srp - search resource pointer
     */
    function count($srp): int
    {
        if (!$this->hascon) {
            $this->error = "Could not connect to LDAP server";
            return 0;
        }

        if (!$this->hasres[$srp]) {
            $this->error = "Perform a Fetch with no Search";
            return 0;
        }

        // if last LDAP operation errnor number was 32
        // there is probably no technical issue (BUT WE DON'T KNOW! CHECK YOUR FILTERS!)
        // so return a 0 value
        if (ldap_errno(self::$cid) === 32) {
            return 0;
        }

        $result = self::$sr[$srp];
        // from version 8.1 ldap methods will return LDAP\Result objects
        // instead of "Resources"
        if (version_compare(PHP_VERSION, '8.1.0', '>=')) {
            // on ldap operation that return error number 32 these operation
            // will return a bool "false". That should be interpreted as 0 count
            if (!$result instanceof LDAP\Result && ldap_errno(self::$cid) !== 32) {
                $this->error = "Search result was not type of LDAP\\Result.";
                return 0;
            }
        } else {
            if (!is_resource($result)) {
                $this->error = "Search result was not type of Resource";
                return 0;
            }
        }

        $rv = ldap_count_entries(self::$cid, $result);
        $this->error = ldap_error(self::$cid);

        // on error check again for ldap error number 32
        if ($rv === false && ldap_errno(self::$cid) === 32) {
            return 0;
        }

        return $rv;
    }

    function rm($attrs = [], $dn = "")
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            if ($dn == "") {
                $dn = $this->basedn;
            }

            $r = ldap_mod_del(self::$cid, self::fix($dn), $attrs);
            $this->error = ldap_error(self::$cid);
            return ($r);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    function mod_add($attrs = [], $dn = "")
    {
        print($attrs);

        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            if ($dn == "") {
                $dn = $this->basedn;
            }

            $r = ldap_mod_add(self::$cid, self::fix($dn), $attrs);
            $this->error = ldap_error(self::$cid);
            return ($r);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    function rename($attrs, $dn = "")
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            if ($dn == "") {
                $dn = $this->basedn;
            }

            $r = ldap_mod_replace(self::$cid, self::fix($dn), $attrs);
            $this->error = ldap_error(self::$cid);
            return ($r);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    function rmdir($deletedn)
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            $r = ldap_delete(self::$cid, self::fix($deletedn));
            $this->error = ldap_error(self::$cid);
            return ($r ? $r : 0);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }


    /*! \brief Move the given Ldap entry from $source to $dest
      param  String  $source The source dn.
      param  String  $dest   The destination dn.
      return Boolean true on success else false.
     */
    function rename_dn($source, $dest)
    {
        /* Check if source and destination are the same entry */
        if (strtolower($source) == strtolower($dest)) {
            trigger_error("Source and destination can't be the same entry.");
            $this->error = "Source and destination can't be the same entry.";
            return (false);
        }

        /* Check if destination entry exists */
        if ($this->dn_exists($dest)) {
            trigger_error("Destination '$dest' already exists.");
            $this->error = "Destination '$dest' already exists.";
            return (false);
        }

        /* Extract the name and the parent part out ouf source dn.
           e.g.  cn=herbert,ou=department,dc=...
           parent   =>  ou=department,dc=...
           dest_rdn =>  cn=herbert
         */
        $parent   = preg_replace("/^[^,]+,/", "", $dest);
        $dest_rdn = preg_replace("/,.*$/", "", $dest);

        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            $r = ldap_rename(self::$cid, self::fix($source), self::fix($dest_rdn), self::fix($parent), true);
            $this->error = ldap_error(self::$cid);

            /* Check if destination dn exists, if not the
               server may not support this operation */
            $r &= is_resource($this->dn_exists($dest));
            return ($r);
        } else {
            $this->error = "Could not connect to LDAP server";
            return (false);
        }
    }


    /**
     *  Function rmdir_recursive
     *
     *  Description: Based in recursive_remove, adding two thing: full subtree remove, and delete own node.
     *  Parameters:  The dn to delete
     *  GiveBack:    True on sucessfull , 0 in error, and "" when we don't get a ldap conection
     *
     */
    function rmdir_recursive($srp, $deletedn)
    {
        if ($this->hascon) {
            if ($this->reconnect) {
                $this->connect();
            }
            $delarray = [];

            /* Get sorted list of dn's to delete */
            $this->ls($srp, "(objectClass=*)", $deletedn);
            while ($this->fetch($srp)) {
                $deldn = $this->getDN($srp);
                $delarray[$deldn] = strlen($deldn);
            }
            arsort($delarray);
            reset($delarray);

            /* Really Delete ALL dn's in subtree */
            foreach ($delarray as $key => $value) {
                $this->rmdir_recursive($srp, $key);
            }

            /* Finally Delete own Node */
            $r = ldap_delete(self::$cid, self::fix($deletedn));
            $this->error = ldap_error(self::$cid);
            return ($r ? $r : 0);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    function makeReadableErrors($error, $attrs)
    {
        global $config;

        if ($this->success()) {
            return ("");
        }

        $str = "";
        if (preg_match("/^objectClass: value #([0-9]*) invalid per syntax$/", $this->get_additional_error())) {
            $oc = preg_replace("/^objectClass: value #([0-9]*) invalid per syntax$/", "\\1", $this->get_additional_error());
            if (isset($attrs['objectClass'][$oc])) {
                $str .= " - <b>objectClass: " . $attrs['objectClass'][$oc] . "</b>";
            }
        }
        if ($error == "Undefined attribute type") {
            $str = " - <b>attribute: " . preg_replace("/:.*$/", "", $this->get_additional_error()) . "</b>";
        }

        DEBUG(DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $attrs, "Erroneous data");

        return ($str);
    }

    function modify($attrs)
    {
        if (count($attrs) == 0) {
            return (0);
        }
        if ($this->hascon) {
            $start = microtime(true);
            if ($this->reconnect) {
                $this->connect();
            }

            $r = ldap_modify(self::$cid, self::fix($this->basedn), $attrs);
            $this->error = ldap_error(self::$cid);
            if (!$this->success()) {
                $this->error .= $this->makeReadableErrors($this->error, $attrs);
            }

            // Create statistic table entry
            stats::log(
                'ldap',
                $class = get_class($this),
                $category = [],
                $action = __FUNCTION__,
                $amount = 1,
                $duration = (microtime(true) - $start)
            );
            return ($r ? $r : 0);
        }

        $this->error = "Could not connect to LDAP server";
        return ("");
    }

    function add($attrs)
    {
        if ($this->hascon) {
            $start = microtime(true);
            if ($this->reconnect) {
                $this->connect();
            }
            $r = ldap_add(self::$cid, self::fix($this->basedn), $attrs);
            $this->error = ldap_error(self::$cid);
            if (!$this->success()) {
                $this->error .= $this->makeReadableErrors($this->error, $attrs);
            }

            // Create statistic table entry
            stats::log(
                'ldap',
                $class = get_class($this),
                $category = [],
                $action = __FUNCTION__,
                $amount = 1,
                $duration = (microtime(true) - $start)
            );

            return ($r ? $r : 0);
        } else {
            $this->error = "Could not connect to LDAP server";
            return ("");
        }
    }

    /**
     * Method to create missing branches.
     * TODO: check this method if it is
     * working correctly and document its function
     */
    function create_missing_trees($srp, $target)
    {
        $real_path = substr($target, 0, strlen($target) - strlen($this->basedn) - 1);

        if ($target == $this->basedn) {
            $l = array("dummy");
        } else {
            $l = array_reverse(gosa_ldap_explode_dn($real_path));
        }
        unset($l['count']);
        $cdn = $this->basedn;
        $tag = "";

        /* Load schema if available... */
        $classes = $this->get_objectclasses();

        foreach ($l as $part) {
            if ($part != "dummy") {
                $cdn = "$part,$cdn";
            }

            /* Ignore referrals */
            $found = false;
            foreach ($this->referrals as $ref) {
                $base = preg_replace('!^[^:]+://[^/]+/([^?]+).*$!', '\\1', $ref['URI']);
                if ($base == $cdn) {
                    $found = true;
                    break;
                }
            }
            if ($found) {
                continue;
            }

            $this->cat($srp, $cdn);
            $attrs = $this->fetch($srp);

            /* Create missing entry? */
            if (count($attrs)) {
                /* Catch the tag - if present */
                if (isset($attrs['gosaUnitTag'][0])) {
                    $tag = $attrs['gosaUnitTag'][0];
                }
            } else {
                $type = preg_replace('/^([^=]+)=.*$/', '\\1', $cdn);
                $param = self::fix(preg_replace('/^[^=]+=([^,]+).*$/', '\\1', $cdn));
                $param = preg_replace(array('/\\\\,/', '/\\\\"/'), array(',', '"'), $param);

                $na = [];

                /* Automatic or traditional? */
                if (count($classes)) {
                    /* Get name of first matching objectClass */
                    $ocname = "";
                    foreach ($classes as $class) {
                        if (isset($class['MUST']) && in_array_strict($type, $class['MUST'])) {
                            /* Look for first classes that is structural... */
                            if (isset($class['STRUCTURAL'])) {
                                $ocname = $class['NAME'];
                                break;
                            }

                            /* Look for classes that are auxiliary... */
                            if (isset($class['AUXILIARY'])) {
                                $ocname = $class['NAME'];
                            }
                        }
                    }

                    /* Bail out, if we've nothing to do... */
                    if ($ocname == "") {
                        msg_dialog::display(_("Internal error"), sprintf(_("Cannot automatically create subtrees with RDN %s: no object class found"), bold($type)), FATAL_ERROR_DIALOG);
                        exit();
                    }

                    /* Assemble_entry */
                    if ($tag != "") {
                        $na['objectClass'] = array($ocname, "gosaAdministrativeUnitTag");
                        $na["gosaUnitTag"] = $tag;
                    } else {
                        $na['objectClass'] = array($ocname);
                    }
                    if (isset($classes[$ocname]['AUXILIARY'])) {
                        $na['objectClass'][] = $classes[$ocname]['SUP'];
                    }
                    if ($type == "dc") {
                        /* This is bad actually, but - tell me a better way? */
                        $na['objectClass'][] = 'locality';
                    }
                    $na[$type] = $param;

                    // Fill in MUST values - but do not overwrite existing ones.
                    if (is_array($classes[$ocname]['MUST'])) {
                        foreach ($classes[$ocname]['MUST'] as $attr) {
                            if (isset($na[$attr]) && !empty($na[$attr])) {
                                continue;
                            }
                            $na[$attr] = "filled";
                        }
                    }
                } else {
                    /* Use alternative add... */
                    switch ($type) {
                        case 'ou':
                            if ($tag != "") {
                                $na["objectClass"] = array("organizationalUnit", "gosaAdministrativeUnitTag");
                                $na["gosaUnitTag"] = $tag;
                            } else {
                                $na["objectClass"] = "organizationalUnit";
                            }
                            $na["ou"] = $param;
                            break;
                        case 'dc':
                            if ($tag != "") {
                                $na["objectClass"] = array("dcObject", "top", "locality", "gosaAdministrativeUnitTag");
                                $na["gosaUnitTag"] = $tag;
                            } else {
                                $na["objectClass"] = array("dcObject", "top", "locality");
                            }
                            $na["dc"] = $param;
                            break;
                        default:
                            msg_dialog::display(_("Internal error"), sprintf(_("Cannot automatically create subtrees with RDN %s: not supported"), bold($type)), FATAL_ERROR_DIALOG);
                            exit();
                    }
                }
                $this->cd($cdn);
                $this->add($na);

                if (!$this->success()) {
                    print_a(array($cdn, $na));

                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($this->get_error(), $cdn, LDAP_ADD, get_class()));
                    return false;
                }
            }
        }

        return true;
    }


    function recursive_remove($srp)
    {
        $delarray = [];

        /* Get sorted list of dn's to delete */
        $this->search($srp, "(objectClass=*)");
        while ($this->fetch($srp)) {
            $deldn = $this->getDN($srp);
            $delarray[$deldn] = strlen($deldn);
        }
        arsort($delarray);
        reset($delarray);

        /* Delete all dn's in subtree */
        foreach ($delarray as $key => $value) {
            $this->rmdir($key);
        }
    }


    function get_attribute($dn, $name, $r_array = 0)
    {
        $data = "";
        if ($this->reconnect) {
            $this->connect();
        }
        $sr = ldap_read(self::$cid, self::fix($dn), "objectClass=*", array("$name"));

        /* fill data from LDAP */
        if ($sr) {
            $ei = ldap_first_entry(self::$cid, $sr);
            if ($ei) {
                if ($info = ldap_get_values_len(self::$cid, $ei, "$name")) {
                    $data = $info[0];
                }
            }
        }
        if ($r_array == 0) {
            return ($data);
        } else {
            return ($info);
        }
    }



    function get_additional_error()
    {
        $error = "";
        ldap_get_option(self::$cid, LDAP_OPT_ERROR_STRING, $error);
        return ($error);
    }


    /**
     * Method to return true if the LDAP "error" is Success otherwise
     * return false.
     *
     * \return bool
     */
    function success(): bool
    {
        return (trim($this->error) === 'Success');
    }


    function get_error(): string
    {
        if (trim($this->error) === 'Success') {
            return $this->error;
        }

        $adderror = $this->get_additional_error();
        if ($adderror != "") {
            $error = $this->error . " (" . $this->get_additional_error() . ", " . sprintf(_("while operating on %s using LDAP server %s"), bold($this->basedn), bold($this->hostname)) . ")";
        } else {
            $error = $this->error . " (" . sprintf(_("while operating on LDAP server %s"), bold($this->hostname)) . ")";
        }
        return $error;
    }

    static function get_credentials($url, $referrals = null)
    {
        $ret = [];
        $url = preg_replace('!\?\?.*$!', '', $url);
        $server = preg_replace('!^([^:]+://[^/]+)/.*$!', '\\1', $url);

        if ($referrals === null) {
            $referrals = self::$referrals;
        }

        if (isset($referrals[$server])) {
            return ($referrals[$server]);
        } else {
            $ret['ADMINDN'] = self::fix(self::$binddn);
            $ret['ADMINPASSWORD'] = self::$bindpw;
        }

        return ($ret);
    }


    /*! \brief  Generates an ldif for all entries matching the filter settings, scope and limit.
     *  param  $dn           The entry to export.
     *  param  $filter       Limit the exported object to those maching this filter.
     *  param  $scope        'base', 'sub' .. see manpage for 'ldapmodify' for details.
     *  param  $limit        Limits the result.
     */
    function generateLdif($dn, $filter = "(objectClass=*)", $scope = 'sub', $limit = 0)
    {
        // Ensure that limit is numeric if not skip here.
        if (!empty($limit) && !is_numeric($limit)) {
            trigger_error(sprintf("Invalid parameter for limit '%s', a numeric value is required."), $limit);
            return (null);
        }
        $limit = (!$limit) ? '' : ' -z ' . $limit;

        // Check scope values
        $scope = trim($scope);
        if (!empty($scope) && !in_array_strict($scope, array('base', 'one', 'sub', 'children'))) {
            trigger_error(sprintf("Invalid parameter for scope '%s', please use 'base', 'one', 'sub' or 'children'."), $scope);
            return (null);
        }
        $scope = (!empty($scope)) ? ' -s ' . $scope : '';

        // First check if we are able to call 'ldapsearch' on the command line.
        $check = shell_exec('which ldapsearch');
        if (empty($check)) {
            $this->error = sprintf(_("Command line programm %s is missing!"), bold('ldapsearch'));
            return (null);
        }

        // Prepare parameters to be valid for shell execution
        $dn = escapeshellarg($dn);
        $pwd = $this->bindpw;
        $host = escapeshellarg($this->hostname);
        $admin = escapeshellarg($this->binddn);
        $filter = escapeshellarg($filter);
        $cmd = "ldapsearch -x -LLLL -D {$admin} {$filter} {$limit} {$scope} -H {$host} -b {$dn} -W ";

        // Create list of process pipes
        $descriptorspec = array(
            0 => array("pipe", "r"),  // stdin
            1 => array("pipe", "w"),  // stdout
            2 => array("pipe", "w")
        ); // stderr

        // Try to open the process
        $process = proc_open($cmd, $descriptorspec, $pipes);
        if (is_resource($process)) {
            // Write the password to stdin
            fwrite($pipes[0], $pwd . "\n");
            fclose($pipes[0]);

            // Get results from stdout and stderr
            $res = stream_get_contents($pipes[1]);
            $err = stream_get_contents($pipes[2]);
            fclose($pipes[1]);

            // Close the process and check its return value
            if (proc_close($process) != 0) {
                $this->error = $err;
                return (null);
            }
        }
        return ($res);
    }


    function gen_xls($srp, $dn, $filter = "(objectClass=*)", $attributes = array('*'), $recursive = true, $r_array = 0)
    {
        $display = [];

        $this->cd($dn);
        $this->search($srp, "$filter");

        $i = 0;
        while ($attrs = $this->fetch($srp)) {
            $j = 0;

            foreach ($attributes as $at) {
                $display[$i][$j] = $this->get_attribute($attrs['dn'], $at, $r_array);
                $j++;
            }

            $i++;
        }

        return ($display);
    }


    function gen_one_entry($dn, $filter = "(objectClass=*)", $name = array("*"))
    {
        $ret = "";
        if ($this->reconnect) {
            $this->connect();
        }

        /* Searching Ldap Tree */
        $sr = ldap_read($this->cid, LDAP::fix($dn), $filter, $name);

        /* Get the first entry */
        $entry = ldap_first_entry($this->cid, $sr);

        /* Get all attributes related to that Objekt */
        $atts = [];

        /* Assemble dn */
        $atts[0]['name']  = "dn";
        $atts[0]['value'] = array('count' => 1, 0 => $dn);

        /* Reset index */
        $i = 1;
        $identifier = [];
        $attribute = ldap_first_attribute($this->cid, $entry, $identifier);
        while ($attribute) {
            $i++;
            $atts[$i]['name']  = $attribute;
            $atts[$i]['value'] = ldap_get_values_len($this->cid, $entry, "$attribute");

            /* Next one */
            $attribute = ldap_next_attribute($this->cid, $entry, $identifier);
        }

        foreach ($atts as $at) {
            for ($i = 0; $i < $at['value']['count']; $i++) {
                /* Check if we must encode the data */
                if (!preg_match('/^[a-z0-9+@#.=, \/ -]+$/i', $at['value'][$i])) {
                    $ret .= $at['name'] . ":: " . base64_encode($at['value'][$i]) . "\n";
                } else {
                    $ret .= $at['name'] . ": " . $at['value'][$i] . "\n";
                }
            }
        }

        return ($ret);
    }

    function dn_exists($dn)
    {
        return @ldap_list(self::$cid, self::fix($dn), "(objectClass=*)", array("objectClass"));
    }

    /*  This function imports ldifs
        If DeleteOldEntries is true, the destination entry will be deleted first.
        If JustModify is true the destination entry will only be touched by the attributes specified in the ldif.
        if JustMofify id false the destination dn will be overwritten by the new ldif.
     */
    function import_complete_ldif($srp, $str_attr, $error, $JustModify, $DeleteOldEntries)
    {
        if ($this->reconnect) {
            $this->connect();
        }

        /* First we have to split the string into empty lines.
           An empty line indicates an new Entry */
        $entries = preg_split("/\n/", $str_attr);

        $data = "";
        $cnt = 0;
        $current_line = 0;

        /* FIX ldif */
        $last = "";
        $tmp  = [];
        $i = 0;
        foreach ($entries as $entry) {
            if (preg_match("/^ /", $entry)) {
                $tmp[$i] .= trim($entry);
            } else {
                $i++;
                $tmp[$i] = trim($entry);
            }
        }

        /* Every single line ... */
        foreach ($tmp as $entry) {
            $current_line++;

            /* Removing Spaces to ..
               .. test if a new entry begins */
            $tmp  = str_replace(" ", "", $data);

            /* .. prevent empty lines in an entry */
            $tmp2 = str_replace(" ", "", $entry);

            /* If the Block ends (Empty Line) */
            if ((empty($entry)) && (!empty($tmp))) {
                /* Add collected lines as a complete block */
                $all[$cnt] = $data;
                $cnt++;
                $data = "";
            } else {
                /* Append lines ... */
                if (!empty($tmp2)) {
                    /* check if we need base64_decode for this line */
                    if (strstr($tmp2, "::") !== false) {
                        $encoded = explode("::", $entry);
                        $attr  = trim($encoded[0]);
                        $value = base64_decode(trim($encoded[1]));
                        /* Add linenumber */
                        $data .= $current_line . "#" . base64_encode($attr . ":" . $value) . "\n";
                    } else {
                        /* Add Linenumber */
                        $data .= $current_line . "#" . base64_encode($entry) . "\n";
                    }
                }
            }
        }

        /* The Data we collected is not in the array all[];
           For example the Data is stored like this..
           all[0] = "1#dn : .... \n
           2#ObjectType: person \n ...."
           Now we check every insertblock and try to insert */
        foreach ($all as $single) {
            $lineone = preg_split("/\n/", $single);
            $ndn = explode("#", $lineone[0]);
            $line = base64_decode($ndn[1]);

            $dnn = explode(":", $line, 2);
            $current_line = $ndn[0];
            $dn    = $dnn[0];
            $value = $dnn[1];

            /* Every block must begin with a dn */
            if ($dn != "dn") {
                $error = sprintf(_("Invalid DN %s: block to be imported should start with 'dn: ...' in line %s"), bold($line), bold($current_line));
                return -2;
            }

            /* Should we use Modify instead of Add */
            $usemodify = false;

            /* Delete before insert */
            $usermdir = false;

            /* The dn address already exists, Don't delete destination entry, overwrite it */
            if (($this->dn_exists($value)) && ((!$JustModify) && (!$DeleteOldEntries))) {
                $usermdir = $usemodify = false;

                /* Delete old entry first, then add new */
            } elseif (($this->dn_exists($value)) && ($DeleteOldEntries)) {
                /* Delete first, then add */
                $usermdir = true;
            } elseif (($this->dn_exists($value)) && ($JustModify)) {
                /* Modify instead of Add */
                $usemodify = true;
            }

            /* If we can't Import, return with a file error */
            if (!$this->import_single_entry($srp, $single, $usemodify, $usermdir)) {
                $error = sprintf(
                    _("Error while importing DN %s: please check LDIF from line %s on!"),
                    bold($line),
                    $current_line
                );
                return UNKNOWN_TOKEN_IN_LDIF_FILE;
            }
        }

        return (INSERT_OK);
    }

    /* Imports a single entry
       If $delete is true;  The old entry will be deleted if it exists.
       if $modify is true;  All variables that are not touched by the new ldif will be kept.
       if $modify is false; The new ldif overwrites the old entry, and all untouched attributes get lost.
     */
    function import_single_entry($srp, $str_attr, $modify, $delete)
    {
        global $config;

        if (!$config) {
            trigger_error("Can't import ldif, can't read config object.");
        }


        if ($this->reconnect) {
            $this->connect();
        }

        $ret = false;
        $rows = preg_split("/\n/", $str_attr);
        $data = false;

        foreach ($rows as $row) {
            /* Check if we use Linenumbers (when import_complete_ldif is called we use
               Linenumbers) Linenumbers are use like this 123#attribute : value */
            if (!empty($row)) {
                if (strpos($row, "#") != false) {
                    /* We are using line numbers
                       Because there is a # before a : */
                    $tmp1 = explode("#", $row);
                    $current_line = $tmp1[0];
                    $row = base64_decode($tmp1[1]);
                }

                /* Split the line into  attribute  and value */
                $attr   = explode(":", $row, 2);
                $attr[0] = trim($attr[0]);  /* attribute */
                $attr[1] = $attr[1];  /* value */

                /* Check :: was used to indicate base64_encoded strings */
                if ($attr[1][0] == ":") {
                    $attr[1] = trim(preg_replace("/^:/", "", $attr[1]));
                    $attr[1] = base64_decode($attr[1]);
                }

                $attr[1] = trim($attr[1]);

                /* Check for attributes that are used more than once */
                if (!isset($data[$attr[0]])) {
                    $data[$attr[0]] = $attr[1];
                } else {
                    $tmp = $data[$attr[0]];

                    if (!is_array($tmp)) {
                        $new[0] = $tmp;
                        $new[1] = $attr[1];
                        $datas[$attr[0]]['count'] = 1;
                        $data[$attr[0]] = $new;
                    } else {
                        $cnt = $data[$attr[0]]['count'];
                        $cnt++;
                        $data[$attr[0]][$cnt] = $attr[1];
                        $datas[$attr[0]]['count'] = $cnt;
                    }
                }
            }
        }

        /* If dn is an index of data, we should try to insert the data */
        if (isset($data['dn'])) {
            /* Fix dn */
            $tmp = gosa_ldap_explode_dn($data['dn']);
            unset($tmp['count']);
            $newdn = "";
            foreach ($tmp as $tm) {
                $newdn .= trim($tm) . ",";
            }
            $newdn = preg_replace("/,$/", "", $newdn);
            $data['dn'] = $newdn;

            /* Creating Entry */
            $this->cd($data['dn']);

            /* Delete existing entry */
            if ($delete) {
                $this->rmdir_recursive($srp, $data['dn']);
            }

            /* Create missing trees */
            $this->cd($this->basedn);
            $this->cd($config->current['BASE']);
            $this->create_missing_trees($srp, preg_replace("/^[^,]+,/", "", $data['dn']));
            $this->cd($data['dn']);

            $dn = $data['dn'];
            unset($data['dn']);

            if (!$modify) {
                $this->cat($srp, $dn);
                if ($this->count($srp)) {
                    /* The destination entry exists, overwrite it with the new entry */
                    $attrs = $this->fetch($srp);
                    foreach ($attrs as $name => $value) {
                        if (!is_numeric($name)) {
                            if (in_array_strict($name, array("dn", "count"))) {
                                continue;
                            }
                            if (!isset($data[$name])) {
                                $data[$name] = [];
                            }
                        }
                    }
                    $ret = $this->modify($data);
                } else {
                    /* The destination entry doesn't exists, create it */
                    $ret = $this->add($data);
                }
            } else {
                /* Keep all vars that aren't touched by this ldif */
                $ret = $this->modify($data);
            }
        }

        if (!$this->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($this->get_error(), $dn, "", get_class()));
        }

        return ($ret);
    }


    function importcsv($str)
    {
        $lines = preg_split("/\n/", $str);
        foreach ($lines as $line) {
            /* continue if theres a comment */
            if (substr(trim($line), 0, 1) == "#") {
                continue;
            }

            $line = str_replace("\t\t", "\t", $line);
            $line = str_replace("\t", ",", $line);
            echo $line;

            $cells = explode(",", $line);
            $linet = str_replace("\t\t", ",", $line);
            $cells = preg_split("/\t/", $line);
            $count = count($cells);
        }
    }

    function get_objectclasses($force_reload = false)
    {
        $objectclasses = [];
        global $config;

        /* Return the cached results. */
        if (class_available('session') && session::global_is_set("LDAP_CACHE::get_objectclasses") && !$force_reload) {
            $objectclasses = session::global_get("LDAP_CACHE::get_objectclasses");
            return ($objectclasses);
        }

        # Make sure we are connected to LDAP.
        $_cid = self::$cid;
        if (is_null($_cid)) {
            self::connect();
            $_cid = self::$cid;
        }

        # Get base to look for schema
        $sr = ldap_read($_cid, "", "objectClass=*", array("subschemaSubentry"));
        $attr = ldap_get_entries($_cid, $sr);
        if (!isset($attr[0]['subschemasubentry'][0])) {
            return [];
        }

        /* Get list of objectclasses and fill array */
        $nb = $attr[0]['subschemasubentry'][0];
        $objectclasses = [];
        $sr = ldap_read($_cid, $nb, "objectClass=*", array("objectclasses"));
        $attrs = ldap_get_entries($_cid, $sr);
        if (!isset($attrs[0])) {
            return [];
        }
        foreach ($attrs[0]['objectclasses'] as $val) {
            if (preg_match('/^[0-9]+$/', $val)) {
                continue;
            }
            $name = "OID";
            $value = "";
            $pattern = explode(' ', $val);
            $ocname = preg_replace("/^.* NAME\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $val);
            $objectclasses[$ocname] = [];

            foreach ($pattern as $chunk) {
                switch ($chunk) {
                    case '(':
                        $value = "";
                        break;

                    case ')':
                        if ($name != "") {
                            $v = $this->value2container($value);
                            if (in_array_strict($name, array('MUST', 'MAY')) && !is_array($v)) {
                                $v = array($v);
                            }
                            $objectclasses[$ocname][$name] = $v;
                        }
                        $name = "";
                        $value = "";
                        break;

                    case 'NAME':
                    case 'DESC':
                    case 'SUP':
                    case 'STRUCTURAL':
                    case 'ABSTRACT':
                    case 'AUXILIARY':
                    case 'MUST':
                    case 'MAY':
                        if ($name != "") {
                            $v = $this->value2container($value);
                            if (in_array_strict($name, array('MUST', 'MAY')) && !is_array($v)) {
                                $v = array($v);
                            }
                            $objectclasses[$ocname][$name] = $v;
                        }
                        $name = $chunk;
                        $value = "";
                        break;

                    default:
                        $value .= $chunk . " ";
                }
            }
        }
        if (class_available("session")) {
            session::global_set("LDAP_CACHE::get_objectclasses", $objectclasses);
        }

        return $objectclasses;
    }

    function value2container($value)
    {
        /* Set emtpy values to "true" only */
        if (preg_match('/^\s*$/', $value)) {
            return true;
        }

        /* Remove ' and " if needed */
        $value = preg_replace('/^[\'"]/', '', $value);
        $value = preg_replace('/[\'"] *$/', '', $value);

        /* Convert to array if $ is inside... */
        if (preg_match('/\$/', $value)) {
            $container = preg_split('/\s*\$\s*/', $value);
        } else {
            $container = chop($value);
        }

        return ($container);
    }


    function log($string)
    {
        if (session::global_is_set('config')) {
            $cfg = session::global_get('config');
            if (isset($cfg->current['LDAPSTATS']) && preg_match('/true/i', $cfg->current['LDAPSTATS'])) {
                syslog(LOG_INFO, $string);
            }
        }
    }

    /* added by Guido Serra aka Zeph <zephpurotesto.it> */
    function getCn($dn)
    {
        $simple = explode(",", $dn);

        foreach ($simple as $piece) {
            $partial = explode("=", $piece);

            if ($partial[0] == "cn") {
                return $partial[1];
            }
        }
    }


    static function get_naming_contexts($server, $admin = "", $password = "")
    {
        /* Build LDAP connection */
        $ds = ldap_connect($server);
        if (!$ds) {
            die("Can't bind to LDAP. No check possible!");
        }
        ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
        $r = ldap_bind($ds, $admin, $password);

        /* Get base to look for naming contexts */
        $sr  = ldap_read($ds, "", "objectClass=*", array(0 => "+"));
        $attr = ldap_get_entries($ds, $sr);

        return ($attr[0]['namingcontexts']);
    }


    function get_root_dse($server, $admin = "", $password = "")
    {
        /* Build LDAP connection */
        $ds = ldap_connect($server);
        if (!$ds) {
            die("Can't bind to LDAP. No check possible!");
        }
        ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
        $r = ldap_bind($ds, $admin, $password);

        /* Get base to look for naming contexts */
        $sr  = ldap_read($ds, "", "objectClass=*", array("+"));
        $attr = ldap_get_entries($ds, $sr);

        /* Return empty array, if nothing was set */
        if (!isset($attr[0])) {
            return [];
        }

        /* Rework array... */
        $result = [];
        for ($i = 0; $i < $attr[0]['count']; $i++) {
            $result[$attr[0][$i]] = $attr[0][$attr[0][$i]];
            unset($result[$attr[0][$i]]['count']);
        }

        return ($result);
    }

    function errno(): int
    {
        return (ldap_errno(self::$cid));
    }

    function __serialize(): array
    {
        $this->re = [];
        return (array)$this;
    }
}
