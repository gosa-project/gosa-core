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

class ogroup extends plugin
{
    public $typeToClass = array(
            "Y" => "gosaUserTemplate",
            "U" => "gosaAccount",
            "G" => "posixGroup",
            "A" => "gosaApplication",
            "D" => "gosaDepartment",
            "S" => "goServer",
            "W" => "gotoWorkstation",
            "O" => "opsiClient",
            "T" => "gotoTerminal",
            "F" => "goFonHardware",
            "P" => "gotoPrinter");

    public $typeToImage = array();
            


    /* Variables */
    public $cn= "";
    public $description= "";
    public $base= "";
    public $gosaGroupObjects= "";
    public $objects= array();
    public $objcache= array();
    public $memberList= array();
    public $member= array();
    public $orig_dn= "";
    public $orig_cn= "";
    public $orig_base= "";
    public $objectSelect= false;
    public $view_logged = false;

    public $copyMembers = true;
    public $wasDyGroup = false;
    public $baseSelector;

    /* Already assigned Workstations. Will be hidden in selection.
     */
    public $used_workstations = array();

    var $theme;

    /* attribute list for save action */
    public $attributes= array("cn", "description", "gosaGroupObjects","member");
    public $objectclasses= array("top", "gosaGroupOfNames");

    public function __construct(&$config, $dn= null)
    {
        $this->theme = getThemeName();
        switch ($this->theme) {
            case 'classic':
                $this->typeToImage = array(
                    "Y" => "plugins/users/images/select_template.png",
                    "U" => "plugins/users/images/select_user.png",
                    "G" => "plugins/groups/images/select_group.png",
                    "A" => "plugins/ogroups/images/application.png",
                    "D" => "plugins/departments/images/department.png",
                    "S" => "plugins/ogroups/images/server.png",
                    "W" => "plugins/ogroups/images/workstation.png",
                    "O" => "plugins/ogroups/images/winstation.png",
                    "T" => "plugins/ogroups/images/terminal.png",
                    "F" => "plugins/ogroups/images/phone.png",
                    "P" => "plugins/ogroups/images/printer.png",
                    "I" => "images/false.png"
                );
                break;
            
            default:
                $this->typeToImage = array(
                    "Y" => "<i class='material-icons'>web_asset</i>",
                    "U" => "<i class='material-icons'>person</i>",
                    "G" => "<i class='material-icons'>groups</i>",
                    "A" => "<i class='material-icons'>apps</i>",
                    "D" => "<i class='material-icons'>domain</i>",
                    "S" => "<i class='material-icons'>dns</i>",
                    "W" => "<i class='material-icons'>computer</i>",
                    "O" => "<i class='material-icons'>desktop_windows</i>",
                    "T" => "<i class='material-icons'>monitor</i>",
                    "F" => "<i class='material-icons'>phone</i>",
                    "P" => "<i class='material-icons'>printer</i>",
                    "I" => "<i class='material-icons'>not_interested</i>"
                );
                break;
        }

        plugin::__construct($config, $dn);

        $this->trustModeDialog = new trustModeDialog($this->config, $this->dn, null);
        $this->trustModeDialog->setAcl('ogroups/ogroup');

        $this->orig_dn= $dn;

        $this->member = array();

        /* Load member objects */
        if (isset($this->attrs['member'])) {
            foreach ($this->attrs['member'] as $key => $value) {
                if ("$key" != "count") {
                    $value= @LDAP::convert($value);
                    $this->member["$value"]= "$value";
                }
            }
        }
        $this->is_account= true;

        /* Set base */
        if ($this->dn == "new") {
            $ui = get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } else {
            $this->base= preg_replace("/^[^,]+,".preg_quote(get_ou("group", "ogroupRDN"), '/')."/i", "", $this->dn);
        }

        /* Detect all workstations, which are already assigned to an object group
           - Those objects will be hidden in the add object dialog.
           - Check() will complain if such a system is assigned to this object group.
         */
        $base = $this->config->current['BASE'];
        $res    = get_list(
            "(|(objectClass=gotoWorkstation)(objectClass=gotoTerminal))",
            "none",
            $base,
            array("dn"),
            GL_NO_ACL_CHECK|GL_SUBSEARCH
        );
        $ws_dns = array();
        foreach ($res as $data) {
            $ws_dns[] = $data['dn'];
        }
        $res=get_list(
            "(&(member=*)(objectClass=gosaGroupOfNames))",
            "none",
            $base,
            array("dn","member", "gosaGroupObjects"),
            GL_NO_ACL_CHECK|GL_SUBSEARCH
        );
        $this->used_workstations = array();
        foreach ($res as $og) {
            if ($og['dn'] == $this->dn) {
                continue;
            }
            $test = array_intersect($ws_dns, LDAP::convert($og['member']));
            if (($og['gosaGroupObjects'] == "[W]" || $og['gosaGroupObjects'] == "[T]") && count($test)) {
                $this->used_workstations = array_merge($this->used_workstations, $test);
            }
        }

        $this->orig_cn = $this->cn;
        $this->orig_base = $this->base;

        /* Get global filter config */
        if (!session::is_set("sysfilter")) {
            $ui= get_userinfo();
            $base= get_base_from_people($ui->dn);
            $sysfilter= array( "depselect"       => $base,
                    "regex"           => "*");
            session::set("sysfilter", $sysfilter);
        }

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        // Prepare lists
        $this->memberListing = new sortableListing();
        $this->memberListing->setDeleteable(true);
        $this->memberListing->setInstantDelete(false);
        $this->memberListing->setEditable(false);
        $this->memberListing->setWidth("100%");
        $this->memberListing->setHeight("300px");
        $this->memberListing->setHeader(array("~",_("Name")));
        $this->memberListing->setColspecs(array('20px','*','20px'));
        $this->memberListing->setDefaultSortColumn(1);

        $this->reload();
    }

