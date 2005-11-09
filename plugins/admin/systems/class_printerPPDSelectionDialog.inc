<?php

class printerPPDSelectionDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $attributes           = array("currentPos","currentSel");
  var $objectclasses        = array("whatever");

  var $list                 =array();
  var $header               =array();

  var $currentPos           = "";
  var $currentSel           = -1;
  var $selectedPPD          = "";

  function printerPPDSelectionDialog ($config, $dn= NULL,$list=false,$headers=false,$ppd=false)
  {
    plugin::plugin ($config, $dn);
    $this->list       = $list;
    $this->header     = $headers;
    $this->depselect  = $this->config->current['BASE'];
    
    /* If there is already a ppd file selected, use this as preselected object in our ppds list
     * /
    if(isset($ppd)){
      print_a( $ppd);
      $tmp2 = split("\n",$ppd['ppd']);
      if(count($tmp2)==2){
        $this->currentPos = $tmp2[0];
        $this->currentSel = preg_replace("/^ -/","",$tmp2[1]);
      }
    }
  
     */
    if(!isset($_SESSION['printerPPDSelectionDialog'])){
      $_SESSION['printerPPDSelectionDialog']['regex'] = "*";
    }  

    /* Order the manufacturers index */
    ksort($this->header);
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
    $s_action = "none";
  
    $regex = $_SESSION['printerPPDSelectionDialog']['regex'];

    if(isset($_GET['search'])){
      $regex = $_GET['search']."*";
    }
  
    if(isset($_POST['regex'])){
      $regex= $_POST['regex'];
    }
    
    $regex = str_replace("**","*",$regex);
    $_SESSION['printerPPDSelectionDialog']['regex'] = $regex;
  

    /* if manufacturer is selected and a ppd file 
     * Set this selected file to our return value.
     */ 
    if(isset($this->list[$this->currentPos][$this->currentSel])){
      $this->selectedPPD = $this->list[$this->currentPos][$this->currentSel];
    }

    /** Added **/
    $list = array();
    foreach($this->list as $cat => $ppds){
      foreach($ppds as $ppd){
        if(preg_match("/^".str_replace("*",".*",$regex)."/i",$ppd['ppd'])){
          $list[$ppd['link']] = $ppd['ppd'];
        }
      }
    }

    $smarty->assign("objects",      $list);
    $smarty->assign("objectKeys",   array_flip($list));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("launchimage",  get_template_path('images/small_filter.png'));
    $smarty->assign("tree_image",   get_template_path('images/tree.png'));
    $smarty->assign("alphabet",     generate_alphabet());
    $smarty->assign("apply",        apply_filter());
    $smarty->assign("regex",        $regex);
  
    $display.= $smarty->fetch(get_template_path('printerPPDSelectionDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function save_object()
  {
  } 

  function check(){
  }

  /* Save to LDAP */
  function save()
  {
    return $this->selectedPPD;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
