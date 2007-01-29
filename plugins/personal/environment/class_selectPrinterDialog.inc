<?php

class selectPrinterDialog extends plugin
{
  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $AlreadyAssigned      = array();  
  var $regex                = "*";
  var $depselect            = "/";
  var $deplist              = "/";

  function selectPrinterDialog ($config, $dn= NULL,$alreadyused=array() )
  {
    $this->AlreadyAssigned = $alreadyused;
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

    $this->deplist=$this->config->idepartments;

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
    /* Call common method to give check the hook */
    $message= plugin::check();

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

    /* Set tag attribute if we've tagging activated */
    $tag= "";
    $ui= get_userinfo();
    if ($ui->gosaUnitTag != "" && isset($this->config->current['STRICT_UNITS']) &&
        preg_match('/TRUE/i', $this->config->current['STRICT_UNITS'])){
      $tag= "(gosaUnitTag=".$ui->gosaUnitTag.")";
    }

    $ldap->search("(&(objectClass=gotoPrinter)$tag(cn=".$this->regex."))",array("*"));
    while($printer = $ldap->fetch()){

      if(isset($this->AlreadyAssigned[$printer['cn'][0]])) continue;

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
