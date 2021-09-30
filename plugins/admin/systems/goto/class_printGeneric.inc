<?php

class printgeneric extends plugin
{
    /* Generic terminal attributes */
    var $interfaces     = array();
    var $ignore_account = FALSE;

    /* Needed values and lists */
    var $base             = "";
    var $cn               = "";
    var $l                = "";
    var $description      = "";
    var $labeledURI       = "";
    var $gotoPrinterPPD   = "";
    var $initial_PPD      = "";
    var $orig_dn          = "";
    var $orig_cn          = "";
    var $orig_base        = "";

    var $UserMember       ="";
    var $UserMembers      =array();
    var $UserMemberKeys   =array();

    var $AdminMember      ="";
    var $AdminMembers     =array();
    var $AdminMemberKeys  =array();

    var $ppdServerPart   = "";

    var $PPDdialogToSave  = NULL;
    var $BelongsTo        = "unknown"; //  Specifies if this is a standalone printer, or belongs to a terminal / WS

    var $member           =array();
    var $strings          = "";
    var $netConfigDNS;
    var $baseSelection    = false;
    var $macAddress       = "";

    var $gotoUserAdminPrinter;
    var $gotoGroupAdminPrinter ;
    var $gotoGroupPrinter;
    var $gotoUserPrinter ;
    var $baseSelector;

    /* attribute list for save action */
    var $attributes     = array("cn", "description", "l", "labeledURI", "gotoPrinterPPD","gotoUserPrinter", "macAddress", 
            "gotoUserAdminPrinter","gotoGroupAdminPrinter","gotoUserPrinter","gotoGroupPrinter","gosaUnitTag");
    var $objectclasses  = array("top", "gotoPrinter");
    var $view_logged    = FALSE;
    var $parent;


    // Memeber dialog object
    var $userSelect;
    var $adminUserSelect;

