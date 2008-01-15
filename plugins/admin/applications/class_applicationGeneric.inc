<?php
class application extends plugin
{
  /* CLI vars */
  var $cli_summary= "Handling of GOsa's application object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* application attributes */
  var $cn= "";
  var $description= "";
  var $base= "";
  var $gosaApplicationExecute= "";
  var $gosaApplicationName= "";
  var $gosaApplicationFlags= "";
  var $gosaApplicationIcon= "";
  var $gotoLogonScript	="";
  var $iconData;
  var $view_logged = FALSE;

  /* Headpage attributes */
  var $last_sorting= "invalid";
  var $applications= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaApplicationExecute", "gosaApplicationName","gosaApplicationIcon",
      "gosaApplicationFlags","gotoLogonScript");
  var $objectclasses= array("top", "gosaApplication");

  var $isReleaseApplikation = false;

  function application (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $appfilter = session::get('appfilter')    ;

    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)) {
      if(!preg_match("/^".get_ou('applicationou')."/",$appfilter['release'])){
        $this->isReleaseApplikation = true;  
      }
    }

    /* Load icon */
    $ldap= $config->get_ldap_link();
    if ($dn != 'new'){
      $this->iconData= $ldap->get_attribute($dn, "gosaApplicationIcon");
      $this->saved_attributes['gosaApplicationIcon'] = $this->iconData;
    }
    if ($this->iconData == ""){
      $this->set_picture("");
    }
    session::set('binary',$this->iconData);
    session::set('binarytype',"image/jpeg");
    $this->gosaApplicationIcon= $this->iconData;

    /* This is always an account */
    $this->is_account= TRUE;

    if ($this->dn == "new"){
      if(session::is_set('CurrentMainBase')){
        $this->base = session::get('CurrentMainBase');
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {

      if($this->isReleaseApplikation){
        $this->base = preg_replace("/^.*,".get_ou('applicationou')."/","",$this->dn);
      }else{
        $this->base= preg_replace ("/^[^,]+,[^,]+,/", "", $this->dn);
      }
    }
  }


  function generateTemplate()
  {
    $str= "# This code is part of GOsa (https://gosa.gonicus.de)\n#\n";

    $values = array();
    $names 	= array();
    if($this->parent->by_object['applicationParameters']->is_account){
      $names = $this->parent->by_object['applicationParameters']->option_name;
      $values = $this->parent->by_object['applicationParameters']->option_value;
    }

    if (count($names)){
      $str .="# This plugin handles these environment variables:\n";
    } else {
      $str .="# This plugin handles no environment variables.\n";
    }

    foreach($names as $index => $name){

      // Fix length
      for($i = strlen($name) ; $i < 30 ; $i++){
        $name= $name." ";
      }
      if((isset($values[$index]))&&(!empty($values[$index]))){
        $str.= "# ".$name."\t(e.g. '".$values[$index]."')\n";
      }else{
        $str.= "# ".$name."\t("._("no example").")\n";
      }
    }
    $str .= "#\n".
      "# Don't remove the following tag, it is used for header update.\n".
      "### END HEADER ###";

    return($str);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","application/".get_class($this),$this->dn);
    }

    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
 
    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no application.")."</b>";
      return ($display);
    }

    /* Download requested */
    foreach($_POST as $name => $value){
      if(preg_match("/^downloadScript/",$name)){
        session::set('binary',$this->gotoLogonScript);
        session::set('binarytype',"octet-stream");
        session::set('binaryfile',$this->cn.".gosaApplication");
        header("location: getbin.php ");
        exit();
      }
    }

    /* Reassign picture data, sometimes its corrupt cause we started a download of application scripts */
    session::set('binary',$this->iconData);
    session::set('binarytype',"image/jpeg");
    
    $smarty->assign("rand", rand(0, 10000));
    $head = $this->generateTemplate();
    $this->gotoLogonScript= $this->generateTemplate().preg_replace('/.*### END HEADER ###/s', '', $this->gotoLogonScript);

    if((isset($_POST['upLoad']))&&(isset($_FILES['ScriptFile']))){
      $str = file_get_contents($_FILES['ScriptFile']['tmp_name']);
      $this->gotoLogonScript = $str;
    }

    /* Fill templating stuff */
    $smarty->assign("cn", $this->cn);
    $smarty->assign("bases", $this->get_allowed_bases());
    if ($this->dn == "new"){
      $smarty->assign("selectmode", "");
      $smarty->assign("namemode", "");
    } else {
      $smarty->assign("namemode", "readonly");
      $smarty->assign("selectmode", "disabled");
    }
    
    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
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
 
        /* Just allow selection valid bases */ 
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$this->dialog->isSelected()])){
          $this->base = $this->dialog->isSelected();
        }
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Get random number for pictures */
    srand((double)microtime()*1000000);
    $smarty->assign("rand", rand(0, 10000));

    /* Variables */
    foreach(array("description", "gosaApplicationExecute", "gosaApplicationName","cn") as $val){
      $smarty->assign($val, $this->$val);
    }

    /* Checkboxes */
    foreach (array("G" => "exec_for_groupmembers", "O" => "overwrite_config",
          "L" => "place_on_kicker",
          "D" => "place_on_desktop", "M" => "place_in_startmenu") as $key => $val){
      if (preg_match("/$key/", $this->gosaApplicationFlags)){
        $smarty->assign("$val", "checked");
      } else {
        $smarty->assign("$val", "");
      }
    }

    $smarty->assign("isReleaseApplikation" , $this->isReleaseApplikation);
    $smarty->assign("gotoLogonScript",htmlentities($this->gotoLogonScript, ENT_COMPAT, 'UTF-8'));
    $smarty->assign("base_select", $this->base);
    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);
    new log("remove","application/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of application with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
      show_ldap_error($ldap->get_error(), sprintf(_("Removing application from objectgroup '%s' failed"), $og->dn));
    }
    $ldap->search ("(&(objectClass=posixGroup)(gosaMemberApplication=".$this->cn."))", array("cn"));
    while ($attrs= $ldap->fetch()){
      $ag= new appgroup($this->config, $ldap->getDN());
      $ag->removeApp($this->cn);
      $ag->save ();
      show_ldap_error($ldap->get_error(), sprintf(_("Removing application from group '%s' failed"), $ag->dn));
    }

  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['cn'])){

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;

      /* Save attributes */
      parent::save_object();

      /* Save application flags */
      $flag= "";
      if (isset($_POST['exec_for_groupmembers']) && $_POST['exec_for_groupmembers'] == 1){
        $flag.= "G";
      }
      if (isset($_POST['place_on_desktop']) && $_POST['place_on_desktop'] == 1){
        $flag.= "D";
      }
      if (isset($_POST['place_on_kicker']) && $_POST['place_on_kicker'] == 1){
        $flag.= "L";
      }
      if (isset($_POST['place_in_startmenu']) && $_POST['place_in_startmenu'] == 1){
        $flag.= "M";
      }
      if (isset($_POST['overwrite_config']) && $_POST['overwrite_config'] == 1){
        $flag.= "O";
      }
      if ($this->acl_is_writeable("gosaApplicationFlags")){
        $this->gosaApplicationFlags= "[$flag]";
      }

      /* Remove current picture */
      if(isset($_POST['remove_picture'])){
        $this->set_picture("");
      }

      /* Check for picture upload */
      if (isset($_FILES['picture_file']['name']) && $_FILES['picture_file']['name'] != ""){

        if (!is_uploaded_file($_FILES['picture_file']['tmp_name'])) {
          print_red (_("The specified picture has not been uploaded correctly."));
        }

        if (!function_exists("imagick_blob2image")){
          /* Get temporary file name for conversation */
          $fname = tempnam ("/tmp", "GOsa");

          /* Open file and write out photoData */
          $fp = fopen ($fname, "w");
          fwrite ($fp, $_FILES['picture_file']['tmp_name']);
          fclose ($fp);

          /* Build conversation query. Filename is generated automatically, so
             we do not need any special security checks. Exec command and save
             output. For PHP safe mode, you'll need a configuration which respects
             image magick as executable... */
          $query= "convert -size 48x48 $fname -resize 48x48 +profile \"*\" -";
          @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $query, "Execute");

          /* Read data written by convert */
          $output= "";
          $sh= popen($query, 'r');
          while (!feof($sh)){
            $output.= fread($sh, 4096);
          }
          pclose($sh);

          unlink($fname);	
        } else {

          /* Load the new uploaded Photo */
          if(!$handle  =  imagick_ReadImage($_FILES['picture_file']['tmp_name'])){
            print_red(_("Can't access uploaded image."));
          }

          /* Resizing image to 147x200 and blur */
          if(!imagick_resize($handle,48,48,IMAGICK_FILTER_GAUSSIAN,0)){
            print_red(_("Uploaded image could not be resized, possilby the image magick extension is missing."));
          }

          /* Converting image to JPEG */
          if(!imagick_convert($handle,"PNG")) {
            print_red(_("Could not convert image to png, possilby the image magick extension is missing."));
          }

          if(!imagick_writeimage($handle,$_FILES['picture_file']['tmp_name'])){
            print_red(sprintf(_("Could not save uploaded image to %s."),$_FILES['picture_file']['tmp_name']));
          }

          imagick_free($handle);
        }

        /* Activate new picture */
        $this->set_picture($_FILES['picture_file']['tmp_name']);
      }	

      if(!$this->isReleaseApplikation){
        $tmp = $this->get_allowed_bases();
        if(isset($_POST['base'])){
          if(isset($tmp[$_POST['base']])){
            $this->base= $_POST['base'];
          }
        }
      }
    }
  }


  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!preg_match("#^/#",$this->gosaApplicationExecute)){
      $message[]=(_("Specified execute path must start with '/'."));
    }

    /* Permissions for that base? */
    if ($this->base != ""){
      $new_dn= "cn=".$this->cn.",".get_ou('applicationou').$this->base;
    } else {
      $new_dn= $this->dn;
    }


    if($this->dn == "new"){
      $this->set_acl_base($this->base);
    }

    /* All required fields are set? */
    if ($this->cn == ""){
      $message[]= _("Required field 'Name' is not filled.");
    }

    if(preg_match("/[^a-z0-9]/",$this->cn))	{
      $message[]=_("Invalid character in application name. Only a-z 0-9 are allowed.");
    }

    if ($this->gosaApplicationExecute == ""){
      $message[]= _("Required field 'Execute' is not filled.");
    }

    /* Check for existing application */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current["BASE"]);

    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    $appfilter = session::get('appfilter');
    if((!empty($tmp)) && (isset($appfilter['release']))){
      $baseDn = str_replace($this->config->current['BASE'],$this->base,$appfilter['release']);
      $baseDn = preg_replace("/".get_ou('applicationou').".*/",get_ou('applicationou').$this->base,$appfilter['release']);
      $ldap->ls("(&(objectClass=gosaApplication)(cn=".$this->cn."))",$baseDn,array("cn"));
      if($ldap->count()){
        $attrs = $ldap->fetch();
        if($this->dn != $attrs['dn']) {
          $message[]= _("There's already an application with this 'Name'.");
        }
      }
    }else{
      $ldap->ls("(&(objectClass=gosaApplication)(cn=".$this->cn."))",get_ou('applicationou').$this->base,array("cn"));
      if ($ldap->count()){
        $attrs = $ldap->fetch();
        if($this->dn != $attrs['dn']) {
          $message[]= _("There's already an application with this 'Name'.");
        }
      }
    }
    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Get application script without header part, to check if we must save the script itself */
    $script = preg_replace('/.*### END HEADER ###/s', '', $this->gotoLogonScript);

    plugin::save();
    $this->attrs["gosaApplicationIcon"]= $this->gosaApplicationIcon;

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn, array('dn'));

    $a= $ldap->fetch();
    if (count($a)){

      /* Remove gotoLogonScript if it is empty */
      if(empty($script))    {
        $this->attrs['gotoLogonScript'] = array();
      }

      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      $this->handle_post_events("modify");
      new log("modify","application/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    } else {
      
      /* Remove gotoLogonScript if it is empty */
      if(empty($script))    {
        unset($this->attrs['gotoLogonScript']);
      }

      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      new log("create","application/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("add");
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of application with dn '%s' failed."),$this->dn));
  }

  function set_picture($filename)
  {
    if (!is_file($filename)){
      $filename= "./images/default_icon.png";
      $this->gosaApplicationIcon= "*removed*";
    }

    if (file_exists($filename)){
      $fd = fopen ($filename, "rb");
      $this->iconData= fread ($fd, filesize ($filename));
      session::set('binary',$this->iconData);
      session::set('binarytype',"image/jpeg");
      $this->gosaApplicationIcon= $this->iconData;

      fclose ($fd);
    }
  }

  function getCopyDialog()
  {
    $vars = array("cn");

    $str ="<h2>"._("Application settings")."</h2>
      <table>
      <tr>
      <td>".
      _("Application name"). 
      "</td>  
      <td>
      <input id='gosaApplicationName' name='cn' size='35' maxlength='60' 
      value='".$this->cn."' 
      title='"._("Application name to be displayed (i.e. below icons)")."'>                     
      </td>
      </tr>
      </table>";
    $ret = array();
    $ret['status'] = "";
    $ret['string'] = $str;
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
    $source_o = new application($this->config,$source['dn']);
    $this->gosaApplicationIcon = $source_o->gosaApplicationIcon;     
  }


  /* Return plugin informations for acl handling
      #FIXME FAIscript seams to ununsed within this class... */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Generic"),
          "plDescription" => _("Application generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("application" => array("description"  => _("Application"),
                                                          "objectClass"  => "gosaApplication")),
          "plProvidedAcls"=> array(
            "cn"                => _("Name"),
            "base"              => _("Base"),
            "description"       => _("Description"),
            "gosaApplicationExecute"  => _("Execute"),
            "gosaApplicationName"     => _("Name"),
            "gosaApplicationIcon"     => _("Icon"),
            "gosaApplicationFlags"    => _("Flag"),
            "gotoLogonScript"         => _("Script content"),

            "exec_for_groupmembers" => _("Only executable for members"),              // G
            "place_on_desktop"      => _("Place icon on members desktop"),            // D
            "place_on_kicker"       => _("Place entry in members launch bar"),        // L
            "place_in_startmenu"    => _("Place entry in members startmenu"),         // M
            "overwrite_config"      => _("Replace user configuration on startup"))  // O
            ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
