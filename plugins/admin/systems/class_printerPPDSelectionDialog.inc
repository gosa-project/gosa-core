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
     */
    if(isset($ppd)){
      $tmp2 = split("\n",$ppd['ppd']);
      $this->currentPos = $tmp2[0];
      $this->currentSel = preg_replace("/^ -/","",$tmp2[1]);
    }
  
    /* Order the manufacturers index */
    ksort($this->header);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
    $s_action = "none";
  
    /* Links used to open/select an entry */ 
    $LINK_openDir     = "<a href='?plug=".$_GET['plug']."&open=%s'>%s</a>";
    $LINK_useThis     = "<a href='?plug=".$_GET['plug']."&use=%s'>%s</a>";

    /* Open manufacturer */
    if(isset($_GET['open'])){
      $this->currentPos=base64_decode($_GET['open']);
    }
 
    /* Select ppd */
    if(isset($_GET['use'])){
      $this->currentSel=base64_decode($_GET['use']);
    }

    /* if manufacturer is selected and a ppd file 
     * Set this selected file to our return value.
     */ 
    if(isset($this->list[$this->currentPos][$this->currentSel])){
      $this->selectedPPD = $this->list[$this->currentPos][$this->currentSel];
    }

    /* "Back" resets our selection
     */
    foreach($_POST as $post => $val){
      if(preg_match("/dep_back/",$post)){
        $s_action = "back";
        $this->currentPos           = "";
        $this->currentSel           = -1;
      }
    }

    /* The listhead is shown on top of the list.
     * It represents a menu that contains basic option for this dialog/list
     */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input type='image' align='middle' src='images/list_back.png' title='"._("Reset list")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input type='image' align='middle' src='images/list_new_user.png' title='"._("Upload PPD")."' alt='"._("Uppload PPD")."' name='upload'>&nbsp;".
      "</div>";

    /* Create new list*/
    $div = new divlist("PPD");
    $div -> SetSummary(_("List with all available ppd files."));
    $div -> SetEntriesPerPage(18);

    /* If nothing is selected open manufacturers list */
    if($this->currentPos==""){
      $div -> SetHeader (array(
            array("string"=>_("Name")),
            array("string"=>_("Entries"),"attach"=>"style='border-right:none;width:100px;'")));
      $options = "";

      /* Append elements */
      foreach($this->header as $header => $nums){
        $field1 = array("string" => sprintf($LINK_openDir,base64_encode($header),$header),"attach"=>"style='border-right:none;height:26px;'");
        $field2 = array("string" => $nums,"attach"=>"style='border-right:none;height:26px;'");
        $div->AddEntry(array($field1,$field2));
      }
    
    /* Display all ppds for the given manufacturer */
    }else{
      $div -> SetHeader (array(
            array("string"=>_("Name")." - ".$this->currentPos   ,"attach"=>"style='border-right:none;'")));

      /* Append elements */
      foreach($this->list[$this->currentPos] as $pos => $ppd){
        if($pos == $this->currentSel){
          $field1 = array("string" => sprintf($LINK_useThis,base64_encode($pos),$ppd['name'])." - "._("selected"),"attach"=>"style='border-right:none;background:#99CCBB;height:25px;'");
        }else{
          $field1 = array("string" => sprintf($LINK_useThis,base64_encode($pos),$ppd['name']),"attach"=>"style='border-right:none;height:25px;'");
        }
        $div->AddEntry(array($field1));
      }
    }
    
    /* print out template */
    $smarty->assign("listhead",$listhead); 
    $smarty->assign("list",$div->DrawList()); 
    $display.= $smarty->fetch(get_template_path('printerPPDSelectionDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function check(){
    $message=array();
    if(empty($this->selectedPPD)){
      $message[] = _("Please select a PPD or press cancel");
    }
    return $message;
  }

  /* Save to LDAP */
  function save()
  {
    return $this->selectedPPD;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
