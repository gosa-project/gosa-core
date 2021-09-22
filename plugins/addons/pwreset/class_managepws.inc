<?php

/*
  This code is an addon for GOsa (https://gosa.gonicus.de)
  Copyright (C) 2015 Mike Gabriel

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

class managepws extends plugin
{
    /* Definitions */
    var $plHeadline= "Reset Passwords";
    var $plDescription= "GOsa2 Password Management Add-On";
    var $access= "";

    /* Array with csv informations*/
    var $csvinfo=array();

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();
    var $view_logged = FALSE;

    function __construct(&$config, $dn= NULL)
    {
        $this->initTime = microtime(TRUE);

        /* Include config object */
        $this->config= &$config;

        $this->ui = get_userinfo();
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                   $amount = 1, $duration = (microtime(TRUE) - $this->initTime));
    }

    function getAttributes()
    {
        /* columns in pw reset CSV */
        $attrs= array ();

	/* passwords can be reset for uid DN objects or cn DN objects */
        $attrs[0]= "uid";
        $attrs[1]= "cn";
        $attrs[2]= "userPassword";

        return $attrs;
    }

    function execute()
    {

        /* Call parent execute */
        plugin::execute();

        /* Log view */
        if(!$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","all/".get_class($this),$this->dn);
        }

        /* initiate smarty */
        $smarty= get_smarty();

        /* PHASE 01 done, set $pwreset_configured to TRUE  */
        $smarty->assign ("pwreset_configured",0);

        /* PHASE 02 done, set $pwreset_reviewed to TRUE  */
        $smarty->assign ("pwreset_reviewed",0);

        /* Get the LDAP link, to generate the Export */
        $this->_ldap=$this->config->get_ldap_link ();

        /* initialize CSV Info array */
        if (!is_array ($this->csvinfo)) {
            $this->csvinfo=array ();
        }

        /* Check permissions for pwreset */
        $acl=$this->ui->get_permissions ($this->config->current['BASE'],"all/all");
        if (!preg_match ("/w/",$acl)) {
            if (isset ($_POST['pwreset_configured'])||isset ($_POST['pwreset_reviewed'])) {
                msg_dialog::display (_ ("Permission error"),_ ("You have no permission to reset passwords!"),ERROR_DIALOG);
            }
            return ($smarty->fetch (get_template_path ('content_pwreset.tpl',TRUE)));
        }

        $this->ou_tree = array();
        $this->ou_tree['ldapsearch'] = $this->_ldap->search("(objectClass=gosaDepartment)",array("ou","description"));

        /* create arrays for search results */
        $this->ou_tree['formfields'] = array();
        $this->ou_tree['OUs'] = array();

        /* add found gosaDepartment objects */
        while($result = $this->_ldap->fetch($this->ou_tree['ldapsearch'])) {
            $this->ou_tree['OUs'][] = $result['ou'][0];
            $this->ou_tree['formfields'][] = $result['ou'][0]." - ".$result['description'][0];
            $this->ou_tree['DNs'][] = $result['dn'];
        }

        /*
         * PHASES
         */

        /* reset our failure status from last template view... */
        $this->failure_in_this_phase = FALSE;

        /*
         * prepare PHASE 01
         */

        /* Search OUs with objectClass gosaDepartment */
        if(isset($_POST['subtree_ou_id'])) {
            $this->subtree_ou_id = $_POST['subtree_ou_id'];
        }
        if(!isset($this->subtree_ou_id)) {
            $this->subtree_ou_id = 0;
        }

        /* pwreset configuration defaults */
        if(isset($_POST['pwreset_mode'])) {
            $this->pwreset_mode = $_POST['pwreset_mode'];
        }
        if(!isset($this->pwreset_mode)) {
            $this->pwreset_mode= 0;
        }

        /* pwreset configuration defaults */
        if(isset($_POST['pwlength'])) {
            $this->pwlength = $_POST['pwlength'] + 8;
        }
        if(!isset($this->pwlength)) {
            $this->pwlength= 12;
        }

        if (isset ($_POST['phase_00']) || ((!isset ($_POST['phase_01'])) && (!isset ($_POST['phase_02'])))) {

            $smarty->assign("ous_available",$this->ou_tree['formfields']);

            /* provide pre-set values for account template forms */
            $smarty->assign("preset_pwreset_mode", $this->pwreset_mode);
            $smarty->assign("preset_subtree_ou_id", $this->subtree_ou_id);
            $smarty->assign("preset_pwlength", $this->pwlength);

        }

        /*
         * PHASE 01
         */
        elseif (isset ($_POST['phase_01'])) {

            /*
             * PW Reset Mode "0" is CSV file import
             */

            if(!isset($_POST['pwreset_mode'])) {
                $this->failure_in_this_phase = TRUE;
            }
            elseif ($_POST['pwreset_mode'] == 0) {

                /* Check if theres a file uploaded */
                if (!empty ($_FILES['credfile']['name'])) {
                    $handle=NULL;
                    $filename=gosa_file_name ($_FILES['credfile']['tmp_name']);
                    if ( (!isset ($_FILES['credfile']['name']))|| (!isset ($_POST['btn_pwreset_configured']))) {
                        $this->failure_in_this_phase = TRUE;
                        msg_dialog::display (_ ("Error"),sprintf (_ ("Cannot read uploaded file: %s"),_ ("file not found")),ERROR_DIALOG);
                    }
                    elseif (!$_FILES['credfile']['size']>0) {
                        $this->failure_in_this_phase = TRUE;
                        msg_dialog::display (_ ("Error"),sprintf (_ ("Cannot read uploaded file: %s"),_ ("file is empty")),ERROR_DIALOG);
                    }
                    /* Is there a tmp file, which we can use ? */
                    elseif (!file_exists ($filename)) {
                        $this->failure_in_this_phase = TRUE;
                        msg_dialog::display (_ ("Error"),sprintf (_ ("Cannot read uploaded file: %s"),_ ("file not found")),ERROR_DIALOG);
                    }
                    elseif (!$handle=@fopen ($filename,"r")) {
                        $this->failure_in_this_phase = TRUE;
                        msg_dialog::display (_ ("Error"),sprintf (_ ("Cannot read uploaded file: %s"),_ ("file not readable")),ERROR_DIALOG);
                    }
                    else {
                        $smarty->assign ("file_uploaded",TRUE);
                        $raw_csv_data="";

                        /* Reading content */
                        while (!feof ($handle)) {
                            $raw_csv_data.=fread ($handle,1024);
                        }

                        @fclose ($handle);

                        if(!mb_check_encoding($raw_csv_data, "UTF-8"))
                        {
                            if(mb_check_encoding($raw_csv_data, "iso-8859-1"))
                            {
                                $raw_csv_data = utf8_encode($raw_csv_data);
                            } else
                            {
                                $smarty->assign ("LDIFError",TRUE);
                                $smarty->assign ("file_uploaded",FALSE);
                                msg_dialog::display (_ ("Error"),_ ("File has an unsupported encoding!"),ERROR_DIALOG);
                            }
                        }

                        $parser_ok = $this->parseCSV($raw_csv_data);

                        if ($parser_ok!==FALSE)
                        {
                            $this->accounts = array();
                            foreach($this->csvinfo['data'] as $row)
                            {
                                $account = array();
                                $account['uid'] = $row[0];
                                $account['checked'] = TRUE;
                                $account['userPassword'] = $row[1];
                                $this->_ldap->cd($this->config->current['BASE']);
                                $_ldapsearch = $this->_ldap->search("(uid=".$account['uid'].")",array("dn","cn"));
                                $ldap_object = $this->_ldap->fetch($_ldapsearch);
                                if(isset($ldap_object['cn'][0])) {
                                    $account['cn'] = $ldap_object['cn'][0];
                                }
                                else {
                                    $account['cn'] = _('not set');
                                }
                                if(isset($ldap_object['dn'])) {
                                    $account['dn'] = $ldap_object['dn'];
                                    $account['valid'] = TRUE;
                                }
                                else {
                                    $account['dn'] = _("no such user object in LDAP database");
                                    $account['valid'] = FALSE;
                                }
                                $this->accounts[] = $account;
                            }
                            if(empty($this->accounts)) {
                                msg_dialog::display (_ ("No Accounts Found"),_ ("No valid LDAP user could be found in the provided CSV data table!"),ERROR_DIALOG);
                            }
                        }
                        else {
                            $this->failure_in_this_phase = TRUE;
                            msg_dialog::display (_("Error"),_("Password reset via CSV data").": "._("Cannot find CSV data in the selected file!"),ERROR_DIALOG);
                        }
                    }
                }
                else {
                    $this->failure_in_this_phase = TRUE;
                    msg_dialog::display (_("Error"),_("Password reset via CSV data").": "._("No CSV file has been selected for upload!"),ERROR_DIALOG);
                }
            }

            /*
             * PW Reset Mode "0" is password generation for a chosen subtree OU
             */

            elseif ($_POST['pwreset_mode'] == 1) {

                if(isset($_POST['pwlength'])) {
                    $this->pwlength = $_POST['pwlength'] + 8;
                }

                if(isset($_POST['subtree_ou_id'])) {
                    $subtree_ou_id = $_POST['subtree_ou_id'];
                    $search_base = $this->ou_tree['DNs'][$subtree_ou_id];
                }
                else {
                    $search_base = $this->config->current['BASE'];
                }
                $this->_ldap->cd($search_base);
                $_search = $this->_ldap->search("(&(uid=*)(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))))",array("uid","cn"));

                $this->accounts = array();
                while ($ldap_object = $this->_ldap->fetch($_search)) {
                    $account = array();
                    $account['dn'] = $ldap_object['dn'];
                    $account['checked'] = TRUE;
                    $account['uid'] = $ldap_object['uid'][0];
                    $account['cn'] = $ldap_object['cn'][0];
                    $account['valid'] = TRUE;
                    $this->accounts[] = $account;
                }
                foreach($this->accounts as $key => $account) {
                    if(!isset($account['userPassword'])) {
                        $this->accounts[$key]['userPassword'] = $this->randomPassword($this->pwlength);
                    }
                }
                if(empty($this->accounts)) {
                    msg_dialog::display (_ ("No Accounts Found"),_ ("In this part of the LDAP tree no user accounts could be found!"),ERROR_DIALOG);
                }
            }

            if($this->failure_in_this_phase===FALSE)
            {
                $smarty->assign ("pwreset_configured",TRUE);
                $smarty->assign ("accounts", $this->accounts);
            }
            else {
                /* prepare for reloading this phase's web page again */
                $smarty->assign("ous_available",$this->ou_tree['formfields']);
                $smarty->assign("preset_pwreset_mode", $this->pwreset_mode);
                $smarty->assign("preset_subtree_ou_id", $this->subtree_ou_id);
                $smarty->assign("preset_pwlength", $this->pwlength);
            }

        }

        /*
         * PHASE 02
         */
        elseif (isset ($_POST['phase_02'])) {

            $smarty->assign ("pwreset_configured",TRUE);

            /* handle accounts being marked for being skipping */
            if(isset($_POST['accounts'])) {
                foreach($this->accounts as $key => $account) {
                    $this->accounts[$key]['checked'] = FALSE;
                }
                foreach($_POST['accounts'] as $cnt => $key) {
                    $this->accounts[$key]['checked'] = TRUE;
                }
            }

            /* handle on-screen modifications of user passwords */
            foreach($_POST as $key => $val) {
                if (preg_match('/^password_([0-9]+)/', $key)) {
                    $idx= preg_replace('/^password_([0-9]+)/', '${1}', $key);
                    $this->accounts[$idx]['userPassword']= $val;
                }
            }

            foreach($this->accounts as $key => $account) {
                if($account['valid'] && $account['checked']) {

                    if(!change_password($account['dn'],$account['userPassword'],FALSE,'','', $message)) {
                        msg_dialog::displayChecks(array($message));
                        $this->accounts[$key]['status'] = "failed";
                    }
                    else {
                        $this->accounts[$key]['status'] = "ok";
                    }

                }
                elseif(!$account['valid']) {
                    $this->accounts[$key]['status'] = "ignored";
                }
                else {
                    $this->accounts[$key]['status'] = "skipped";
                }
            }

            $smarty->assign ("pwreset_reviewed",TRUE);
            $smarty->assign ("accounts", $this->accounts);
        }

        /* Show main page */
        return ($smarty->fetch (get_template_path('content_managepws.tpl', TRUE)));
    }

    function parseCSV($raw_csv_data)
    {
        $lines=preg_split ("/\n/",$raw_csv_data);
        $num_columns=0;
        $rest=0;
        $data=array ();

        /* check colum count, if it stays zero, we probably don't have a comma separated CSV file */
        if (is_array ($lines))
            foreach ($lines as $line) {

                /* continue if theres a comment */
                if (substr (trim ($line),0,1)=="#") {
                    continue;
                }
                $line=str_replace ("\t","",$line);
                $cells=explode (",",$line);
                if (count ($cells)>$num_columns) {
                    $num_columns=count ($cells);
                }
            }

        /* parse rows and import into $this->csvinfo */
        if ($num_columns > 1) {

            /* Generate array with output info  */
            if (is_array ($lines))
                foreach ($lines as $line) {
                    $rest=0;
                    $cnt=0;

                    /* do not use comments or empty lines */
                    if ( (substr (trim ($line),0,1)=="#")|| (empty ($line))) {
                        continue;
                    }

                    /* replace \t to , */
                    $line=str_replace ("\t",",",$line);

                    /* get all elements  */
                    $cells=explode (",",$line);

                    /* attach all elements to data array */
                    if (is_array ($cells))

                        /* cell count less than num_columns, attach some empty fields */
                        if ( count($cells) <= $num_columns ) {
                            $rest=$num_columns - count($cells);
                            for ($i=0;$i<$rest;$i++) {
                                $cells[]="";
                            }
                        }
                    $data[]=$cells;
                }
            unset ($this->csvinfo['data_sorted']);
            $this->csvinfo['num_cols']=$num_columns;
            $this->csvinfo['data']=$data;
            $this->csvinfo['num_rows']=count($this->csvinfo['data']);
            return TRUE;
        }
        return FALSE;
    }

    function randomPassword($length)
    {
        $alphabet = "abcdefghijklmnopqrstuwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789";
        $pass = array(); //remember to declare $pass as an array
        $alphaLength = strlen($alphabet) - 1; //put the length -1 in cache
        for ($i = 0; $i < $length; $i++) {
            $n = rand(0, $alphaLength);
            $pass[] = $alphabet[$n];
        }
        return implode($pass); //turn the array into a string
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
