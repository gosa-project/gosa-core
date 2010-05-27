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


class Step_Schema extends setup_step
{
    var $checked = array();
    var $not_checked = array();
    var $b_displayCheckbutton = true;

    function Step_Schema()
    {
        $this->update_strings();
    }


    function update_strings()
    {
        $this->s_title      = _("LDAP schema check");
        $this->s_title_long = _("LDAP schema check");
        $this->s_info       = _("Perform test on your current LDAP schema");
    }


    function execute()
    {
        // Establish ldap connection
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        // Detect Schema Info
        $ldap = new ldapMultiplexer($ldap_l);
        $ldap->cd($cv['base']);
        $objectclasses = $ldap->get_objectclasses();
   
        // Check if we can find something
        $ldap->set_size_limit(1);
        $res = $ldap->search("(objectClass=*)");  
        $ldap->set_size_limit(0);
    
        // Validate schema
        $cR = new configRegistry(NULL);;
        $cR->validateSchemata(TRUE,TRUE,$objectclasses);
        $info = $cR->getSchemaResults();
        $disabled = $cR->getDisabledPlugins();

        // Check if the 'AT LEAST' required classes are available.  
        $smarty = get_smarty();
        $smarty->assign('message', "");
        $smarty->assign('database_initialised', ($res==TRUE));
        $smarty->assign('ocCount', count($objectclasses));

        // We are fine here once we got object classes. 
        $this->is_completed = count($objectclasses);

        // Now check if the core requirements are fulfilled.
        if(in_array('core', $disabled)){
            $message = "";
            $this->is_completed = FALSE;
            $coreDefs = core::plInfo();
            $coreRequired = $coreDefs['plRequirements']['ldapSchema'];
            $missing = $version = array();
            foreach($coreRequired as $oc => $requirement){
                if(isset($info['missing'][$oc])){
                    $missing[$oc] = $info['missing'][$oc];
                }
                if(isset($info['versionMismatch'][$oc])){
                    $version[$oc] = $info['versionMismatch'][$oc];
                }
            }

            $message .= "<hr>";
            if(count($missing)){
                $message.= _("The following object classes are missing:").msgPool::buildList(array_values($missing));
            }
            if(count($version)){
                $message.= _("The following object classes are outdated:").msgPool::buildList(array_values($version));
            }
            $smarty->assign('message', $message);
        }
        $smarty->assign('checkFailed', !$this->is_completed);
        return($smarty->fetch (get_template_path("../setup/setup_schema.tpl")));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
