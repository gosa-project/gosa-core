<?php

class faiTemplate extends plugin
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
  var $objectclasses    = array("top","FAIclass","FAItemplate");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAItemplateEntry";
  var $subClasses       = array("top","FAIclass","FAItemplateEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "faiTemplateEntry";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description","FAItemplateFile","FAItemplatePath","FAIowner","FAImode"); 
  var $sub64coded       = array();

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  function faiTemplate ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */
    if($dn != "new"){
      $this->dn =$dn;

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

        $this->SubObjects[$object['cn'][0]]['FAItemplateFile'] =addslashes( $this->readBinary("FAItemplateFile",$object['dn']));
       
        $this->SubObjects[$object['cn'][0]]['status']      = "edited";
        $this->SubObjects[$object['cn'][0]]['dn']          = $object['dn'];
      }
      ksort($this->SubObjects);
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
      $this->dialog= new $this->subClassName($this->config,"new");
      $this->is_dialog=true;
    }

    $_SESSION['objectinfo'] = $this->dn;
    /* Edit selected Sub Object */
    if((isset($_POST['EditSubObject']))&&(isset($_POST['SubObject']))){
      $this->dialog= new $this->subClassName($this->config,$this->dn,$this->SubObjects[$_POST['SubObject']]);
      $_SESSION['objectinfo'] = $this->SubObjects[$_POST['SubObject']]['dn'];
      $this->is_dialog=true;
    }
    
    /* Remove Sub object */
    if((isset($_POST['DelSubObject']))&&(isset($_POST['SubObject']))){
      if($this->SubObjects[$_POST['SubObject']]['status'] == "edited"){
        $this->SubObjects[$_POST['SubObject']]['status']= "delete";
      }else{
        unset($this->SubObjects[$_POST['SubObject']]);
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
        ksort($this->SubObjects);
      }
    }

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
    $smarty->assign("SubObjectKeys",array_flip($this->getList()));

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


    $display.= $smarty->fetch(get_template_path('faiTemplate.tpl', TRUE));
    return($display);
  }

  /* Generate listbox friendly SubObject list
  */
  function getList(){
    $a_return=array();
    foreach($this->SubObjects as $obj){
      if($obj['status'] != "delete"){
        if((isset($obj['description']))&&(!empty($obj['description']))){
          $a_return[$obj['cn']]= $obj['cn']." [".$obj['description']."]";
        }else{
          $a_return[$obj['cn']]= $obj['cn'];
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
    if(isset($_POST['FAItemplate_posted'])){
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
    $message= array();
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
 
    $ldap = $this->config->get_ldap_link();
  
    $ldap->cat($this->dn);
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
 
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
        $ldap->cat($sub_dn);
        if($ldap->count()){
          $obj['status']="modify";
        }
      }

      if($obj['status'] == "delete"){
        $ldap->cd($sub_dn);
        $ldap->rmdir_recursive($sub_dn);
        $this->handle_post_events("remove");
      }elseif($obj['status'] == "edited"){
        $ldap->cd($sub_dn);
        $ldap->modify($tmp);
        $this->handle_post_events("modify");
      }elseif($obj['status']=="new"){

        if($tmp['description']==array()){
          unset($tmp['description']);
        }
        $ldap->cd($this->config->current['BASE']);
        $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $sub_dn));
        $ldap->cd($sub_dn);
        $ldap->add($tmp); 
        $this->handle_post_events("add");
      }
      show_ldap_error($ldap->get_error()); 
    }
  }

  function readBinary($attr,$dn){
    $Data  ="";
    $ds= ldap_connect($this->config->current['SERVER']);
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    if (function_exists("ldap_set_rebind_proc") && isset($this->config->current['RECURSIVE']) && $this->config->current['RECURSIVE'] == "true") {
      ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
      ldap_set_rebind_proc($ds, array(&$this, "rebind"));
    }

    if(isset($this->config->current['TLS']) &&  $this->config->current['TLS'] == "true"){
      ldap_start_tls($ds);
    }

    $r  = ldap_bind($ds);
    $sr = @ldap_read($ds, $dn, $attr."=*", array($attr));

    if ($sr) {
      $ei=ldap_first_entry($ds, $sr);
      if ($ei) {
        if ($info = ldap_get_values_len($ds, $ei, $attr)){
          $Data= $info[0];
        }
      }
    }

    /* close conncetion */
    ldap_unbind($ds);
    return($Data);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
