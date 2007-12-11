<?php

class faiVariable extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIvariable");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAIvariableEntry";
  var $subClasses       = array("top","FAIclass","FAIvariableEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "faiVariableEntry";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description","FAIvariableContent"); 
  var $sub64coded       = array();  

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  var $FAIstate         = "";
  var $base             = "";
  var $release          = "";
  var $copy_paste_mode  = false;
  var $cut_paste_mode   = false;

  var $CopyPasteVars  = array("SubObjects");

  function faiVariable ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    $this->acl ="#all#";

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */

    if($dn != "new"){
      $this->dn =$dn;

      /* Set acls
       */
      $ui   = get_userinfo();
      $acl  = get_permissions ($this->dn, $ui->subtreeACL);
      $acli = get_module_permission($acl, "FAIclass", $this->dn);
      $this->acl=$acli;

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
  
        foreach($this->subAttributes as $attrs){
          if(!isset($object[$attrs][0])){
            $this->SubObjects[$object['cn'][0]][$attrs]="";
          }else{
            $this->SubObjects[$object['cn'][0]][$attrs]=$object[$attrs][0];
          }
        }
     
        foreach($this->sub64coded as $codeIt){
          $this->SubObjects[$object['cn'][0]][$codeIt]=base64_decode($this->SubObjects[$object['cn'][0]][$codeIt]);
        }
 
        $this->SubObjects[$object['cn'][0]]['status']      = "edited";
        $this->SubObjects[$object['cn'][0]]['dn']          = $object['dn'];
      }
    }
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
      $this->dialog         = new $this->subClassName($this->config,"new");
      $this->dialog->acl    = $this->acl;
      $this->dialog->parent = $this;
      $this->is_dialog=true;
    }

    if($this->dn != "new"){
      $_SESSION['objectinfo']= $this->dn;
    }


    /* Edit selected Sub Object */
    if((isset($_POST['EditSubObject']))&&(isset($_POST['SubObject']))){

      $var = $_POST['SubObject'][0];
      $this->dialog= new $this->subClassName($this->config,$this->dn,$this->SubObjects[$var]);
      $this->dialog->acl = $this->acl;
      $this->dialog->parent = $this;
      $_SESSION['objectinfo'] = $this->SubObjects[$var]['dn'];
      $this->is_dialog=true;
    }
    
    /* Remove Sub object */
    if((isset($_POST['DelSubObject']))&&(isset($_POST['SubObject']))){
      foreach($_POST['SubObject'] as $var){
        if($this->SubObjects[$var]['status'] == "edited"){
          $this->SubObjects[$var]['status']= "delete";
        }else{
          unset($this->SubObjects[$var]);
        }
      }
    }

    /* Save Dialog */
    if(isset($_POST['SaveSubObject'])){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)>0){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $obj = $this->dialog->save();
        if(isset($obj['remove'])){
          if($this->SubObjects[$obj['remove']['from']]['status']=="edited"){
            $this->SubObjects[$obj['remove']['from']]['status'] = "delete";
          }elseif($this->SubObjects[$obj['remove']['from']]['status']=="new"){
            unset($this->SubObjects[$obj['remove']['from']]);
          }
          $obj['status'] = "new";
          $this->SubObjects[$obj['remove']['to']] = $obj;
          unset($this->SubObjects[$obj['remove']['to']]['remove']);
        }else{
          $this->SubObjects[$obj['cn']]=$obj;
        }
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=NULL;
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
      $this->dialog=NULL;
    }

    /* Print dialog if $this->dialog is set */
    if($this->dialog){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return($display);
    }

    $smarty->assign("SubObjects",$this->getList());

      /* Magic quotes GPC, escapes every ' " \, to solve some security risks
     * If we post the escaped strings they will be escaped again
     */

    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
        $smarty->assign($attrs,htmlentities (stripslashes(utf8_decode($this->$attrs))));
      }else{
        $smarty->assign($attrs,htmlentities (utf8_decode($this->$attrs)));
      }
    }

      foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }


    $display.= $smarty->fetch(get_template_path('faiVariable.tpl', TRUE));
    return($display);
  }

  /* Generate listbox friendly SubObject list
  */
  function getList(){
    $a_return=array();
    foreach($this->SubObjects as $obj){
      if($obj['status'] != "delete"){

        if((isset($obj['description']))&&(!empty($obj['description']))&&(!preg_match("/\[\*\]/",$obj['description']))){
          if (preg_match("/\[\*\]/", $obj['description'])){
            $a_return[$obj['cn']]= $obj['cn']." [".preg_replace("/\s*\[\*\]\s*/", "", $obj['description'])."]";
          } else {
            $a_return[$obj['cn']]= $obj['cn']."=".$obj['FAIvariableContent']." [".$obj['description']."]";
          }
        }else{
          if (preg_match("/\[\*\]/", $obj['description'])){
            $a_return[$obj['cn']]= $obj['cn'];
          } else {
            $a_return[$obj['cn']]= $obj['cn']."=".$obj['FAIvariableContent'];
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
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    if((isset($_POST['FAIvariable_posted'])) && ($this->FAIstate != "freeze") ){
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

    /* If this is a new script, check if a script with this name already exists */
    if(!empty($this->release) && ($this->copy_paste_mode || $this->cut_paste_mode) ){

      /* Check if current name is already used for fai scripts in selected release */
      $dn = 'cn='.$this->cn.",ou=variables,".$this->release;
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($dn);
      if($ldap->count()){

        $r =convert_department_dn($this->release);;
        $message[] = sprintf(_("Can't insert a fai variable named '%s' in '%s' there is already a variable with the given name."),$this->cn,$r);
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
      $ldap->cd(preg_replace('/^[^,]+,[^,]+,/', '', $this->dn));
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), _("Saving FAI variable base failed"));
 
    /* Do object tagging */
    $this->handle_object_tagging();
 
    /* Prepare FAIscriptEntry to write it to ldap
     * First sort array.
     *  Because we must delete old entries first.
     * After deletion, we perform add and modify 
     */
    $Objects = array();
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
        $obj[$codeIt]=base64_encode($obj[$codeIt]);
      }

      $tmp = array();
      foreach($this->subAttributes as $attrs){
        if(empty($obj[$attrs])){
          $obj[$attrs] = array();
        }
        $tmp[$attrs] = $obj[$attrs];
      }    
        
      $tmp['objectClass'] = $this->subClasses;

      $sub_dn = "cn=".$obj['cn'].",".$this->dn;

      if($this->copy_paste_mode){
        $obj['status'] = "new";
      }

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
        $ldap->cd($sub_dn);
        $ldap->rmdir_recursive($sub_dn);
        $this->handle_post_events("remove");
        show_ldap_error($ldap->get_error(), _("Removing FAI variable failed")); 
      }elseif($obj['status'] == "edited"){
        $ldap->cd($sub_dn);
        $this->cleanup();
        $ldap->modify ($tmp); 

        $this->handle_post_events("modify");
        show_ldap_error($ldap->get_error(), _("Saving FAI variable failed")); 
      }elseif($obj['status']=="new"){

        if($tmp['description'] == array()){
          unset($tmp['description']);
        }

        $ldap->cd(preg_replace('/^[^,]+,[^,]+,/', '', $sub_dn));
        $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $sub_dn));
        $ldap->cd($sub_dn);
        $ldap->add($tmp); 
        $this->handle_post_events("add");
        show_ldap_error($ldap->get_error(), _("Saving FAI variable failed")); 
      }

      $this->handle_object_tagging($sub_dn, $this->gosaUnitTag);
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
