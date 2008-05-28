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

/* Sort multidimensional arrays for key 'text' */
function sort_list($val1, $val2)
{
  $v1= strtolower($val1['text']);
  $v2= strtolower($val2['text']);
  if ($v1 > $v2){
    return 1;
  }
  if ($v1 < $v2){
    return -1;
  }
  return 0;
}


class ogroup extends plugin
{
  /* Variables */
  var $cn= "";
  var $description= "";
  var $base= "";
  var $gosaGroupObjects= "";
  var $department= "";
  var $objects= array();
  var $objcache= array();
  var $memberList= array();
  var $member= array();
  var $orig_dn= "";
  var $orig_cn= "";
  var $orig_base= "";
  var $group_dialog= FALSE;
  var $view_logged = FALSE;

  var $accessTo= array();
  var $trustModel= "";
  var $show_ws_dialog = FALSE;

  var $was_trust_account= FALSE;

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaGroupObjects","member","accessTo","trustModel");
  var $objectclasses= array("top", "gosaGroupOfNames");

  function ogroup (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->orig_dn= $dn;

    $this->member = array();

    /* Load member objects */
    if (isset($this->attrs['member'])){
      foreach ($this->attrs['member'] as $key => $value){
        if ("$key" != "count"){
          $value= @LDAP::convert($value);
          $this->member["$value"]= "$value";
        }
      }
    }
    $this->is_account= TRUE;

    /* Get global filter config */
    if (!session::is_set("ogfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $ogfilter= array( "dselect"       => $base,
          "regex"           => "*");
      session::set("ogfilter", $ogfilter);
    }
    $ogfilter= session::get('ogfilter');

    /* Adjust flags */
    foreach( array(   "U" => "accounts",
          "G" => "groups",
          "A" => "applications",
          "D" => "departments",
          "S" => "servers",
          "W" => "workstations",
          "O" => "winstations",
          "T" => "terminals",
          "F" => "phones",
          "_" => "subtrees",
          "P" => "printers") as $key => $val){

      if (preg_match("/$key/", $this->gosaGroupObjects)){
        $ogfilter[$val]= "checked";
      } else {
        $ogfilter[$val]= "";
      }
    }
    session::set("ogfilter", $ogfilter);
  
    if(session::is_set('CurrentMainBase')){
     $this->base  = session::get('CurrentMainBase');
    }

    /* Set base */
    if ($this->dn == "new"){
      $this->base = session::get('CurrentMainBase');
    } else {
      $this->base= preg_replace("/^[^,]+,".normalizePreg(get_ou("ogroupou"))."/","",$this->dn);

      /* Is this account a trustAccount? */
      if ($this->is_account && isset($this->attrs['trustModel'])){
        $this->trustModel= $this->attrs['trustModel'][0];
        $this->was_trust_account= TRUE;
      } else {
        $this->was_trust_account= FALSE;
        $this->trustModel= "";
      }

      $this->accessTo = array();
      if ($this->is_account && isset($this->attrs['accessTo'])){
        for ($i= 0; $i<$this->attrs['accessTo']['count']; $i++){
          $tmp= $this->attrs['accessTo'][$i];
          $this->accessTo[$tmp]= $tmp;
        }
      }
    }

    $this->orig_cn = $this->cn;
    $this->orig_base = $this->base;

    /* Load member data */
    $this->reload();
  }

