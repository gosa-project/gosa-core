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
  var $is_terminal      = false;

  var $UserMember       ="";
  var $UserMembers      =array();
  var $UserMemberKeys   =array();
  
  var $AdminMember      ="";
  var $AdminMembers     =array();
  var $AdminMemberKeys  =array();

  var $is_terminalBased = false; 

  var $PPDdialogToSave  = NULL;
 
  var $member           =array();
  var $strings          = "";
  var $type             = "";
  var $dialog           =NULL;

  var $netConfigDNS;
  var $baseSelection    = false;

  /* attribute list for save action */
  var $attributes     = array("cn", "description", "l", "labeledURI", "gotoPrinterPPD","gotoUserPrinter");
  var $objectclasses  = array("top", "gotoPrinter");

  function printgeneric ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $ldap= $this->config->get_ldap_link();
    
    $this->netConfigDNS = new termDNS($this->config, $this->dn,$this->objectclasses);


    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
      $this->is_terminal = true;
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }

    if(isset($this->gotoPrinterPPD)){
      $this->gotoPrinterPPD = preg_replace("/^http.*ppd\//i","",$this->gotoPrinterPPD);
    }

    /* In case of gotoWorkstation this tab is calles from workstation plugin
     * in case of gotoTerminal it is called from a terminal tab
     * else it is a standalone printer
     */
    if((isset($this->attrs['objectClass']))&&(in_array("gotoWorkstation",$this->attrs['objectClass']))){
      /* Fix for new Workstations */
      if(preg_match("/ou=incoming/",$this->dn)){    
        $this->is_terminal = "true";   
        $this->dn   = preg_replace("/ou=incoming/","ou=printers",$this->dn);
        $this->type = "station";
      }else{
        $this->is_terminal = "true";   
        $this->dn   = preg_replace("/ou=workstations/","ou=printers",$this->dn);
        $this->type = "station";
      }
    }elseif((isset($this->attrs['objectClass']))&&(in_array("gotoTerminal",$this->attrs['objectClass']))){
      if(preg_match("/ou=incoming/",$this->dn)){    
        $this->is_terminal = "true";   
        $this->dn   = preg_replace("/ou=incoming/","ou=printers",$this->dn);
        $this->type = "terminal";
      }else{
        $this->type = "terminal";
        $this->is_terminal = "true";
        $this->dn   = preg_replace("/ou=terminals/","ou=printers",$this->dn);
      }
    }else{
      /* Save dn for later references */
      $this->orig_dn= $this->dn;
    }

    /* If it is no standalone printer 
     */
    if($this->is_terminal){
      // Reload plugin with new dn... (ou=printers instead of ou=terminals)
      plugin::plugin ($this->config, $this->dn);
      $ldap->cat($this->dn);
      if(count($ldap->fetch())>0){
        $this->orig_dn= $this->dn;
        $this->is_account=true;
        $this->initially_was_account = true;
      }else{
        $this->orig_dn = "new";
        $this->is_account=false;
        $this->initially_was_account = false;
      }
    }

    /* Prepare different member types 
     */ 
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
            if(isset($entry['description'])){
              $this->member[$type][$entry['cn'][0]]=$entry;
            }
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
  
    $ldap->search("(&(cn=".$this->cn.")(|(objectClass=gotoTerminal)(objectClass=gotoWorkstation)))",array("cn","objectClass"));
    if($attrs = $ldap->fetch()){
      if(in_array("gotoTerminal",$attrs['objectClass'])){
        $this->is_terminalBased = "Terminal";
      }else{
        $this->is_terminalBased = "Workstation";
      }
    }

    /* A printer was only saved if the printer belongs to a workstation. 
     * If there was a new single printer created, it was never saved.
     * -This allows new printers to be saved.
     */
    if(($this->dn == "new")&&(!$this->is_terminal)){
      $this->is_account = true;
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    if(isset($this->parent->by_name['workgeneric'])){
//      echo "workstation";
      if($this->parent->by_object['workgeneric']->cn == "wdefault"){
        $display= $this->show_header(_("Add printer extension"),
            _("This is a workstation template, printer tab is disabled."),TRUE,TRUE);
        $this->is_account= false;
        return $display;
      }
    }elseif(isset($this->parent->by_name['termgeneric'])){
      if($this->parent->by_object['termgeneric']->cn == "default"){
        $display= $this->show_header(_("Add printer extension"),
            _("This is a terminal template, printer tab is disabled."),TRUE,TRUE);
        $this->is_account= false;
        return $display;
      }
    }elseif(isset($this->parent->by_name['printgeneric'])){
//      echo "printer";
    }


    $smarty= get_smarty();
    $display="";
    /* Template management.
     * There are two ways to call this tab.
     * 1. From a printer Dialog, here we will see the full template, without a toggle state button
     * 2. As a terminal tab, here we hide the top (name,base,description) of the template. 
     *    Toggle Account state will be shown in this case, to disable or enable this tab.
     *
     * $this->is_terminal indecates this two different types.
     */
    if($this->is_terminal){    
      $smarty->assign("is_terminal","true");
    }else{
      $smarty->assign("is_terminal","false");
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_modified = true;
      $this->is_account= !$this->is_account;
    }

    if($this->is_terminal){
      if(empty($this->cn)){
        if(!in_array("gotoPrinter",$this->objectclasses)){
          $display= $this->show_header(_("Add printer extension"),
              _("This workstation has printer extension disabled. You can't enable it while 'cn' is not present in entry. Possibly you are currently creating a new terminal template"),TRUE,TRUE);
          $this->is_account= false;
          return $display;
        }else{
          $smarty->assign("is_terminal","false");
          $skip = true;
        }
      }

      if(!isset($skip)){
        if (($this->is_account)){
          if($this->type=="station"){
            $display= $this->show_header(_("Remove printer extension"),
                _("This workstation has printer extension enabled.You can disable it by clicking below."));
          }else{
            $display= $this->show_header(_("Remove printer extension"),
                _("This terminal has printer extension enabled. You can disable it by clicking below."));
          }
        }else{
          if($this->type=="station"){
            $display= $this->show_header(_("Add printer extension"),
                _("This workstation has printer extension disabled. You can enable it by clicking below."));
          }else{
            $display= $this->show_header(_("Add printer extension"),
                _("This terminal has printer extension disabled. You can enable it by clicking below."));
          }  
          return ($display);
        }
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

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
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

    /* Do we represent a valid printer? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no printer features.")."</b>";
      return($display);
    }

    /* Fill templating stuff */
    $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("base_select", $this->base);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->acl, $attr));
      $smarty->assign("$attr", $this->$attr);
    }

    if(isset($_POST['AddUser'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, get_userinfo(),"AddUser");
    }
    if(isset($_POST['AddGroup'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, get_userinfo(),"AddGroup");
    }
    if(isset($_POST['AddAdminUser'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, get_userinfo(),"AddAdminUser");
    }
    if(isset($_POST['AddAdminGroup'])){
      $this->dialog = new selectUserToPrinterDialog($this->config, get_userinfo(),"AddAdminGroup");
    }

    if(isset($_POST['EditDriver'])){
      if($this->PPDdialogToSave){
        $this->dialog = $this->PPDdialogToSave;
      }else{
        $this->dialog = new printerPPDDialog($this->config, $this->dn,$this->gotoPrinterPPD);
      }
    }

    if(isset($_POST['PrinterCancel'])){
      unset($this->dialog);
      $this->dialog= NULL;
    }
    
    if(isset($_POST['RemoveDriver'])){
      $this->gotoPrinterPPD = array();
      $this->PPDdialogToSave = NULL;
    }

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

    if(isset($_POST['ClosePPD'])){
      unset($this->dialog);
      $this->dialog=NULL;
    }

  

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

    if($this->dialog != NULL){
      $this->dialog->save_object();
      $display = $this->dialog->execute();
      return $display;
    }

    require_once ("class_ppdManager.inc");

    if((isset($_SESSION['config']->data['MAIN']['PPD_PATH']))&&(is_dir($_SESSION['config']->data['MAIN']['PPD_PATH']))){

      $path = $_SESSION['config']->data['MAIN']['PPD_PATH'];
      if(!preg_match("/\/$/",$path)){
        $path = $path."/";
      }

      $ppdManager= new ppdManager($path);
      if(!empty($this->gotoPrinterPPD)){
        if((!file_exists($path.$this->gotoPrinterPPD))){
          $smarty->assign("driverInfo", "<b>".sprintf(_("Your currently selected PPD file '%s' doesn't exist."),$this->gotoPrinterPPD))."</b>";
        }else{
          $smarty->assign("driverInfo", $ppdManager->loadDescription($path.$this->gotoPrinterPPD));
        }
      }else{
        $smarty->assign("driverInfo", _("not defined"));
      }
    }else{
      $smarty->assign("driverInfo",_("can't get ppd informations."));
    }


    $list=$this->generateList();
    $userlist   = array_merge($list['AddUser'],$list['AddGroup']);
    $adminlist  = array_merge($list['AddAdminUser'],$list['AddAdminGroup']);

    asort($userlist);
    asort($adminlist);
    if($this->is_terminalBased){
      if($this->is_terminalBased == "Terminal"){
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
    if($this->initially_was_account){
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn);
      if(count($ldap->fetch()) ){

        $this->netConfigDNS->remove_from_parent();
        $ldap->rmdir($this->dn);
        show_ldap_error($ldap->get_error());
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
    $message= $this->netConfigDNS->check();

    if(isset($this->parent->by_name['workgeneric'])){
      if($this->parent->by_object['workgeneric']->cn == "wdefault"){
        return $message;
      }
    }elseif(isset($this->parent->by_name['termgeneric'])){
      if($this->parent->by_object['termgeneric']->cn == "default"){
        return $message;
      }
    }

    $this->dn= "cn=".$this->cn.",ou=printers,ou=systems,".$this->base;

    /* must: cn */
    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= "The required field 'Printer name' is not set.";
    }

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "printer", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a printer on this 'Base'.");
    }

    if (($this->orig_dn != $this->dn)&&(!$this->is_terminal)){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
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
    if (!$this->is_account){
      return;
    }
    if(isset($this->parent->by_name['workgeneric'])){
      if($this->parent->by_object['workgeneric']->cn == "wdefault"){
        return;
      }
    }elseif(isset($this->parent->by_name['termgeneric'])){
      if($this->parent->by_object['termgeneric']->cn == "default"){
        return;
      }
    }

    if($this->PPDdialogToSave){
      $this->PPDdialogToSave->save_ppd();
    }
    $dn= $this->dn;
    plugin::save();
    $ldap= $this->config->get_ldap_link();
   
    if((in_array("gotoTerminal",$this->attrs['objectClass']))){
      $this->dn= preg_replace("/ou=terminals,/","ou=printers,",$this->dn);
    }
    
    if((in_array("gotoWorkstation",$this->attrs['objectClass']))){
      $this->dn= preg_replace("/ou=workstations,/","ou=printers,",$this->dn);
    }

    /* We are currently editing a Terminal, so we want to save a seperate printer which cn is the terminla cn 
    */
    if($this->is_terminal){
      
      /* reduce objectClasses to minimun */
      $this->attrs['objectClass']= $this->objectclasses;

      /* If a printer with the given dn exists, modify else create new one */
      $ldap->cat($this->dn);
      if($ldap->fetch()){
        $this->orig_dn= $this->dn;
      }else{
        $this->orig_dn = "new";
      }
    }

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
  
    if(!is_array($this->attrs['gotoPrinterPPD'])) {
      $this->attrs['gotoPrinterPPD'] = $this->attrs['gotoPrinterPPD'];
      $this->attrs['gotoPrinterPPD'] = $method.str_replace("//","/",$_SERVER['SERVER_NAME']."/ppd/".$this->attrs['gotoPrinterPPD']);
    }

    /* Append printer user 
     */
    if(is_string( $this->attrs['gotoUserPrinter'])){
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
    show_ldap_error($ldap->get_error());

    $this->netConfigDNS->cn = $this->cn;
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
