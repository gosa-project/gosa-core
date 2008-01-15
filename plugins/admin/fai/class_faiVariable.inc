<?php

class faiVariable extends plugin
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
  var $SubObjects       = array();  // All leafobjects of this object

  var $FAIstate         = "";
  var $ui   ;
  var $view_logged      = FALSE;

  function faiVariable (&$config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

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
      $ldap->search("(&(objectClass=FAIclass)(objectClass=".$this->subClass."))",$attrs_to_search);

      while($object = $ldap->fetch()){
      
        /* Skip objects, that are tagged as removed */
        if(isset($object['FAIstate'][0])){
          if(preg_match("/removed$/",$object['FAIstate'][0])){
            continue;
          }
        }

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
    $this->ui = get_userinfo();
  }


  function acl_base_for_current_object($dn)
  {
    if($dn == "new"){
      if($this->dn == "new"){
        $dn = session::get('CurrentMainBase');
      }else{
        $dn = $this->dn;
      }
    }
    return($dn);
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

    /* Add new sub object */
    if(isset($_POST['AddSubObject'])){
      $this->dialog= new $this->subClassName($this->config,"new");
      $this->dialog->set_acl_base($this->acl_base_for_current_object($this->dn));
      $this->dialog->set_acl_category("fai");
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }

    if($this->dn != "new"){
      session::set('objectinfo',$this->dn);
    }


    /* Edit selected Sub Object */
    if((isset($_POST['EditSubObject']))&&(isset($_POST['SubObject']))){

      $var = $_POST['SubObject'][0];
      $c_dn = $this->acl_base_for_current_object($this->SubObjects[$var]['dn']);
      $this->dialog= new $this->subClassName($this->config,$this->dn,$this->SubObjects[$var]);
      $this->dialog->set_acl_category("fai");
      $this->dialog->set_acl_base($c_dn);
      $this->dialog->parent = &$this;
      session::set('objectinfo',$this->SubObjects[$var]['dn']);
      $this->is_dialog=true;
    }
    
    /* Remove Sub object */
    if((isset($_POST['DelSubObject']))&&(isset($_POST['SubObject']))){
      foreach($_POST['SubObject'] as $var){

        $c_dn = $this->acl_base_for_current_object($this->SubObjects[$var]['dn']);
        $acl = $this->ui->get_permissions($c_dn,"fai/faiVariable");
        if(preg_match("/d/",$acl)){
          if($this->SubObjects[$var]['status'] == "edited"){
            $this->SubObjects[$var]['status']= "delete";
          }else{
            unset($this->SubObjects[$var]);
          }
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

    $ui = get_userinfo();
    $ret = $this->getList();
    $tmp = array();
    foreach($this->SubObjects as $key => $obj){
      $acl = $ui->get_permissions($obj['dn'],"fai/faiVariableEntry");
      if(preg_match("/r/",$acl) || $obj['dn'] == "new"){
        $tmp[$key] = $ret[$key];
      } 
    }
    $smarty->assign("SubObjects",$tmp);


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

    $c_dn = $this->acl_base_for_current_object($this->dn);
    $smarty->assign("is_createable",     preg_match("/c/",$this->ui->get_permissions($c_dn,"fai/faiVariableEntry")) && $this->FAIstate!="freeze");
    $smarty->assign("is_removeable",  preg_match("/d/",$this->ui->get_permissions($c_dn,"fai/faiVariableEntry")) && $this->FAIstate!="freeze");

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation) {
      $smarty->assign($name."ACL",$this->getacl($name));
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
    if($this->acl_is_removeable()){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd ($this->dn);

      $faifilter = session::get('faifilter');
      $use_dn = preg_replace("/".normalizePreg(FAI::get_release_dn($this->dn))."/i", $faifilter['branch'], $this->dn);
      if($faifilter['branch'] == "main"){
        $use_dn = $this->dn;
      }
  
      FAI::prepare_to_save_FAI_object($use_dn,array(),true);
      new log("remove","fai/".get_class($this),$use_dn,$this->attributes);

      foreach($this->SubObjects as $name => $obj){
        $use_dn = preg_replace("/".normalizePreg(FAI::get_release_dn($this->dn))."/i", $faifilter['branch'], $obj['dn']);
        if($faifilter['branch'] == "main"){
          $use_dn = $obj['dn'];
        }
        FAI::prepare_to_save_FAI_object($use_dn,array(),true);
      }
      $this->handle_post_events("remove");
    }
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

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
 
    $ldap = $this->config->get_ldap_link();
    FAI::prepare_to_save_FAI_object($this->dn,$this->attrs);
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of FAI/variable with dn '%s' failed."),$this->dn));

    if($this->initially_was_account){
      new log("modify","fai/".get_class($this),$this->dn,$this->attributes);
    }else{
      new log("create","fai/".get_class($this),$this->dn,$this->attributes);
    }
 
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
        FAI::prepare_to_save_FAI_object($sub_dn,array(),true);
        $this->handle_post_events("remove");
      }elseif($obj['status'] == "edited"){
        FAI::prepare_to_save_FAI_object($sub_dn,$tmp);
        $this->handle_post_events("modify");
      }elseif($obj['status']=="new"){
        FAI::prepare_to_save_FAI_object($sub_dn,$tmp);
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

 
  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName" => _("Variable"),
          "plDescription" => _("FAI variable"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 22,
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
