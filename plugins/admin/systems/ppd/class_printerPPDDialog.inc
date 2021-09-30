<?php

class printerPPDDialog extends plugin
{
    /* attribute list for save action */
    var $ignore_account       = TRUE;
    var $attributes           = array("cn");
    var $objectclasses        = array("whatever");

    /* PPD Handling */
    var $selectedPPD          = false;        // e.g. /vendor/device.ppd 
    var $ppdManager           = false;        // new ppdManager;
    var $ppdConfig            = false;        // $this->ppdManager->loadProperties($this->selectedPPD['link']);
    var $ppdList              = array();      // Contains all Printer models
    var $ppdListHeader        = array();      // Contains all printer vendors

    /* Paths */
    var $pathToPPD            = "";           // Base path, defined in gosa.conf e.g. "/var/spool/ppd/"
    var $pathToModified       = "modified/";  // used to store the modified ppds  

    /* Object Info */
    var $cn                   = "" ;          // Used to tag the ppds modified by the printer object, 
    var $ui;

    /* If there is already a ppd file for the same type of printer, 
     *  remember the path to ppd file and display a dialog which allows 
     *  to overwrite the current ppd file. 
     */
    var $add_ppd_later            = "";
    var $add_later_msg_dialog     = NULL;

    function __construct(&$config, $dn= NULL, $ppdfile=NULL )
    {
        plugin::__construct ($config, $dn);
        $this->depselect = $this->config->current['BASE'];

        $this->ui = get_userinfo();

        /* Get PPD path and remove double //, and add trailing /  */
        $config = session::get('config');
        if($config->get_cfg_value("core","ppdPath") != ""){
            $this->pathToPPD = $config->get_cfg_value("core","ppdPath");
            $this->pathToPPD= preg_replace("/\/\//", "/", $this->pathToPPD);
            if(!preg_match("/\/$/",$this->pathToPPD)){
                $this->pathToPPD = $this->pathToPPD."/";
            }
        }else{
            $this->pathToPPD = "";
        }

        /* Check if ppdPath is set in gosa.conf. 
         */
        if(empty($this->pathToPPD)){
            msg_dialog::display(_("Configuration error"), msgPool::invalidConfigurationAttribute("ppdPath"), ERROR_DIALOG);
        }

        /* It seams that we have an existing PPD path, so go on */
        if(!((!is_dir($this->pathToPPD))||(empty($this->pathToPPD)))){ 

            /* Load all available PPD files and sort them into an array */
            $this->ppdManager= new ppdManager($this->pathToPPD);
            $this->getPrinterReload ();

            /* The user has already a valid PPD assigned
             * Get some informations about this PPD
             * and set it as selected. 
             * The ppdpath ['link'] should be relative from .../ppd/modified/ 
             *     e.g. "/Compaq/Compaq-J1200.ppd" */
            if(($ppdfile!== NULL)&&(strlen($ppdfile)>0)){
                $ppdfile = preg_replace("#".$this->pathToModified."#","",$ppdfile);
                if(!file_exists($this->pathToPPD.$this->pathToModified.$ppdfile)){
                    msg_dialog::display(_("PPD error"), sprintf(_("Cannot open PPD '%s'!"), $ppdfile), ERROR_DIALOG);
                }else{
                    $ppdDesc = $this->ppdManager->loadDescription($this->pathToPPD.$this->pathToModified.$ppdfile);
                    if($ppdDesc){            
                        $selectedPPD = array(); 
                        $selectedPPD['name']   = $ppdDesc['name'];
                        $selectedPPD['link']   = $ppdfile;
                        $selectedPPD['ppd']    = $ppdDesc;
                    }    
                    $this->selectedPPD = $selectedPPD;
                }
            }
        }
    }


    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Fill templating stuff */
        $display= "";
        $smarty= get_smarty();
        $smarty->assign("ppdString", _("Can't get PPD information."));
        $smarty->assign("showOptions", "");
        $smarty->assign("path_valid", TRUE);
        $smarty->assign("acl",$this->ui->get_permissions($this->acl_base,"printer/printgeneric","gotoPrinterPPD"));

