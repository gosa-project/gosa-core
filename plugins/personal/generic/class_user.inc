<?php
/*!
  \brief   user plugin
  \author  Cajus Pollmeier <pollmeier@gonicus.de>
  \version 2.00
  \date    24.07.2003

  This class provides the functionality to read and write all attributes
  relevant for person, organizationalPerson, inetOrgPerson and gosaAccount
  from/to the LDAP. It does syntax checking and displays the formulars required.
 */

class user extends plugin
{
  /* Definitions */
  var $plHeadline= "Generic";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Handling of GOsa's user base object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Plugin specific values */
  var $base= "";
  var $cn= "";
  var $personalTitle= "";
  var $academicTitle= "";
  var $homePostalAddress= "";
  var $homePhone= "";
  var $labeledURI= "";
  var $o= "";
  var $ou= "";
  var $departmentNumber= "";
  var $employeeNumber= "";
  var $employeeType= "";
  var $roomNumber= "";
  var $telephoneNumber= "";
  var $facsimileTelephoneNumber= "";
  var $mobile= "";
  var $pager= "";
  var $l= "";
  var $st= "";
  var $postalAddress= "";
  var $dateOfBirth;
  var $use_dob= "0";
  var $gender="0";
  var $preferredLanguage="0";

  var $jpegPhoto= "*removed*";
  var $photoData= "";
  var $old_jpegPhoto= "";
  var $old_photoData= "";
  var $cert_dialog= FALSE;
  var $picture_dialog= FALSE;

  var $userPKCS12= "";
  var $userSMIMECertificate= "";
  var $userCertificate= "";
  var $certificateSerialNumber= "";
  var $old_certificateSerialNumber= "";
  var $old_userPKCS12= "";
  var $old_userSMIMECertificate= "";
  var $old_userCertificate= "";

  var $gouvernmentOrganizationalUnit= "";
  var $houseIdentifier= "";
  var $street= "";
  var $postalCode= "";
  var $vocation= "";
  var $ivbbLastDeliveryCollective= "";
  var $gouvernmentOrganizationalPersonLocality= "";
  var $gouvernmentOrganizationalUnitDescription= "";
  var $gouvernmentOrganizationalUnitSubjectArea= "";
  var $functionalTitle= "";
  var $role= "";
  var $publicVisible= "";

  var $dialog;

  /* variables to trigger password changes */
  var $pw_storage= "crypt";
  var $last_pw_storage= "unset";
  var $had_userCertificate= FALSE;

  /* attribute list for save action */
  var $attributes= array("sn", "givenName", "uid", "personalTitle", "academicTitle",
      "homePostalAddress", "homePhone", "labeledURI", "o", "ou", "dateOfBirth", "gender","preferredLanguage",
      "departmentNumber", "employeeNumber", "employeeType", "l", "st",
      "roomNumber", "telephoneNumber", "mobile", "pager", "cn", "userPKCS12",
      "postalAddress", "facsimileTelephoneNumber", "userSMIMECertificate");

  var $objectclasses= array("top", "person", "organizationalPerson", "inetOrgPerson",
      "gosaAccount");

  /* attributes that are part of the government mode */
  var $govattrs= array("gouvernmentOrganizationalUnit", "houseIdentifier", "vocation",
      "ivbbLastDeliveryCollective", "gouvernmentOrganizationalPersonLocality",
      "gouvernmentOrganizationalUnitDescription","gouvernmentOrganizationalUnitSubjectArea",
      "functionalTitle", "certificateSerialNumber", "publicVisible", "street", "role",
      "postalCode");


  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function user ($config, $dn= NULL)
  {
    $this->config= $config;
    /* Configuration is fine, allways */
    if ($this->config->current['GOVERNMENTMODE']){
      $this->attributes=array_merge($this->attributes,$this->govattrs);
    }

    /* Load base attributes */
    plugin::plugin ($config, $dn);

    if ($this->config->current['GOVERNMENTMODE']){
      /* Fix public visible attribute if unset */
      if (!isset($this->attrs['publicVisible'])){
        $this->publicVisible == "nein";
      }
    }

    /* Load government mode attributes */
    if ($this->config->current['GOVERNMENTMODE']){
      /* Copy all attributs */
      foreach ($this->govattrs as $val){
        if (isset($this->attrs["$val"][0])){
          $this->$val= $this->attrs["$val"][0];
        }
      }
    }

    /* Create me for new accounts */
    if ($dn == "new"){
      $this->is_account= TRUE;
    }

    /* Make hash default to md5 if not set in config */
    if (!isset($this->config->current['HASH'])){
      $hash= "md5";
    } else {
      $hash= $this->config->current['HASH'];
    }

    /* Load data from LDAP? */
    if ($dn != NULL){

      /* Do base conversation */
      if ($this->dn == "new"){
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      } else {
        $this->base= dn2base($dn);
      }

      /* get password storage type */
      if (isset ($this->attrs['userPassword'][0])){
      	/* Initialize local array */
      	$matches= array();
        if (preg_match ("/^{([^}]+)}(.+)/", $this->attrs['userPassword'][0], $matches)){
          $this->pw_storage= strtolower($matches[1]);
        } else {
          if ($this->attrs['userPassword'][0] != ""){
            $this->pw_storage= "clear";
          } else {
            $this->pw_storage= $hash;
          }
        }
      } else {
        /* Preset with vaule from configuration */
        $this->pw_storage= $hash;
      }

      /* Load extra attributes: certificate and picture */
      $this->load_picture();
      $this->load_cert();
      if ($this->userCertificate != ""){
        $this->had_userCertificate= TRUE;
      }
    }

    /* Reset password storage indicator, used by password_change_needed() */
    if ($dn == "new"){
      $this->last_pw_storage= "unset";
    } else {
      $this->last_pw_storage= $this->pw_storage;
    }

    /* Generate dateOfBirth entry */
    if (isset ($this->attrs['dateOfBirth'])){
      /* This entry is ISO 8601 conform */
      list($year, $month, $day)= split("-", $this->attrs['dateOfBirth'][0], 3);
    
      $this->dateOfBirth=array( 'mon'=> $month,"mday"=> $day,"year"=> $year);
      $this->use_dob= "1";
    } else {
      $this->use_dob= "0";
    }

    /* Put gender attribute to upper case */
    if (isset ($this->attrs['gender'])){
      $this->gender= strtoupper($this->attrs['gender'][0]);
    }
  }


