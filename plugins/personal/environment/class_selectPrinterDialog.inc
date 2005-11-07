<?php

class selectPrinterDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  
  var $regex                = "*";
  var $depselect            = "/";
  var $deplist              = "/";

  function selectPrinterDialog ($config, $dn= NULL,$use_existing=false )
  {
    $this->use_existing = $use_existing;
    plugin::plugin ($config, $dn);
    $this->depselect = $this->config->current['BASE'];
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

    $smarty->assign("regexPrinter"    ,$this->regex);
    $smarty->assign("deplistPrinter"  ,$this->deplist);
    $smarty->assign("depselectPrinter",$this->depselect);
    $smarty->assign("gotoPrinters",$this->getPrinter());
    $smarty->assign("gotoPrinterKeys",array_flip($this->getPrinter()));
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("deplist", $this->config->idepartments);

    $display.= $smarty->fetch(get_template_path('selectPrinterDialog.tpl', TRUE,dirname(__FILE__)));
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
    return($_POST['gotoPrinter']);
  }

  /* This function generates the Printerlist
   * All printers are returned that match regex and and depselect
   */
  function getPrinter($detailed = false)
  {
    $a_return=array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->depselect);
    $ldap->search("(&(objectClass=gotoPrinter)(cn=".$this->regex."))",array("*"));
    while($printer = $ldap->fetch()){
      if($detailed ==true){
        $a_return[$printer['cn'][0]] = $printer;
      }else{
        if(isset($printer['description'][0])){
          $a_return[$printer['cn'][0]] = $printer['cn'][0]." - ".$printer['description'][0];  
        }else{
          $a_return[$printer['cn'][0]] = $printer['cn'][0];  
        }
      }
    }
    return($a_return);
  }
}





// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