        $acl = $this->ui->get_permissions($this->acl_base,"printer/printgeneric","gotoPrinterPPD");

        if(empty($this->pathToPPD)){

            /* Print out template */
            $smarty->assign("path_valid", FALSE);
            $display.= $smarty->fetch(get_template_path('printerPPDDialog.tpl', TRUE,dirname(__FILE__)));
            return($display);
        }

        /* Check these paths */
        $paths = array($this->pathToPPD, $this->pathToPPD.$this->pathToModified);

        /* If one of our required paths is not available, stop here and display some info */
        foreach($paths as $path){

            /* Check if path is write/readable*/
            $is_r = @is_readable($path);  
            if(((!is_dir($path))||(empty($path)) || (!$is_r)) && (!@mkdir($path))){
                msg_dialog::display(_("Configuration error"), 
                        sprintf(_("Cannot open PPD path '%s' for reading and writing!"), $path), ERROR_DIALOG);

                /* Print out template */
                $display.= $smarty->fetch(get_template_path('printerPPDDialog.tpl', TRUE,dirname(__FILE__)));
                return($display);
            }
        }

        // PPD selection / upload / dialog handling 

        /* Is there a new PPD file uploaded ? */
        if(preg_match("/w/",$acl)){
            if((isset($_FILES['NewPPDFile']))&&(isset($_POST['SubmitNewPPDFile']))){
                $file = ($_FILES['NewPPDFile']);
                if($file['size'] != 0 ){
                    if($name = $this->AddPPD(gosa_file_name($file['tmp_name']))){
                        $this->SelectPPD($name); 
                    }
                }else{
                    msg_dialog::display(_("PPD error"), msgPool::incorrectUpload(_("file is empty")), ERROR_DIALOG);
                }
            }
        }

        /* Overwrite existing PPD file and select it as currently used for this object */
        if(is_object($this->add_later_msg_dialog) && ($this->add_later_msg_dialog->is_confirmed()) && $this->add_ppd_later != ""){
            if($name = $this->AddPPD($this->add_ppd_later,TRUE)){
                $this->SelectPPD($name);
            }
            $this->add_ppd_later = "";
            $this->add_later_msg_dialog = NULL;
        } 

        /* Open a dialog that allow us to select different PPDs */
        if(preg_match("/w/",$acl)){
            if(isset($_POST['SelectPPD'])){
                $this->dialog= new printerPPDSelectionDialog($this->config,$this->dn,$this->ppdList,$this->ppdListHeader,$this->selectedPPD);
            }
        }

        /* The selection dialog fpr PPDs is canceled */
        if(isset($_POST['ClosePPDSelection'])){
            unset($this->dialog);
            $this->dialog=FALSE;
        }

        /* Div Selection */ 
        if($this->dialog instanceOf printerPPDSelectionDialog){
            $this->dialog->save_object();
            if($this->dialog->isSelected()){
                $this->SelectPPD($this->dialog->save());
                unset($this->dialog);
                $this->dialog=FALSE;
            }
        }

        /* if a dialog is open, print the dialog instead of this class */
        if(is_object($this->dialog)){
            $display = $this->dialog->execute();
            return($display);
        }

        // ENDE  PPD selection / upload / dialog handling 

        /* Give smarty the information it needs */
        $smarty->assign("ppdString" ,$this->getPPDInformation());
        $tmp= $this->generateProperties();
        if ($tmp == ""){
            $smarty->assign("showOptions", 0);
        } else {
            $smarty->assign("showOptions", 1);
            $smarty->assign("properties",$this->generateProperties());
        }

