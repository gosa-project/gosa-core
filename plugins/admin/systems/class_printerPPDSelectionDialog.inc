<?php

class printerPPDSelectionDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");

  var $list                 =array();
  var $header               =array();

  var $Vendor               = "";

  function printerPPDSelectionDialog ($config, $dn= NULL,$list=false,$headers=false,$ppd=false)
  {
    plugin::plugin ($config, $dn);
    $this->list       = $list;
    $this->header     = $headers;
    $this->depselect  = $this->config->current['BASE'];
    
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
  
    /** Added **/
    $list = array();
    foreach($this->list as $cat => $ppds){
      foreach($ppds as $ppd){
        if(preg_match("/^".str_replace("*",".*",$regex)."/i",$ppd['ppd'])){
          $list[$ppd['link']] = $ppd;
        }
      }
    }
  
    if((isset($_GET['act']))&&($_GET['act']=="open"))  {
      $this->Vendor = $_GET['id'];
    }

    $div = new divSelectBox("printerPPDSelectionDialog");
    $div ->SetHeight(450);
    $div ->SetSummary(_("Printer ppd selection."));
    
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>%s</a>";
    $uselink  = "<a href='?plug=".$_GET['plug']."&amp;act=use&amp;id=%s'>%s</a>";
    

    if(empty($this->Vendor)){
      foreach($this-> header as $key => $entry){
        $div ->AddEntry (array(
              array("string"=>sprintf($linkopen,$key,$key))
              ));
      }
    }else{
      $div ->AddEntry (array(
            array("string"=>sprintf($linkopen,"",".. ["._("back")."]"))
            ));
      foreach($list as $key => $ppd){
        if(preg_match("/^".$this->Vendor."/",$ppd['ppd'])){
          $div ->AddEntry (array(
                array("string"=>sprintf($uselink,$key,$ppd['ppd']))
                ));
        }
      }
    }

    $smarty->assign("List",         $div -> DrawList());
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
