<?php

class faiScript extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIscript");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAIscriptEntry";
  var $subClasses       = array("top","FAIclass","FAIscriptEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "faiScriptEntry";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description","FAIpriority"); 
  var $sub_Load_Later   = array("FAIscript");
  var $sub64coded       = array();
  var $subBinary        = array("FAIscript");

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  var $FAIstate         = "";
  var $sort_by          = "name";
  var $sort_order       = "up";

  var $base             ="";
  var $release          ="";
  var $copy_paste_mode  = false;
  var $cut_paste_mode   = false;
  var $CopyPasteVars    = array("SubObjects");


  function faiScript ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    $this->acl ="#all#";
    
    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */
    if($dn != "new"){

      /* Set acls
       */
      $ui   = get_userinfo();
      $acl  = get_permissions ($this->dn, $ui->subtreeACL);
      $acli = get_module_permission($acl, "FAIclass", $this->dn);
      $this->acl=$acli;

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
      $ldap->search("(&(objectClass=FAIclass)(objectClass=".$this->subClass."))",$this->subAttributes);

      while($object = $ldap->fetch()){
        /* Set status for save management */
        $objects = array();
        $objects['status']      = "FreshLoaded";
        $objects['dn']          = $object['dn'];
        $objects                = $this->get_object_attributes($objects,$this->subAttributes);
        $this->SubObjects[$objects['cn']] = $objects;

      }
    }
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


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new sub object */
    if(isset($_POST['AddSubObject'])){
      $this->dialog= new $this->subClassName($this->config,"new");
      $this->dialog->acl = $this->acl;
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }

    if($this->dn != "new"){
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Handle posts */
    $s_action = $s_entry = "";
    foreach($_POST as $name => $value){

      /* Edit script posted */
      if(preg_match("/^editscript_/",$name)){
        $s_action = "edit";
        $s_entry = preg_replace("/^editscript_/","",$name);
        $s_entry = base64_decode(preg_replace("/_.*/","",$s_entry));
        break;
      }

      /* Delete script requested */
      if(preg_match("/^deletescript_/",$name)){
        $s_action = "remove";
        $s_entry = preg_replace("/^deletescript_/","",$name);
        $s_entry = base64_decode(preg_replace("/_.*/","",$s_entry));
        break;
      }
    }

    if(isset($_GET['act']) && $_GET['act'] == "edit" && isset($_GET['id'])){
      $s_entry = $_GET['id'];
      if(isset($this->SubObjects[$s_entry])){
        $s_action = "edit";
      }
    }

    if($s_action =="edit" && isset($this->SubObjects[$s_entry])){

      /* Get object, and load missing entry values */
      $obj  = $this->SubObjects[$s_entry];
      if($obj['status'] == "FreshLoaded"){
        $obj  = $this->get_object_attributes($obj,$this->sub_Load_Later);
      }
      
      /* Create new dialog and set acl attributes  */
      $this->dialog= new $this->subClassName($this->config,$this->dn,$obj);
      $this->dialog->acl = $this->acl;
      $_SESSION['objectinfo'] = $obj['dn'];
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }

    /* Check acls, are we allowed to delete an entry */
    if($s_action == "remove" && isset($this->SubObjects[$s_entry])){
      $entry = $this->SubObjects[$s_entry];
      $status = $entry['status'];
      if($status == "edited" || $status == "FreshLoaded"){
        $this->SubObjects[$s_entry]['status']= "delete";
      }else{
        unset($this->SubObjects[$s_entry]);
      }
    }

    ///// Ende new list handling
    if(isset($_POST['SaveSubObject'])){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
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
        $this->dialog=NULL;
      }
    }

    /* Sort entries */
    $tmp = $keys = array();

    if($this->sort_by == "name"){
      foreach($this->SubObjects as $key => $entry){
        $keys[$key]=$entry['cn'];
      }
    }elseif($this->sort_by == "priority"){
      foreach($this->SubObjects as $key => $entry){
        $keys[$key]=$entry['FAIpriority'];
      }
    }

    natcasesort($keys);

    if($this->sort_order == "down"){
      $keys =array_reverse($keys);
    }

    foreach($keys as $key => $order_var){
      $tmp[$key]=$this->SubObjects[$key];
    }
    $this->SubObjects = $tmp;

    /* Cancel Dialog */
    if(isset($_POST['CancelSubObject'])){
      $this->is_dialog=false; 
      unset($this->dialog);
      $this->dialog=NULL;
    }

    /* Print dialog if $this->dialog is set */
    if($this->dialog){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return($display);
    }

    /* Divlist            added 23.02.2006 
       Containing FAIscripts 
     */
    $divlist = new divlist("FAIscripts");
    $divlist -> SetEntriesPerPage(0); 
    $plug = $_GET['plug'];

     if($this->sort_order == "up"){
       $dir = "<img src='images/sort_up.png' title='"._("Sort direction")."' alt='\/' border=0>";
     }else{
       $dir = "<img src='images/sort_down.png' title='"._("Sort direction")."' alt='/\' border=0>";
     }

     if($this->sort_by == "name"){
       $sort_name = $dir;
       $sort_prio = "";
     }else{
       $sort_name = "";
       $sort_prio = $dir;
     }

     $divlist->SetHeader(array(  
          array("string"=>"<a href='?plug=".$plug."&amp;sort=name'>"._("Name").$sort_name."</a>"),
          array("string"=>"<a href='?plug=".$plug."&amp;sort=priority'>".$sort_prio._("Priority")."</a>",
                "attach" => "style='width:100px;'"),
          array("string"=>_("Download"),
                "attach" => "style='width:100px;'"),
          array("string"=>_("Action"),
                "attach"=>"style='border-right: 0px;width:100px;text-align:right;'")));
    $divlist->SetHeight(300);
    $divlist->SetWidth("100%");

    if((chkacl($this->acl,"cn")!="") || ($this->FAIstate == "freeze")){
      $img_edit = "<input type='image' src='images/edit.png'      name='editscript_%s'    title='"._("edit")."' alt='"._("edit")."'>";
      $img_remo = "";
    }else{
      $img_edit = "<input type='image' src='images/edit.png'      name='editscript_%s'    title='"._("edit")."' alt='"._("edit")."'>";
      $img_remo = "<input type='image' src='images/edittrash.png' name='deletescript_%s'  title='"._("delete")."' alt='"._("delete")."'>";
    }

    foreach($this->getList(true) as $key => $name){

      if(($this->SubObjects[$key]['status'] == "new") || ($this->SubObjects[$key]['dn'] == "new")){
        $down = "";
      }else{
        $down = "<a href='getFAIscript.php?id=".base64_encode($name['dn'])."' >
          <img src='images/save.png' alt='"._("Download")."' title='"._("Download")."' border=0>
          </a>"; 
      } 

      $divlist->AddEntry(array( array("string"=>"<a href='?plug=".$_GET['plug']."&act=edit&id=".$key."'>".$name['name']."</a>"),
            array("string"=>$name['FAIpriority'] , "attach" => "style='width:100px;'"),
            array("string"=>$down , "attach" => "style='width:100px;'"),
            array("string"=>str_replace("%s",base64_encode($key),$img_edit.$img_remo),
              "attach"=>"style='border-right: 0px;width:100px;text-align:right;'")));
    }
    $smarty->assign("Entry_divlist",$divlist->DrawList());

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

    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    $display.= $smarty->fetch(get_template_path('faiScript.tpl', TRUE));
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
          $a_return[$obj['cn']]['FAIpriority']= $obj['FAIpriority'];
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
    $ldap->rmdir_recursive($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing FAI script base failed"));
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    if((isset($_POST['FAIscript_posted'])) && ($this->FAIstate != "freeze")){
      plugin::save_object();
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }
      }
    }
    
    /* Get sort order */
    if(isset($_GET['sort']) && in_array($_GET['sort'],array("name","priority"))){
      if($this->sort_by == $_GET['sort']){
        if($this->sort_order == "up"){
          $this->sort_order = "down";
        }elseif($this->sort_order == "down"){
          $this->sort_order = "up";
        }
      }
      $this->sort_by = $_GET['sort'];
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* If this is a new script, check if a script with this name already exists */
    if(!empty($this->release) && ($this->copy_paste_mode || $this->cut_paste_mode) ){
     
      /* Check if current name is already used for fai scripts in selected release */ 
      $dn = 'cn='.$this->cn.",ou=scripts,".$this->release;
      $ldap = $this->config->get_ldap_link(); 
      $ldap->cat($dn);
      if($ldap->count()){

        $r =convert_department_dn($this->release);;
        $message[] = sprintf(_("Can't insert a script named '%s' in '%s' there is already a script with the given name."),$this->cn,$r);
      }
    }
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    $ldap = $this->config->get_ldap_link();

    /* Copy & Paste : Ensure that FAIstate is copied too */
    if($this->copy_paste_mode && preg_match("/freeze/",$this->FAIstate)){
      $this->attrs['FAIstate'] = $this->FAIstate;
    }

    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), _("Creating FAI script base failed"));

    /* Prepare FAIscriptEntry to write it to ldap
     * First sort array.
     *  Because we must delete old entries first.
     * After deletion, we perform add and modify 
     */
    $Objects = array();

    /* We do not need to save untouched objects */
    foreach($this->SubObjects as $name => $obj){
      if($obj['status'] == "FreshLoaded"){
        if($this->copy_paste_mode){
          $this->SubObjects[$name] = $this->get_object_attributes($obj,$this->sub_Load_Later);
          $this->SubObjects[$name]['status'] = "new";
        }else{
          unset($this->SubObjects[$name]);
        }
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

      if(empty($tmp['FAIpriority'])){
        $tmp['FAIpriority']  ="0";
      }

      /* Tag object */
      $this->tag_attrs(&$tmp, $sub_dn, $this->gosaUnitTag);

      if($obj['status'] == "delete"){
        $ldap->cd($sub_dn);
        $ldap->rmdir_recursive($sub_dn);
        $this->handle_post_events("remove");
        show_ldap_error($ldap->get_error(), _("Removing FAI script failed")); 
      }elseif($obj['status'] == "edited"){
        $ldap->cd($sub_dn);
        $this->cleanup();
        $ldap->modify ($tmp); 

        $this->handle_post_events("modify");
      }elseif($obj['status']=="new"){
        if($tmp['description']==array()){
          unset($tmp['description']);
        }
        if($tmp['FAIscript']==array()){
          $tmp['FAIscript']=" ";
        }
        $ldap->cd($this->config->current['BASE']);
        $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        $ldap->cd($sub_dn);
        $ldap->add($tmp); 
        $this->handle_post_events("add");
        show_ldap_error($ldap->get_error(), _("Saving FAI script failed")); 
      }
    }
  }

  
  /* return copy & paste dialog
   */
  function getCopyDialog()
  {
    /* Ask for cn */
    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $str = $smarty->fetch(get_template_path("paste_fai_object.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }

  /* Get posted cn */
  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
