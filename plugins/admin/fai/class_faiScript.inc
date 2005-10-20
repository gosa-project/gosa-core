<?php

class faiScript extends plugin
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
  var $objectclasses    = array("top","FAIclass","FAIscript");

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  /* new dn*/
  var $use_dn ="";

  function faiScript ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new sub object */
    if(isset($_POST['AddSubObject'])){
      $this->dialog= new faiScriptEntry($this->config,"new");
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }

    /* Edit selected Sub Object */
    if((isset($_POST['EditSubObject']))&&(isset($_POST['SubObject']))){
      $this->dialog= new faiScriptEntry($this->config,$this->SubObjects[$_POST['SubObject']]['dn']);
      $this->dialog->parent = &$this;
      $this->is_dialog=true;
    }
    
    /* Remove Sub object */
    if((isset($_POST['DelSubObject']))&&(isset($_POST['SubObject']))){
      $tmp = new faiScriptEntry($this->config,$this->SubObjects[$_POST['SubObject']]['dn']);
      $tmp->remove_from_parent();
      unset($tmp);
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
        $this->dialog->save_object();
        $this->dialog->save();
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=NULL;
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

    $buffer = $this->getList();
    $smarty->assign("SubObjects",$buffer);
    $smarty->assign("SubObjectKeys",array_flip($buffer));
     
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

    $display.= $smarty->fetch(get_template_path('faiScript.tpl', TRUE));
    return($display);
  }

  /* Generate listbox friendly SubObject list
  */
  function getList(){
    $a_return         = array();
    $this->SubObjects = array();

    $ldap     = $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    $ldap->cd ($this->dn);
  
    $ldap->search("(objectClass=FAIscriptEntry)",array("cn"));
    
    while($entry = $ldap->fetch()){
      $tmp = array();
      $tmp['cn'] = $entry['cn'][0]; 
      $tmp['dn']=$entry['dn']; 
      $this->SubObjects[$tmp['cn']] = $tmp;

      if(isset($entry['description'][0])){
        $a_return[$tmp['cn']] = $tmp['cn']." [".$entry['description'][0]."]";
      }else{
        $a_return[$tmp['cn']] = $tmp['cn'];
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
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    if(isset($_POST['FAIscript_submit'])){
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

    if($this->dn == "new"){
      $this->dn = $this->use_dn;
    }
 
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
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
