<?php

class servRepositorySetup  extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage FAI repositories";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("Release","ParentServer","Url");
  var $objectclasses    = array("whatever");

  /* Attributes */
  var $Release          ="";
  var $ParentServer     ="";
  var $Url              ="";   
  var $Sections         =array();
  var $ParentServers    ="";
  var $initialy_was=false;

  function servRepositorySetup ($config, $dn= NULL,$data = false)
  {
    plugin::plugin ($config, $dn);
    if($data != false){
      foreach(array("Sections","Release","Url","ParentServer","initialy_was") as $atr){
        if(isset($data[$atr])){
          $this->$atr = $data[$atr];
        }
      }
    }
  }

  function GetName()
  {
    return($this->Release);
  }
  
  function is_new_name()
  {
    if(!$this->initialy_was){
      return(true);
    }else{
      if($this->Release != $this->initialy_was){
        return(true);
      }
    }
    return(false);
  }

  

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();

    if((isset($_POST['AddSection']))&&(isset($_POST['SectionName']))&&(!empty($_POST['SectionName']))){
      $this->Sections[$_POST['SectionName']]=$_POST['SectionName'];
    }
    
    foreach($_POST as $name => $value){
      if(preg_match("/^delete_/",$name)){
        if(isset($this->Sections[$value])){
          unset($this->Sections[$value]);
        }
      }
    }

    $divlist = new divSelectBox("servRepositorySetup");
    $divlist->setHeight("220");

    $dellink = "<input type='image' src='images/edittrash.png' title='delete' alt='delete' name='delete_%s' value='%s'>";

    foreach($this->Sections as $sec){
      $divlist->AddEntry(array(
                              array("string"=>$sec),
                              array("string"=>sprintf($dellink,$sec,$sec),"attach"=>"style='border-right:0px;width:20px;'")
                              ));
    }
  
    $smarty->assign("Sections",$divlist->DrawList());

    foreach($this->attributes as $attr){
      $smarty->assign($attr       ,$this->$attr);
      $smarty->assign($attr."ACL" ,chkacl($this->acl,$attr));
    }

    $smarty->assign("ParentServers"   ,$this->getParentServers());
    $smarty->assign("ParentServerKeys",array_flip($this->getParentServers()));

    return($smarty->fetch(get_template_path('servRepositorySetup.tpl', TRUE)));
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    if(empty($this->Release)){
      $message[]=_("Please enter a value for 'release'.");
    }

    if(empty($this->Url)){
      $message[] = _("Please specify a valid value for 'url'.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    $tmp = array();
    $tmp['ParentServer']  = $this->ParentServer;
    $tmp['Url']           = $this->Url;
    $tmp['Release']       = $this->Release;
    $tmp['Sections']      = $this->Sections;
    return($tmp);
  }

  function getParentServers()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=FAIrepositoryServer)",array("*"));
    while($attr = $ldap->fetch()){
      $ret[$attr['cn'][0]]= $attr['cn'][0];   
    }
    $ret['none']= "&nbsp;";
    return($ret);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
