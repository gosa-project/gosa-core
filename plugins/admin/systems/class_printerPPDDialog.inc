<?php

class printerPPDDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");

  var $ppdList              = array();
  var $ppdListHeader        = array();

  var $dialog               = NULL;   
  var $selectedPPD          = false;

  var $ppdManager     = false;
  function printerPPDDialog ($config, $dn= NULL,$ppdfile=NULL )
  {
    plugin::plugin ($config, $dn);
    $this->depselect = $this->config->current['BASE'];

    /* Load all available PPD files and sort them into an array 
     */
    require_once ("class_ppdManager.inc");
    $this->ppdManager= new ppdManager('/var/spool/ppd/');
    $tmp = $this->ppdManager->getPrinterList();

    /* Sort all available files, and create header (Vendor index) */
    foreach($tmp as $file=>$ppd){
      $tmp2 = split("\n",$ppd);
      if(!isset($this->ppdListHeader[$tmp2[0]])){
        $this->ppdListHeader[$tmp2[0]]=0;
      }
      $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]);
      $tmp3['link']   =$file;
      $tmp3['ppd']    =$ppd;
  
      $this->ppdListHeader[$tmp2[0]]++;
      
      $this->ppdList[$tmp2[0]][preg_replace("/^ -/","",$tmp2[1])]=$tmp3;
    }

   /* The user has already a valid PPD assigned
    * Get some informations about this PPD
    */
   if(($ppdfile!= NULL)&&(strlen($ppdfile)>0)){
      $tmp2= split("\n", $this->ppdManager->loadDescription($ppdfile));
      $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]);
      $tmp3['link']   =$ppdfile;
      $tmp3['ppd']    =$this->ppdManager->loadDescription($ppdfile);
      $this->selectedPPD = $tmp3;
   }

 
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";


    /* Open a dialog that allow us to select different PPDs
     */
    if(isset($_POST['SelectPPD'])){
      $this->dialog= new printerPPDSelectionDialog($this->config,$this->dn,$this->ppdList,$this->ppdListHeader,$this->selectedPPD);
    }

    /* The selection dialog fpr PPDs is canceled
     */
    if(isset($_POST['ClosePPDSelection'])){
      unset($this->dialog);
      $this->dialog=NULL;
    }
  
    /* A new PPDs was selected in the PPDs selection Dialog
     * Perform a Check. If everything is fine, use the new PPD.
     */
    if(isset($_POST['SavePPDSelection'])){
      if(count($this->dialog->check())>0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $this->selectedPPD = $this->dialog->save();
        unset($this->dialog);
        $this->dialog=NULL;
      }
    }
  
    /* if a dialog is open, print the dialog instead of this class
     */
    if($this->dialog!=NULL){
      $display = $this->dialog->execute();
      return($display);
    }

    /* Give smarty the information it needs */
    $smarty->assign("ppdString" ,$this->getPPDInformation());
    $smarty->assign("properties",$this->generateProperties());
  
    /* Print out template */
    $display.= $smarty->fetch(get_template_path('printerPPDDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function check(){
    /* Check the given data
     */
    $message=array();
    return $message;
  }

  /* Save to LDAP */
  function save()
  {
    /* return the selected PPD, and in future the selected options too */
    return($this->selectedPPD['link']);
  }

  function getPPDInformation()
  {
    /* Get Information for a single PPD entry 
     * This will be shown on top of template
     */
    $str = "none";
    if(!empty($this->selectedPPD)){
      $str = $this->selectedPPD['link'];
      $str = $this->ppdManager->loadDescription($this->selectedPPD['link']);
    }
    return($str) ; 
  }

  function generateProperties()
  { 
    /* In future there will be a schema parser that provide us all settings that can be made in the selected PPD file. 
     * This function will generate a userfriendly post based form with this informations
     */
    return("<table><tr><td>adfasdf</td></tr></table/table>");
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
