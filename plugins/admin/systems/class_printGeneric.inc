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
  var $orig_dn          = "";

  var $UserMember       ="";
  var $UserMembers      =array();
  var $UserMemberKeys   =array();

  var $AdminMember      ="";
  var $AdminMembers     =array();
  var $AdminMemberKeys  =array();

  var $gotoUserPrinter ;
  var $PPDdialogToSave  = NULL;
  var $BelongsTo        = "unknown"; //  Specifies if this is a standalone printer, or belongs to a terminal / WS

  var $member           =array();
  var $strings          = "";
  var $dialog           =NULL;

  var $netConfigDNS;
  var $baseSelection    = false;
  var $macAddress       = "";

  /* attribute list for save action */
  var $attributes     = array("cn", "description", "l", "labeledURI", "gotoPrinterPPD","gotoUserPrinter", "macAddress");
  var $objectclasses  = array("top", "gotoPrinter");

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
    if($this->BelongsTo == "Terminal"){
      $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
    }

    if($this->BelongsTo == "Workstation"){
      $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
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
    $this->netConfigDNS = new termDNS($this->config, $this->dn,$this->objectclasses);

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
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
            $entry = $ldap->fetch();
            $this->member[$type][$entry['cn'][0]]=$entry;
          }else{
            $ldap->search("(&(objectClass=person)(objectClass=inetOrgPerson)(uid=".$mem."))",array("cn","uid"));
            $entry = $ldap->fetch();
            if(isset($entry['uid'])){
              $this->member[$type][$entry['uid'][0]]=$entry;
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
    /* Call parent execute */
    plugin::execute();

    /* If type of printer couldn't be detected (because of missing parent object in construction) 
     * hide this tab.
     */
    if($this->BelongsTo == "unknown"){
      $display= $this->show_header(_("Add printer extension"),
          _("Could not intialize printer tab, parameter parent was missing while construction."),TRUE,TRUE);
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

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_modified = true;
      $this->is_account= !$this->is_account;
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
      if(preg_match("/^chooseBase/",$name) && $once){
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
      if($this->PPDdialogToSave){
        $this->dialog = $this->PPDdialogToSave;
      }else{
        $this->dialog = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
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
    if((isset($_POST['DelUser']))&&(isset($_POST['UserMember']))){
      if(isset($this->member['AddUser'][$_POST['UserMember']])){
        unset($this->member['AddUser'][$_POST['UserMember']]);
      }
    }

    if((isset($_POST['DelUser']))&&(isset($_POST['UserMember']))){
      if(isset($this->member['AddGroup'][$_POST['UserMember']])){
        unset($this->member['AddGroup'][$_POST['UserMember']]);
      }
    }

    if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember']))){
      if(isset($this->member['AddAdminUser'][$_POST['AdminMember']])){
        unset($this->member['AddAdminUser'][$_POST['AdminMember']]);
      }
    }

    if((isset($_POST['DelAdmin']))&&(isset($_POST['AdminMember']))){
      if(isset($this->member['AddAdminGroup'][$_POST['AdminMember']])){
        unset($this->member['AddAdminGroup'][$_POST['AdminMember']]);
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
        $new = $this->dialog->save();
        $data = $new;
        unset($data['type']);

        if(preg_match("/User/",$new['type'])){
          $use = "uid";
        }else{
          $use = "cn";
        }

        foreach($data as $mem){
          $this->member[$new['type']][$mem[$use][0]]=$mem;    
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
        $smarty->assign("driverInfo", _("not defined"));
      }
    }else{
      $smarty->assign("driverInfo",_("can't get ppd informations."));
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
    $smarty->assign("netconfig", $this->netConfigDNS->execute());

    return($display.$smarty->fetch (get_template_path('printer.tpl', TRUE)));
  }

  function remove_from_parent()
  {
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
      $this->handle_post_events("remove");

      /* Delete references to object groups */
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
      while ($ldap->fetch()){
        $og= new ogroup($this->config, $ldap->getDN());
        unset($og->member[$this->dn]);
        $og->save ();
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
    $message= array_merge($message, $this->netConfigDNS->check());

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
      $message[]= "The required field 'Printer name' is not set.";
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
    if($this->BelongsTo == "Terminal"){
      $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
    }

    if($this->BelongsTo == "Workstation"){
      $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
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

    if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
      $method="https://";
    }else{
      $method="http://";
    }
  
    $this->attrs['gotoPrinterPPD'] = $method.str_replace("//","/",$_SERVER['SERVER_NAME']."/ppd/".$this->gotoPrinterPPD);

    /* Append printer user 
     */
    if((isset($this->attrs['gotoUserPrinter'])) && (is_string($this->attrs['gotoUserPrinter']))){
      $this->attrs['gotoUserPrinter']=array();
    }
    foreach($this->member['AddUser'] as $mem){
      $this->attrs['gotoUserPrinter'][]=$mem['uid'][0];
    }

    /* Append printer group 
     */
    foreach($this->member['AddGroup'] as $mem){
      $this->attrs['gotoGroupPrinter'][]=$mem['cn'][0];
    }

    /* Append printer admin user 
     */
    foreach($this->member['AddAdminUser'] as $mem){
      $this->attrs['gotoUserAdminPrinter'][]=$mem['uid'][0];
    }

    /* Append printer admin group 
     */
    foreach($this->member['AddAdminGroup'] as $mem){
      $this->attrs['gotoGroupAdminPrinter'][]=$mem['cn'][0];
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }

      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      $this->handle_post_events("modify");
    }
    show_ldap_error($ldap->get_error(), _("Saving printer failed"));

    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->dn = $this->dn;
    $this->netConfigDNS->save($this->dn);

    /* Optionally execute a command after we're done */
    $this->postcreate();
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
}



// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
