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

  var $pathToPPD            = "";

  function printerPPDDialog ($config, $dn= NULL,$ppdfile=NULL )
  {
    plugin::plugin ($config, $dn);
    $this->depselect = $this->config->current['BASE'];

    if(isset($_SESSION['config']->data['MAIN']['PPD_PATH'])){
      $this->pathToPPD = $_SESSION['config']->data['MAIN']['PPD_PATH'];
      $this->pathToPPD= preg_replace("/\/\//", "/", $this->pathToPPD);
      if(!preg_match("/\/$/",$this->pathToPPD)){
        $this->pathToPPD = $this->pathToPPD."/";
      }
    }else{
      $this->pathToPPD = "";
    }

    if(!((!is_dir($this->pathToPPD))||(empty($this->pathToPPD)))){ 

      /* Load all available PPD files and sort them into an array 
       */
      require_once ("class_ppdManager.inc");
      $this->ppdManager= new ppdManager($this->pathToPPD);
      $tmp = $this->ppdManager->getPrinterList();

      /* Sort all available files, and create header (Vendor index) */
      foreach($tmp as $file=>$ppd){
        $tmp2 = split("\n",$ppd);
        if(!isset($this->ppdListHeader[$tmp2[0]])){
          $this->ppdListHeader[$tmp2[0]]=0;
        }
        $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]." - ".$tmp2[2]);
        $tmp3['link']   =preg_replace("/".str_replace("/","\/",$this->pathToPPD)."/i","",$file);
        $tmp3['ppd']    =$ppd;
        $this->ppdListHeader[$tmp2[0]]++;
        $this->ppdList[$tmp2[0]][preg_replace("/^ -/","",$tmp2[1]." - ".$tmp2[2])]=$tmp3;
      }

      /* The user has already a valid PPD assigned
       * Get some informations about this PPD
       */
      if(($ppdfile!= NULL)&&(strlen($ppdfile)>0)){
        if(!file_exists($this->pathToPPD.$ppdfile)){
          print_red(sprintf(_("Can't open '%s', ppd settings resetted."),$ppdfile));
        }else{
          $tmp2= split("\n", $this->ppdManager->loadDescription($this->pathToPPD.$ppdfile));
          $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]);
          $tmp3['link']   =$ppdfile;
          $tmp3['ppd']    =$this->ppdManager->loadDescription($this->pathToPPD.$ppdfile);
          $this->selectedPPD = $tmp3;
        }
      }
    }
  }

  function getPrinterReload()
  {
    $tmp = $this->ppdManager->getPrinterList(true);

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

  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    if((!is_dir($this->pathToPPD))||(empty($this->pathToPPD))){
      print_red(sprintf(_("The specified PPD_PATH='%s' in your gosa.conf is invalid, can't read any ppd informations."),$this->pathToPPD));
      $smarty->assign("ppdString", _("Can't get ppd informations."));
      $smarty->assign("showOptions", "");
    }else{


      /* Is there a new PPD file uploaded ? */
      if((isset($_FILES['NewPPDFile']))&&(isset($_POST['SubmitNewPPDFile']))){
        $file = ($_FILES['NewPPDFile']);
        if($file['size'] != 0 ){
          $res  = @$this->ppdManager->loadDescription($file['tmp_name']);
          $name = (trim(preg_replace("/-/","",$res)));
          if(!empty($name)){
            $vendor = trim(preg_replace("/-.*$/i","",$res));
            $model  = trim(preg_replace("/".$vendor."/","",$name));
            if(!is_dir($this->pathToPPD.$vendor)){
              if(!(@mkdir($this->pathToPPD.$vendor))){
                print_red(sprintf(_("Can't create folder '%s' for the uploaded ppd file."),$vendor));
              }
            }

            if(!isset($this->ppdList[$vendor])){
              $this->ppdList[$vendor] = array();
            }

            if(is_dir($this->pathToPPD.$vendor)){  
              $found = false;
              foreach($this->ppdList[$vendor] as $key => $val){
                if(preg_match("/".$model.".*/i",$key)){
                  $found = true;
                  print_red(sprintf(_("There is already a ppd file for this kind of printer.")));
                }
              }// Foreach
              if(!$found){
                $ppdname = $vendor."/".$vendor."-".preg_replace("/^[^ ]/","",str_replace("-","",$model)).".ppd";
                $filename = $this->pathToPPD.$vendor."/".$vendor."-".preg_replace("/^[^ ]/","",str_replace("-","",$model)).".ppd";
                $fp = @fopen($filename,"w+");
                if(!$fp){
                  print_red(sprintf(_("Can't save file '%s'."),$filename));
                }else{
                  $str = file_get_contents($file['tmp_name']);
                  fputs($fp,$str);
                  @fclose($fp);

                  $tmp2= split("\n", $res);
                  $tmp3['name']   =preg_replace("/^ -/","",$tmp2[1]);
                  $tmp3['link']   =$ppdname;
                  $tmp3['ppd']    =$res;

                  $this->selectedPPD = $tmp3;
                  $this->getPrinterReload(); 
                }
              }
            }// If dir
          }else{
            print_red(_("Please specify a valid ppd file."));
          }
        }else{
          print_red(_("Please specify a valid ppd file."));
        }

      }


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
          print_red(_("Please select a valid ppd."));
        }else{
          $this->selectedPPD['link'] = $_POST['PPDselection'];
          $this->ppdConfig   = false;
          unset($this->dialog);
          $this->dialog=NULL;
        }
      }

      /* Div Selection */ 
      if((isset($_GET['act']))&&($_GET['act']=="use")){
        $hit = false;
        foreach($this->ppdList as $vendor => $ppds ){
          foreach($ppds as $name => $ppd){
            if($ppd['link']==base64_decode($_GET['id'])){
              $hit = true;
            }
          }
        }
        if($hit == true){
          $this->selectedPPD['link'] = str_replace($this->pathToPPD,"",base64_decode($_GET['id']));
          $this->ppdConfig   = false;
        }
        unset($this->dialog);
        $this->dialog=NULL;
        
      }

      /* if a dialog is open, print the dialog instead of this class
       */
      if($this->dialog!=NULL){
        $display = $this->dialog->execute();
        return($display);
      }

      /* Give smarty the information it needs */
      $smarty->assign("ppdString" ,$this->getPPDInformation());
      $tmp= $this->generateProperties();
      if ($tmp == ""){
        $smarty->assign("showOptions", 0);
      } else {
        $smarty->assign("showOptions", 1);
        $smarty->assign("properties",$this->generateProperties());
      }
    } 
    /* Print out template */
    $display.= $smarty->fetch(get_template_path('printerPPDDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function check(){
    /* Call common method to give check the hook */
    $message= plugin::check();

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

  function save_ppd(){
    if($this->ppdManager){
      $this->ppdManager->saveProperties($this->pathToPPD.$this->selectedPPD['link'],$this->ppdConfig);
    }
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
      $str = $this->ppdManager->loadDescription($this->pathToPPD."/".$this->selectedPPD['link']);
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
    $feed= "";

    $s_ppd = $this->pathToPPD.$this->selectedPPD['link'];

    /* If ppd exists and is readable */
    if((!empty($this->selectedPPD['link']))&&(file_exists($s_ppd))){

      /* If there is no initial Configuration, load it */
      if($this->ppdConfig == false){
        $this->ppdConfig = $this->ppdManager->loadProperties($s_ppd);
      }

      /* Create a table */
      $str .= "<div style='padding-left:30px;'><table summary=''>";

      /* Input all data to the table */
      foreach($this->ppdConfig as $cat => $obj){

        /* Add new category */
        $str .= "<tr><td colspan='2'>$feed";
        if ($feed == ""){
          $feed= "<br>";
        }
        $str .= "<b>"._("Section")." '".$cat."'&nbsp;</b><br>";
        $str .= "</td></tr>";       

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
              $values[$vname]= $value;
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
            print_red(sprintf(_("Unsupported ppd type '%s' used for '%s' "),$type,$name));
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