        /* Print out template */
        $display.= $smarty->fetch(get_template_path('printerPPDDialog.tpl', TRUE,dirname(__FILE__)));
        return($display);
    }


    /* Select PPD */
    function SelectPPD($name)
    {
        /* Replace base path we don't need it here 
           The path we need looks like this : "/Vendor/ModellName.ppd"; 
           thats all */
        $name = preg_replace("#".$this->pathToPPD."#","",$name);

        /* Intialise some base vars */
        $AbsoluteSourceName       = $this->pathToPPD.$name;   
        $AbsoluteDestinationPath  = $this->pathToPPD.$this->pathToModified;
        $Vendor                   = ""; // Vendor 
        $Name                     = ""; // Name 
        $Modell                   = ""; // Modell
        $PrinterName              = ""; // The new name of the printer 
        $PPDName                  = "";

        /* Force reload of config dialog */ 
        $this->ppdConfig            = false;
        $this->selectedPPD['link']  = false;

        /* Get PPD informations and set vendor / modell / name */ 
        if((!file_exists($AbsoluteSourceName)) || (!is_readable($AbsoluteSourceName))){
            msg_dialog::display(_("PPD error"), msgPool::cannotReadFile($AbsoluteSourceName), ERROR_DIALOG);
            return;
        }
        $ppdDesc  = $this->ppdManager->loadDescription($AbsoluteSourceName);
        if($ppdDesc){
            $Name   = $ppdDesc['name'];
            $Vendor = $ppdDesc['manufacturer'];
            $Model  = $ppdDesc['model'];
        }    

        $PrinterName  = $this->cn."-".preg_replace("/[^a-z0-9-_\.]/i","_",$Name);
        $PPDName      = $Vendor."/".$PrinterName.".ppd";

        /* Create the vendors path, if it doesn't exists already */
        if(!is_dir($AbsoluteDestinationPath.$Vendor)){
            if(!(@mkdir($AbsoluteDestinationPath.$Vendor))){
                msg_dialog::display(_("PPD error"), msgPool::cannotCreateFolder($AbsoluteDestinationPath.$Vendor), ERROR_DIALOG);
                return(false);
            }
        }

        /* Create destination file handle */
        $fp = @fopen($AbsoluteDestinationPath.$PPDName,"w+");
        if(!$fp){
            msg_dialog::display(_("PPD error"), msgPool::cannotWriteFile($AbsoluteDestinationPath.$PPDName), ERROR_DIALOG);
            return(false);
        }

        $str = file_get_contents($AbsoluteSourceName);
        fputs($fp,$str);
        @fclose($fp);

        //$this->ppdManager->updateAttribute($filename,"NO_SECTION","ModelName",$printerName);

        $tmp3['link']   =$PPDName;
        $this->selectedPPD = $tmp3;
        $this->getPrinterReload(); 
        return($PPDName);
    }


    /* This function adds a new ppd file to the list of available ppds. 
       All required paths and files will be created 
       $_PathOnHdd e.g. = /tmp/PHP_tmpfile213452 */
    function AddPPD($_PathOnHdd,$overwrite = FALSE)
    {
        /* Check if file exists && is readable */
        if((!is_file($_PathOnHdd)) || (!is_readable($_PathOnHdd))){
            msg_dialog::display(_("PPD error"), msgPool::cannotReadFile($_PathOnHdd), ERROR_DIALOG);
            return(false);
        }

        /* Reload list to detect changes e.g. a file has been deleted */
        $this->getPrinterReload();

        /* Get Description from ppd, & parse out some informations */   
        $ppdDesc  = @$this->ppdManager->loadDescription($_PathOnHdd);
        if($ppdDesc){
            $name   = preg_replace("/\//","-",$ppdDesc['name']);
            $vendor = $ppdDesc['manufacturer'];
            $model  = $ppdDesc['model'];
        }    

        /* Check if parse was successfull */
        if(empty($name) || empty($vendor)){
            msg_dialog::display(_("PPD error"), sprintf(_("Cannot parse PPD '%s'!"), $_PathOnHdd), ERROR_DIALOG);
            return(false);
        }

        /* Prepare list of ppds */
        if(!isset($this->ppdList[$vendor])){
            $this->ppdList[$vendor] = array();
        }

        /* Create ppd file and fill in the source contents */
        $ppdname      = $vendor."/".$name.".ppd";
        $filename     = $this->pathToPPD.preg_replace("/[^a-z0-9-_\.\/]/i","_",$ppdname);
        $filename     = $this->pathToPPD.$ppdname;
        $contents     = file_get_contents($_PathOnHdd);


        /* Check if this ppd already exists */
        $found = false;

        foreach($this->ppdList[$vendor] as $key => $val){
            if(preg_match("/".preg_quote($model,'/').".*/i",$key)){
                $found = true;
                if(!$overwrite){
                    if(!copy($_PathOnHdd,$_PathOnHdd."_back")){
                        msg_dialog::display(_("PPD error"), msgPool::cannotReadFile($_PathOnHdd), ERROR_DIALOG);
                    }else{
                        $this->add_ppd_later = $_PathOnHdd."_back";
                        $this->add_later_msg_dialog = new msg_dialog(_("Overwrite existing PPD"),
                                _("There is already a PPD file for this kind of printer. Do you want to overwrite it?"),CONFIRM_DIALOG);
                    }
                    return;
                }
            }
        }

        /* Create the vendors path, if it doesn't exists already */
        if(!is_dir($this->pathToPPD.$vendor)){
            if(!(@mkdir($this->pathToPPD.$vendor))){
                msg_dialog::display(_("PPD error"), msgPool::cannotCreateFolder($this->pathToPPD.$vendor), ERROR_DIALOG);
                return(false);
            }
        }

        /* Open file handle */
        $fp           = fopen($filename,"w+");

        /* Check file handle & contents */
        if(!$fp){
            msg_dialog::display(_("PPD error"), msgPool::cannotWriteFile($filename), ERROR_DIALOG);
            return;
        }
        if(empty($contents)){
            msg_dialog::display(_("PPD error"), msgPool::incorrectUpload(_("file is empty")), ERROR_DIALOG);
            return;
        }

        /* Fille file with data */
        fputs($fp,$contents);
        @fclose($fp);

        /* Our job is done here */
        return($ppdname);
    }


    /* This function reloads the list of available printers/vendors 
       $this->ppdListHeader 
       Compaq        => 1
       $this->ppdList
       Compaq => Compaq IJ1200 => name => Compaq IJ1200 
       link => /var/spool/ppd/Compaq/Compaq-J1200.ppd
       ppd  => Compaq - Co    
     */
    function getPrinterReload()
    {
        if(is_readable($this->pathToPPD)){
            $tmp = @$this->ppdManager->getPrinterList(true);

            $this->ppdListHeader = $this->ppdList = array();

            /* Sort all available files, and create header (Vendor index) */
            foreach($tmp as $file=>$ppd){

                // Append the nickname to the ppds name, to be able to have multiple 
                //  ppd files for a printer type
                if(isset($ppd['nickname']) && !empty($ppd['nickname'])){
                    $ppd['name'] .= " (".$ppd['nickname'].")";
                }

                if(preg_match("#".preg_quote($this->pathToModified,'#')."#",$file)) continue;

                if(!isset($ppd['manufacturer'])) continue;

                if(!isset($this->ppdListHeader[$ppd['manufacturer']])){
                    $this->ppdListHeader[$ppd['manufacturer']]=0;
                }

                $tmp3['name']   =$ppd['name'];
                $tmp3['link']   =$file;
                $tmp3['ppd']    =$ppd;
                $this->ppdListHeader[$ppd['manufacturer']]++;
                $this->ppdList[$ppd['manufacturer']][$ppd['name']]=$tmp3;
            }
        }
    }


    /* Save all options posted from ppd dialog */
    function save_object()
    {
        if(!((isset($_POST['PPDDisSubmitted'])) && (is_array($this->ppdConfig)))){
            return;
        }

        if(preg_match("/w/",$this->ui->get_permissions($this->acl_base,"printer/printgeneric","gotoPrinterPPD"))){
            foreach($this->ppdConfig as $cat => $obj){
                foreach($obj as $attr => $attributes){
                    if(isset($_POST[base64_encode($attributes['_name'])])){
                        $this->ppdConfig[$cat][$attr]['_default'] = get_post(base64_encode($attributes['_name']));
                    }
                }
            }
        }
    }


    /* Save modified ppd */
    function save_ppd()
    {
        if($this->ppdManager){
            $this->ppdManager->saveProperties($this->pathToPPD.$this->pathToModified.$this->selectedPPD['link'],$this->ppdConfig);
        }
    }


    /* Return selected ppd path, if none is selected then false */
    function save()
    {
        /* return the selected PPD, and in future the selected options too */
        return($this->pathToModified.$this->selectedPPD['link']);
    }


    /* Get Information for a single PPD entry 
     * This will be shown on top of template
     */
    function getPPDInformation()
    {
        $str = "none";
        if(!empty($this->selectedPPD)){
            $ppdDesc = $this->ppdManager->loadDescription($this->pathToPPD.$this->pathToModified.$this->selectedPPD['link']);
            $str = $ppdDesc['name'];
        }
        return($str) ; 
    }

    /* Display all options from the selected ppd file */
    function generateProperties()
    { 
        /* Set Headline */
        $str = "";
        $feed= "";

        $s_ppd = $this->pathToPPD.$this->pathToModified.$this->selectedPPD['link'];

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

                    if (!isset($settings['_default'])){
                        $default  = "";
                    } else {
                        $default  = $settings['_default'];
                    }

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

                    $acl ="";
                    if(!preg_match("/w/",$this->ui->get_permissions($this->acl_base,"printer/printgeneric","gotoPrinterPPD"))){
                        $acl = "disabled";
                    }

                    /* preparing Html output
                     * Supported types are PickOne/Boolean
                     */

                    /* If type is PickOne, create a select box */
                    if(($type == "PickOne")||(($type=="Boolean")&&(count($values)>1))){

                        $str  .=  "<select name='".base64_encode($name)."' $acl size='1'>\n";
                        foreach($values as $optionKey => $value){
                            $selected = "";
                            if($optionKey == $default){
                                $selected = " selected ";
                            }
                            $str  .=  "<option value='".$optionKey."' ".$selected.">".$value."</option>\n";
                        }
                        $str  .=  "</select>\n";

                    }elseif($type == "Boolean"){

                        /* If type is Boolean & no values are given */
                        $str  .=  "<select name='".base64_encode($name)."' $acl size='1'>\n";
                        if($default == "False"){
                            $str  .=    "<option value='True' >"._("True")."</option>\n";
                            $str  .=    "<option value='False' selected>"._("False")."</option>\n";
                        }else{
                            $str  .=    "<option value='True' selected>"._("True")."</option>\n";
                            $str  .=    "<option value='False' >"._("False")."</option>\n";
                        }          
                        $str  .=  "</select>\n";

                    }elseif($this->config->get_cfg_value("core","displayErrors") == "true"){
                        msg_dialog::display(_("PPD error"), sprintf(_("PPD type '%s' is not supported!"), $type), ERROR_DIALOG);
                    }
                    $str .= "</td></tr>\n";
                }
            }
            $str .= "</table></div>\n";
        }
        return($str);
    }

    function removeModifiedPPD()
    {
        $path = $this->pathToPPD.$this->pathToModified.$this->selectedPPD['link'];

        if(file_exists($path)){
            if(is_writeable($path)){
                if(!@unlink($path)){
                    msg_dialog::display(_("PPD error"), msgPool::cannotDeleteFile($path), ERROR_DIALOG);
                }
            }else{
                msg_dialog::display(_("PPD error"), msgPool::cannotDeleteFile($path), ERROR_DIALOG);
            }
        }else{
            msg_dialog::display(_("PPD error"), msgPool::cannotDeleteFile($path), ERROR_DIALOG);
        }
    }

    function update_ppd_url()
    {
        $this->SelectPPD("modified/".$this->selectedPPD['link']);
        return("modified/".$this->selectedPPD['link']);
    }

    function check()
    {  
        $message = plugin::check();
        if(empty($this->selectedPPD['link'])){
            $message[] = _("Please select a valid PPD file or use 'Cancel' to go back to printer configuration.");
        }
        return($message); 
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
