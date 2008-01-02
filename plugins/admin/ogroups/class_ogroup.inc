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
  var $view_logged = FALSE;

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaGroupObjects","member");
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
          "_" => "subtrees",
          "P" => "printers") as $key => $val){

      if (preg_match("/$key/", $this->gosaGroupObjects)){
        $ogfilter[$val]= "checked";
      } else {
        $ogfilter[$val]= "";
      }
    }
    register_global("ogfilter", $ogfilter);
  
    if(isset($_SESSION['CurrentMainBase'])){
     $this->base = $_SESSION['CurrentMainBase'];
    }

    /* Set base */
    if ($this->dn == "new"){
      $this->base= $_SESSION['CurrentMainBase'];
    } else {
      $this->base= preg_replace("/^[^,]+,".get_groups_ou()."/","",$this->dn);
    }

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

    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","ogroups/".get_class($this),$this->dn);
    }


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
      $ogfilter= get_global("ogfilter");
      foreach( array("dselect", "regex") as $type){
        if (isset($_POST[$type])){
          $ogfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_POST['dselect'])){
        foreach( array("accounts", "groups", "applications", "departments",
              "servers", "workstations", "terminals", "printers","subtrees",
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
            "departments", "servers", "workstations", "terminals","subtrees",
            "printers", "phones") as $type){
        $smarty->assign("$type", $ogfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("apply", apply_filter());

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

    return ($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  /* Save data to object */
  function save_object()
  {
    /* Save additional values for possible next step */
    if (isset($_POST['ogroupedit'])){

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
    
    $ogfilter               = get_global("ogfilter");
    $regex                  = $ogfilter['regex'];

    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($ogfilter['dselect']);


    /*###########
      Generate Filter 
      ###########*/

    $p_f= array("accounts"        => array("CLASS"=>"gosaAccount"    ,"DN"=> get_people_ou()                  ,"ACL" => "users"), 
                "groups"          => array("CLASS"=>"posixGroup"     ,"DN"=> get_groups_ou()                  ,"ACL" => "groups"), 
                "applications"    => array("CLASS"=>"gosaApplication","DN"=> get_ou('applicationou')          ,"ACL" => "application"), 
                "departments"     => array("CLASS"=>"gosaDepartment" ,"DN"=> ""                               ,"ACL" => "department"), 
                "servers"         => array("CLASS"=>"goServer"       ,"DN"=> get_ou('serverou')               ,"ACL" => "server"),
                "workstations"    => array("CLASS"=>"gotoWorkstation","DN"=> "ou=workstations,ou=systems,"    ,"ACL" => "workstation"),
                "terminals"       => array("CLASS"=>"gotoTerminal"   ,"DN"=> "ou=terminals,ou=systems,"       ,"ACL" => "terminal"),
                "printers"        => array("CLASS"=>"gotoPrinter"    ,"DN"=> "ou=printers,ou=systems,"        ,"ACL" => "printer"),
                "phones"          => array("CLASS"=>"goFonHardware"  ,"DN"=> "ou=phones,ou=systems,"          ,"ACL" => "phone"));


    /*###########
      Perform search for selected objectClasses & regex to fill list with objects   
      ###########*/

    $Get_list_flags = 0;
    if($ogfilter['subtrees'] == "checked"){
      $Get_list_flags |= GL_SUBSEARCH;
    }    

    foreach($p_f as $post_name => $data){

      if($ogfilter[$post_name] == "checked"){

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
      $temp.= "<option title='".addslashes( $key)."' value=\"$key\" class=\"select\" style=\"background-image:url('".get_template_path("images/".$conv[$value['type']])."');\">".$value['text']."</option>\n";
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

    /* Set new acl base */
    if($this->dn == "new") {
      $this->set_acl_base($this->base);
    }

    /* must: cn */
    if ($this->cn == ""){
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

    if($mode == "add"){
      new log("create","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("modify","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    /* Trigger post signal */
    $this->handle_post_events($mode);

    $ret= 0;
    if (show_ldap_error($ldap->get_error(), sprintf(_("Saving of object group/generic with dn '%s' failed."),$this->dn))){
      $ret= 1;
    }

    return ($ret);
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of object group/generic with dn '%s' failed."),$this->dn));

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
