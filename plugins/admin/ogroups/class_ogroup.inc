<?php


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
  var $group_dialog= FALSE;

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaGroupObjects","member");
  var $objectclasses= array("top", "gosaGroupOfNames");

  function ogroup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->orig_dn= $dn;

    $this->member = array();

    /* Load member objects */
    if (isset($this->attrs['member'])){
      foreach ($this->attrs['member'] as $key => $value){
        if ("$key" != "count"){
          $this->member["$value"]= "$value";
        }
      }
    }
    $this->is_account= TRUE;

    /* Get global filter config */
    if (!is_global("ogfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $ogfilter= array( "dselect"       => $base,
          "regex"           => "*");
      register_global("ogfilter", $ogfilter);
    }
    $ogfilter= get_global('ogfilter');

    /* Adjust flags */
    foreach( array(   "U" => "accounts",
          "G" => "groups",
          "A" => "applications",
          "D" => "departments",
          "S" => "servers",
          "W" => "workstations",
          "T" => "terminals",
          "F" => "phones",
          "P" => "printers") as $key => $val){

      if (preg_match("/$key/", $this->gosaGroupObjects)){
        $ogfilter[$val]= "checked";
      } else {
        $ogfilter[$val]= "";
      }
    }
    register_global("ogfilter", $ogfilter);
 
    /* Set base */
    if ($this->dn == "new"){
      $this->base= $_SESSION['CurrentMainBase'];
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,/", "", $this->dn);
    }

    /* set permissions */
    $ui= get_userinfo();
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "ogroup", $ui->dn);

    /* Load member data */
    $this->reload();
  }

  function AddDelMembership($NewMember = false)
  {
    if($NewMember){
      $this->importMember($NewMember);
      $this->memberList[$NewMember]= $this->objcache[$NewMember];
      $this->member[$NewMember]= $NewMember;
      unset ($this->objects[$NewMember]);
      uasort ($this->memberList, 'sort_list');
      reset ($this->memberList);
      $this->reload();
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
        foreach ($_POST['objects'] as $value){
          $this->memberList["$value"]= $this->objects[$value];
          $this->member["$value"]= $value;
          unset ($this->objects[$value]);
          uasort ($this->memberList, 'sort_list');
          reset ($this->memberList);
        }
        $this->reload();
      }
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

//    $this->reload();

    /* Do we represent a valid group? */
    if (!$this->is_account){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no object group.")."</b>";
      return ($display);
    }

    /* Delete objects from group */
    if (isset($_POST['delete_membership']) && isset($_POST['members'])){
      foreach ($_POST['members'] as $value){
        if(isset($this->memberList[$value])){
          $this->objects["$value"]= $this->memberList[$value];
          unset ($this->memberList["$value"]);
          unset ($this->member["$value"]);
          uasort ($this->objects, 'sort_list');
          reset ($this->objects);
        }
      }
      $this->reload();
    }

    /* Add objects to group */
    if (isset($_POST['add_object_finish']) && isset($_POST['objects'])){
      foreach ($_POST['objects'] as $value){
        if(isset($this->objects[$value])){
          $this->memberList["$value"]= $this->objects[$value];
          $this->member["$value"]= $value;
          unset ($this->objects[$value]);
          uasort ($this->memberList, 'sort_list');
          reset ($this->memberList);
        }
      }
      $this->reload();
    }

    /* Load templating engine */
    $smarty= get_smarty();

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
        $this->base = $this->dialog->isSelected();
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
      $ogfilter= get_global("ogfilter");
      foreach( array("dselect", "regex") as $type){
        if (isset($_POST[$type])){
          $ogfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_POST['dselect'])){
        foreach( array("accounts", "groups", "applications", "departments",
              "servers", "workstations", "terminals", "printers",
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
      register_global("ogfilter", $ogfilter);
      $this->reload();

      /* Calculate actual groups */
      $smarty->assign("objects", $this->convert_list($this->objects));

      /* Show dialog */
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("dselect", "regex", "accounts", "groups", "applications",
            "departments", "servers", "workstations", "terminals",
            "printers", "phones") as $type){
        $smarty->assign("$type", $ogfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("apply", apply_filter());

      $display= $smarty->fetch (get_template_path('ogroup_objects.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Bases / Departments */
   
      if (isset($_POST['base'])){
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
          "T" => _("terminals"),
          "F" => _("phones"),
          "P" => _("printers"));

      $type= preg_replace('/[\[\]]/', '', $this->gosaGroupObjects);
      $p1= $conv[$type[0]];
      error_reporting(0);
      if (isset($type[1]) && preg_match('/[UGADSFWTP]/', $type[1])){
        $p2= $conv[$type[1]];
        $smarty->assign("combinedObjects", "$p1 "._("and")." $p2");
      } else {
        $smarty->assign("combinedObjects", "$p1");
      }
      error_reporting(E_ALL);
    }

    /* Assign variables */
    $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("base_select", $this->base);
    $smarty->assign("department", $this->department);
    $smarty->assign("members", $this->convert_list($this->memberList));

    /* Objects have to be tuned... */
    $smarty->assign("objects", $this->convert_list($this->objects));

    /* Fields */
    foreach ($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    /* Assign ACL's */
    foreach (array("base", "members") as $val){
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    return ($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  /* Save data to object */
  function save_object()
  {
    /* Save additional values for possible next step */
    if (isset($_POST['ogroupedit'])){
      plugin::save_object();

      if (chkacl ($this->acl, "base") == "" && isset($_POST["base"])){
        $this->base= $_POST["base"];
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
    
    $ogfilter               = get_global("ogfilter");
    $regex                  = $ogfilter['regex'];

    /* Get ldap connection */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($ogfilter['dselect']);


    /*###########
      Generate Filter 
      ###########*/

    /* Assemble filter */
    if ($ogfilter['accounts'] == "checked"){
      $filter.= "(objectClass=gosaAccount)";
      $objectClasses["gosaAccount"]     = get_people_ou();
    }
    if ($ogfilter['groups'] == "checked"){
      $filter.= "(objectClass=posixGroup)";
      $objectClasses["posixGroup"]      = get_groups_ou();
    }
    if ($ogfilter['applications'] == "checked"){
      $filter.= "(objectClass=gosaApplication)";
      $objectClasses["gosaApplication"] = "ou=apps,";
    }
    if ($ogfilter['departments'] == "checked"){
      $filter.= "(objectClass=gosaDepartment)";
      $objectClasses["gosaDepartment"]  = "";
    }
    if ($ogfilter['servers'] == "checked"){
      $filter.= "(objectClass=goServer)";
      $objectClasses["goServer"]        = "ou=servers,ou=systems,";
    }
    if ($ogfilter['workstations'] == "checked"){
      $filter.= "(objectClass=gotoWorkstation)";
      $objectClasses["gotoWorkstation"] = "ou=workstations,ou=systems,";
    }
    if ($ogfilter['terminals'] == "checked"){
      $filter.= "(objectClass=gotoTerminal)";
      $objectClasses["gotoTerminal"]    = "ou=terminals,ou=systems,";
    }
    if ($ogfilter['printers'] == "checked"){
      $filter.= "(objectClass=gotoPrinter)";

      $objectClasses["gotoPrinter"]     = "ou=printers,ou=systems,";
    }
    if ($ogfilter['phones'] == "checked"){
      $filter.= "(objectClass=goFonHardware)";
      $objectClasses["goFonHardware"]   = "ou=phones,ou=systems,";
    }


    /*###########
      Perform search for selected objectClasses & regex to fill list with objects   
      ###########*/

    /* Perform search for selected objectClasses */
    foreach($objectClasses as $class=> $basedn){
      $ldap->ls("(&(objectClass=".$class.")(|(uid=$regex)(cn=$regex)(ou=$regex)))",$basedn.$ogfilter['dselect'] ,
          array("dn", "cn", "description", "objectClass", "sn", "givenName", "uid","ou"));
      
      /* fetch results and append them to the list */
      while($attrs = $ldap->fetch()){

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
    uasort ($this->objects, 'sort_list');
    reset ($this->objects);

    
    /*###########
      Build member list and try to detect obsolete entries 
      ###########*/

    $this->memberList = array();
  
    /* Walk through all single member entry */
    foreach($this->member as $dn){
      $this->importMember($dn);
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


  function importMember($dn)
  {
    $ldap= $this->config->get_ldap_link();

    /* Try to resolv the entry again, if it still fails, display error msg */
    $ldap->cat($dn, array("cn", "sn", "givenName", "ou", "description", "objectClass"));

    /* It has failed, add entry with type flag I (Invalid)*/
    if ($ldap->error != "Success"){
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

  function convert_list($input)
  {
    $temp= "";
    $conv= array(  "U" => "select_user.png",
        "G" => "select_groups.png",
        "A" => "select_application.png",
        "D" => "select_department.png",
        "S" => "select_server.png",
        "W" => "select_workstation.png",
        "T" => "select_terminal.png",
        "F" => "select_phone.png",
        "I" => "flag.png",
        "P" => "select_printer.png");

    foreach ($input as $key => $value){
      /* Generate output */
      $temp.= "<option value=\"$key\" class=\"select\" style=\"background-image:url('".get_template_path("images/".$conv[$value['type']])."');\">".$value['text']."</option>\n";
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
      $new_dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
    } else {
      $new_dn= $this->dn;
    }


    $ldap = $this->config->get_ldap_link();
    if($this->dn != $new_dn){
      $ldap->cat ($new_dn, array('dn'));
    }
    
    if($ldap->count() !=0){
      $message[]= _("There is already an object with this cn.");
    } 

    $ui= get_userinfo();
    $acl= get_permissions ($new_dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "group", $new_dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a group on this 'Base'.");
    }

    /* must: cn */
    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= "The required field 'Name' is not set.";
    }

    /* To many different object types? */
    if (strlen($this->gosaGroupObjects) > 4){
      $message[]= _("You can combine two different object types at maximum only!");
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
      $this->dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
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

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->$mode($this->attrs);

    /* Trigger post signal */
    $this->handle_post_events($mode);

    $ret= 0;
    if (show_ldap_error($ldap->get_error(), _("Saving object group failed"))){
      $ret= 1;
    }

    return ($ret);
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing object group failed"));

    /* Trigger remove signal */
    $this->handle_post_events("remove");
  }

  function getCopyDialog()
  {
    $str  = "";
    $str .= _("Group name");
    $str .= "&nbsp;<input type='text' name='cn' value='".$this->cn."'>";
    return($str);
  }

  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
