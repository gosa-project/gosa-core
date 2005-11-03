<?php

class selectUserToPrinterDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $regex                = "*";
  var $depselect            = "/";
  var $deplist              = "/";

  var $searchObjects        = "";
  var $searchAttrs          = "";
  var $searchAppend         = "";
  var $type                 = "";
  
  function selectUserToPrinterDialog ($config, $dn= NULL,$type=false )
  {
    plugin::plugin ($config, $dn);
    $this->depselect = $this->config->current['BASE'];

    switch($type){
      case "AddUser" : 
        $this->searchObjects = "(objectClass=person)(objectClass=inetOrgPerson)";
        $this->searchAttrs   = array("cn","uid");
        $this->searchAppend  = "uid"; 
          ;break;
      case "AddGroup" :
        $this->searchObjects = "(objectClass=posixGroup)";
        $this->searchAttrs   = array("cn","description");
        $this->searchAppend  = "cn"; 
          ;break;
      case "AddAdminUser" : 
        $this->searchObjects = "(objectClass=person)(objectClass=inetOrgPerson)";
        $this->searchAttrs   = array("cn","uid");
        $this->searchAppend  = "uid"; 
          ;break;
      case "AddAdminGroup" :
        $this->searchObjects = "(objectClass=posixGroup)";
        $this->searchAttrs   = array("cn","description");
        $this->searchAppend  = "cn"; 
          ;break;
    }
    $this->type = $type;

  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    if(isset($_POST['dialogissubmitted'])){
      $this->regex=$_POST['regexPrinter'];
      $this->depselect = $_POST['depselectPrinter'];
    }

    if((isset($_GET['search']))&&(!empty($_GET['search']))){
      $this->regex=$_GET['search']."*";
      $this->regex=preg_replace("/\*\*/","*",$this->regex);
    }

    $this->deplist=array_flip($this->config->departments);

    $smarty->assign("regexPrinter"        , $this->regex);
    $smarty->assign("deplistPrinter"      , $this->deplist);
    $smarty->assign("depselectPrinter"    , $this->depselect);
    $smarty->assign("gotoPrinters"        , $this->getPrinter());
    $smarty->assign("gotoPrinterKeys"     , array_flip($this->getPrinter()));
    $smarty->assign("apply"               , apply_filter());
    $smarty->assign("alphabet"            , generate_alphabet());
    $smarty->assign("search_image"        , get_template_path('images/search.png'));
    $smarty->assign("tree_image"          , get_template_path('images/tree.png'));
    $smarty->assign("infoimage"           , get_template_path('images/info.png'));
    $smarty->assign("launchimage"         , get_template_path('images/launch.png'));
    $smarty->assign("deplist"             , $this->config->idepartments);

    $display.= $smarty->fetch(get_template_path('selectUserToPrinterDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function check(){
    $message=array();
    if(empty($_POST['gotoPrinter'])){
      $message[] = _("Please select a printer or press cancel.");
    }
    return $message;
  }

  /* Save to LDAP */
  function save()
  {
    $a_return['type']=$this->type;
    
    foreach($_POST['gotoPrinter'] as $name){
      $data = $this->getPrinter(true);
      $a_return[$name]= $data[$name];
    }
    return($a_return);
  }

  /* This function generates the Printerlist
   * All printers are returned that match regex and and depselect
   */
  function getPrinter($detailed = false)
  {
    $a_return=array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->depselect);
    $ldap->search("(&".$this->searchObjects."(cn=".$this->regex."))",$this->searchAttrs);
    while($printer = $ldap->fetch()){
      if(($detailed ==true)){
        if(isset($printer[$this->searchAppend])){
          $a_return[$printer[$this->searchAppend][0]] = $printer;
        }
      }else{
        if(isset($printer[$this->searchAppend])){
          if(isset($printer['description'][0])){
            $a_return[$printer[$this->searchAppend][0]] = $printer['cn'][0]." - ".$printer['description'][0]; 
          }else{
            $a_return[$printer[$this->searchAppend][0]] = $printer['cn'][0];
          }
        }
      }
    }
    return($a_return);
  }
}





// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
