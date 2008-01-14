<?php

class servRepositorySetup  extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage FAI repositories";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("Release","ParentServer","Url","cn");
  var $objectclasses    = array("whatever");

  /* Attributes */
  var $Release          = "";
  var $ParentServer     = "";
  var $Url              = "";   
  var $Sections         = array();
  var $ParentServers    = "";
  var $initialy_was     = false;
  var $cn               = "";
  var $parent           = "";

  function servRepositorySetup (&$config, $dn= NULL,$data = false)
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

      /* Replace multiple spaces with a single, and cut of white spaces (trim)*/
      $val = preg_replace("/\ \ * /" , " ", trim($_POST['SectionName']));

      /* check if there are more than one entry given ( "section1 section2 )*/
      if(preg_match("/ /",$val)){

        /* Generate list of new section names */
        $vals = split(" ",$val);

        /* Add new entries */
        foreach($vals as $entry){
          $entry = trim($entry);
          $this->Sections[$entry]=$entry;
        }
      }else{
        $this->Sections[$val]=$val;
      }
    }
    
    foreach($_POST as $name => $value){
      if(preg_match("/^delete_/",$name)){

        $val = preg_replace("/^delete_/","",$name);
        $val = base64_decode(preg_replace("/_.*$/","",$val));

        if(isset($this->Sections[$val])){
          unset($this->Sections[$val]);
        }
      }
    }

    $divlist = new divSelectBox("servRepositorySetup");
    $divlist->setHeight("220");

    $dellink = "<input type='image' src='images/edittrash.png' title='delete' alt='delete' name='delete_%s' value='%s'>";

    foreach($this->Sections as $sec){
      $divlist->AddEntry(array(
                              array("string"=>$sec),
                              array("string"=>sprintf($dellink,base64_encode($sec),$sec),"attach"=>"style='border-right:0px;width:20px;'")
                              ));
    }
  
    $smarty->assign("Sections",$divlist->DrawList());

    /* Get && assign acls */
    $tmp = $this->parent->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->parent->getacl($name));
    }

    /* Assign values */
    foreach($this->attributes as $attr){
      $smarty->assign($attr       ,$this->$attr);
    }
  
    $tmp = $this->getParentServers();
    $smarty->assign("ParentServers"   ,$tmp);
    $smarty->assign("ParentServerKeys",array_flip($tmp));

    return($smarty->fetch(get_template_path('servRepositorySetup.tpl', TRUE,dirname(__FILE__))));
  }

  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['servRepositorySetup_Posted'])) {
  
      foreach($this->attributes as $attr){
        if(($this->parent->acl_is_writeable($attr)) && (isset($_POST[$attr]))){
          $this->$attr = $_POST[$attr];      
        }
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

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
      if($attr['cn'][0] == $this->cn) continue;
      $ret[$attr['cn'][0]]= $attr['cn'][0];   
    }

    $ret = array_merge($ret,$this->GetHookElements());
    
    $ret['none']= "&nbsp;";
    asort($ret);
    return($ret);
  }

  /* this funtions calls a defined hook 
      and parses all additional serverdata 
   */
  function GetHookElements()
  {
    $ret = array();
    $cmd = $this->config->search("servrepository", "REPOSITORY_HOOK",array('tabs'));
    if(!empty($cmd)){
      $res = shell_exec($cmd);
      $res2 = trim($res);
      if(!$res){
        print_red(sprintf(_("Can't execute specified REPOSITORY_HOOK '%s'. Please check your gosa.conf."),$cmd));
      }elseif(empty($res2)){
        print_red(sprintf(_("The specified REPOSITORY_HOOK '%s', specified in your gosa.conf, returns an empty string."),$cmd));
      }else{  
        $tmp = split("\n",$res);
        foreach($tmp as $hook){
          /* skip empty */
          if(empty($hook)) continue;

          if(preg_match("/;/",$hook)){ 
            $hookinfo = split(";",$hook);
            $ret[$hookinfo[0]] = $hookinfo[0];
          }else{
            $ret[$hook] = $hook;
          }
        }
      }
    }
    return($ret);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
