<?php

class faiHookEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("Object_cn","Object_description","Object_FAIscript","Object_FAItask");
  var $objectclasses= array();

  var $orig_cn              = "";

  var $Object_dn            = "";
  var $Object_cn            = "";
  var $Object_FAItask       = "";
  var $Object_FAIscript     = "";
  var $Object_description   = "";
  var $Object_status        = "new";
  
  function faiHookEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = "Object_".$name;
        $this->$oname=$value;
      }
    }else{
      $this->Object_status = "new";
      $this->orig_cn       = false;
    }

    if(isset($this->Object_FAIscript)){
      $ds= ldap_connect($this->config->current['SERVER']);
      ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
      if(function_exists("ldap_set_rebind_proc") && isset($this->config->current['RECURSIVE']) && $this->config->current['RECURSIVE'] == "true") {
        ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
        ldap_set_rebind_proc($ds, array(&$this, "rebind"));
      }

      if(isset($this->config->current['TLS']) && $this->config->current['TLS'] == "true"){
        ldap_start_tls($ds);
      }

      $r = ldap_bind($ds);
      $sr= @ldap_read($ds, $this->dn, "FAIscript=*", array("FAIscript"));
      if ($sr) {
        $ei=ldap_first_entry($ds, $sr);
        if ($ei) {
          if ($info = ldap_get_values_len($ds, $ei, "FAIscript")){
            $this->Object_FAIscript = base64_decode($info[0]);
          }
        }
      }

      /* close conncetion */
      ldap_unbind($ds);
    }

  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";
        
    if(isset($_POST['ImportUpload'])){
      if(($_FILES['ImportFile']['error']!=0)){
        print_red(_("Please select a valid file."));
      }else
      if(($_FILES['ImportFile']['size']==0)){
        print_red(_("Selected file is empty."));
      }else{
        $str = utf8_encode(file_get_contents($_FILES['ImportFile']['tmp_name']));
        $this->Object_FAIscript = $str;
      }
    }

    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,stripslashes($this->$attrs));
    }

    for($i =1 ; $i <= 100 ; $i++){
      $Object_FAIprioritys[$i]=$i;
    }
    $smarty->assign("Object_FAIprioritys",$Object_FAIprioritys);
    $display.= $smarty->fetch(get_template_path('faiHookEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['SubObjectFormSubmitted'])){
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = "";
        }
      }
    }
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
    if(empty($this->Object_FAItask)) {
      $message[]=_("Please enter a value for task.");
    }
    if(empty($this->Object_FAIscript)) {
      $message[]=_("Please enter a value for script.");
    }

    $str = utf8_encode("üצה");

    if(preg_match("/[^a-z0-9".$str."\.,;:\-_\? ]/i",$this->Object_description)){
      $message[] = _("Invalid character in description. Please enter a valid description.");
    }

    if(empty($this->Object_cn)){
      $message[] = _("Please enter a name.");
    }

    if(preg_match("/[^0-9a-z]/i",$this->Object_cn)){
      $message[] = _("Please enter a valid name. Only a-Z 0-9 are allowed.");
    }
  
    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $attr = preg_replace("/^Object_/","",$attrs);
      $tmp[$attr] = $this->$attrs;
    }

    if(($this->orig_cn)&&($tmp['cn']!=$this->orig_cn)){
      $tmp['remove']['from']  = $this->orig_cn;
      $tmp['remove']['to']    = $tmp['cn'];
    }
  
    $tmp['dn']      = $this->dn;  
    $tmp['status']  = $this->Object_status;  
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
