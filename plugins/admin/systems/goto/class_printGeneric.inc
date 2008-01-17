<?php

class printgeneric extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage terminal base objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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
  var $netConfigDNS;
  var $baseSelection    = false;
  var $macAddress       = "";

  var $gotoUserAdminPrinter;
  var $gotoGroupAdminPrinter ;
  var $gotoGroupPrinter;
  var $gotoUserPrinter ;

  /* attribute list for save action */
  var $attributes     = array("cn", "description", "l", "labeledURI", "gotoPrinterPPD","gotoUserPrinter", "macAddress", 
                              "gotoUserAdminPrinter","gotoGroupAdminPrinter","gotoUserPrinter","gotoGroupPrinter");
  var $objectclasses  = array("top", "gotoPrinter");
  var $view_logged    = FALSE;
  var $parent;

  function printgeneric (&$config, $dn,$parent_init,$parent)
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
      $this->dn= preg_replace("/".normalizePreg(get_ou('terminalou'))."/",get_ou('printerou'),$this->dn);
    }

    if(preg_match("/Workstation/i",$this->BelongsTo) || preg_match("/WorkstationTemplate/i",$this->BelongsTo)){
      $this->dn= preg_replace("/".normalizePreg(get_ou('workstationou'))."/",get_ou('printerou'),$this->dn);
    }

    $this->orig_dn = $this->dn;

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
    $this->netConfigDNS = new termDNS($this->config, $this,$this->objectclasses);

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
    
      /* Set base and check if the extracted base exists */
      if(preg_match("/".normalizePreg(get_ou('incomingou'))."/",$this->dn)){
        $this->base= preg_replace("/".normalizePreg(get_ou('incomingou'))."/","",dn2base($this->dn));
      }else{
        $this->base= preg_replace("/".normalizePreg(get_ou('printerou'))."/","",dn2base($this->dn));
      }

      if(!isset($this->config->idepartments[$this->base])){
        print_red(_("Can't extract a valid base out of object dn, setting base to '%s'."),session::get('CurrentMainBase'));
        $this->base  = session::get('CurrentMainBase');
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

      if(isset($this->attrs[$attr])){
        foreach($this->attrs[$attr] as $mem){
          if(preg_match("/Group/",$type)){
            $ldap->search("(&(objectClass=posixGroup)(cn=".$mem."))",array("cn","description"));
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
        $this->dn= preg_replace("/".normalizePreg(get_ou('terminalou'))."/",get_ou('printerou'),$this->dn);
      }

      if(preg_match("/workstation/i",$this->BelongsTo)){
        $this->dn= preg_replace("/".normalizePreg(get_ou('workstationou'))."/",get_ou('printerou'),$this->dn);
      }

      /* Detect if this is a valid printer account;
       */
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
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no printer features.")."</b>";
      return($display);
    }

    /* If this is a WS / Terminal depending printer, display account state button */
    if(!preg_match("/^Printer$/i",$this->BelongsTo)){
      if((empty($this->cn)) && ($this->dn != "new")){
        $display= $this->show_enable_header(_("Add printer extension"),
            _("This object has printer extension disabled. You can't enable it while 'cn' is not present in entry. Possibly you are currently creating a new terminal template"),TRUE,TRUE);
        $this->is_account= false;
        return $display;
      }

      if (($this->is_account)){
        if(preg_match("/^Workstation$/i",$this->BelongsTo)){
          $display= $this->show_disable_header(_("Remove printer extension"),
              _("This workstation has printer extension enabled.You can disable it by clicking below."));
        }elseif(preg_match("/^Terminal$/i",$this->BelongsTo)){
          $display= $this->show_disable_header(_("Remove printer extension"),
              _("This terminal has printer extension enabled. You can disable it by clicking below."));
        }
      }else{
        if(preg_match("/^Workstation$/i",$this->BelongsTo)){
          $display= $this->show_disable_header(_("Add printer extension"),
              _("This workstation has printer extension disabled. You can enable it by clicking below."));
        }elseif(preg_match("/^Terminal$/i",$this->BelongsTo)){
          $display= $this->show_enable_header(_("Add printer extension"),
              _("This terminal has printer extension disabled. You can enable it by clicking below."));
        }  
        return ($display);
      }
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once && $this->acl_is_moveable()){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
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

          /* A new base was selected, check if it is a valid one */
          $tmp = $this->get_allowed_bases();
          if(isset($tmp[$this->dialog->isSelected()])){
            $this->base = $this->dialog->isSelected();
          }

          $this->dialog= false;
          $this->baseSelection = false;
        }else{
          return($this->dialog->execute());
        }
      }
    }

    /* Fill templating stuff */
    $smarty->assign("bases", $this->get_allowed_bases());
    $smarty->assign("base_select", $this->base);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
    }

    if(isset($_POST['AddUser'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddUser");
    }
    if(isset($_POST['AddGroup'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddGroup");
    }
    if(isset($_POST['AddAdminUser'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddAdminUser");
    }
    if(isset($_POST['AddAdminGroup'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, NULL,"AddAdminGroup");
    }

    /* Display ppd configure/select dialog      */
    if(isset($_POST['EditDriver'])){
      if($this->PPDdialogToSave && is_object($this->PPDdialogToSave)){
        $this->dialog = $this->PPDdialogToSave;
      }else{
        $this->dialog = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
        $this->dialog->cn= $this->cn;
      }
    }

    /* remove ppd */
    if(isset($_POST['RemoveDriver'])){
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
          print_red($msg);
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

    /* Abort user / group adding dialog */
    if(isset($_POST['PrinterCancel'])){
      unset($this->dialog);
      $this->dialog= FALSE;
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
        $this->dialog=FALSE;
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
    if((isset($config->data['MAIN']['PPD_PATH']))&&(is_dir($config->data['MAIN']['PPD_PATH']))){

      $path = $config->data['MAIN']['PPD_PATH'];
      if(!preg_match("/\/$/",$path)){
        $path = $path."/";
      }

      $ppdManager= new ppdManager($path);
      if(!empty($this->gotoPrinterPPD)){
        if((!file_exists($path.$this->gotoPrinterPPD))){
          $smarty->assign("driverInfo", "<b>".sprintf(_("Your currently selected PPD file '%s' doesn't exist."),$path.$this->gotoPrinterPPD)."</b>");
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

    if(!preg_match("/Printer/i",$this->BelongsTo)){
      if(preg_match("/Terminal/i",$this->BelongsTo)){
        $smarty->assign("desc"    ,sprintf(_("This printer belongs to terminal %s. You can't rename this printer."),"<b>".$this->cn."</b>"));
      }else{
        $smarty->assign("desc"    ,sprintf(_("This printer belongs to workstation %s. You can't rename this printer."),"<b>".$this->cn."</b>"));
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

    return($display.$smarty->fetch (get_template_path('printer.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* Only remove if there was initially an account */
    if($this->initially_was_account){

      /* Update dn, to ensure storing as printer instead of WS / terminal
       */
      if(preg_match("/terminal/i",$this->BelongsTo)){
        $this->dn= preg_replace("/".normalizePreg(get_ou('terminalou')).",/",get_ou('printerou'),$this->dn);
      }

      if(preg_match("/workstation/i",$this->BelongsTo)){
        $this->dn= preg_replace("/".normalizePreg(get_ou('workstationou'))."/",get_ou('printerou'),$this->dn);
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

      new log("remove","printer/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
  
      show_ldap_error($ldap->get_error(), sprintf(_("Removing of system print/generic with dn '%s' failed."),$this->dn));
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

    if(is_object($this->netConfigDNS)){
      $this->netConfigDNS->save_object();
    }
    
    /* Set new base if allowed */
    $tmp = $this->get_allowed_bases();
    if(isset($_POST['base'])){
      if(isset($tmp[$_POST['base']])){
        $this->base= $_POST['base'];
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

    $dn= "cn=".$this->cn.get_ou('printerou').",".$this->base;

    /* must: cn */
    if(($this->BelongsTo == "Printer") && (empty($this->cn))){
      $message[]= _("The required field 'Printer name' is not set.");
    }

    /* must: cn */
    if(($this->BelongsTo == "Printer") && !tests::is_dns_name($this->cn)){
      $message[]= _("Invalid character in printer name.");
    }

    /* must: labeledURI */
    if(empty($this->labeledURI)){
      $message[]= "The required field 'Printer URL' is not set.";
    }
    
    /* Check if there is already an entry with this cn*/
    if (($this->orig_dn != $dn)&&( preg_match("/printer/i",$this->BelongsTo))){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->ls("(cn=".$this->cn.")",get_ou('printerou').$this->base, array("cn"));
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
    /* Update dn, to ensure storing as printer instead of WS / terminal
     */
    if(preg_match("/terminal/i",$this->BelongsTo)){
      $this->dn= preg_replace("/".normalizePreg(get_ou('terminalou')).",/",get_ou('printerou'),$this->dn);
    }

    if(preg_match("/workstation/i",$this->BelongsTo)){
      $this->dn= preg_replace("/".normalizePreg(get_ou('workstationou'))."/",get_ou('printerou'),$this->dn);
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
    if(preg_match("/unknown/i",$this->BelongsTo)){
      return;
    }

    /* save ppd configuration */
    if($this->PPDdialogToSave && is_object($this->PPDdialogToSave)){
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
    }else{
      if(($this->gosaUnitTag) && (!in_array_ics("gosaAdministrativeUnitTag",$this->attrs['objectClass']))){
        $this->attrs['objectClass'][] = "gosaAdministrativeUnitTag";
      }
    }

    /* Ensure to create a new object */
    if(preg_match("/".normalizePreg(get_ou('incomingou'))."/",$this->orig_dn)){
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
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system print/generic with dn '%s' failed."),$this->dn));

    if(preg_match("/printer/i",$this->BelongsTo)){
      $this->netConfigDNS->cn = $this->cn;
      $this->netConfigDNS->dn = $this->dn;
      $this->netConfigDNS->save();
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
          "plCategory"    => array("printer" => array("description"  => _("Printer"),
                                                    "objectClass"  => "gotoPrinter"),"workstation","terminal"),
          "plProvidedAcls"=> array(
            "cn"                => _("Name"),
            "base"                => _("Base") ,         
            "description"       => _("Description"), 
            "l"                 => _("Location"), 
            "labeledURI"        => _("LabeledURL"), 
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
      print_red(sprintf(_("You are not allowed to remove the given object '%s' from the list of members of printer '%s'."),$id,$this->dn));
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
      print_red(sprintf(_("Illegal printer type while adding '%s' to the list of '%s' printers,"),$dn,$type));
      return(FALSE);
    }

    if(!$this->acl_is_writeable("gotoUserPrinter")){
      print_red(sprintf(_("You are not allowed to add the given object '%s' to the list of members of '%s'."),$dn,$this->dn));
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
            print_red(sprintf(_("Can't add '%s' to the list of members, it is already used."),$attrs[$var][0]));
            return(FALSE);
          }
        }

        /* Everything is fine. So add the given object to members */
        $this->member[$type][$attrs[$var][0]] = $attrs ;
      }else{
        print_a($attrs);
      }
    }else{
      print_red(sprintf(_("Can't add '%s' to list of members, it is not reachable."),$dn));
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
      $this->cn = $_POST['cn'];
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
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
