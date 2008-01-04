<?php

class faiHook extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIhook");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAIhookEntry";
  var $subClasses       = array("top","FAIclass","FAIhookEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "faiHookEntry";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description","FAItask"); 
  var $sub_Load_Later   = array("FAIscript");
  var $sub64coded       = array();
  var $subBinary        = array("FAIscript");

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $SubObjects       = array();  // All leafobjects of this object
  var $view_logged      = FALSE;
  var $FAIstate         = "";  
  var $ui;

  function faiHook (&$config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     */
    if($dn != "new"){
      $this->dn =$dn;
    
      /* Get FAIstate
       */
      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }

      /* Read all leaf objects of this object (For FAIscript this would be FAIscriptEntry)
       */
      $ldap     = $this->config->get_ldap_link();
      $ldap->cd ($this->dn);
      $attrs_to_search = $this->subAttributes;
      $attrs_to_search[] = "FAIstate";
      $ldap->search("(&(objectClass=FAIclass)(objectClass=".$this->subClass."))", $attrs_to_search);

      while($object = $ldap->fetch()){

       /* Skip objects, that are tagged as removed */
        if(isset($object['FAIstate'][0])){
          if(preg_match("/removed$/",$object['FAIstate'][0])){
            continue;
          }
        }

        /* Set status for save management */
        $objects = array();
        $objects['status']      = "FreshLoaded";
        $objects['dn']          = $object['dn'];
        $objects                = $this->get_object_attributes($objects,$this->subAttributes);
        $this->SubObjects[$objects['cn']] = $objects;
      }
    }
    $this->ui = get_userinfo();
  }


  /* Reload some attributes */
  function get_object_attributes($object,$attributes)
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->cat($object['dn'],$attributes);
    $tmp  = $ldap->fetch();

    foreach($attributes as $attrs){
      if(isset($tmp[$attrs][0])){
        $var = $tmp[$attrs][0];

        /* Check if we must decode some attributes */
        if(in_array_ics($attrs,$this->sub64coded)){
          $var = base64_decode($var);
        }

        /*  check if this is a binary entry */
        if(in_array_ics($attrs,$this->subBinary)){
          $var = $ldap->get_attribute($object['dn'], $attrs,$r_array=0);
        }

        /* Fix slashes */
        $var = addslashes($var);
        $object[$attrs] = $var;
      }
    }
    return($object);
  }


  function acl_base_for_current_object($dn)
  {
    if($dn == "new"){
      if($this->dn == "new"){
        $dn= $_SESSION['CurrentMainBase'];
      }else{
        $dn = $this->dn;
      }
    }
    return($dn);
  }


  function getUsedFAItask($cn)
  {
    $ret = array();
    foreach($this->SubObjects as $name => $class){
      if($class['cn'] == $cn){
        continue;
      } 
      if($class['status'] != "delete"){
        $ret[$class['FAItask']] = $class['FAItask'];
      }
    }
    return($ret);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","fai/".get_class($this),$this->dn);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* New Listhandling
     */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^editscript_/",$name)&&($once)){
        $once = false;
        $entry = preg_replace("/^editscript_/","",$name);
        $entry = base64_decode(preg_replace("/_.*/","",$entry));

        $obj  = $this->SubObjects[$entry];
        if($obj['status'] == "FreshLoaded"){
          $obj  = $this->get_object_attributes($obj,$this->sub_Load_Later);
        }

        $this->dialog= new $this->subClassName($this->config,$this->dn,$obj);
        $this->dialog->set_acl_base($this->acl_base_for_current_object($obj['dn']));
        $this->dialog->set_acl_category("fai");

        session::set('objectinfo',$obj['dn']);
        $this->dialog->parent = &$this;
        $this->is_dialog=true;
      }
      if(preg_match("/^deletescript_/",$name)&&($once)){
        $entry = preg_replace("/^deletescript_/","",$name);
        $entry = base64_decode(preg_replace("/_.*/","",$entry));

        $dn = $this->acl_base_for_current_object($this->SubObjects[$entry]['dn']);
        $acl = $this->ui -> get_permissions($dn,"fai/faiScriptEntry")  ;

        if(preg_match("/d/",$acl)){
          $once = false;

          $status = $this->SubObjects[$entry]['status'];
          if($status == "edited" || $status == "FreshLoaded"){
            $this->SubObjects[$entry]['status']= "delete";
          }else{
            unset($this->SubObjects[$entry]);
          }
        }
      }
    }

    /* Edit entries via GET */
    if(isset($_GET['act']) && isset($_GET['id'])){
      if($_GET['act'] == "edit" && isset($this->SubObjects[$_GET['id']])){
        $obj = $this->SubObjects[$_GET['id']];
          if($obj['status'] == "FreshLoaded"){
          $obj  = $this->get_object_attributes($obj,$this->sub_Load_Later);
        }
        $this->dialog= new $this->subClassName($this->config,$this->dn,$obj);
        $this->dialog->acl = $this->acl;
        session::set('objectinfo',$obj['dn']);
        $this->dialog->parent = &$this;
        $this->is_dialog=true;
      }
    }

    ///// Ende new list handling

    /* Add new sub object */
    if(isset($_POST['AddSubObject'])){

      $c_dn = $this->acl_base_for_current_object($this->dn);
      $this->dialog= new $this->subClassName($this->config,"new");
      $this->dialog->set_acl_base($c_dn);
      $this->dialog->set_acl_category("fai");
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }


    /* Save Dialog */
    if(isset($_POST['SaveSubObject'])){

      /* Perform post check*/
      $this->dialog->save_object();

      /* Get messages */
      $msgs = $this->dialog->check();

      /* print errors */
      if(count($msgs)>0){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{

        /* Get return object */
        $obj = $this->dialog->save();
        if(isset($obj['remove'])){

          $old_stat = $this->SubObjects[$obj['remove']['from']]['status'];
  
          /* Depending on status, set new status */
          if($old_stat == "edited" || $old_stat == "FreshLoaded"){
            $this->SubObjects[$obj['remove']['from']]['status'] = "delete";
          }elseif($this->SubObjects[$obj['remove']['from']]['status']=="new"){
            unset($this->SubObjects[$obj['remove']['from']]);
          }
          $obj['status'] = "new";
          $this->SubObjects[$obj['remove']['to']] = $obj;
          unset($this->SubObjects[$obj['remove']['to']]['remove']);
        }else{
          if($obj['status'] == "FreshLoaded"){
            $obj['status'] = "edited";
          }
          $this->SubObjects[$obj['cn']]=$obj;
        }
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=FALSE;
      }
    }

    /* Sort entries */
    $tmp = $keys = array();
    foreach($this->SubObjects as $key => $entry){
      $keys[$key]=$key;
    }
    natcasesort($keys);
    foreach($keys as $key){
      $tmp[$key]=$this->SubObjects[$key];
    }
    $this->SubObjects = $tmp;

    /* Cancel Dialog */
    if(isset($_POST['CancelSubObject'])){
      $this->is_dialog=false; 
      unset($this->dialog);
      $this->dialog=FALSE;
    }

    /* Print dialog if $this->dialog is set */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return($display);
    }



    /* Divlist            added 28.02.2006
       Containing FAIscripts
     */

    $divlist = new divSelectBox("FAIhooks");
    $divlist->setHeight(400);

    foreach($this->getList(true) as $key => $name){

      $dn= $this->acl_base_for_current_object($name['dn']);
      $acl = $this->ui->get_permissions($dn,"fai/faiHookEntry")  ;
      $act = "";

      /* Check if this object is freezed, in this case hide the delete icon */
      if($this->FAIstate == "freeze"){
        $act .= "<input type='image' src='images/edit.png'      name='editscript_%s'    title='"._("edit")."' alt='"._("edit")."'>";
      }else{
        $act .= "<input type='image' src='images/edit.png'      name='editscript_%s'    title='"._("edit")."' alt='"._("edit")."'>";
        if(preg_match("/d/",$acl)){
          $act .="<input type='image' src='images/edittrash.png' name='deletescript_%s'  title='"._("delete")."' alt='"._("delete")."'>";
        }
      }

      /* Check if we are allowed to use the export button for this object */
      $s_acl = $this->ui->get_permissions($dn,"fai/faiHookEntry","FAIscript")  ;
      if(($this->SubObjects[$key]['status'] == "new") || ($this->SubObjects[$key]['dn'] == "new") || !preg_match("/r/",$s_acl)){
        $down = "";
      }else{
        $down = "<a href='getFAIscript.php?id=".base64_encode($name['dn'])."'>
          <img src='images/save.png' alt='"._("Download")."' title='"._("Download")."' border=0>
          </a>";
      }

      /* Check if we are allowed to view the object */
      $s_acl = $this->ui->get_permissions($dn,"fai/faiHookEntry","cn")  ;
      if(preg_match("/r/",$s_acl)){

        $edit_link = "<a href='?plug=".$_GET['plug']."&amp;act=edit&amp;id=".$key."'>".$name['name']."</a>";
        $divlist->AddEntry(array( array("string"=>$edit_link),
              array("string"=>$down , "attach" => "style='width:20px;'"),
              array("string"=>str_replace("%s",base64_encode($key),$act),
                "attach"=>"style='border-right: 0px;width:50px;text-align:right;'")));
      }
    }
    $smarty->assign("Entry_divlist",$divlist->DrawList());
    /* Divlist creation complete
     */

    $smarty->assign("SubObjects",$this->getList());

    /* Magic quotes GPC, escapes every ' " \, to solve some security risks
     * If we post the escaped strings they will be escaped again
     */
    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
        $smarty->assign($attrs,stripslashes($this->$attrs));
      }else{
        $smarty->assign($attrs,($this->$attrs));
      }
    }

    $tmp = $this->plInfo();
      
    $c_dn = $this->acl_base_for_current_object($this->dn);
    $smarty->assign("sub_object_is_addable", preg_match("/c/",$this->ui->get_permissions($c_dn,"fai/faiHookEntry")) && $this->FAIstate!="freeze");
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $display.= $smarty->fetch(get_template_path('faiHook.tpl', TRUE));
    return($display);
  }

  /* Generate listbox friendly SubObject list
   */
  function getList($use_dns=false){
    $a_return=array();
    foreach($this->SubObjects as $obj){
      if($obj['status'] != "delete"){
        if($use_dns){
          if((isset($obj['description']))&&(!empty($obj['description']))){
            $a_return[$obj['cn']]['name']= $obj['cn']." [".stripslashes($obj['description'])."]";
          }else{
            $a_return[$obj['cn']]['name']= $obj['cn'];
          }
          $a_return[$obj['cn']]['dn']= $obj['dn'];
        }else{
          if((isset($obj['description']))&&(!empty($obj['description']))){
            $a_return[$obj['cn']]= $obj['cn']." [".stripslashes($obj['description'])."]";
          }else{
            $a_return[$obj['cn']]= $obj['cn'];
          }
        }
      }
    }
    return($a_return);
  }


  /* Delete me, and all my subtrees
   */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);