  /* execute generates the html output for this node */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $smarty= get_smarty();

    /* Fill calendar */
    if ($this->dateOfBirth == "0"){
      $date= getdate();
    } else {
      if(is_array($this->dateOfBirth)){
        $date = $this->dateOfBirth;
      }else{
        $date = getdate($this->dateOfBirth);
      } 
    }

    $days= array();
    for($d= 1; $d<32; $d++){
      $days[$d]= $d;
    }
    $years= array();

    if(($date['year']-100)<1901){
      $start = 1901;
    }else{
      $start = $date['year']-100;
    }

    $end = $start +100;
    
    for($y= $start; $y<=$end; $y++){
      $years[]= $y;
    }
    $years['-']= "-&nbsp;";
    $months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"), '-' => '-&nbsp;');
    $smarty->assign("day", $date["mday"]);
    $smarty->assign("days", $days);
    $smarty->assign("months", $months);
    $smarty->assign("month", $date["mon"]-1);
    $smarty->assign("years", $years);
    $smarty->assign("year", $date["year"]);

    /* Assign sex */
    $sex= array(0 => "&nbsp;", "F" => _("female"), "M" => _("male"));
    $smarty->assign("gender_list", $sex);

    /* Assign prefered langage */
    $language= array(0 => "&nbsp;", "fr_FR" => ("fr_FR"), "en_EN" => ("en_EN"), 
                                    "de_DE" => ("de_DE"), "it_IT" => ("it_IT"), 
                                    "nl_NL" => ("nl_NL"), "ru_RU" => ("ru_RU"));
    $smarty->assign("preferredLanguage_list", $language);

    /* Get random number for pictures */
    srand((double)microtime()*1000000); 
    $smarty->assign("rand", rand(0, 10000));