    public function AddDelMembership($NewMember = false)
    {
        if ($NewMember) {

            // Ensure that we definitely know the new members attributes.
            //  - Fetch unknown objects here.
            if (!isset($this->memberList[$NewMember])) {
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($this->config->current['BASE']);
                $ldap->cat($NewMember);
                $attrs = $ldap->fetch();
                $this->objcache[$NewMember] = $attrs;
            }

            /* Add member and force reload */
            $this->member[$NewMember]= $NewMember;


            $this->memberList[$NewMember]= $this->objcache[$NewMember];
            unset($this->objects[$NewMember]);
            reset($this->memberList);
            $this->reload();
        } else {

            // Act on list modifications
            $this->memberListing->save_object();
            $action = $this->memberListing->getAction();
            if ($action['action'] == 'delete') {
                foreach ($action['targets'] as $id) {
                    $value = $this->memberListing->getKey($id);
                    $this->objects["$value"]= $this->memberList[$value];
                    unset($this->memberList["$value"]);
                    unset($this->member["$value"]);
                }
                $this->reload();
            }


            /* Add objects to group */
            if ((isset($_POST['objectSelect_save']) || isset($_POST['ok-save'])) && $this->objectSelect instanceof objectSelect) {
                $objects = $this->objectSelect->save();
                $skipped = false;
                foreach ($objects as $object) {
                    $dn = $object['dn'];

                    // Do not add existing members twice!
                    if (isset($this->member["$dn"])) {
                        continue;
                    }

                    $tmp = "";
                    foreach ($this->memberList as $obj) {
                        $tmp .= $obj['type'];
                    }

                    $type  = $this->getObjectType($object);
                    $name= $this->getObjectName($object);

                    /* Fill array */
                    if (isset($object["description"][0])) {
                        $object= array("text" => "$name [".$object["description"][0]."]", "type" => "$type");
                    } elseif (isset($object["uid"][0])) {
                        $object= array("text" => "$name [".$object["uid"][0]."]", "type" => "$type");
                    } else {
                        $object= array("text" => "$name", "type" => "$type");
                    }

                    if (preg_match("/T/", $tmp) && $type == "W") {
                        $skipped =true;
                    } elseif (preg_match("/W/", $tmp) && $type == "T") {
                        $skipped =true;
                    } else {
                        $this->memberList["$dn"]= $object;
                        $this->member["$dn"]= $dn;
                        reset($this->memberList);
                    }
                }
                if ($skipped) {
                    msg_dialog::display(_("Information"), _("You cannot combine terminals and workstations in one object group!"), INFO_DIALOG);
                }
                $this->objectSelect= false;
                $this->dialog= false;
                $this->reload();
            }
        }
    }

