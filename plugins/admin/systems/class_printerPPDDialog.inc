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

  var $ppdList              = array();  // Contains all Printer models
  var $ppdListHeader        = array();  // Contains all printer vendors

  var $dialog               = NULL;     
  var $selectedPPD          = false;    // e.g. /var/spool/ppd/vendor/device.ppd

  var $ppdManager           = false;    // new ppdManager;
  var $ppdConfig            = false;    // $this->ppdManager->loadProperties($this->selectedPPD['link']);

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
      $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]." - ".$tmp2[2]);
      $tmp3['link']   =$file;
      $tmp3['ppd']    =$ppd;
      $this->ppdListHeader[$tmp2[0]]++;
      $this->ppdList[$tmp2[0]][preg_replace("/^ -/","",$tmp2[1]." - ".$tmp2[2])]=$tmp3;
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
      if(!isset($_POST['PPDselection'])){
        print_red(_("Please select a valid PPD."));
      }else{
        $this->selectedPPD['link'] = $_POST['PPDselection'];
        $this->ppdConfig   = false;
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

  function save_object()
  {
    if(isset($_POST['PPDDisSubmitted'])){
      if(is_array($this->ppdConfig)){
        foreach($this->ppdConfig as $cat => $obj){
          foreach($obj as $attr => $attributes){
            if(isset($_POST[base64_encode($attributes['_name'])])){
              $this->ppdConfig[$cat][$attr]['_default'] = $_POST[base64_encode($attributes['_name'])];
            }
          }
        }
      }
    }

  }

  /* Save to LDAP */
  function save()
  {
    /* return the selected PPD, and in future the selected options too */
    $this->ppdManager->saveProperties($this->selectedPPD['link'],$this->ppdConfig);
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
  
    /* Set Headline */
    $str = "";

    /* If ppd exists and is readable */
    if((!empty($this->selectedPPD['link']))&&(file_exists($this->selectedPPD['link']))){
    
      /* If there is no initial Configuration, load it */
      if($this->ppdConfig == false){
        $this->ppdConfig = $this->ppdManager->loadProperties($this->selectedPPD['link']);
      }

      /* Create a table */
      $str .= "<div style='padding-left:30px;'><table>";

      /* Input all data to the table */
      foreach($this->ppdConfig as $cat => $obj){
    
        /* Add new category */
        $str .= "<tr><td colspan='2'><br>";
        $str .= "<b>".$cat."&nbsp;:&nbsp;</b><br>";
        $str .= "</tr></td>";       

        /* Add attributes of the current category */
        foreach($obj as $attr => $settings){
        
          /* Skip all entries beginning with _ */
          if($attr[0] == "_") continue;  
          
          /* Prepare data */
          $values   = array();
          $name     = $settings['_name'];
          $default  = $settings['_default'];
          $type     = $settings['_type'];
 
          /* Add name to table */ 
          $str .= "<tr><td style='padding-left:40px;'>\n";
          $str .= $name."<br>\n";
          $str .= "</td><td>\n";
   
          /* Get all values */ 
          foreach( $settings as $vname => $value){
            if($vname[0] != "_"){
              $values[$vname]= $vname;
            }
          }

          /* preparing Html output
           * Supported types are PickOne/Boolean
           */

          /* If type is PickOne, create a select box */
          if(($type == "PickOne")||(($type=="Boolean")&&(count($values)>1))){

            $str  .=  "<select name='".base64_encode($name)."'>\n";
            foreach($values as $value){
              $selected = "";
              if($value == $default){
                $selected = " selected ";
              }
              $str  .=  "<option value='".$value."' ".$selected.">".$value."</option>\n";
            }
            $str  .=  "</select>\n";

          }elseif($type == "Boolean"){

            /* If type is Boolean & no values are given */
            $str  .=  "<select name='".base64_encode($name)."'>\n";
            if($default == "False"){
              $str  .=    "<option value='True' >"._("True")."</option>\n";
              $str  .=    "<option value='False' selected>"._("False")."</option>\n";
            }else{
              $str  .=    "<option value='True' selected>"._("True")."</option>\n";
              $str  .=    "<option value='False' >"._("False")."</option>\n";
            }          
            $str  .=  "</select>\n";

          }else{
            print_red(sprintf(_("Unsupported PPD type '%s' used for '%s' "),$type,$name));
          }
          $str .= "</td></tr>\n";
        }
      }
      $str .= "</table></div>\n";

    }
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