    /* Do we represent a valid gosaAccount? */
    if (!$this->is_account){
      echo "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no valid GOsa extensions.")."</b>";
      return;
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this->allowedBasesToMoveTo());
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();
   
      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
        $this->base = $this->dialog->isSelected();
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Want picture edit dialog? */
    if (isset($_POST['edit_picture'])){
      /* Save values for later recovery, in case some presses
         the cancel button. */
      $this->old_jpegPhoto= $this->jpegPhoto;
      $this->old_photoData= $this->photoData;
      $this->picture_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Remove picture? */
    if (isset($_POST['picture_remove'])){
      $this->jpegPhoto= "*removed*";
      $this->set_picture ("./images/default.jpg");
      $this->is_modified= TRUE;

      return($smarty->fetch (get_template_path('generic_picture.tpl', TRUE, dirname(__FILE__))));
    }

    /* Save picture */
    if (isset($_POST['picture_edit_finish'])){

      /* Check for clean upload */
      if ($_FILES['picture_file']['name'] != ""){
        if (!is_uploaded_file($_FILES['picture_file']['tmp_name'])) {
          print_red(_("The specified file has not been uploaded via HTTP POST! Aborted."));
          exit;
        }

        /* Activate new picture */
        $this->set_picture($_FILES['picture_file']['tmp_name']);
      }
      $this->picture_dialog= FALSE;
      $this->dialog= FALSE;
      $this->is_modified= TRUE;
    }


    /* Cancel picture */
    if (isset($_POST['picture_edit_cancel'])){

      /* Restore values */
      $this->jpegPhoto= $this->old_jpegPhoto;
      $this->photoData= $this->old_photoData;

      /* Update picture */
      $_SESSION['binary']= $this->photoData;
      $_SESSION['binarytype']= "image/jpeg";
      $this->picture_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Toggle dateOfBirth information */
    if (isset($_POST['set_dob'])){
      $this->use_dob= ($this->use_dob == "0")?"1":"0";
    }


    /* Want certificate= */
    if (isset($_POST['edit_cert'])){

      /* Save original values for later reconstruction */
      foreach (array("certificateSerialNumber", "userCertificate",
            "userSMIMECertificate", "userPKCS12") as $val){

        $oval= "old_$val";
        $this->$oval= $this->$val;
      }

      $this->cert_dialog= TRUE;
      $this->dialog= TRUE;
    }


    /* Cancel certificate dialog */
    if (isset($_POST['cert_edit_cancel'])){

      /* Restore original values in case of 'cancel' */
      foreach (array("certificateSerialNumber", "userCertificate",
            "userSMIMECertificate", "userPKCS12") as $val){

        $oval= "old_$val";
        $this->$val= $this->$oval;
      }
      $this->cert_dialog= FALSE;
      $this->dialog= FALSE;
    }


    /* Remove certificate? */
    foreach (array ("userCertificate", "userSMIMECertificate", "userPKCS12") as $val){
      if (isset($_POST["remove_$val"])){

        /* Reset specified cert*/
        $this->$val= "";
        $this->is_modified= TRUE;
      }
    }


    /* Upload new cert and close dialog? */	
    if (isset($_POST['cert_edit_finish'])){

      /* for all certificates do */
      foreach (array ("userCertificate", "userSMIMECertificate", "userPKCS12")
          as $val){

        /* Check for clean upload */
        if (array_key_exists($val."_file", $_FILES) &&
            array_key_exists('name', $_FILES[$val."_file"]) &&
            $_FILES[$val."_file"]['name'] != "" &&
            is_uploaded_file($_FILES[$val."_file"]['tmp_name'])) {
          $this->set_cert("$val", $_FILES[$val."_file"]['tmp_name']);
        }
      }

      /* Save serial number */
      if (isset($_POST["certificateSerialNumber"]) &&
          $_POST["certificateSerialNumber"] != ""){

        if (!is_id($_POST["certificateSerialNumber"])){
          print_red (_("Please enter a valid serial number"));

          foreach(array("userCertificate", "userSMIMECertificate", "userPKCS12") as $cert){
            if ($this->$cert != ""){
              $smarty->assign("$cert"."_state", "true");
            } else {
              $smarty->assign("$cert"."_state", "");
            }
          }
          return ($smarty->fetch (get_template_path('generic_certs.tpl', TRUE, dirname(__FILE__))));
        }

        $this->certificateSerialNumber= $_POST["certificateSerialNumber"];
        $this->is_modified= TRUE;
      }

      $this->cert_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Display picture dialog */
    if ($this->picture_dialog){
      return($smarty->fetch (get_template_path('generic_picture.tpl', TRUE, dirname(__FILE__))));
    }

    /* Display cert dialog */
    if ($this->cert_dialog){
      foreach(array("userCertificate", "userSMIMECertificate", "userPKCS12") as $cert){
        if ($this->$cert != ""){
          /* import certificate */
          $certificate = new certificate;
          $certificate->import($this->$cert);
      
          /* Read out data*/
          $timeto   = $certificate->getvalidto_date();
          $timefrom = $certificate->getvalidfrom_date();
          $str = "<table summary=\"\" border=0><tr><td style='vertical-align:top'>CN</td><td>".preg_replace("/ /", "&nbsp;", $certificate->getname())."</td></tr></table><br>".
                  sprintf(_("Certificate is valid from %s to %s and is currently %s."), "<b>".date('d M Y',$timefrom)."</b>","<b>".date('d M Y',$timeto)."</b>", $certificate->isvalid()?"<b><font style='color:green'>"._("valid")."</font></b>":"<b><font style='color:red'>"._("invalid")."</font></b>");
          $smarty->assign($cert."info",$str);
          $smarty->assign($cert."_state","true");
        } else {
          $smarty->assign($cert."info", "<i>"._("No certificate installed")."</i>");
          $smarty->assign($cert."_state","");
        }
      }
      $smarty->assign("governmentmode", "false");
      return($smarty->fetch (get_template_path('generic_certs.tpl', TRUE, dirname(__FILE__))));
    }

    /* Show us the edit screen */
    @$smarty->assign("bases", $this->allowedBasesToMoveTo());
#  $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("base_select",      $this->base);
    $smarty->assign("selectmode",       chkacl($this->acl, "create"));
    $smarty->assign("certificatesACL",  chkacl($this->acl, "certificates"));
    $smarty->assign("jpegPhotoACL",     chkacl($this->acl, "jpegPhoto"));

    /* Prepare password hashes */
    if ($this->pw_storage == ""){
      $this->pw_storage= $this->config->current['HASH'];
    }

    $temp   = @passwordMethod::get_available_methods();
    $hashes = $temp['name'];
    
    $smarty->assign("pwmode", $hashes);
    $smarty->assign("pwmode_select", $this->pw_storage);
    $smarty->assign("passwordStorageACL", chkacl($this->acl, "passwordStorage"));

    /* Load attributes and acl's */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl,$val));
    }

    /* Save government mode attributes */
    if (isset($this->config->current['GOVERNMENTMODE']) &&
        preg_match('/true/i', $this->config->current['GOVERNMENTMODE'])){
      $smarty->assign("governmentmode", "true");
      $ivbbmodes= array("nein", "ivbv", "testa", "ivbv,testa", "internet",
          "internet,ivbv", "internet,testa", "internet,ivbv,testa");
      $smarty->assign("ivbbmodes", $ivbbmodes);
      foreach ($this->govattrs as $val){
        $smarty->assign("$val", $this->$val);
        $smarty->assign("$val"."ACL", chkacl($this->acl,$val));
      }
    } else {
      $smarty->assign("governmentmode", "false");
    }

    /* Special mode for uid */
    $uidACL= "";
    if (isset ($this->dn)){
      if ($this->dn != "new"){
        $uidACL="readonly";
      }
    }  else {
      $uidACL= "readonly";
    }
    $uidACL.= " ".chkacl($this->acl, "uid");
    
    $smarty->assign("uidACL", $uidACL);
    $smarty->assign("is_template", $this->is_template);
    $smarty->assign("use_dob", $this->use_dob);

    if (isset($this->parent)){
      if (isset($this->parent->by_object['phoneAccount']) &&
          $this->parent->by_object['phoneAccount']->is_account){
        $smarty->assign("has_phoneaccount", "true");
      } else {
        $smarty->assign("has_phoneaccount", "false");
      }
    } else {
      $smarty->assign("has_phoneaccount", "false");
    }
    return($smarty->fetch (get_template_path('generic.tpl', TRUE, dirname(__FILE__))));
  }


  /* remove object from parent */
  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir ($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing generic user account failed"));

    /* Delete references to groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=posixGroup)(memberUid=".$this->uid."))", array("uid"));
    while ($ldap->fetch()){
      $g= new group($this->config, $ldap->getDN());
      $g->removeUser($this->uid);
      $g->save ();
    }

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['generic'])){

      /* Parents save function */
      plugin::save_object ();

      /* Save government mode attributes */
      if ($this->config->current['GOVERNMENTMODE']){
        foreach ($this->govattrs as $val){
          if (chkacl ($this->acl, "$val") == "" && isset ($_POST["$val"])){
            $data= stripcslashes($_POST["$val"]);
            if ($data != $this->$val){
              $this->is_modified= TRUE;
            }
            $this->$val= $data;
          }
        }
      }

      /* In template mode, the uid is autogenerated... */
      if ($this->is_template){
        $this->uid= strtolower($this->sn);
        $this->givenName= $this->sn;
      }

      /* Save base and pw_storage, since these are no LDAP attributes */
      if (isset($_POST['base'])){
        foreach(array("base", "pw_storage") as $val){
          if(isset($_POST[$val])){
            $data= validate($_POST[$val]);
            if ($data != $this->$val){
              $this->is_modified= TRUE;
            }
            $this->$val= $data;
          }
        }
      }
    }
  }

  function rebind($ldap, $referral)
  {
    $credentials= LDAP::get_credentials($referral, $this->config->current['REFERRAL']);
    if (ldap_bind($ldap, $credentials['ADMIN'], $credentials['PASSWORD'])) {
      $this->error = "Success";
      $this->hascon=true;
      $this->reconnect= true;
      return (0);
    } else {
      $this->error = "Could not bind to " . $credentials['ADMIN'];
      return NULL;
    }
  }

  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
    /* Only force save of changes .... 
       If this attributes aren't changed, avoid saving.
     */
    if ($this->use_dob == "1"){
      $this->dateOfBirth= date("Y-m-d", $this->dateOfBirth);
    }
    if($this->gender=="0") $this->gender ="";
    if($this->preferredLanguage=="0") $this->preferredLanguage ="";
    
 
    /* First use parents methods to do some basic fillup in $this->attrs */
    plugin::save ();

    /* Remove additional objectClasses */
    $tmp= array();
    foreach ($this->attrs['objectClass'] as $key => $set){
      $found= false;
      foreach (array("ivbbentry", "gosaUserTemplate") as $val){
        if (preg_match ("/^$set$/i", $val)){
          $found= true;
          break;
        }
      }
      if (!$found){
        $tmp[]= $set;
      }
    }

    /* Replace the objectClass array. This is done because of the
       separation into government and normal mode. */
    $this->attrs['objectClass']= $tmp;

    /* Add objectClasss for template mode? */
    if ($this->is_template){
      $this->attrs['objectClass'][]= "gosaUserTemplate";
    }

    /* Hard coded government mode? */
    if ($this->config->current['GOVERNMENTMODE'] != 'false'){
      $this->attrs['objectClass'][]= "ivbbentry";

      /* Copy standard attributes */
      foreach ($this->govattrs as $val){
        if ($this->$val != ""){
          $this->attrs["$val"]= $this->$val;
        } elseif (!$this->new) {
          $this->attrs["$val"]= array();
        }
      }

      /* Remove attribute if set to "nein" */
      if ($this->publicVisible == "nein"){
        $this->attrs['publicVisible']= array();
        if($this->new){
          unset($this->attrs['publicVisible']);
        }else{
          $this->attrs['publicVisible']=array();
        }

      }

    }

    /* Special handling for attribute userCertificate needed */
    if ($this->userCertificate != ""){
      $this->attrs["userCertificate;binary"]= $this->userCertificate;
      $remove_userCertificate= false;
    } else {
      $remove_userCertificate= true;
    }

    /* Special handling for dateOfBirth value */
    if ($this->use_dob != "1"){
      if ($this->new) {
        unset($this->attrs["dateOfBirth"]);
      } else {
        $this->attrs["dateOfBirth"]= array();
      }
    }
    if (!$this->gender){
      if ($this->new) {
        unset($this->attrs["gender"]);
      } else {
        $this->attrs["gender"]= array();
      }
    }
    if (!$this->preferredLanguage){
      if ($this->new) {
        unset($this->attrs["preferredLanguage"]);
      } else {
        $this->attrs["preferredLanguage"]= array();
      }
    }

    /* Special handling for attribute jpegPhote needed, scale image via
       image magick to 147x200 pixels and inject resulting data. */
    if ($this->jpegPhoto != "*removed*"){

      /* Fallback if there's no image magick inside PHP */
      if (!function_exists("imagick_blob2image")){
        /* Get temporary file name for conversation */
        $fname = tempnam ("/tmp", "GOsa");

        /* Open file and write out photoData */
        $fp = fopen ($fname, "w");
        fwrite ($fp, $this->photoData);
        fclose ($fp);

        /* Build conversation query. Filename is generated automatically, so
           we do not need any special security checks. Exec command and save
           output. For PHP safe mode, you'll need a configuration which respects
           image magick as executable... */
        $query= "convert -size 147x200 $fname -resize 147x200 +profile \"*\" -";
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $query, "Execute");

        /* Read data written by convert */
        $output= "";
        $sh= popen($query, 'r');
        while (!feof($sh)){
          $output.= fread($sh, 4096);
        }
        pclose($sh);

        unlink($fname);

        /* Save attribute */
        $this->attrs["jpegPhoto"] = $output;

      } else {

        /* Load the new uploaded Photo */
        if(!$handle  =  imagick_blob2image($this->photoData))  {
          gosa_log("Can't Load image");
        }

        /* Resizing image to 147x200 and blur */
        if(!imagick_resize($handle,147,200,IMAGICK_FILTER_GAUSSIAN,0)){
          gosa_log("imagick_resize failed");
        }

        /* Converting image to JPEG */
        if(!imagick_convert($handle,"JPEG")) {
          gosa_log("Can't Convert to JPEG");
        }

        /* Creating binary Code for the Image */
        if(!$dump = imagick_image2blob($handle)){
          gosa_log("Can't create blob for image");
        }

        /* Sending Image */
        $output=  $dump;

        /* Save attribute */
        $this->attrs["jpegPhoto"] = $output;
      }

    } elseif(!$this->new) {
      $this->attrs["jpegPhoto"] = array();
    }

    /* Build new dn */
    if (isset($this->config->current['DNMODE']) && $this->config->current['DNMODE'] == "uid"){
      $new_dn= 'uid='.$this->uid.','.get_people_ou().$this->base;
    } else {
      $new_dn= 'cn='.$this->cn.','.get_people_ou().$this->base;
    }

    /* This only gets called when user is renaming himself */
    $ldap= $this->config->get_ldap_link();
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      $this->move($this->dn, $new_dn);

      /* Happen to use the new one */
      change_ui_dn($this->dn, $new_dn);
      $this->dn= $new_dn;
    }


    /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
       new entries. So do a check first... */
    $ldap->cat ($this->dn, array('dn'));
    if ($ldap->fetch()){
      $mode= "modify";
    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    /* Set password to some junk stuff in case of templates */
    if ($this->is_template){
      $this->attrs['userPassword']= '{crypt}N0T$3T4N0W';
    }

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save via $mode");

    /* Finally write data with selected 'mode' */
    $this->cleanup();
    $ldap->cd ($this->dn);
    $ldap->$mode ($this->attrs);
    if (show_ldap_error($ldap->get_error(), _("Saving generic user account failed"))){
      return (1);
    }

    /* Remove cert? 
       For some reason, the 'ldap' class doesn't want to remove binary entries, so I need
       to work around myself. */
    if ($remove_userCertificate == true && !$this->new && $this->had_userCertificate){

      /* Reset array, assemble new, this should be reworked */
      $this->attrs= array();
      $this->attrs['userCertificate;binary']= array();

      /* Prepare connection */
      if (!($ds = ldap_connect($this->config->current['SERVER']))) {
        die ("Could not connect to LDAP server");
      }
      ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
      if (function_exists("ldap_set_rebind_proc") && isset($this->config->current['RECURSIVE']) && $this->config->current['RECURSIVE'] == "true") {
        ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
        ldap_set_rebind_proc($ds, array(&$this, "rebind"));
      }
      if(isset($config->current['TLS']) && $config->current['TLS'] == "true"){
        ldap_start_tls($ds);
      }
      if (!($res = @ldap_bind($ds, $this->config->current['ADMIN'],
              $this->config->current['PASSWORD']))) {
        die ("Could not bind to LDAP");
      }

      /* Modify using attrs */
      ldap_mod_del($ds,$this->dn,$this->attrs);
      ldap_close($ds);
    }

    /* Kerberos server defined? */
    if (isset($this->config->data['SERVERS']['KERBEROS'])){
      $cfg= $this->config->data['SERVERS']['KERBEROS'];
    }
    if (isset($cfg['SERVER']) && function_exists('kadm5_init_with_password')){

      /* Connect to the admin interface */
      $handle = kadm5_init_with_password($cfg['SERVER'], $cfg['REALM'],
          $cfg['ADMIN'], $cfg['PASSWORD']);

      /* Errors? */		
      if ($handle === FALSE){
        print_red (_("Kerberos database communication failed"));
        return (2);
      }

      /* Build user principal, get list of existsing principals */
      $principal= $this->uid."@".$cfg['REALM'];
      $principals = kadm5_get_principals($handle);

      /* User exists in database? */
      if (in_array($principal, $principals)){

        /* Ok. User exists. Remove him/her when pw_storage has
           changed to be NOT kerberos. */
        if ($this->pw_storage != "kerberos"){
          $ret= kadm5_delete_principal ( $handle, $principal);

          if ($ret === FALSE){
            print_red (_("Can't remove user from kerberos database."));
          }
        }

      } else {

        /* User doesn't exists, create it when pw_storage is kerberos. */
        if ($this->pw_storage == "kerberos"){
          $ret= kadm5_create_principal ( $handle, $principal);

          if ($ret === FALSE){
            print_red (_("Can't add user to kerberos database."));
          }
        }

      }

      /* Free kerberos admin handle */
      kadm5_destroy($handle);
    }

    /* Optionally execute a command after we're done */
    if ($mode == "add"){
      $this->handle_post_events("add");
    } elseif ($this->is_modified){
      $this->handle_post_events("modify");
    }

    /* Fix tagging if needed */
    $this->handle_object_tagging();

    return (0);
  }


  /* Check formular input */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Assemble cn */
    $this->cn= $this->givenName." ".$this->sn;

    /* Permissions for that base? */
    if (isset($this->config->current['DNMODE']) && $this->config->current['DNMODE'] == "uid"){
      $new_dn= 'uid='.$this->uid.','.get_people_ou().$this->base;
    } else {
      $new_dn= 'cn='.$this->cn.','.get_people_ou().$this->base;
    }

    $ui= get_userinfo();
    $acl= get_permissions ($new_dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "user", $new_dn);
    if ($this->dn == "new" && chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a user on this 'Base'.");
    } elseif ($this->dn != $new_dn && $this->dn != "new"){
      $acl= get_permissions ($this->dn, $ui->subtreeACL);
      $acl= get_module_permission($acl, "user", $this->dn);
      if (chkacl($acl, "create") != ""){
        $message[]= _("You have no permissions to move a user from the original 'Base'.");
      }
    }

    /* must: sn, givenName, uid */
    if ($this->sn == "" && chkacl ($this->acl, "sn") == ""){
      $message[]= _("The required field 'Name' is not set.");
    }

    /* UID already used? */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(uid=$this->uid)", array("uid"));
    $ldap->fetch();
    if ($ldap->count() != 0 && $this->dn == 'new'){
      $message[]= _("There's already a person with this 'Login' in the database.");
    }

    /* In template mode, the uid and givenName are autogenerated... */
    if (!$this->is_template){
      if ($this->givenName == "" && chkacl ($this->acl, "givenName") == ""){
        $message[]= _("The required field 'Given name' is not set.");
      }
      if ($this->uid == "" && chkacl ($this->acl, "uid") == ""){
        $message[]= _("The required field 'Login' is not set.");
      }
      if (!(isset($this->config->current['DNMODE']) && $this->config->current['DNMODE'] == "uid")){
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(cn=".$this->cn.")", array("uid"));
        $ldap->fetch();
        if ($ldap->count() != 0 && $this->dn != $new_dn && $this->dn == 'new'){
          $message[]= _("There's already a person with this 'Name'/'Given name' combination in the database.");
        }
      }
    }

    /* Check for valid input */
    if ($this->is_modified && !is_uid($this->uid)){
      $message[]= _("The field 'Login' contains invalid characters. Lowercase, numbers and dashes are allowed.");
    }
    if (!is_url($this->labeledURI)){
      $message[]= _("The field 'Homepage' contains an invalid URL definition.");
    }
    if (preg_match ("/[\\\\]/", $this->sn)){
      $message[]= _("The field 'Name' contains invalid characters.");
    }
    if (preg_match ("/[\\\\]/", $this->givenName)){
      $message[]= _("The field 'Given name' contains invalid characters.");
    }

    /* Check phone numbers */
    if (!is_phone_nr($this->telephoneNumber)){
      $message[]= _("The field 'Phone' contains an invalid phone number.");
    }
    if (!is_phone_nr($this->facsimileTelephoneNumber)){
      $message[]= _("The field 'Fax' contains an invalid phone number.");
    }
    if (!is_phone_nr($this->mobile)){
      $message[]= _("The field 'Mobile' contains an invalid phone number.");
    }
    if (!is_phone_nr($this->pager)){
      $message[]= _("The field 'Pager' contains an invalid phone number.");
    }

    /* Check for reserved characers */
    if (preg_match ('/[,+"?\'()=<>;]/', $this->givenName)){
      $message[]= _("The field 'Given name' contains invalid characters.");
    }
    if (preg_match ('/[,+"?\'()=<>;]/', $this->sn)){
      $message[]= _("The field 'Name' contains invalid characters.");
    }

  return $message;
  }


  /* Indicate whether a password change is needed or not */
  function password_change_needed()
  {
    return ($this->pw_storage != $this->last_pw_storage);
  }


  /* Load a jpegPhoto from LDAP, this is going to be simplified later on */
  function load_picture()
  {
    /* make connection and read jpegPhoto */
    $ds= ldap_connect($this->config->current['SERVER']);
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    if (function_exists("ldap_set_rebind_proc") && isset($this->config->current['RECURSIVE']) && $this->config->current['RECURSIVE'] == "true") {
      ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
      ldap_set_rebind_proc($ds, array(&$this, "rebind"));
    }

    if(isset($this->config->current['TLS']) &&
        $this->config->current['TLS'] == "true"){

      ldap_start_tls($ds);
    }

    $r= ldap_bind($ds);
    $sr= @ldap_read($ds, $this->dn, "jpegPhoto=*", array("jpegPhoto"));

    /* in case we don't get an entry, load a default picture */
    $this->set_picture ("./images/default.jpg");
    $this->jpegPhoto= "*removed*";

    /* fill data from LDAP */
    if ($sr) {
      $ei=ldap_first_entry($ds, $sr);
      if ($ei) {
        if ($info = ldap_get_values_len($ds, $ei, "jpegPhoto")){
          $this->photoData= $info[0];
          $_SESSION['binary']= $this->photoData;
          $_SESSION['binarytype']= "image/jpeg";
          $this->jpegPhoto= "";
        }
      }
    }

    /* close conncetion */
    ldap_unbind($ds);
  }


  /* Load a certificate from LDAP, this is going to be simplified later on */
  function load_cert()
  {
    $ds= ldap_connect($this->config->current['SERVER']);
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    if (function_exists("ldap_set_rebind_proc") && isset($this->config->current['RECURSIVE']) && $this->config->current['RECURSIVE'] == "true") {
      ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
      ldap_set_rebind_proc($ds, array(&$this, "rebind"));
    }
    if(isset($this->config->current['TLS']) &&
        $this->config->current['TLS'] == "true"){

      ldap_start_tls($ds);
    }

    $r= ldap_bind($ds);
    $sr= @ldap_read($ds, $this->dn, "userCertificate=*", array("userCertificate"));

    if ($sr) {
      $ei= @ldap_first_entry($ds, $sr);
      
      if ($ei) {
        if (!$info = @ldap_get_values_len($ds, $ei, "userCertificate;binary")){
          $this->userCertificate= "";
        } else {
          $this->userCertificate= $info[0];
        }
      }
    } else {
      $this->userCertificate= "";
    }

    ldap_unbind($ds);
  }


  /* Load picture from file to object */
  function set_picture($filename)
  {
    if (!is_file($filename)){
      $filename= "./images/default.jpg";
      $this->jpegPhoto= "*removed*";
    }

    $fd = fopen ($filename, "rb");
    $this->photoData= fread ($fd, filesize ($filename));
    $_SESSION['binary']= $this->photoData;
    $_SESSION['binarytype']= "image/jpeg";
    $this->jpegPhoto= "";

    fclose ($fd);
  }


  /* Load certificate from file to object */
  function set_cert($cert, $filename)
  {
    $fd = fopen ($filename, "rb");
    if (filesize($filename)>0) {
      $this->$cert= fread ($fd, filesize ($filename));
      fclose ($fd);
      $this->is_modified= TRUE;
    } else {
      print_red(_("Could not open specified certificate!"));
    }
  }

  /* Adapt from given 'dn' */
  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);

    /* Get base */
    $this->base= preg_replace('/^[^,]+,'.get_people_ou().'/i', '', $dn);

    if ($this->config->current['GOVERNMENTMODE']){

      /* Walk through govattrs */
      foreach ($this->govattrs as $val){

        if (isset($this->attrs["$val"][0])){

          /* If attribute is set, replace dynamic parts: 
             %sn, %givenName and %uid. Fill these in our local variables. */
          $value= $this->attrs["$val"][0];

          foreach (array("sn", "givenName", "uid") as $repl){
            if (preg_match("/%$repl/i", $value)){
              $value= preg_replace ("/%$repl/i",
                  $this->parent->$repl, $value);
            }
          }
          $this->$val= $value;
        }
      }
    }

    /* Get back uid/sn/givenName */
    if ($this->parent != NULL){
      $this->uid= $this->parent->uid;
      $this->sn= $this->parent->sn;
      $this->givenName= $this->parent->givenName;
    }
  }

 
  /* This avoids that users move themselves out of their rights. 
   */
  function allowedBasesToMoveTo()
  {
    $allowed = array();
    $ret_all = false;
    if($this->uid == $_SESSION['ui']->username){
      $ldap= $this->config->get_ldap_link(); 
      $ldap->cd($this->config->current['BASE']); 
      $ldap->search("(&(objectClass=posixGroup)(memberUid=".$_SESSION['ui']->username."))",array("gosaSubtreeACL"));
       
      while($attrs = $ldap->fetch()){
    
        if(isset($attrs['gosaSubtreeACL'])){
        
          foreach($attrs['gosaSubtreeACL'] as $attr){
            if((preg_match("/:user#/",$attr))||(preg_match("/:all/",$attr))){
              $s =  preg_replace("/^.*ou=groups,/","",$attrs['dn']);

              foreach($this->config->idepartments as $key => $dep) {
                if(preg_match("/".$s."/i",$key)){
                  $allowed[$key] = $dep;
                }
              }
            }
          }
        }
      }
      if(count($allowed) == 0){
        foreach($this->config->idepartments as $key => $dep) {
          if($this->base==$key){
            $allowed[$key] = $dep;
          }
        }
      }  
  
      return($allowed);
      
    }else{
      return($this->config->idepartments);
    }
  } 


  function getCopyDialog()
  {
    $str = "";

    $_SESSION['binary'] = $this->photoData; 
    $_SESSION['binarytype']= "image/jpeg";

    /* Get random number for pictures */
    srand((double)microtime()*1000000); 
    $rand = rand(0, 10000);

    $smarty = get_smarty();

    $smarty->assign("passwordTodo","clear");

    if(isset($_POST['passwordTodo'])){
      $smarty->assign("passwordTodo",$_POST['passwordTodo']);
    }

    $smarty->assign("sn",       $this->sn);
    $smarty->assign("givenName",$this->givenName);
    $smarty->assign("uid",      $this->uid);
    $smarty->assign("rand",     $rand);
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));


    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";  
    return($ret);
  }

  function saveCopyDialog()
  {

    if((isset($_FILES['picture_file']['tmp_name'])) && ($_FILES['picture_file']['size'] > 0)){
      $this->set_picture($_FILES['picture_file']['tmp_name']);
    }

    /* Remove picture? */
    if (isset($_POST['picture_remove'])){
      $this->jpegPhoto= "*removed*";
      $this->set_picture ("./images/default.jpg");
      $this->is_modified= TRUE;
    }

    $attrs = array("uid","givenName","sn");
    foreach($attrs as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    } 
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