  function AddDelMembership($NewMember = false){

    if($NewMember){

      /* Add member and force reload */
      $this->member[$NewMember]= $NewMember;
      $this->reload(); 

      $this->memberList[$NewMember]= $this->objcache[$NewMember];
      unset ($this->objects[$NewMember]);
      uasort ($this->memberList, 'sort_list');
      reset ($this->memberList);
    }else{
      /* Delete objects from group */
      if (isset($_POST['delete_membership']) && isset($_POST['members'])){
        foreach ($_POST['members'] as $value){
          $this->objects["$value"]= $this->memberList[$value];
          unset ($this->memberList["$value"]);
          unset ($this->member["$value"]);
          uasort ($this->objects, 'sort_list');
          reset ($this->objects);
        }
        $this->reload();
      }

      /* Add objects to group */
      if (isset($_POST['add_object_finish']) && isset($_POST['objects'])){

        $tmp = "";
        foreach($this->memberList as $obj){
          $tmp .= $obj['type'];
        }
        $skipped = FALSE;
        foreach ($_POST['objects'] as $value){
          if(preg_match("/T/",$tmp) && $this->objects[$value]['type'] == "W"){
            $skipped =TRUE;
          }elseif(preg_match("/W/",$tmp) && $this->objects[$value]['type'] == "T"){
            $skipped =TRUE;
          }else{
            $this->memberList["$value"]= $this->objects[$value];
            $this->member["$value"]= $value;
            unset ($this->objects[$value]);
            uasort ($this->memberList, 'sort_list');
            reset ($this->memberList);
          }
        }
        if($skipped){
          msg_dialog::display(_("Information"), _("You cannot combine terminals and workstations in one object group!"), INFO_DIALOG);
        }
        $this->reload();
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","ogroups/".get_class($this),$this->dn);
    }


    /* Do we represent a valid group? */
    if (!$this->is_account){
      $display= "<img alt=\"\" src=\"images/small-error.png\" align=\"middle\">&nbsp;<b>".
        msgPool::noValidExtension("object group")."</b>";
      return ($display);
    }


    /* Load templating engine */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once && $this->acl_is_moveable()){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /***********
     * Trusts 
     ***********/

    /* Add user workstation? */
    if (isset($_POST["add_ws"])){
      $this->show_ws_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Add user workstation? */
    if (isset($_POST["add_ws_finish"]) && isset($_POST['wslist'])){
      foreach($_POST['wslist'] as $ws){
        $this->accessTo[$ws]= $ws;
      }
      ksort($this->accessTo);
      $this->is_modified= TRUE;
    }

    /* Remove user workstations? */
    if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list'])){
      foreach($_POST['workstation_list'] as $name){
        unset ($this->accessTo[$name]);
      }
      $this->is_modified= TRUE;
    }

    /* Add user workstation finished? */
    if (isset($_POST["add_ws_finish"]) || isset($_POST["add_ws_cancel"])){
      $this->show_ws_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Show ws dialog */
    if ($this->show_ws_dialog){
      /* Save data */
      $sysfilter= session::get("sysfilter");
      foreach( array("depselect", "regex") as $type){
        if (isset($_POST[$type])){
          $sysfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $sysfilter['regex']= $s;
      }
      session::set("sysfilter", $sysfilter);

      /* Get workstation list */
      $exclude= "";
      foreach($this->accessTo as $ws){
        $exclude.= "(cn=$ws)";
      }
      if ($exclude != ""){
        $exclude= "(!(|$exclude))";
      }
      $regex= $sysfilter['regex'];
      $filter= "(&(|(objectClass=goServer)(objectClass=gotoWorkstation)(objectClass=gotoTerminal))$exclude(cn=*)(cn=$regex))";

      $deps_a = array(get_ou("serverou"),
          get_ou("terminalou"),
          get_ou("workstationou"));
      $res= get_sub_list($filter, array("terminal","server","workstation"), $deps_a, get_ou("systemsou").$sysfilter['depselect'],
          array("cn"), GL_SUBSEARCH | GL_SIZELIMIT);
      $wslist= array();
      foreach ($res as $attrs){
        $wslist[]= preg_replace('/\$/', '', $attrs['cn'][0]);
      }
      asort($wslist);
      $smarty->assign("search_image", get_template_path('images/lists/search.png'));
      $smarty->assign("launchimage", get_template_path('images/lists/action.png'));
      $smarty->assign("tree_image", get_template_path('images/lists/search-subtree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("depselect", "regex") as $type){
        $smarty->assign("$type", $sysfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("wslist", $wslist);
      $smarty->assign("apply", apply_filter());
      $display= $smarty->fetch (get_template_path('trust_machines.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /***********
     * Ende - Trusts 
     ***********/


    /* Dialog handling */
    if(is_object($this->dialog) && $this->acl_is_moveable()){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){

        /* A new base was selected, check if it is a valid one */
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$this->dialog->isSelected()])){
          $this->base = $this->dialog->isSelected();
        }
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Add objects? */
    if (isset($_POST["edit_membership"])){
      $this->group_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Add objects finished? */
    if (isset($_POST["add_object_finish"]) || isset($_POST["add_object_cancel"])){
      $this->group_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Manage object add dialog */
    if ($this->group_dialog){

      /* Save data */
      $ogfilter= session::get("ogfilter");
      foreach( array("dselect", "regex") as $type){
        if (isset($_POST[$type])){
          $ogfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_POST['dselect'])){
        foreach( array("accounts", "groups", "applications", "departments",
              "servers", "workstations", "winstations", "terminals", "printers","subtrees",
              "phones") as $type){

          if (isset($_POST[$type])) {
            $ogfilter[$type]= "checked";
          } else {
            $ogfilter[$type]= "";
          }
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $ogfilter['regex']= $s;
      }
      session::set("ogfilter", $ogfilter);
      $this->reload();

      /* Calculate actual groups */
      $smarty->assign("objects", $this->convert_list($this->objects));

      /* Show dialog */
      $smarty->assign("search_image", get_template_path('images/lists/search.png'));
      $smarty->assign("launchimage", get_template_path('images/lists/action.png'));
      $smarty->assign("tree_image", get_template_path('images/lists/search-subtree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("dselect", "regex", "subtrees") as $type){
        $smarty->assign("$type", $ogfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("apply", apply_filter());

      /* Build up checkboxes 
       */
      $ar = array(
          "departments" => array(
            "T" => msgPool::selectToView(_("departments")),
            "C" => (isset($ogfilter['departments']) && ($ogfilter['departments'])),
            "L" => sprintf(_("Show %s"),_("departments"))),
          "accounts" => array(
            "T" => msgPool::selectToView(_("people")),
            "C" => (isset($ogfilter['accounts']) && ($ogfilter['accounts'])),
            "L" => sprintf(_("Show %s"),_("people"))),
          "groups"=> array(
            "T" => msgPool::selectToView(_("groups")),
            "C" => (isset($ogfilter['groups']) && ($ogfilter['groups'])),
            "L" => sprintf(_("Show %s"),_("groups"))),
          "servers"=> array(
            "T" => msgPool::selectToView(_("servers")),
            "C" => (isset($ogfilter['servers']) && ($ogfilter['servers'])),
            "L" => sprintf(_("Show %s"),_("servers"))),
          "workstations"=> array(
            "T" => msgPool::selectToView(_("workstations")),
            "C" => (isset($ogfilter['workstations']) && ($ogfilter['workstations'])),
            "L" => sprintf(_("Show %s"),_("workstations"))),
          "terminals"=> array(
            "T" => msgPool::selectToView(_("terminals")),
            "C" => (isset($ogfilter['terminals']) && ($ogfilter['terminals'])),
            "L" => sprintf(_("Show %s"),_("terminals"))),
          "printers"=> array(
            "T" => msgPool::selectToView(_("printer")),
            "C" => (isset($ogfilter['printers']) && ($ogfilter['printers'])),
            "L" => sprintf(_("Show %s"),_("printers"))),
          "phones"=> array(
            "T" => msgPool::selectToView(_("phones")),
            "C" => (isset($ogfilter['phones']) && ($ogfilter['phones'])),
            "L" => sprintf(_("Show %s"),_("phones"))));
 
      /* Allow selecting applications if we are having a non 
          release managed application storage */ 
      if(!$this->IsReleaseManagementActivated()){
        $ar["applications"] = array(
            "T" => msgPool::selectToView(_("applications")),
            "C" => (isset($ogfilter['applications']) && ($ogfilter['applications'])),
            "L" => sprintf(_("Show %s"),_("applications")));
      }

      $smarty->assign("checkboxes",$ar);
      $display= $smarty->fetch (get_template_path('ogroup_objects.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Bases / Departments */
      if ((isset($_POST['base'])) && ($this->acl_is_moveable())){
        $this->base= $_POST['base'];
      }

    /* Assemble combine string */
    if ($this->gosaGroupObjects == "[]"){
      $smarty->assign("combinedObjects", _("none"));
    } elseif (strlen($this->gosaGroupObjects) > 4){
      $smarty->assign("combinedObjects", "<font color=red>"._("too many different objects!")."</font>");
    } else {
      $conv= array(   "U" => _("users"),
          "G" => _("groups"),
          "A" => _("applications"),
          "D" => _("departments"),
          "S" => _("servers"),
          "W" => _("workstations"),
          "O" => _("winstations"),
          "T" => _("terminals"),
          "F" => _("phones"),
          "P" => _("printers"));

      $type= preg_replace('/[\[\]]/', '', $this->gosaGroupObjects);
      $p1= $conv[$type[0]];
      error_reporting(0);
      if (isset($type[1]) && preg_match('/[UGADSFOWTP]/', $type[1])){
        $p2= $conv[$type[1]];
        $smarty->assign("combinedObjects", sprintf("'%s' and '%s'", $p1, $p2));
      } else {
        $smarty->assign("combinedObjects", "$p1");
      }
      error_reporting(E_ALL | E_STRICT);
    }

    /* Assign variables */
    $smarty->assign("bases", $this->get_allowed_bases());
    $smarty->assign("base_select", $this->base);
    $smarty->assign("department", $this->department);
    $smarty->assign("members", $this->convert_list($this->memberList));

    /* Objects have to be tuned... */
    $smarty->assign("objects", $this->convert_list($this->objects));

    /* Fields */
    foreach ($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
    }

    /******
      Trust account
     ******/
    $smarty->assign("trusthide", " disabled ");
    $smarty->assign("trustmodeACL",  $this->getacl("trustModel"));
    if ($this->trustModel == "fullaccess"){
      $trustmode= 1;
      // pervent double disable tag in html code, this will disturb our clean w3c html
      $smarty->assign("trustmode",  $this->getacl("trustModel"));

    } elseif ($this->trustModel == "byhost"){
      $trustmode= 2;
      $smarty->assign("trusthide", "");
    } else {
      // pervent double disable tag in html code, this will disturb our clean w3c html
      $smarty->assign("trustmode",  $this->getacl("trustModel"));
      $trustmode= 0;
    }
    $smarty->assign("trustmode", $trustmode);
    $smarty->assign("trustmodes", array( 0 => _("disabled"), 1 => _("full access"),
          2 => _("allow access to these hosts")));

    $smarty->assign("workstations", $this->accessTo);

    if((count($this->accessTo))==0){
      $smarty->assign("emptyArrAccess",true);
    }else{
      $smarty->assign("emptyArrAccess",false);
    }
    /******
      Ende - Trust account
     ******/

    return ($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  /* Save data to object */
  function save_object()
  {
    /* Save additional values for possible next step */
    if (isset($_POST['ogroupedit'])){

      /******
        Trust account 
       ******/

      if($this->acl_is_writeable("trustModel")){
        if (isset($_POST['trustmode'])){
          $saved= $this->trustModel;
          if ($_POST['trustmode'] == "1"){
            $this->trustModel= "fullaccess";
          } elseif ($_POST['trustmode'] == "2"){
            $this->trustModel= "byhost";
          } else {
            $this->trustModel= "";
          }
          if ($this->trustModel != $saved){
            $this->is_modified= TRUE;
          }
        }
      }
      /******
        Ende Trust account
       ******/

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;

      /* Save base, since this is no LDAP attribute */
      $tmp = $this->get_allowed_bases();
      if(isset($_POST['base'])){
        if(isset($tmp[$_POST['base']])){
          $this->base= $_POST['base'];
        }
      }
    }
  }


  /* (Re-)Load objects */
  function reload()
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
    $ldap->cd ($ogfilter['dselect']);


    /*###########
      Generate Filter 
      ###########*/

    $p_f= array("accounts"=> array("OBJ"=>"user", "CLASS"=>"gosaAccount"    ,
          "DN"=> get_people_ou()           ,"ACL" => "users"), 
        "groups"          => array("OBJ"=>"group", "CLASS"=>"posixGroup"     ,
          "DN"=> get_groups_ou('ogroupou') ,"ACL" => "groups"), 
        "departments"     => array("OBJ"=>"department", "CLASS"=>"gosaDepartment" ,
          "DN"=> ""                        ,"ACL" => "department"), 
        "servers"         => array("OBJ"=>"servgeneric", "CLASS"=>"goServer"       ,
          "DN"=> get_ou('serverou')        ,"ACL" => "server"),
        "workstations"    => array("OBJ"=>"workgeneric", "CLASS"=>"gotoWorkstation",
          "DN"=> get_ou('workstationou')   ,"ACL" => "workstation"),
        "winstations"     => array("OBJ"=>"wingeneric", "CLASS"=>"opsiClient",        
          "DN"=> get_ou('WINSTATIONS')     ,"ACL" => "winstation"),
        "terminals"       => array("OBJ"=>"termgeneric", "CLASS"=>"gotoTerminal"   ,
          "DN"=> get_ou('terminalou')      ,"ACL" => "terminal"),
        "printers"        => array("OBJ"=>"printgeneric", "CLASS"=>"gotoPrinter"    ,
          "DN"=> get_ou('printerou')       ,"ACL" => "printer"),
        "phones"          => array("OBJ"=>"phoneGeneric", "CLASS"=>"goFonHardware"  ,
          "DN"=> get_ou('phoneou')         ,"ACL" => "phone"));


    /* Allow searching for applications, if we are not using release managed applications 
      */
    if(!$this->IsReleaseManagementActivated()){
      $p_f[      "applications"]    = array("OBJ"=>"application", "CLASS"=>"gosaApplication",
          "DN"=> get_ou('applicationou')   ,"ACL" => "application"); 
    }
           
    /*###########
      Perform search for selected objectClasses & regex to fill list with objects   
      ###########*/

    $Get_list_flags = 0;
    if($ogfilter['subtrees'] == "checked"){
      $Get_list_flags |= GL_SUBSEARCH;
    }    

    foreach($p_f as $post_name => $data){

      if($ogfilter[$post_name] == "checked" && class_available($data['OBJ'])){

        if($ogfilter['subtrees']){
          $base =  $ogfilter['dselect'];
        }else{
          $base =  $data['DN'].$ogfilter['dselect'];
        }
   
         
        $filter = "(&(objectClass=".$data['CLASS'].")(|(uid=$regex)(cn=$regex)(ou=$regex)))";
        $res    = get_list($filter, $data['ACL']  , $base, 
                    array("description", "objectClass", "sn", "givenName", "uid","ou","cn"),$Get_list_flags);

        /* fetch results and append them to the list */
        foreach($res as $attrs){

          $type= $this->getObjectType($attrs);
          $name= $this->getObjectName($attrs);

          /* Fill array */
          if (isset($attrs["description"][0])){
            $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
          } elseif (isset($attrs["uid"][0])) {
            $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["uid"][0]."]", "type" => "$type");
          } else {
            $this->objects[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
          }
        }
      }
    }
    uasort ($this->objects, 'sort_list');
    reset ($this->objects);

    
    /*###########
      Build member list and try to detect obsolete entries 
      ###########*/

    $this->memberList = array();
  
    /* Walk through all single member entry */
    foreach($this->member as $dn){

      /* The dn for the current member can't be resolved 
         it seams that this entry was removed 
       */ 
      /* Try to resolv the entry again, if it still fails, display error msg */
      $ldap->cat($dn, array("cn", "sn", "givenName", "ou", "description", "objectClass", "macAddress"));

      /* It has failed, add entry with type flag I (Invalid)*/
      if (!$ldap->success()){
        $this->memberList[$dn]= array('text' => _("Non existing dn:")." ".@LDAP::fix($dn),"type" => "I");

      } else {

        /* Append this entry to our all object list */

        /* Fetch object */
        $attrs= $ldap->fetch();

        $type= $this->getObjectType($attrs);
        $name= $this->getObjectName($attrs);

        if (isset($attrs["description"][0])){
          $this->objcache[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
        } elseif (isset($attrs["uid"][0])) {
          $this->objcache[$attrs["dn"]]= array("text" => "$name [".$attrs["uid"][0]."]", "type" => "$type");
        } else {
          $this->objcache[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
        }
        $this->objcache[$attrs["dn"]]['objectClass']  = $attrs['objectClass'];

        if(isset($attrs['macAddress'][0])){
          $this->objcache[$attrs["dn"]]['macAddress']  = $attrs['macAddress'][0];
        }else{
          $this->objcache[$attrs["dn"]]['macAddress']  = "";
        }

        if(isset($attrs['uid'])){
          $this->objcache[$attrs["dn"]]['uid']          = $attrs['uid'];
        }

        /* Fill array */
        if (isset($attrs["description"][0])){
          $this->objects[$attrs["dn"]]= array("text" => "$name [".$attrs["description"][0]."]", "type" => "$type");
        } else {
          $this->objects[$attrs["dn"]]= array("text" => "$name", "type" => "$type");
        }

        $this->memberList[$dn]= $this->objects[$attrs["dn"]];
      }
    }
    uasort ($this->memberList, 'sort_list');
    reset ($this->memberList);

    /* Assemble types of currently combined objects */
    $objectTypes= "";
    foreach ($this->memberList as $dn => $desc){

      /* Invalid object? */
      if ($desc['type'] == 'I'){
        continue;
      }

      /* Fine. Add to list. */
      if (!preg_match('/'.$desc['type'].'/', $objectTypes)){
        $objectTypes.= $desc['type'];
      }
    }
    $this->gosaGroupObjects= "[$objectTypes]";
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(  "U" => "select_user.png",
        "G" => "plugins/groups/images/groups.png",
        "A" => "plugins/ogroups/images/application.png",
        "D" => "plugins/departments/images/department.png",
        "S" => "plugins/ogroups/images/server.png",
        "W" => "plugins/ogroups/images/workstation.png",
        "O" => "plugins/ogroups/images/winstation.png",
        "T" => "plugins/ogroups/images/terminal.png",
        "F" => "plugins/ogroups/images/phone.png",
        "I" => "images/lists/flag.png",
        "P" => "plugins/ogroups/images/printer.png");

    foreach ($input as $key => $value){
      /* Generate output */
      $temp.= "<option title='".addslashes( $key)."' value=\"$key\" class=\"select\" style=\"background-image:url('".get_template_path($conv[$value['type']])."');\">".$value['text']."</option>\n";
    }

    return ($temp);
  }


  function getObjectType($attrs)
  {
    $type= "I";

    foreach(array(  "U" => "gosaAccount",
          "G" => "posixGroup",
          "A" => "gosaApplication",
          "D" => "gosaDepartment",
          "S" => "goServer",
          "W" => "gotoWorkstation",
          "O" => "opsiClient",
          "T" => "gotoTerminal",
          "F" => "goFonHardware",
          "P" => "gotoPrinter") as $index => $class){
      if (in_array($class, $attrs['objectClass'])){
        $type= $index;
        break;
      }
    }

    return ($type);
  }


  function getObjectName($attrs)
  {
    /* Person? */
    $name =""; 
    if (in_array('gosaAccount', $attrs['objectClass'])){
      if(isset($attrs['sn']) && isset($attrs['givenName'])){
        $name= $attrs['sn'][0].", ".$attrs['givenName'][0];
      } else {
        $name= $attrs['uid'][0];
      }
    } else {
      if(isset($attrs["cn"][0])) {
        $name= $attrs['cn'][0];
      } else {
        $name= $attrs['ou'][0];
      }
    }

    return ($name);
  }


  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Permissions for that base? */
    if ($this->base != ""){
      $new_dn= 'cn='.$this->cn.','.get_ou('ogroupou').$this->base;
    } else {
      $new_dn= $this->dn;
    }


    $ldap = $this->config->get_ldap_link();
    if($this->dn != $new_dn){
      $ldap->cat ($new_dn, array('dn'));
    }
    
    if($ldap->count() !=0){
      $message[]= msgPool::duplicated(_("Name"));
    } 

    /* Set new acl base */
    if($this->dn == "new") {
      $this->set_acl_base($this->base);
    }

    /* must: cn */
    if ($this->cn == ""){
      $message[]= msgPool::required(_("Name"));
    }

    /* To many different object types? */
    if (strlen($this->gosaGroupObjects) > 4){
      $message[]= _("You can combine two different object types at maximum, only!");
    }

    /* Check if we are allowed to create or move this object 
     */
    if($this->dn == "new" && !$this->acl_is_createable($this->base)){
      $message[] = msgPool::permCreate();
    }elseif($this->cn != $this->orig_cn || $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
      $message[] = msgPool::permMove();
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Move members to target array */
    $this->attrs['member'] =array();
    foreach ($this->member as $key => $desc){
      $this->attrs['member'][]= @LDAP::fix($key);
    }

    $ldap= $this->config->get_ldap_link();

    /* New accounts need proper 'dn', propagate it to remaining objects */
    if ($this->dn == 'new'){
      $this->dn= 'cn='.$this->cn.','.get_ou('ogroupou').$this->base;
    }

    /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
       new entries. So do a check first... */
    $ldap->cat ($this->dn, array('dn'));
    if ($ldap->fetch()){
      /* Modify needs array() to remove values :-( */
      if (!count ($this->member)){
        $this->attrs['member']= array();
      }
      $mode= "modify";

    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    /******
      Trust accounts 
     ******/
    $objectclasses= array();
    foreach ($this->attrs['objectClass'] as $key => $class){
      if (preg_match('/trustAccount/i', $class)){
        continue;
      }
      $objectclasses[]= $this->attrs['objectClass'][$key];
    }
    $this->attrs['objectClass']= $objectclasses;
    if ($this->trustModel != ""){
      $this->attrs['objectClass'][]= "trustAccount";
      $this->attrs['trustModel']= $this->trustModel;
      $this->attrs['accessTo']= array();
      if ($this->trustModel == "byhost"){
        foreach ($this->accessTo as $host){
          $this->attrs['accessTo'][]= $host;
        }
      }
    } else {
      if ($this->was_trust_account){
        $this->attrs['accessTo']= array();
        $this->attrs['trustModel']= array();
      }
    }

    /******
      Ende - Trust accounts 
     ******/

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->$mode($this->attrs);

    if($mode == "add"){
      new log("create","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("modify","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    /* Trigger post signal */
    $this->handle_post_events($mode);

    $ret= 0;
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
      $ret= 1;
    }

    return ($ret);
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    new log("remove","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    /* Trigger remove signal */
    $this->handle_post_events("remove");
  }

  
  function PrepareForCopyPaste($source)
  {
    /* Update available object types */
    if(isset($source['gosaGroupObjects'][0])){
      $this->gosaGroupObjects =  $source['gosaGroupObjects'][0];
    }

    /* Reload tabs */
    $this->parent->reload($this->gosaGroupObjects );
   
    /* Reload plugins */ 
    foreach($this->parent->by_object as $name => $class ){
      if(get_class($this) != $name) {
        $this->parent->by_object[$name]->PrepareForCopyPaste($source);
      }
    }

    /* Load member objects */
    if (isset($source['member'])){
      foreach ($source['member'] as $key => $value){
        if ("$key" != "count"){
          $value= @LDAP::convert($value);
          $this->member["$value"]= "$value";
        }
      }
    }

  }


  function getCopyDialog()
  {
    $smarty = get_smarty();
    $smarty->assign("cn",     $this->cn);
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }

  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }


  function IsReleaseManagementActivated()
  {
    /* Check if we should enable the release selection */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      return(true);
    }
    return(false);
  }


  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Generic"),
          "plDescription" => _("Object group generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 1,
          "plSection"     => array("administration"),
          "plCategory"    => array("ogroups" => array("description"  => _("Object groups"),
                                                      "objectClass"  => "gosaGroupOfNames")),
          "plProvidedAcls"=> array(
            "cn"                => _("Name"),
            "base"              => _("Base"),
            "description"       => _("Description"),
            "member"            => _("Member"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