#    $use_dn = str_ireplace( get_release_dn($this->dn), $_SESSION['faifilter']['branch'], $this->dn);
    $use_dn = preg_replace("/".normalizePreg(get_release_dn($this->dn))."/i", $_SESSION['faifilter']['branch'], $this->dn);
    if($_SESSION['faifilter']['branch'] == "main"){
      $use_dn = $this->dn;
    }

    prepare_to_save_FAI_object($use_dn,array(),true);

    new log("remove","fai/".get_class($this),$use_dn,$this->attributes);

    foreach($this->SubObjects as $name => $obj){
#      $use_dn = str_ireplace( get_release_dn($this->dn), $_SESSION['faifilter']['branch'], $obj['dn']);
      $use_dn = preg_replace("/".normalizePreg(get_release_dn($this->dn))."/i", $_SESSION['faifilter']['branch'], $obj['dn']);
      if($_SESSION['faifilter']['branch'] == "main"){
        $use_dn = $obj['dn'];
      }
      prepare_to_save_FAI_object($use_dn,array(),true);
    }
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    if((isset($_POST['FAIhook_posted'])) && ($this->FAIstate != "freeze")){
      plugin::save_object();
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    $ldap = $this->config->get_ldap_link();

    prepare_to_save_FAI_object($this->dn,$this->attrs);
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of FAI/hook with dn '%s' failed."),$this->dn));

    if($this->initially_was_account){
      new log("modify","fai/".get_class($this),$this->dn,$this->attributes);
    }else{
      new log("create","fai/".get_class($this),$this->dn,$this->attributes);
    }

    /* Do object tagging */
    $this->handle_object_tagging();

    $ldap->cd($this->dn);

    /* Prepare FAIscriptEntry to write it to ldap
     * First sort array.
     *  Because we must delete old entries first.
     * After deletion, we perform add and modify 
     */
    $Objects = array();

    /* We do not need to save untouched objects */
    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] == "FreshLoaded"){
        unset($this->SubObjects[$name]);
      }
    }

    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] == "delete"){
        $Objects[$name] = $obj; 
      }
    }
    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] != "delete"){
        $Objects[$name] = $obj; 
      }
    }

    foreach($Objects as $name => $obj){

      foreach($this->sub64coded as $codeIt){
        $obj[$codeIt]=base64_encode(stripslashes($obj[$codeIt]));
      }

      $tmp = array();
      $attributes = array_merge($this->sub_Load_Later,$this->subAttributes);
      foreach($attributes as $attrs){
        if(empty($obj[$attrs])){
          $obj[$attrs] = array();
        }
        if(!is_array($obj[$attrs])){
          $tmp[$attrs] = stripslashes($obj[$attrs]);
        }else{
          $tmp[$attrs] = $obj[$attrs];
        }
      }    

      $tmp['objectClass'] = $this->subClasses;

      $sub_dn = "cn=".$obj['cn'].",".$this->dn;

      if($obj['status']=="new"){
        $ldap->cat($sub_dn,array("objectClass"));
        if($ldap->count()){
          $obj['status']="edited";
        }
      }

      /* Check if gosaAdministrativeUnitTag is required as object class */
      if($obj['status'] == "edited"){
        $ldap->cat($sub_dn,array("objectClass"));
        $attrs = $ldap->fetch();
        if(isset($attrs['objectClass'])){
          if(in_array_ics("gosaAdministrativeUnitTag",$attrs['objectClass'])){
            $tmp['objectClass'][] = "gosaAdministrativeUnitTag";
          }
        }
      }

      if($obj['status'] == "delete"){
        prepare_to_save_FAI_object($sub_dn,array(),true);
        $this->handle_post_events("remove");
      }elseif($obj['status'] == "edited"){
        prepare_to_save_FAI_object($sub_dn,$tmp);
        $this->handle_post_events("modify");
      }elseif($obj['status']=="new"){
        prepare_to_save_FAI_object($sub_dn,$tmp);
        $this->handle_post_events("add");
      }
      $this->handle_object_tagging($sub_dn, $this->gosaUnitTag);
    }
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    /* Read all leaf objects of this object (For FAIscript this would be FAIscriptEntry)
     */
    $ldap     = $this->config->get_ldap_link();
    $ldap->cd ($source['dn']);

    $attrs_to_search = $this->subAttributes;
    $attrs_to_search[] = "FAIstate";
    $ldap->search("(&(objectClass=FAIclass)(objectClass=".$this->subClass."))",$attrs_to_search);

    while($object = $ldap->fetch()){

      /* Skip objects, that are tagged as removed */
      if(isset($object['FAIstate'][0])){
        if(preg_match("/removed$/",$object['FAIstate'][0])){
          continue;
        }
      }

      /* Set status for save management */
      $objects = array();
      $objects['status']      = "edited";
      $objects['dn']          = $object['dn'];
      $objects                = $this->get_object_attributes($objects,$this->subAttributes);
      $objects                = $this->get_object_attributes($objects,$this->sub_Load_Later);

      $this->SubObjects[$objects['cn']] = $objects;
    }
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName" => _("Hook"),
          "plDescription" => _("FAI hook"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 20,
          "plSection"     => array("administration"),
          "plCategory"    => array("fai"),
          "plProvidedAcls" => array(
            "cn"                => _("Name")." ("._("Read only").")",
            "description"       => _("Description"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