    function __construct(&$config, $dn,$parent_init,$parent)
    {
        $this->config = &$config;
        $this->dn = $dn; 

        /* If parent was posted(the tabs object) we can detect the printer type. */
        if($parent){
            $this->parent = $parent;
            $this->getTypeOfPrinter();
        }else{
            $this->BelongsTo = "unknown";
            return;
        }

        /* Update dn, to ensure storing as printer instead of WS / terminal */
        if(preg_match("/Terminal/i",$this->BelongsTo) || preg_match("/TerminalTemplate/i",$this->BelongsTo)){
            $this->dn= preg_replace("/".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
        }

        if(preg_match("/Workstation/i",$this->BelongsTo) || preg_match("/WorkstationTemplate/i",$this->BelongsTo)){
            $this->dn= preg_replace("/".preg_quote(get_ou("workgeneric", "workstationRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
        }

        $this->orig_dn = $this->dn;

        /* Get printer settings, possibly dn has changed */
        plugin::__construct ($config, $this->dn);

        /* Get is_account initially_was_account status */
        $this->getTypeOfPrinter(true);

        /* set orig dn to new if object is new */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn, array('dn'));
        if(!$ldap->count()){
            $this->orig_dn = "new";
        }

        /* create dns object */
        $this->netConfigDNS = new termDNS($this->config, $this,$this->objectclasses);

        /* Set base */
        if ($this->dn == "new"){
            $ui= get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
            $this->cn= "";
        } else {

            /* Set base and check if the extracted base exists */
            if(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i",$this->dn)){
                $this->base= preg_replace("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i","",dn2base($this->dn));
            }else{
                $this->base= preg_replace("/".preg_quote(get_ou("printgeneric", "printerRDN"), '/')."/i","",dn2base($this->dn));
            }

            if(!isset($this->config->idepartments[$this->base])){
                msg_dialog::display(_("Internal error"), sprintf(_("Cannot determine a valid department for this object. Setting base to '%s'!"), session::get('CurrentMainBase')) , WARNING_DIALOG);
                $this->base  = session::get('CurrentMainBase');
            }
        }

        /* If no ppd is selected, remove this attribute */
        if(!empty($this->gotoPrinterPPD) && $this->initially_was_account) {
            $this->ppdServerPart = preg_replace("/^(http.*ppd)\/.*$/i","\\1",$this->gotoPrinterPPD);
        }else{

            /* Detect PPD server part */
            if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
                $method="https://";
            }else{
                $method="http://";
            }

            /* Get servername */
            $server = $_SERVER['SERVER_NAME'];
            if(tests::is_ip($server)){  
                $server_name = @gethostbyaddr($server);
            }else{
                $server_name = @gethostbyaddr(gethostbyname($server));
            }
            $this->ppdServerPart  = $method.str_replace("//","/",$server_name."/ppd");
        }

        /* Extract selected ppd */
        if(isset($this->gotoPrinterPPD)){
            $this->gotoPrinterPPD = preg_replace("/^http.*ppd\//i","",$this->gotoPrinterPPD);
        }

        $this->initial_PPD = $this->gotoPrinterPPD;

        /* Prepare different member types */ 
        foreach(array("AddUser"       =>"gotoUserPrinter",
                    "AddGroup"      =>"gotoGroupPrinter",
                    "AddAdminUser"  =>"gotoUserAdminPrinter",
                    "AddAdminGroup" =>"gotoGroupAdminPrinter") as $type => $attr){

            /* $this->members contains all members */
            $this->member[$type]=array();

            if(isset($this->attrs[$attr])){
                $ldap->cd($this->config->current['BASE']) ;
                for($i = 0 ;  $i < $this->attrs[$attr]['count']; $i++){

                    $mem = $this->attrs[$attr][$i];
                    if(preg_match("/Group/",$type)){
                        $ldap->search("(&(|(objectClass=posixGroup)(objectClass=gosaGroupOfNames))(cn=".$mem."))",array("cn","description"));
                        if($ldap->count()){
                            $entry = $ldap->fetch();
                            $this->member[$type][$entry['cn'][0]]=$entry;
                        }
                    }else{
                        $ldap->search("(&(objectClass=person)(objectClass=inetOrgPerson)(uid=".$mem."))",array("cn","uid"));
                        if($ldap->count()){
                            $entry = $ldap->fetch();
                            $this->member[$type][$entry['uid'][0]]=$entry;
                        }
                    }
                }
            }
        }
        $this->orig_cn    = $this->cn;
        $this->orig_base  = $this->base;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
    }

    function set_acl_base($base)
    {
        plugin::set_acl_base($base);
        if(is_object($this->netConfigDNS)){
            $this->netConfigDNS->set_acl_base($base);
        }
    }

    function set_acl_category($cat)
    {
        plugin::set_acl_category($cat);
        if(is_object($this->netConfigDNS)){
            $this->netConfigDNS->set_acl_category($cat);
        }
    }

    /* Detect type of printer.
     * Printer can be stand alone, belong to a workstation or belong to a terminal. 
     * We can detect the type printer type when comparing the tabs objects
     */
    function getTypeOfPrinter($UpdateAccountStatus = false)
    {
        /* Disable account as default
         */  
        $this->is_account = $this->initially_was_account = false;

        /* Detect type of printer via parent tabs.
         */

        $class = get_class($this->parent);
        if(isset($this->parent->by_object['workgeneric'])){

            /* Exclude templates 
             */
            $this->cn = $this->parent->by_object['workgeneric']->cn;
            if($this->parent->by_object['workgeneric']->cn == "wdefault"){
                $this->BelongsTo = "WorkstationTemplate";
            }else{
                $this->BelongsTo = "Workstation";
            }
        }elseif(isset($this->parent->by_object['termgeneric'])){

            /* Exclude templates 
             */
            $this->cn = $this->parent->by_object['termgeneric']->cn;
            if($this->parent->by_object['termgeneric']->cn == "default"){
                $this->BelongsTo = "TerminalTemplate";  
            }else{
                $this->BelongsTo = "Terminal";
            }
        }elseif(isset($this->parent->by_name['printgeneric'])){
            $this->BelongsTo  = "Printer";
        }

        if($UpdateAccountStatus){

            /* Set is_account / was account 
             */
            if($this->dn == "new"){
                $this->initially_was_account = false;
            }

            /* If is printer it must be a true account.
             */
            if(preg_match("/printer/i",$this->BelongsTo)){
                $this->is_account = true;
            }

            /* Update dn, to ensure storing as printer instead of WS / terminal
             */
            if(preg_match("/terminal/i",$this->BelongsTo)){
                $this->dn= preg_replace("/".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
            }

            if(preg_match("/workstation/i",$this->BelongsTo)){
                $this->dn= preg_replace("/".preg_quote(get_ou("workgeneric", "workstationRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
            }

            /* Detect if this is a valid printer account;
             */
            $ldap = $this->config->get_ldap_link();
            $ldap->cat($this->dn, array('objectClass'));

            if($ldap->count()){
                $attrs = $ldap->fetch();
                if(in_array_strict("gotoPrinter",$attrs['objectClass'])){
                    $this->initially_was_account = true;
                    $this->is_account             = true;
                }else{
                    $this->is_account = false;
                }
            }
        }
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","printer/".get_class($this),$this->dn);
        }

        /* If type of printer couldn't be detected (because of missing parent object in construction) 
         * hide this tab.
         */
        if(preg_match("/unknown/i",$this->BelongsTo)){
            $display= $this->show_enable_header(_("Add printer extension"),
                    _("Could not initialize printer tab, parameter parent was missing while construction."),TRUE,TRUE);
            return($display);
        }

        /* Templates can't have printer extensions 
         */
        if(preg_match("/WorkstationTemplate/i",$this->BelongsTo)){
            $display= $this->show_enable_header(_("Add printer extension"),
                    _("This is a workstation template, printer tab is disabled."),TRUE,TRUE);
            return($display);
        }
        if(preg_match("/TerminalTemplate/i",$this->BelongsTo)){
            $display= $this->show_enable_header(_("Add printer extension"),
                    _("This is a terminal template, printer tab is disabled."),TRUE,TRUE);
            return($display);
        }

        /* Get cn from base object */
        if(preg_match("/^Workstation$/i",$this->BelongsTo)){
            $this->cn = $this->parent->by_object['workgeneric']->cn;
        }
        if(preg_match("/^Terminal$/i",$this->BelongsTo)){
            $this->cn = $this->parent->by_object['termgeneric']->cn;
        }

        $smarty= get_smarty();


        /* Assign acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL", $this->getacl($name));
        }

        $display="";

        /* Tell smarty if this is a standalone object or a terminal / WS depending printer */
        if(preg_match("/^Printer$/i",$this->BelongsTo)){    
            $smarty->assign("StandAlone",true);
        }else{
            $smarty->assign("StandAlone",false);
        }

        /* Do we need to flip is_account state? */
        if(isset($_POST['modify_state'])){
            if($this->is_account && $this->acl_is_removeable()){
                $this->is_account= FALSE;
            }elseif(!$this->is_account && $this->acl_is_createable()){
                $this->is_account= TRUE;
            }
        }

        /* Do we represent a valid printer? */
        if (!$this->is_account && $this->parent === NULL){
            $display= "<img alt=\"\" src=\"images/small-error.png\" align=middle>&nbsp;<b>".
                msgPool::noValidExtension(_("printer"))."</b>";
            return($display);
        }

        /* If this is a WS / Terminal depending printer, display account state button */
        if(!preg_match("/^Printer$/i",$this->BelongsTo)){
            if($this->cn == "" && ($this->dn != "new")){
                $display= $this->show_enable_header(_("Add printer extension"),
                        msgPool::featuresDisabled(_("printer"))._("You can't enable it while 'cn' is not present in entry. Possibly you are currently creating a new terminal template."),TRUE,TRUE);
                $this->is_account= false;
                return $display;
            }

            if (($this->is_account)){
                if(preg_match("/^Workstation$/i",$this->BelongsTo)){
                    $display= $this->show_disable_header(_("Remove printer extension"),
                            msgPool::featuresEnabled(_("printer")));
                }elseif(preg_match("/^Terminal$/i",$this->BelongsTo)){
                    $display= $this->show_disable_header(_("Remove printer extension"),
                            msgPool::featuresDisabled(_("printer")));
                }
            }else{
                if(preg_match("/^Workstation$/i",$this->BelongsTo)){
                    $display= $this->show_enable_header(_("Add printer extension"),
                            msgPool::featuresEnabled(_("printer")));
                }elseif(preg_match("/^Terminal$/i",$this->BelongsTo)){
                    $display= $this->show_enable_header(_("Add printer extension"),
                            msgPool::featuresDisabled(_("printer")));
                }  
                return ($display);
            }
        }

        /* Assign attributes */
        foreach ($this->attributes as $attr){
            $smarty->assign("$attr", set_post($this->$attr));
        }

        $smarty->assign("base", $this->baseSelector->render());

        // Act on add user/grouo requests 
        if(isset($_POST['AddUser'])){
            $this->userSelect = new userGroupSelect($this->config, get_userinfo());
            $this->dialog = TRUE;
        }
        if(isset($_POST['AddAdminUser'])){
            $this->adminUserSelect = new userGroupSelect($this->config, get_userinfo());
            $this->dialog = TRUE;
        }

        /* Display ppd configure/select dialog      */
        if(isset($_POST['EditDriver'])){
            if($this->PPDdialogToSave && is_object($this->PPDdialogToSave)){
                $this->dialog = $this->PPDdialogToSave;
            }else{

                if(is_array($this->gotoPrinterPPD)){
                    $this->dialog = new printerPPDDialog($this->config, $this->dn,"");
                }else{
                    $this->dialog = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
                }
                $this->dialog->cn= $this->cn;
            }

            // Detect edit acl base
            $ldap = $this->config->get_ldap_link();
            if($ldap->dn_exists($this->dn)){
                $acl_base = $this->dn;
            }else{
                $acl_base = $this->base;
            }

            $this->dialog->set_acl_base($acl_base);
        }

        /* remove ppd */
        if(isset($_POST['RemoveDriver'])){
            /* Detect PPD server part */
            if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
                $method="https://";
            }else{
                $method="http://";
            }

            /* Get servername */
            $server = $_SERVER['SERVER_NAME'];
            if(tests::is_ip($server)){  
                $server_name = @gethostbyaddr($server);
            }else{
                $server_name = @gethostbyaddr(gethostbyname($server));
            }
            $this->ppdServerPart  = $method.str_replace("//","/",$server_name."/ppd");
            $this->gotoPrinterPPD = array();
            $this->PPDdialogToSave = NULL;
        }

        /* Close ppd dialog */
        if(isset($_POST['ClosePPD'])){
            unset($this->dialog);
            $this->dialog=FALSE;
        }

        /* Save selected ppd */
        if(isset($_POST['SavePPD'])){
            $this->dialog->save_object();
            if(count($this->dialog->check())){
                foreach($this->dialog->check() as $msg){
                    msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
                }
            }else{
                $this->gotoPrinterPPD = array();
                $this->gotoPrinterPPD = $this->dialog->save();
                $this->PPDdialogToSave = $this->dialog;
                unset($this->dialog);
                $this->dialog=FALSE;
            }
        }

        /* Member management, delete user / group / admin ..*/
        if((isset($_POST['DelUser']))&&(isset($_POST['UserMember']))){
            foreach($_POST['UserMember'] as $mem){
                $this->DelMember('AddUser',$mem);
            }
        }

        if((isset($_POST['DelUser']))&&(isset($_POST['UserMember']))){
            foreach($_POST['UserMember'] as $mem){
                $this->DelMember('AddGroup',$mem);
            }
        }

        if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember']))){
            foreach($_POST['AdminMember'] as $mem){
                $this->DelMember('AddAdminUser',$mem);
            }
        }

        if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember']))){
            foreach($_POST['AdminMember'] as $mem){
                $this->DelMember('AddAdminGroup',$mem);
            }
        }

        // Abort user / group adding dialog 
        if(isset($_POST['userGroupSelect_cancel'])){
            $this->dialog=FALSE;
            $this->userSelect = NULL;
            $this->adminUserSelect = NULL;
        }

        // Save selected users / groups 
        if(isset($_POST['userGroupSelect_save'])){
            $users = array();
            if($this->userSelect instanceOf userGroupSelect){
                $users = $this->userSelect->save();
                $add = '';
            }elseif($this->adminUserSelect instanceOf userGroupSelect){
                $users = $this->adminUserSelect->save();
                $add = 'Admin';
            }
            foreach($users as $user){
                if(in_array_strict('gosaAccount', $user['objectClass'])){
                    $type = 'Add'.$add.'User';  
                }else{
                    $type = 'Add'.$add.'Group';  
                }
                $this->AddMember($type, $user['dn']);
            }
            $this->dialog=FALSE;
            $this->userSelect = NULL;
            $this->adminUserSelect = NULL;
        }

        // Display add user/group dialogs 
        if($this->userSelect instanceOf userGroupSelect || $this->adminUserSelect instanceOf userGroupSelect){

            // Build up blocklist
            $used = array();
            foreach($this->member as $type => $members){
                foreach($members as $member){
                    $used['dn'][] = $member['dn'];
                }
            }
            session::set('filterBlacklist', $used);
            if($this->userSelect instanceOf userGroupSelect){
                return($this->userSelect->execute());
            }elseif($this->adminUserSelect instanceOf userGroupSelect){
                return($this->adminUserSelect->execute());
            }
        }


        /* Display dialog, if there is currently one open*/
        if(is_object($this->dialog)){
            $this->dialog->save_object();
            $display = $this->dialog->execute();
            return $display;
        }

        /* Parse selected ppd file */
        $config = session::get('config');
        if ($config->get_cfg_value("core","ppdPath") != ""){
            $path = $config->get_cfg_value("core","ppdPath");
            if(!preg_match("/\/$/",$path)){
                $path = $path."/";
            }

            $ppdManager= new ppdManager($path);
            $smarty->assign("displayServerPath",true);
            if(!empty($this->gotoPrinterPPD)){
                if((!file_exists($path.$this->gotoPrinterPPD))){
                    $smarty->assign("driverInfo", "<b>".sprintf(_("Your currently selected PPD file '%s' doesn't exist."),$path.$this->gotoPrinterPPD)."</b>");
                }else{
                    $ppdDesc = $ppdManager->loadDescription($path.$this->gotoPrinterPPD);
                    $smarty->assign("driverInfo", set_post($ppdDesc['name']));
                }
            }else{
                $smarty->assign("driverInfo", _("Not defined"));
                $smarty->assign("displayServerPath",false);
            }
        }else{
            $smarty->assign("driverInfo",_("Can't get PPD informations."));
            $smarty->assign("displayServerPath",true);
        }
        $smarty->assign("ppdServerPart",$this->ppdServerPart);


        /* Create user & admin user list */
        $list=$this->generateList();
        $userlist   = array_merge($list['AddUser'],$list['AddGroup']);
        $adminlist  = array_merge($list['AddAdminUser'],$list['AddAdminGroup']);

        asort($userlist);
        asort($adminlist);

        if(!preg_match("/Printer/i",$this->BelongsTo)){
            if(preg_match("/Terminal/i",$this->BelongsTo)){
                $smarty->assign("desc"    ,sprintf(_("This printer belongs to %s. You can't rename this printer."),_("terminal"),"<b>".$this->cn."</b>"));
            }else{
                $smarty->assign("desc"    ,sprintf(_("This printer belongs to %s. You can't rename this printer."),_("workstation"),"<b>".$this->cn."</b>"));
            }
            $smarty->assign("cnACL"    , $this->getacl("cn",true));
        }else{
            $smarty->assign("desc"    ,"");
        }
        $smarty->assign("UserMember"    ,$this->UserMember);
        $smarty->assign("UserMembers"   ,$userlist);
        $smarty->assign("UserMemberKeys",array_flip($userlist));

        $smarty->assign("AdminMember"    ,$this->AdminMember);
        $smarty->assign("AdminMembers"   ,$adminlist);
        $smarty->assign("AdminMemberKeys",array_flip($adminlist));
        if(preg_match("/Printer/i",$this->BelongsTo)){

            /* Show main page */
            $str = $this->netConfigDNS->execute();
            if(is_object($this->netConfigDNS->dialog)){
                return($str);
            }
            $smarty->assign("netconfig", $str);
        } else {
            $smarty->assign("netconfig", "");
        }

        return($display.$smarty->fetch (get_template_path('printer.tpl', TRUE, dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        /* Only remove if there was initially an account */
        if($this->initially_was_account){

            /* Update dn, to ensure storing as printer instead of WS / terminal
             */
            if(preg_match("/terminal/i",$this->BelongsTo)){
                $this->dn= preg_replace("/".preg_quote(get_ou("termgeneric", "terminalRDN"), '/').",/i",get_ou("printgeneric", "printerRDN"),$this->dn);
            }

            if(preg_match("/workstation/i",$this->BelongsTo)){
                $this->dn= preg_replace("/".preg_quote(get_ou("workgeneric", "workstationRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
            }

            /* Check if this dn points to a printer, to avoid deleting something else */
            $ldap= $this->config->get_ldap_link();
            $ldap->cat($this->dn, array('dn',"objectClass"));
            if(!$ldap->count()){
                msg_dialog::display(_("Error"), _("Object is no printer!"), ERROR_DIALOG);
                return;
            }

            /* Check if obejct is a printer */
            $CheckPrinter = $ldap->fetch();
            if(!in_array_strict("gotoPrinter",$CheckPrinter['objectClass'])){
                msg_dialog::display(_("Error"), _("Object is no printer!"), ERROR_DIALOG);
                return;
            }

            /* Remove account & dns extension */ 
            $this->netConfigDNS->remove_from_parent();
            $ldap->rmdir($this->dn);

            new log("remove","printer/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
            }
            $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

            /* Delete references to object groups */
            $ldap->cd ($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
            while ($ldap->fetch()){
                $og= new ogroup($this->config, $ldap->getDN());
                unset($og->member[$this->dn]);
                $og->save ();
            }

            /* Remove previously selected ppd file.*/
            if(!empty($this->initial_PPD)){
                $tmp = new printerPPDDialog($this->config, $this->dn,$this->initial_PPD);
                $tmp->removeModifiedPPD();
            }
        }
    }


    /* Save data to object */
    function save_object()
    {
        /* Create a base backup and reset the
           base directly after calling plugin::save_object();
           Base will be set seperatly a few lines below */
        $base_tmp = $this->base;
        plugin::save_object();
        $this->base = $base_tmp;

        if(isset($_POST['ppdServerPart'])){
            $this->ppdServerPart = get_post('ppdServerPart');
        }

        if(is_object($this->netConfigDNS)){
            $this->netConfigDNS->save_object();
        }

        /* Refresh base */
        if ($this->acl_is_moveable($this->base)){
            if (!$this->baseSelector->update()) {
                msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
            }
            if ($this->base != $this->baseSelector->getBase()) {
                $this->base= $this->baseSelector->getBase();
                $this->is_modified= TRUE;
            }
        }

    }

    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();
        if (preg_match("/printer/i",$this->BelongsTo)){
            $message= array_merge($message, $this->netConfigDNS->check());
        }

        /* Don't display check messages if this is a template object */
        if(isset($this->parent->by_object['workgeneric'])){
            if($this->parent->by_object['workgeneric']->cn == "wdefault"){
                return $message;
            }
        }elseif(isset($this->parent->by_object['termgeneric'])){
            if($this->parent->by_object['termgeneric']->cn == "default"){
                return $message;
            }
        }

        $dn= "cn=".$this->cn.get_ou("printgeneric", "printerRDN").",".$this->base;

        /* must: cn */
        if(($this->BelongsTo == "Printer") && $this->cn == ""){
            $message[]= msgPool::required(_("Name"));
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();
        }

        /* must: cn */
        if(($this->BelongsTo == "Printer") && !tests::is_dns_name($this->cn)){
            $message[]= msgPool::invalid(_("Name"));
        }

        /* must: labeledURI */
        if(empty($this->labeledURI)){
            $message[]= msgPool::required(_("Printer URL"));
        }

        /* Check if there is already an entry with this cn*/
        if (($this->orig_dn != $dn)&&( preg_match("/printer/i",$this->BelongsTo))){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($this->base);
            $ldap->ls("(cn=".$this->cn.")",get_ou("printgeneric", "printerRDN").$this->base, array("cn"));
            if ($ldap->count() != 0){
                while ($attrs= $ldap->fetch()){
                    if(preg_match("/cn=dhcp,/",$attrs['dn'])){
                        continue;
                    }
                    if ($attrs['dn'] != $this->orig_dn){
                        $message[]= msgPool::duplicated(_("Name"));
                        break;
                    }
                }
            }
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        /* Update dn, to ensure storing as printer instead of WS / terminal
         */
        if(preg_match("/terminal/i",$this->BelongsTo)){
            $this->dn= preg_replace("/".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
        }

        if(preg_match("/workstation/i",$this->BelongsTo)){
            $this->dn= preg_replace("/".preg_quote(get_ou("workgeneric", "workstationRDN"), '/')."/i",get_ou("printgeneric", "printerRDN"),$this->dn);
        }

        if(!$this->is_account) return;
        if(isset($this->parent->by_object['workgeneric'])){
            if($this->parent->by_object['workgeneric']->cn == "wdefault"){
                return;
            }

            /* Adapt IP & mac from parent object */
            $this->netConfigDNS->ipHostNumber = $this->parent->by_object['workgeneric']->netConfigDNS->ipHostNumber;
            $this->netConfigDNS->macAddress = $this->parent->by_object['workgeneric']->netConfigDNS->macAddress;

        }elseif(isset($this->parent->by_object['termgeneric'])){
            if($this->parent->by_object['termgeneric']->cn == "default"){
                return;
            }

            /* Adapt IP & mac from parent object */
            $this->netConfigDNS->ipHostNumber = $this->parent->by_object['termgeneric']->netConfigDNS->ipHostNumber;
            $this->netConfigDNS->macAddress = $this->parent->by_object['termgeneric']->netConfigDNS->macAddress;
        }

        /* If type is still unknown, the initialisation of this printer failed, abort. */
        if(preg_match("/unknown/i",$this->BelongsTo)){
            return;
        }

        /* save ppd configuration */
        if($this->PPDdialogToSave && is_object($this->PPDdialogToSave)){

            $this->PPDdialogToSave->save_ppd();

            /* Rename the generated ppd to match the gzip ending '.gz', if necessary.
             */
            $path = $this->config->get_cfg_value("core","ppdPath");
            if(!preg_match("/\/$/",$path)){
                $path = $path."/";
            }
            $ppdManager= new ppdManager($path);
            if($ppdManager->useGzip && !preg_match('/\.gz$/',$this->gotoPrinterPPD)){
                if(rename($path.$this->gotoPrinterPPD,$path.$this->gotoPrinterPPD.'.gz')){
                    $this->gotoPrinterPPD .= '.gz';
                }
            }elseif(!$ppdManager->useGzip && preg_match('/\.gz$/',$this->gotoPrinterPPD)){
                $new_ppd = preg_replace('/\.gz$/','',$this->gotoPrinterPPD);
                if(rename($path.$this->gotoPrinterPPD,$path.$new_ppd)){
                    $this->gotoPrinterPPD = $new_ppd; 
                }
            }
        }
        if($this->orig_dn != $this->dn && $this->orig_dn != "new"){
            if(!empty($this->gotoPrinterPPD)) {
                $this->PPDdialogToSave = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
                $this->PPDdialogToSave->cn = $this->cn;
                $this->PPDdialogToSave->generateProperties();
                $this->gotoPrinterPPD = $this->PPDdialogToSave->update_ppd_url();
            }
        }

        /* Remove previously selected ppd file.*/
        if($this->initial_PPD != $this->gotoPrinterPPD && $this->initially_was_account){
            if(!empty($this->initial_PPD)){
                $tmp = new printerPPDDialog($this->config, $this->dn,$this->initial_PPD);
                $tmp->removeModifiedPPD();
            }
        }

        /* If no ppd is selected, remove this attribute */
        if(!empty($this->gotoPrinterPPD)) {
            $this->gotoPrinterPPD = $this->ppdServerPart.'/'.$this->gotoPrinterPPD;
        }else{
            $this->gotoPrinterPPD = array();
        }

        $dn= $this->dn;

        /* reduce objectClasses to minimun */
        $this->attrs['objectClass']= $this->objectclasses;

        plugin::save();
        $ldap= $this->config->get_ldap_link();

        /* Remove all empty values */
        if ($this->orig_dn == 'new'){
            $attrs= array();
            foreach ($this->attrs as $key => $val){
                if (is_array($val) && count($val) == 0){
                    continue;
                }
                $attrs[$key]= $val;
            }
            $this->attrs= $attrs;
        }

        /* Append printer user 
         */
        $this->attrs['gotoUserPrinter']=array();
        foreach($this->member['AddUser'] as $mem){
            $this->attrs['gotoUserPrinter'][]=$mem['uid'][0];
        }

        /* Append printer group 
         */
        $this->attrs['gotoGroupPrinter'] = array();
        foreach($this->member['AddGroup'] as $mem){
            $this->attrs['gotoGroupPrinter'][]=$mem['cn'][0];
        }

        /* Append printer admin user 
         */
        $this->attrs['gotoUserAdminPrinter'] = array();
        foreach($this->member['AddAdminUser'] as $mem){
            $this->attrs['gotoUserAdminPrinter'][]=$mem['uid'][0];
        }

        /* Append printer admin group 
         */
        $this->attrs['gotoGroupAdminPrinter']= array();
        foreach($this->member['AddAdminGroup'] as $mem){
            $this->attrs['gotoGroupAdminPrinter'][]=$mem['cn'][0];
        }

        if($this->orig_dn == 'new'){
            foreach(array("gotoGroupPrinter","gotoUserAdminPrinter","gotoGroupAdminPrinter","gotoUserPrinter") as $checkVar){
                if(count($this->attrs[$checkVar])  == 0 || empty($this->attrs[$checkVar])){
                    unset($this->attrs[$checkVar]);
                }
            }
        }

        /* Ensure to create a new object */
        if(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i",$this->orig_dn)){
            $this->orig_dn = "new";
        }

        /* Move object in necessary*/
        if (($this->orig_dn != $this->dn) && ($this->orig_dn != 'new')){
            $this->move($this->orig_dn, $this->dn);
        }

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn);
        if(!$ldap->count()){
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);

            /* Remove empty values */ 
            foreach($this->attrs as $name => $value){
                if(empty($value)){
                    unset($this->attrs[$name]);
                }
            }

            $ldap->add($this->attrs);
            $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
            new log("create","printer/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        } else {
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 
            $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
            new log("modify","printer/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

#if(preg_match("/printer/i",$this->BelongsTo)){
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->dn = $this->dn;
    $this->netConfigDNS->save();
#}
    }

    function generateList(){
        $a_return=array();

        foreach($this->member as $type => $values){
            $a_return[$type]=array();
            foreach($values as $value){
                if((preg_match("/Group/i",$type))){
                    if(!isset($value['description'])){
                        $a_return[$type][$value['cn'][0]]= _("Group")." : ".$value['cn'][0];
                    }else{
                        $a_return[$type][$value['cn'][0]]= _("Group")." : ".$value['cn'][0]." [".$value['description'][0]."]";
                    }
                }else{
                    $a_return[$type][$value['uid'][0]]=_("User")." : ".$value['cn'][0];
                }
            }
        }
        return($a_return);
    }

    /* Return plugin informations for acl handling
#FIXME FAIscript seams to ununsed within this class... */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Print generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 4,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoPrinter' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plCategory"    => array("printer" => array("description"  => _("Printer"),
                            "objectClass"  => "gotoPrinter"),"workstation","terminal"),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "printerRDN",
                            "type"          => "rdn",
                            "default"       => "ou=printers,ou=systems,",
                            "description"   => _("RDN for printer storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_printerRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),

            "plProvidedAcls"=> array(
                    "cn"                => _("Name"),
                    "base"                => _("Base") ,         
                    "description"       => _("Description"), 
                    "l"                 => _("Location"), 
                    "labeledURI"        => _("Labeled URI"), 
                    "macAddress"        => _("Mac address"), 
                    "gotoPrinterPPD"    => _("Printer PPD"),
                    "gotoUserPrinter"   => _("Permissions")) 
                ));
    }


    /* Delete member */
    function DelMember($type,$id)
    {
        /* Check if there was a printer "dn" given, or the "cn" */
        foreach($this->member[$type] as $key => $printer){
            if($printer['dn'] == $id) {
                $id = $key;
            }
        }

        if(!$this->acl_is_writeable("gotoUserPrinter")){
            msg_dialog::display(_("Permission error"), msgPool::permDelete(_("printer user"), $id), INFO_DIALOG);
            return(FALSE);
        }

        if(isset($this->member[$type][$id])){
            unset($this->member[$type][$id]);
            return(TRUE);
        }
        return(FALSE);
    }


    /* Add given obejct to members */
    function AddMember($type,$dn)
    {
        $types = array("AddUser","AddGroup","AddAdminUser","AddAdminGroup");
        if(!in_array_ics($type, $types)){
            msg_dialog::display(_("Internal error"), sprintf(_("Illegal member type '%s'!"), $type), ERROR_DIALOG);
            return(FALSE);
        }

        if(!$this->acl_is_writeable("gotoUserPrinter")){
            msg_dialog::display(_("Permission error"), msgPool::permModify(_("printer user"), $this->dn), INFO_DIALOG);
            return(FALSE);
        }

        /* Get name of index attributes */
        if(preg_match("/user/i",$type)){
            $var = "uid";
        }else{
            $var = "cn";
        }

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($dn);
        $ldap->cat($dn,array($var,"cn"));
        if($ldap->count()){

            $attrs = $ldap->fetch();

            if(isset($attrs[$var][0])){
                $name = $attrs[$var][0];

                /* Check if this uid/cn is already assigned to any permission */
                foreach($types as $ctype){

                    /* If we want to add a user, only check user/userAdmin members */
                    if((preg_match("/user/i",$type)) && (!preg_match("/user/i",$ctype))){
                        continue;
                    }

                    /* If we want to add a group, only check groups/adminGroups .. */
                    if((preg_match("/group/i",$type)) && (!preg_match("/group/i",$ctype))){
                        continue;
                    }

                    if(isset(  $this->member[$ctype][$name])){
                        msg_dialog::display(_("Error"), sprintf(_("'%s' is already used!"), $attrs[$var][0]), ERROR_DIALOG);
                        return(FALSE);
                    }
                }

                /* Everything is fine. So add the given object to members */
                $this->member[$type][$attrs[$var][0]] = $attrs ;
            }else{
                print_a($attrs);
            }
        }else{
            msg_dialog::display(_("Error"), sprintf(_("'%s' does not exist!"), $dn), ERROR_DIALOG);
            return(FALSE);
        }
        return(TRUE);
    }


    /* Display generic part for server copy & paste */
    function getCopyDialog()
    {
        $vars = array("cn");
        $smarty = get_smarty();
        $smarty->assign("cn" ,$this->cn);
        $smarty->assign("object","printer");
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    function saveCopyDialog()
    {
        if(isset($_POST['cn'])){
            $this->cn = get_post('cn');
        }
    }

    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);
        if(isset($source['macAddress'][0])){
            $this->netConfigDNS->macAddress = $source['macAddress'][0];
        }
        if(isset($source['ipHostNumber'][0])){
            $this->netConfigDNS->ipHostNumber = $source['ipHostNumber'][0];
        }

        $source_o = new printgeneric($this->config,$source['dn'],NULL,$this->parent);
        foreach($this->attributes as $attr){
            $this->$attr = $source_o->$attr;
        }
        $this->member = $source_o -> member;

        $this->gotoPrinterPPD = "";
    }


    function is_modal_dialog()
    {
        return((isset($this->dialog) && $this->dialog) || (isset($this->netConfigDNS->dialog) && $this->netConfigDNS->dialog));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