    public function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if (!$this->view_logged) {
            $this->view_logged = true;
            new log("view", "ogroups/".get_class($this), $this->dn);
        }


        /* Do we represent a valid group? */
        if (!$this->is_account) {
            switch ($this->theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
            }
            $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("object group"))."</div>";
            return ($display);
        }


        /* Load templating engine */
        $smarty= get_smarty();


        $tmp = $this->plInfo();
        foreach ($tmp['plProvidedAcls'] as $name => $translation) {
            $smarty->assign($name."ACL", $this->getacl($name));
        }

        /***********
         * Trusts
         ***********/

        // Handle trust mode dialog
        $this->dialog = false;
        $trustModeDialog = $this->trustModeDialog->execute();
        if ($this->trustModeDialog->trustSelect) {
            $this->dialog = true;
            return($trustModeDialog);
        }
        $smarty->assign("trustModeDialog", $trustModeDialog);

        /***********
         * Ende - Trusts
         ***********/


        /* Add objects? */
        if (isset($_POST["edit_membership"])) {
            $this->objectSelect= new objectSelect($this->config, get_userinfo());
        }

        /* Add objects finished? */
        if (isset($_POST["objectSelect_cancel"]) || isset($_POST['cancel-abort'])) {
            $this->objectSelect= false;
        }

        /* Manage object add dialog */
        if ($this->objectSelect) {
            session::set('filterBlacklist', array('dn'=> $this->member));
            $this->dialog= true;
            return($this->objectSelect->execute());
        }

        /* Assemble combine string */
        if ($this->gosaGroupObjects == "[]") {
            $smarty->assign("combinedObjects", _("none"));
        } elseif (strlen($this->gosaGroupObjects) > 4) {
            $smarty->assign("combinedObjects", "<font color=red>"._("too many different objects!")."</font>");
        } else {
            $conv= array(   "U" => _("users"),
                    "G" => _("groups"),
                    "A" => _("applications"),
                    "D" => _("departments"),
                    "S" => _("servers"),
                    "W" => _("workstations"),
                    "O" => _("Windows workstations"),
                    "T" => _("terminals"),
                    "F" => _("phones"),
                    "P" => _("printers"));

            $type= preg_replace('/[\[\]]/', '', $this->gosaGroupObjects);
            $p1= $conv[$type[0]];
            error_reporting(0);
            if (isset($type[1]) && preg_match('/[UGADSFOWTP]/', $type[1])) {
                $p2= $conv[$type[1]];
                $smarty->assign("combinedObjects", sprintf("'%s' and '%s'", $p1, $p2));
            } else {
                $smarty->assign("combinedObjects", "$p1");
            }
            error_reporting(E_ALL | E_STRICT);
        }

        /* Assign variables */
        $smarty->assign("base", $this->baseSelector->render());



        $this->memberListing->setAcl($this->getacl("member"));
        $data = $lData = array();
        foreach ($this->member as $key => $dn) {
            $image = 'images/lists/element.png';
            $name = $dn;
            if (isset($this->memberList[$dn])) {
                $name  = $this->memberList[$dn]['text'];
                if (isset($this->typeToImage[$this->memberList[$dn]['type']])) {
                    $image = $this->typeToImage[$this->memberList[$dn]['type']];
                }
            }
            $data[$key] = $dn;
            $lData[$key] = array('data'=> array(image($image),$name));
        }

        if ($this->isRestrictedByDynGroup()) {
            $this->memberListing->setDeleteable(false);
            $smarty->assign("memberACL", preg_replace("/[^r]/", "", $this->getacl("member")));
            $smarty->assign("isRestrictedByDynGroup", true);
        } else {
            $this->memberListing->setDeleteable(true);
            $smarty->assign("isRestrictedByDynGroup", false);
        }

        $this->memberListing->setListData($data, $lData);
        $this->memberListing->update();
        $smarty->assign("memberList", $this->memberListing->render());

        /* Fields */
        foreach ($this->attributes as $val) {
            $smarty->assign("$val", set_post($this->$val));
        }

        return ($smarty->fetch(get_template_path('generic.tpl', true)));
    }


    public function isRestrictedByDynGroup()
    {
        $bool = false;
        if (isset($this->parent->by_object['DynamicLdapGroup'])) {
            $bool = $this->parent->by_object['DynamicLdapGroup']->isAttributeDynamic('member');
        }
        $this->wasDyGroup |= $bool;
        return($bool);
    }


    public function set_acl_base($base)
    {
        plugin::set_acl_base($base);
        $this->trustModeDialog->set_acl_base($base);
    }


    /* Save data to object */
    public function save_object()
    {
        /* Save additional values for possible next step */
        if (isset($_POST['ogroupedit'])) {
            $this->trustModeDialog->save_object();

            /* Create a base backup and reset the
               base directly after calling plugin::save_object();
               Base will be set seperatly a few lines below */
            $base_tmp = $this->base;
            plugin::save_object();
            $this->base = $base_tmp;

            /* Refresh base */
            if ($this->acl_is_moveable($this->base)) {
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base= $this->baseSelector->getBase();
                    $this->is_modified= true;
                }
            }
        }
    }


    /* (Re-)Load objects */
    public function reload()
    {
        /*###########
          Variable initialisation
        ###########*/

        $this->objects                = array();
        $this->ui                     = get_userinfo();
        $filter                       = "";
        $objectClasses                = array();

        $ogfilter               = session::get("ogfilter");
        $regex                  = $ogfilter['regex'];

        $ldap= $this->config->get_ldap_link();
        $ldap->cd($ogfilter['dselect']);


        /*###########
          Generate Filter
        ###########*/

        $p_f= array("accounts"=> array("OBJ"=>"user", "CLASS"=>"gosaAccount"    ,
                    "DN"=> get_people_ou()           ,"ACL" => "users"),
                "groups"          => array("OBJ"=>"group", "CLASS"=>"posixGroup"     ,
                    "DN"=> get_ou('group', 'ogroupRDN') ,"ACL" => "groups"),
                "departments"     => array("OBJ"=>"department", "CLASS"=>"gosaDepartment" ,
                    "DN"=> ""                        ,"ACL" => "department"),
                "servers"         => array("OBJ"=>"servgeneric", "CLASS"=>"goServer"       ,
                    "DN"=> get_ou("servgeneric", "serverRDN")        ,"ACL" => "server"),
                "workstations"    => array("OBJ"=>"workgeneric", "CLASS"=>"gotoWorkstation",
                    "DN"=> get_ou("workgeneric", "workstationRDN")   ,"ACL" => "workstation"),
                "winstations"     => array("OBJ"=>"wingeneric", "CLASS"=>"opsiClient",
                    "DN"=> get_ou("wingeneric", 'sambaMachineAccountRDN')     ,"ACL" => "winstation"),
                "terminals"       => array("OBJ"=>"termgeneric", "CLASS"=>"gotoTerminal"   ,
                    "DN"=> get_ou("termgeneric", "terminalRDN")      ,"ACL" => "terminal"),
                "printers"        => array("OBJ"=>"printgeneric", "CLASS"=>"gotoPrinter"    ,
                    "DN"=> get_ou("printgeneric", "printerRDN")       ,"ACL" => "printer"),
                "phones"          => array("OBJ"=>"phoneGeneric", "CLASS"=>"goFonHardware"  ,
                    "DN"=> get_ou("phoneGeneric", "phoneRDN")         ,"ACL" => "phone"));


        /* Allow searching for applications, if we are not using release managed applications
         */
        if (!$this->IsReleaseManagementActivated()) {
            $p_f[      "applications"]    = array("OBJ"=>"application", "CLASS"=>"gosaApplication",
                    "DN"=> get_ou("application", "applicationRDN")   ,"ACL" => "application");
        }

        /*###########
          Perform search for selected objectClasses & regex to fill list with objects
        ###########*/

        $Get_list_flags = 0;
        if ($ogfilter['subtrees'] == "checked") {
            $Get_list_flags |= GL_SUBSEARCH;
        }

        foreach ($p_f as $post_name => $data) {
            if ($ogfilter[$post_name] == "checked" && class_available($data['OBJ'])) {
                if ($ogfilter['subtrees']) {
                    $base =  $ogfilter['dselect'];
                } else {
                    $base =  $data['DN'].$ogfilter['dselect'];
                }


                $filter = "(&(objectClass=".$data['CLASS'].")(|(uid=$regex)(cn=$regex)(ou=$regex)))";
                $res    = get_list(
                    $filter,
                    $data['ACL']  ,
                    $base,
                    array("description", "objectClass", "sn", "givenName", "uid","ou","cn"),
                    $Get_list_flags
                );

                /* fetch results and append them to the list */
                foreach ($res as $attrs) {

                    /* Skip workstations which are already assigned to an object group.
                     */
                    if ($this->gosaGroupObjects == "[W]" || $this->gosaGroupObjects == "[T]") {
                        if (in_array_strict($attrs['dn'], $this->used_workstations)) {
                            continue;
                        }
                    }

                    $type= $this->getObjectType($attrs);
                    $name= $this->getObjectName($attrs);

                    /* Fill array */
                    if (isset($attrs["description"][0])) {
                        $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
                    } elseif (isset($attrs["uid"][0])) {
                        $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["uid"][0]."]", "type" => "$type");
                    } else {
                        $this->objects[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
                    }
                }
            }
        }
        reset($this->objects);


        /*###########
          Build member list and try to detect obsolete entries
        ###########*/

        $this->memberList = array();

        /* Walk through all single member entry */
        foreach ($this->member as $dn) {

            /* The dn for the current member can't be resolved
               it seams that this entry was removed
             */
            /* Try to resolv the entry again, if it still fails, display error msg */
            $ldap->cat($dn, array("cn", "sn", "givenName", "ou", "description", "objectClass", "macAddress"));

            /* It has failed, add entry with type flag I (Invalid)*/
            if (!$ldap->success()) {
                $this->memberList[$dn]= array('text' => _("Non existing DN:")." ".LDAP::fix($dn),"type" => "I");
            } else {

                /* Append this entry to our all object list */

                /* Fetch object */
                $attrs= $ldap->fetch();

                $type= $this->getObjectType($attrs);
                $name= $this->getObjectName($attrs);

                if (isset($attrs["description"][0])) {
                    $this->objcache[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
                } elseif (isset($attrs["uid"][0])) {
                    $this->objcache[$attrs["dn"]]= array("text" => "$name [".$attrs["uid"][0]."]", "type" => "$type");
                } else {
                    $this->objcache[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
                }
                $this->objcache[$attrs["dn"]]['objectClass']  = $attrs['objectClass'];

                if (isset($attrs['macAddress'][0])) {
                    $this->objcache[$attrs["dn"]]['macAddress']  = $attrs['macAddress'][0];
                } else {
                    $this->objcache[$attrs["dn"]]['macAddress']  = "";
                }

                if (isset($attrs['uid'])) {
                    $this->objcache[$attrs["dn"]]['uid']          = $attrs['uid'];
                }

                /* Fill array */
                if (isset($attrs["description"][0])) {
                    $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
                } else {
                    $this->objects[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
                }

                $this->memberList[$dn]= $this->objects[$attrs["dn"]];
            }
        }
        reset($this->memberList);

        /* Assemble types of currently combined objects */
        $objectTypes= "";
        foreach ($this->memberList as $dn => $desc) {

            /* Invalid object? */
            if ($desc['type'] == 'I') {
                continue;
            }

            /* Fine. Add to list. */
            if (!preg_match('/'.$desc['type'].'/', $objectTypes)) {
                $objectTypes.= $desc['type'];
            }
        }
        $this->gosaGroupObjects= "[$objectTypes]";
    }


    public function getObjectType($attrs)
    {
        $type= "I";

        foreach ($this->typeToClass as $index => $class) {
            if (in_array_strict($class, $attrs['objectClass'])) {
                $type= $index;
                break;
            }
        }
        return ($type);
    }


    public function getObjectName($attrs)
    {
        /* Person? */
        $name ="";
        if (in_array_strict('gosaAccount', $attrs['objectClass'])) {
            if (isset($attrs['sn']) && isset($attrs['givenName'])) {
                $name= $attrs['sn'][0].", ".$attrs['givenName'][0];
            } else {
                $name= $attrs['uid'][0];
            }
        } else {
            if (isset($attrs["cn"][0])) {
                $name= $attrs['cn'][0];
            } else {
                $name= $attrs['ou'][0];
            }
        }

        return ($name);
    }


    public function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        /* Permissions for that base? */
        if ($this->base != "") {
            $new_dn= 'cn='.$this->cn.','.get_ou("group", "ogroupRDN").$this->base;
        } else {
            $new_dn= $this->dn;
        }

        /* Check if we have workstations assigned, that are already assigned to
           another object group.  */
        if ($this->gosaGroupObjects == "[W]" || $this->gosaGroupObjects == "[T]") {
            $test =array_intersect($this->used_workstations, $this->member);
            if (count($test)) {
                $str = "";
                foreach ($test as $dn) {
                    $str .= "<li>".$dn."</li>";
                }
                $message[] = sprintf(_("These systems are already configured by other object groups and cannot be added:")."<br><ul>%s</ul>", $str);
            }
        }

        $ldap = $this->config->get_ldap_link();
        if (LDAP::fix($this->dn) != LDAP::fix($new_dn)) {
            $ldap->cat($new_dn, array('dn'));
        }

        if ($ldap->count() !=0) {
            $message[]= msgPool::duplicated(_("Name"));
        }

        // Check if a wrong base was supplied
        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[]= msgPool::check_base();
            ;
        }

        /* Set new acl base */
        if ($this->dn == "new") {
            $this->set_acl_base($this->base);
        }

        /* must: cn */
        if ($this->cn == "") {
            $message[]= msgPool::required(_("Name"));
        }

        if (preg_match('/[=,+<>#;]/', $this->cn)) {
            $message[] = msgPool::invalid(_("Name"), $this->cn, "/[^=+,<>#;]/");
        }

        /* To many different object types? */
        if (strlen($this->gosaGroupObjects) > 4) {
            $message[]= _("You can combine two different object types at maximum, only!");
        }

        /* Check if we are allowed to create or move this object
         */
        if ($this->orig_dn == "new" && !$this->acl_is_createable($this->base)) {
            $message[] = msgPool::permCreate();
        } elseif ($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)) {
            $message[] = msgPool::permMove();
        }

        return ($message);
    }


    /* Save to LDAP */
    public function save()
    {
        // Do not save members if we don't want to.
        // This may be the case if we've copied an ogroup containing systems!
        if (!$this->copyMembers) {
            $this->member = array();
            $this->reload();
        }

        plugin::save();

        /* Move members to target array */
        if (!$this->wasDyGroup && !$this->isRestrictedByDynGroup()) {
            $this->attrs['member'] =array();
            foreach ($this->member as $key => $desc) {
                $this->attrs['member'][]= LDAP::fix($key);
            }
        }

        $ldap= $this->config->get_ldap_link();

        /* New accounts need proper 'dn', propagate it to remaining objects */
        if ($this->dn == 'new') {
            $this->dn= 'cn='.$this->cn.','.get_ou("group", "ogroupRDN").$this->base;
        }

        /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
           new entries. So do a check first... */
        $ldap->cat($this->dn, array('dn'));
        if ($ldap->fetch()) {
            /* Modify needs array() to remove values :-( */
            if (!count($this->member)) {
                $this->attrs['member']= array();
            }
            $mode= "modify";
        } else {
            $mode= "add";
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }


        /* Write back to ldap */
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->$mode($this->attrs);

        if ($mode == "add") {
            new log("create", "ogroups/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        } else {
            new log("modify", "ogroups/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        }

        /* Trigger post signal */
        $this->handle_post_events($mode);

        $ret= 0;
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
            $ret= 1;
        } else {
            $this->trustModeDialog->dn = $this->dn;
            $this->trustModeDialog->save();
        }

        return ($ret);
    }

    public function remove_from_parent()
    {
        plugin::remove_from_parent();

        $ldap= $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

        new log("remove", "ogroups/".get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());

        /* Trigger remove signal */
        $this->handle_post_events("remove");
    }


    public function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        // Preselect "Copy members" state.
        // If we've terminals, workstations or servers in our members list,
        //  then disable this option by default, to avoid problems with
        //  inheritance of ogroup values.
        if (preg_match("/[STW]/", $this->gosaGroupObjects) || !isset($source['member'])) {
            $this->copyMembers = false;
        } else {
            $this->copyMembers = true;
        }

        /* Reload tabs */
        $this->parent->reload($this->gosaGroupObjects);

        $this->trustModeDialog->PrepareForCopyPaste($source);

        /* Reload plugins */
        foreach ($this->parent->by_object as $name => $class) {
            if (get_class($this) != $name) {
                $this->parent->by_object[$name]->PrepareForCopyPaste($source);
            }
        }

        $source_o = new ogroup($this->config, $source['dn']);
        foreach (array("member","gosaGroupObjects")  as $attr) {
            $this->$attr = $source_o->$attr;
        }
    }


    public function getCopyDialog()
    {
        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("copyMembers", $this->copyMembers);
        $str = $smarty->fetch(get_template_path("paste_generic.tpl", true, dirname(__FILE__)));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }

    public function saveCopyDialog()
    {
        if (isset($_POST['cn'])) {
            $this->cn = get_post('cn');
        }
        $this->copyMembers = isset($_POST['copyMembers']);
    }


    public function IsReleaseManagementActivated()
    {
        return($this->config->pluginEnabled("faiManagement"));
    }


    public static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Object group generic"),
                    "plSelfModify"  => false,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gosaGroupOfNames' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(), 'ogroupManagement')
                        ),
                    "plCategory"    => array("ogroups" => array("description"  => _("Object groups"),
                            "objectClass"  => "gosaGroupOfNames")),
                    "plProvidedAcls"=> array(
                        "cn"                => _("Name"),
                        "base"              => _("Base"),
                        "description"       => _("Description"),
                        "accessTo"          => _("System trust"),
                        "member"            => _("Member"))
                    ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
