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

  var $UserMember       ="";
  var $UserMembers      =array();
  var $UserMemberKeys   =array();

  var $AdminMember      ="";
  var $AdminMembers     =array();
  var $AdminMemberKeys  =array();

  var $PPDdialogToSave  = NULL;
  var $BelongsTo        = "unknown"; //  Specifies if this is a standalone printer, or belongs to a terminal / WS

  var $member           =array();
  var $strings          = "";
  var $dialog           =NULL;

  var $netConfigDNS;
  var $baseSelection    = false;
  var $macAddress       = "";

  /* attribute list for save action */
  var $attributes     = array("cn", "description", "l", "labeledURI", "gotoPrinterPPD","gotoUserPrinter", "macAddress",
                                "gotoUserAdminPrinter","gotoGroupAdminPrinter","gotoUserPrinter","gotoGroupPrinter");
  var $objectclasses  = array("top", "gotoPrinter");

    var $gotoUserAdminPrinter;
  var $gotoGroupAdminPrinter ;
  var $gotoGroupPrinter;
  var $gotoUserPrinter ;

  function printgeneric ($config, $dn= NULL,$parent = NULL)
  {
    $this->config = $config;
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
    if($this->BelongsTo == "Terminal" || $this->BelongsTo == "TerminalTemplate"){
      $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
    }

    if($this->BelongsTo == "Workstation" || $this->BelongsTo == "WorkstationTemplate"){
      $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
    }

    $this->orig_dn = $this->dn;
    $ui= get_userinfo();

    /* Get printer settings, possibly dn has changed */
    plugin::plugin ($config, $this->dn);

    /* Get is_account initially_was_account status */
    $this->getTypeOfPrinter(true);

    /* set orig dn to new if object is new */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn, array('dn'));
    if(!$ldap->count()){
      $this->orig_dn = "new";
    }
    
    /* create dns object */
    $this->netConfigDNS = new termDNS($this->config, $this->dn,$this->objectclasses);
    $this->netConfigDNS->acl = $this->acl;

    /* Set base */
    if ($this->dn == "new"){
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
    
      /* Set base and check if the extracted base exists */
      if(preg_match("/ou=incoming,/",$this->dn)){
        $this->base= preg_replace("/ou=incoming,/","",dn2base($this->dn));
      }else{
        $this->base= preg_replace("/ou=printers,ou=systems,/","",dn2base($this->dn));
      }

      if(!isset($this->config->idepartments[$this->base])){
        print_red(_("Can't extract a valid base out of object dn, setting base to '%s'."),$_SESSION['CurrentMainBase']);
        $this->base = $_SESSION['CurrentMainBase'];
      }
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

      if (isset($this->attrs[$attr]['count'])) {
        unset($this->attrs[$attr]['count']);
      }

      /* Set tag attribute if we've tagging activated */
      $tag= "";
      if ($ui->gosaUnitTag != "" && isset($config->current['STRICT_UNITS']) &&
          preg_match('/TRUE/i', $config->current['STRICT_UNITS'])){
        $tag= "(gosaUnitTag=".$ui->gosaUnitTag.")";
      }

      if(isset($this->attrs[$attr])){
        foreach($this->attrs[$attr] as $mem){
          if(preg_match("/Group/",$type)){
            $ldap->search("(&(objectClass=posixGroup)$tag(cn=".$mem."))",array("cn","description"));
            if($ldap->count()){
              $entry = $ldap->fetch();
              if(isset($entry['cn'])){
                $this->member[$type][$entry['cn'][0]]=$entry;
              }
            }
          }else{
            $ldap->search("(&(objectClass=person)$tag(objectClass=inetOrgPerson)(uid=".$mem."))",array("cn","uid"));
            if($ldap->count()){
              $entry = $ldap->fetch();
              if(isset($entry['uid'])){
                $this->member[$type][$entry['uid'][0]]=$entry;
              }
            }
          }
        }
      }
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
      if($this->BelongsTo == "Printer"){
        $this->is_account = true;
      }

      /* Update dn, to ensure storing as printer instead of WS / terminal
       */
      if($this->BelongsTo == "Terminal"){
        $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
      }

      if($this->BelongsTo == "Workstation"){
        $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
      }

      /* Detect if this is a valid printer account;
       */
      //FIXME: do we need to do this? we've already everything we need in $this->attrs...
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('objectClass'));

      if($ldap->count()){
        $attrs = $ldap->fetch();
        if(in_array("gotoPrinter",$attrs['objectClass'])){
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
    $this->netConfigDNS->acl = $this->acl;

    /* Call parent execute */
    plugin::execute();

    /* If type of printer couldn't be detected (because of missing parent object in construction) 
     * hide this tab.
     */
    if($this->BelongsTo == "unknown"){
      $display= $this->show_header(_("Add printer extension"),
          _("Could not initialize printer tab, parameter parent was missing while construction."),TRUE,TRUE);
      return($display);
    }

    /* Templates can't have printer extensions 
     */
    if($this->BelongsTo == "WorkstationTemplate"){
      $display= $this->show_header(_("Add printer extension"),
          _("This is a workstation template, printer tab is disabled."),TRUE,TRUE);
      return($display);
    }
    if($this->BelongsTo == "TerminalTemplate"){
      $display= $this->show_header(_("Add printer extension"),
          _("This is a terminal template, printer tab is disabled."),TRUE,TRUE);
      return($display);
    }

    /* Get cn from base object */
    if($this->BelongsTo == "Workstation"){
      $this->cn = $this->parent->by_object['workgeneric']->cn;
    }
    if($this->BelongsTo == "Terminal"){
      $this->cn = $this->parent->by_object['termgeneric']->cn;
    }

    $smarty= get_smarty();
    $display="";

    /* Tell smarty if this is a standalone object or a terminal / WS depending printer */
    if($this->BelongsTo == "Printer"){    
      $smarty->assign("StandAlone",true);
    }else{
      $smarty->assign("StandAlone",false);
    }


    if (isset($_POST['modify_state'])){

      /* Only change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""  && !((empty($this->cn)) && ($this->dn != "new"))){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }
    }

    /* Do we represent a valid printer? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no printer features.")."</b>";
      return($display);
    }

    /* If this is a WS / Terminal depending printer, display account state button */
    if($this->BelongsTo != "Printer"){
      if((empty($this->cn)) && ($this->dn != "new")){
        $display= $this->show_header(_("Add printer extension"),
            _("This object has printer extension disabled. You can't enable it while 'cn' is not present in entry. Possibly you are currently creating a new terminal template"),TRUE,TRUE);
        $this->is_account= false;
        return $display;
      }

      if (($this->is_account)){
        if($this->BelongsTo=="Workstation"){
          $display= $this->show_header(_("Remove printer extension"),
              _("This workstation has printer extension enabled.You can disable it by clicking below."));
        }elseif($this->BelongsTo=="Terminal"){
          $display= $this->show_header(_("Remove printer extension"),
              _("This terminal has printer extension enabled. You can disable it by clicking below."));
        }
      }else{
        if($this->BelongsTo=="Workstation"){
          $display= $this->show_header(_("Add printer extension"),
              _("This workstation has printer extension disabled. You can enable it by clicking below."));
        }elseif($this->BelongsTo=="Terminal"){
          $display= $this->show_header(_("Add printer extension"),
              _("This terminal has printer extension disabled. You can enable it by clicking below."));
        }  
        return ($display);
      }
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once && chkacl($this->acl,"base")){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
        $this->dialog->setCurrentBase($this->base);
        $this->baseSelection = true;
      }
    }

    /* Dialog handling for base select dialog
     * Check if base was selected, dialog aborted etc */
    if(is_object($this->dialog)){

      $this->dialog->save_object();
      if($this->baseSelection){
        if($this->dialog->isClosed()){
          $this->dialog = false;
          $this->baseSelection = false;
        }elseif($this->dialog->isSelected()){
          $this->base = $this->dialog->isSelected();
          $this->dialog= false;
          $this->baseSelection = false;
        }else{
          return($this->dialog->execute());
        }
      }
    }

    /* Fill templating stuff */
    $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("base_select", $this->base);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->acl, $attr));
      $smarty->assign("$attr", $this->$attr);
    }

    $smarty->assign("baseACL", chkacl($this->acl,"base"));

    if(isset($_POST['AddUser']) && chkacl($this->acl,"gotoUserPrinter") == ""){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddUser");
    }
    if(isset($_POST['AddGroup']) && chkacl($this->acl,"gotoGroupPrinter") == ""){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddGroup");
    }
    if(isset($_POST['AddAdminUser']) && chkacl($this->acl,"gotoUserAdminPrinter") == ""){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddAdminUser");
    }
    if(isset($_POST['AddAdminGroup']) && chkacl($this->acl,"gotoGroupAdminPrinter") == ""){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddAdminGroup");
    }

    /* Display ppd configure/select dialog      */
    if(isset($_POST['EditDriver']) && chkacl($this->acl,"gotoPrinterPPD")==""){
      if($this->PPDdialogToSave){
        $this->dialog = $this->PPDdialogToSave;
      }else{
        $this->dialog = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
        $this->dialog->cn= $this->cn;
      }
    }

    /* remove ppd */
    if(isset($_POST['RemoveDriver']) && chkacl($this->acl,"gotoPrinterPPD")==""){
      $this->gotoPrinterPPD = array();
      $this->PPDdialogToSave = NULL;
    }

    /* Close ppd dialog */
    if(isset($_POST['ClosePPD'])){
      unset($this->dialog);
      $this->dialog=NULL;
    }

    /* Save selected ppd */
    if(isset($_POST['SavePPD'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $this->gotoPrinterPPD = array();
        $this->gotoPrinterPPD = $this->dialog->save();
        $this->PPDdialogToSave = $this->dialog;
        unset($this->dialog);
        $this->dialog=NULL;
      }
    }

    /* Member management, delete user / group / admin ..*/
    if((isset($_POST['DelUser']))&&(isset($_POST['UserMember'])) && chkacl($this->acl,"gotoUserPrinter")==""){
      foreach($_POST['UserMember'] as $mem){
        $this->DelMember('AddUser',$mem);
      }
    }

    if((isset($_POST['DelUser']))&&(isset($_POST['UserMember'])) && chkacl($this->acl,"gotoGroupPrinter")==""){
      foreach($_POST['UserMember'] as $mem){
        $this->DelMember('AddGroup',$mem);
      }
    }

    if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember'])) && chkacl($this->acl,"gotoUserAdminPrinter")==""){
      foreach($_POST['AdminMember'] as $mem){
        $this->DelMember('AddAdminUser',$mem);
      }
    }

    if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember'])) && chkacl($this->acl,"gotoGroupAdminPrinter")==""){
      foreach($_POST['AdminMember'] as $mem){
        $this->DelMember('AddAdminGroup',$mem);
      }
    }

    /* Abort user / group adding dialog */
    if(isset($_POST['PrinterCancel'])){
      unset($this->dialog);
      $this->dialog= NULL;
    }

    /* Save selected users / groups */
    if(isset($_POST['PrinterSave'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $data= $new = $this->dialog->save();
        unset($data['type']);
        foreach($data as $mem){  
          $this->AddMember($new['type'], $mem['dn']);    
        }
        unset($this->dialog);
        $this->dialog=NULL; 
      }
    }

    /* Display dialog, if there is currently one open*/
    if($this->dialog != NULL){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return $display;
    }

    /* Parse selected ppd file */
    require_once ("class_ppdManager.inc");
    if((isset($_SESSION['config']->data['MAIN']['PPD_PATH']))&&(is_dir($_SESSION['config']->data['MAIN']['PPD_PATH']))){

      $path = $_SESSION['config']->data['MAIN']['PPD_PATH'];
      if(!preg_match("/\/$/",$path)){
        $path = $path."/";
      }

      $ppdManager= new ppdManager($path);
      if(!empty($this->gotoPrinterPPD)){
        if((!file_exists($path.$this->gotoPrinterPPD))){
          $smarty->assign("driverInfo", "<b>".sprintf(_("Your currently selected PPD file '%s' doesn't exist."),$path.$this->gotoPrinterPPD))."</b>";
        }else{
          $smarty->assign("driverInfo", $ppdManager->loadDescription($path.$this->gotoPrinterPPD));
        }
      }else{
        $smarty->assign("driverInfo", _("Not defined"));
      }
    }else{
      $smarty->assign("driverInfo",_("Can't get ppd informations."));
    }

    /* Create user & admin user list */
    $list=$this->generateList();
    $userlist   = array_merge($list['AddUser'],$list['AddGroup']);
    $adminlist  = array_merge($list['AddAdminUser'],$list['AddAdminGroup']);

    asort($userlist);
    asort($adminlist);

    if($this->BelongsTo != "Printer"){
      if($this->BelongsTo == "Terminal"){
        $smarty->assign("desc"    ,sprintf(_("This printer belongs to terminal %s. You can't rename this printer."),"<b>".$this->cn."</b>"));
      }else{
        $smarty->assign("desc"    ,sprintf(_("This printer belongs to workstation %s. You can't rename this printer."),"<b>".$this->cn."</b>"));
      }
      $smarty->assign("cnACL"    ," disabled ");
    }else{
      $smarty->assign("desc"    ,"");
    }
    $smarty->assign("UserMember"    ,$this->UserMember);
    $smarty->assign("UserMembers"   ,$userlist);
    $smarty->assign("UserMemberKeys",array_flip($userlist));

    $smarty->assign("AdminMember"    ,$this->AdminMember);
    $smarty->assign("AdminMembers"   ,$adminlist);
    $smarty->assign("AdminMemberKeys",array_flip($adminlist));
    if($this->BelongsTo == "Printer"){
      $this->netConfigDNS->cn= $this->cn;
      $smarty->assign("netconfig", $this->netConfigDNS->execute());

      /* Display sub dialog from network settings */
      if($this->netConfigDNS->dialog){
        $this->dialog = TRUE;
        return($this->netConfigDNS->execute());
      }else{ 
        $this->dialog = FALSE;
      }
    } else {
      $smarty->assign("netconfig", "");
    }


    return($display.$smarty->fetch (get_template_path('printer.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $this->netConfigDNS->acl = $this->acl;

    /* Only remove if there was initially an account */
    if($this->initially_was_account){

      /* Update dn, to ensure storing as printer instead of WS / terminal
       */
      if($this->BelongsTo == "Terminal"){
        $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
      }

      if($this->BelongsTo == "Workstation"){
        $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
      }

      /* Check if this dn points to a printer, to avoid deleting something else */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('dn',"objectClass"));
      if(!$ldap->count()){
        print_red("Trying to remove printer object which isn't a printer. Aborted to avoid data loss.");
        return;
      }

      /* Check if obejct is a printer */
      $CheckPrinter = $ldap->fetch();
      if(!in_array("gotoPrinter",$CheckPrinter['objectClass'])){
        print_red("Trying to remove printer object which isn't a printer. Aborted to avoid data loss.");
        return;
      }

      /* Remove account & dns extension */ 
      $this->netConfigDNS->remove_from_parent();
      $ldap->rmdir($this->dn);
      show_ldap_error($ldap->get_error(), _("Removing printer failed"));
      $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

      /* Delete references to object groups */
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".@LDAP::prepare4filter($this->dn)."))", array("cn"));
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
    plugin::save_object();
    $this->netConfigDNS->save_object();
    /* Save base, since this is no LDAP attribute */
    if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
      $this->base= $_POST['base'];
    }
  }

  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    if ($this->BelongsTo == 'printer'){
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

    $dn= "cn=".$this->cn.",ou=printers,ou=systems,".$this->base;

    /* must: cn */
    if(($this->BelongsTo == "Printer") && (empty($this->cn))){
      $message[]= _("The required field 'Printer name' is not set.");
    }

    /* must: cn */
    if(($this->BelongsTo == "Printer") && !is_dns_name($this->cn)){
      $message[]= _("Invalid character in printer name.");
    }

    if($this->BelongsTo == "Printer"){
      $ui= get_userinfo();
      $acl= get_permissions ($dn, $ui->subtreeACL);
      $acl= get_module_permission($acl, "printgeneric", $dn);
      if (chkacl($acl, "create") != ""){
        $message[]= _("You have no permissions to create a printer on this 'Base'.");
      }
    }

    
    /* must: labeledURI */
    if(empty($this->labeledURI)){
      $message[]= "The required field 'Printer URL' is not set.";
    }
    
    /* Check if there is already an entry with this cn*/
    if (($this->orig_dn != $dn)&&($this->BelongsTo == "Printer")){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->ls("(cn=".$this->cn.")","ou=printers,ou=systems,".$this->base, array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if(preg_match("/cn=dhcp,/",$attrs['dn'])){
            continue;
          }
          if ($attrs['dn'] != $this->orig_dn){
            $message[]= sprintf (_("There is already an entry '%s' in the base choosen by you"), $this->cn);
            break;
          }
        }
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    $this->netConfigDNS->acl = $this->acl;

    /* Update dn, to ensure storing as printer instead of WS / terminal
     */
    if($this->BelongsTo == "Terminal"){
      $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
    }

    if($this->BelongsTo == "Workstation"){
      $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
      $this->macAddress = $this->parent->by_object['workgeneric']->netConfigDNS->macAddress;
    }
    
    if(!$this->is_account) return;
    if(isset($this->parent->by_object['workgeneric'])){
      if($this->parent->by_object['workgeneric']->cn == "wdefault"){
        return;
      }
    }elseif(isset($this->parent->by_object['termgeneric'])){
      if($this->parent->by_object['termgeneric']->cn == "default"){
        return;
      }
    }

    /* If type is still unknown, the initialisation of this printer failed, abort. */
    if($this->BelongsTo == "unknown"){
      return;
    }

    /* save ppd configuration */
    if($this->PPDdialogToSave){
      $this->PPDdialogToSave->save_ppd();
    }
    if($this->orig_dn != $this->dn){
      if(!empty($this->gotoPrinterPPD)) {
        $this->PPDdialogToSave = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
        $this->PPDdialogToSave->cn = $this->cn;
        $this->PPDdialogToSave->generateProperties();
        $this->PPDdialogToSave->update_ppd_url();
      }
    }

    /* Remove previously selected ppd file.*/
    if($this->initial_PPD != $this->gotoPrinterPPD){
      if(!empty($this->initial_PPD)){
        $tmp = new printerPPDDialog($this->config, $this->dn,$this->initial_PPD);
        $tmp->removeModifiedPPD();
      }
    }

    if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
      $method="https://";
    }else{
      $method="http://";
    }

    /* If no ppd is selected, remove this attribute */
    if(!empty($this->gotoPrinterPPD)) {
      $this->gotoPrinterPPD = $method.str_replace("//","/",$_SERVER['SERVER_NAME']."/ppd/".$this->gotoPrinterPPD);
    }else{
      $this->gotoPrinterPPD = array();
    }

    $dn= $this->dn;
    plugin::save();
    $ldap= $this->config->get_ldap_link();

    /* reduce objectClasses to minimun */
    $this->attrs['objectClass']= $this->objectclasses;

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
    $this->attrs['gotoUserAdminPrinter'] =array();
    foreach($this->member['AddAdminUser'] as $mem){
      $this->attrs['gotoUserAdminPrinter'][]=$mem['uid'][0];
    }

    /* Append printer admin group 
     */
    $this->attrs['gotoGroupAdminPrinter'] = array();
    foreach($this->member['AddAdminGroup'] as $mem){
      $this->attrs['gotoGroupAdminPrinter'][]=$mem['cn'][0];
    }

    if(($this->gosaUnitTag) && (!in_array_ics("gosaAdministrativeUnitTag",$this->attrs['objectClass']))){
      $this->attrs['objectClass'][] = "gosaAdministrativeUnitTag";
      $this->attrs['gosaUnitTag'] = $this->gosaUnitTag;
    }

    if($this->orig_dn == 'new'){
      foreach(array("gotoGroupPrinter","gotoUserAdminPrinter","gotoGroupAdminPrinter","gotoUserPrinter") as $checkVar){
        if(count($this->attrs[$checkVar])  == 0 || empty($this->attrs[$checkVar])){
          unset($this->attrs[$checkVar]);
        }
      }
    }

    /* Ensure to create a new object */
    if(preg_match("/ou=incoming,/",$this->orig_dn)){
      $this->orig_dn = "new";
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
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
    } else {
      if ($this->dn != $this->orig_dn){
        $this->move($this->orig_dn, $this->dn);
      }

      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }
    show_ldap_error($ldap->get_error(), _("Saving printer failed"));

    /* Only save dns stuff if we are a standalone printer */
    if($this->BelongsTo == "Printer"){
      $this->netConfigDNS->cn = $this->cn;
      $this->netConfigDNS->dn = $this->dn;
      $this->netConfigDNS->save($this->dn);
    }
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

  /* Delete member */
  function DelMember($type,$id)
  {
    if(isset($this->member[$type][$id])){
      unset($this->member[$type][$id]);
    }
  }

  /* Add given obejct to members */
  function AddMember($type,$dn)
  {
    $types = array("AddUser","AddGroup","AddAdminUser","AddAdminGroup");
    if(!in_array_ics($type, $types)){
      print_red(sprintf(_("Illegal printer type while adding '%s' to the list of '%s' printers,"),$dn,$type));
      return;
    }

    /* Get name of index attributes */
    if(preg_match("/user/i",$type)){
      $var = "uid";
    }else{
      $var = "cn";
    }

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($dn);
    $ldap->cat($dn,array("cn","uid"));
    if($ldap->count()){

      $attrs = $ldap->fetch();
      $name = $attrs[$var][0];

      /* Check if this uid/cn is already assigned to any permission */
      foreach($types as $ctype){
        if(isset(  $this->member[$ctype][$name])){
          print_red(sprintf(_("Can't add '%s' to the list of members, it is already used."),$attrs[$var][0]));
          return;
        }
      }

      /* Everything is fine. So add the given object to members */
      $this->member[$type][$attrs[$var][0]] = $attrs ;
    }else{
      print_red(sprintf(_("Can't add '%s' to list of members, it is not reachable."),$dn));
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
