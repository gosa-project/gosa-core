<?php
class application extends plugin
{
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

  /* Headpage attributes */
  var $last_sorting= "invalid";
  var $applications= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaApplicationExecute", "gosaApplicationName","gosaApplicationIcon",
      "gosaApplicationFlags","gotoLogonScript");
  var $objectclasses= array("top", "gosaApplication");

  var $isReleaseApplikation = false;

  function application ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if(!empty($tmp)) {
      if(!preg_match("/^ou=apps,/",$_SESSION['appfilter']['release'])){
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
    $_SESSION['binary']= $this->iconData;
    $_SESSION['binarytype']= "image/jpeg";
    $this->gosaApplicationIcon= $this->iconData;

    /* This is always an account */
    $this->is_account= TRUE;

    if ($this->dn == "new"){
      if(isset($_SESSION['CurrentMainBase'])){
        $this->base= $_SESSION['CurrentMainBase'];
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,/", "", $this->dn);
    }
  }


  function generateTemplate(){
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

    $smarty= get_smarty();

    if(isset($_POST['download'])){
      header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
      header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
      header("Cache-Control: no-cache");
      header("Pragma: no-cache");
      header("Cache-Control: post-check=0, pre-check=0");
      header("Content-type: application/octet-stream");
      header( "Content-disposition: attachment; filename=".$this->cn.".gotoLogonScript" );
      echo $this->gotoLogonScript;
      exit();
    }

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no application.")."</b>";
      return ($display);
    }

    $head = $this->generateTemplate();
    $this->gotoLogonScript= $this->generateTemplate().preg_replace('/.*### END HEADER ###/s', '', $this->gotoLogonScript);

    if((isset($_POST['upLoad']))&&(isset($_FILES['ScriptFile'])) && chkacl($this->acl,"gotoLogonScript") == ""){
      $str = file_get_contents($_FILES['ScriptFile']['tmp_name']);
      $this->gotoLogonScript = $str;
    }

    /* Fill templating stuff */
    $smarty->assign("cn", $this->cn);
    $smarty->assign("bases", $this->config->idepartments);
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
      if(preg_match("/^chooseBase/",$name) && $once && chkacl($this->acl,"base") == ""){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
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

    /* Get random number for pictures */
    srand((double)microtime()*1000000);
    $smarty->assign("rand", rand(0, 10000));

    /* Variables */
    foreach(array("description", "gosaApplicationExecute", "gosaApplicationName","cn","gotoLogonScript","gosaApplicationIcon") as $val){
      $smarty->assign($val, $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }
    $smarty->assign("baseACL", chkacl($this->acl,"base"));

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
    $smarty->assign("gosaApplicationFlagsACL", chkacl($this->acl, "gosaApplicationFlags"));
    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing application failed"));

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
    if($this->isReleaseApplikation){
      $tmpBase = $this->base;
    } 
 
    if (isset($_POST['cn'])){

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
      if (chkacl ($this->acl, "gosaApplicationFlags") ==""){
        $this->gosaApplicationFlags= "[$flag]";
      }

      /* Check for picture upload */
      if (isset($_FILES['picture_file']['name']) && $_FILES['picture_file']['name'] != "" && chkacl($this->acl,"gosaApplicationIcon") == ""){
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
            gosa_log("Can't Load image");
          }

          /* Resizing image to 147x200 and blur */
          if(!imagick_resize($handle,48,48,IMAGICK_FILTER_GAUSSIAN,0)){
            gosa_log("imagick_resize failed");
          }

          /* Converting image to JPEG */
          if(!imagick_convert($handle,"PNG")) {
            gosa_log("Can't Convert to PNG");
          }

          if(imagick_writeimage($handle,$_FILES['picture_file']['tmp_name'])){
            gosa_log("can't write to specified folder");
          }

          imagick_free($handle);
        }

        /* Activate new picture */
        $this->set_picture($_FILES['picture_file']['tmp_name']);
      }	


      /* Save base, since this is no LDAP attribute */
      if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
        $this->base= $_POST['base'];
      }
    }

    if($this->isReleaseApplikation){
      $this->base = $tmpBase;
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
      $new_dn= "cn=".$this->cn.",ou=apps,".$this->base;
    } else {
      $new_dn= $this->dn;
    }

    $ui= get_userinfo();
    $acl= get_permissions ($new_dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "application", $new_dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a application on this 'Base'.");
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

    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if((!empty($tmp)) && (isset($_SESSION['appfilter']['release']))){
      $baseDn = str_replace($this->config->current['BASE'],$this->base,$_SESSION['appfilter']['release']);
      $baseDn = preg_replace("/ou=apps,.*/","ou=apps,".$this->base,$_SESSION['appfilter']['release']);
      $ldap->ls("(&(objectClass=gosaApplication)(cn=".$this->cn."))",$baseDn,array("cn"));
      if($ldap->count()){
        $attrs = $ldap->fetch();
        if($this->dn != $attrs['dn']) {
          $message[]= _("There's already an application with this 'Name'.");
        }
      }
    }else{
      $ldap->ls("(&(objectClass=gosaApplication)(cn=".$this->cn."))","ou=apps,".$this->base,array("cn"));
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
    } else {

      /* Remove gotoLogonScript if it is empty */
      if(empty($script))    {
        unset($this->attrs['gotoLogonScript']);
      }

      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
    show_ldap_error($ldap->get_error(), _("Saving application failed"));
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
      $_SESSION['binary']= $this->iconData;
      $_SESSION['binarytype']= "image/jpeg";
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

    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if(!empty($tmp)) {
      if(!preg_match("/^ou=apps,/",$_SESSION['appfilter']['release'])){
        $this->isReleaseApplikation = true;  
      }
    }

    if($_SESSION['CurrentMainBase']){
      $this->base= $_SESSION['CurrentMainBase'];
    }
    
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
