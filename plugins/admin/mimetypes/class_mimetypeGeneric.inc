<?php
class mimetype extends plugin
{
	/* CLI vars */
	var $cli_summary		  = "This tab allows you to modify, add or remove mime types in the selected department.";
	var $cli_description	= "Management dialog to handle mime types";
	var $ignore_account 	= true;	// This is always a valid account
	var $cli_parameters		= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");
	var $objectclasses		= array("top","gotoMimeType");
	var $attributes			  = array("cn","gotoMimeApplication","gotoMimeLeftClickAction","gotoMimeIcon",
									"description","gotoMimeFilePattern","gotoMimeEmbeddedApplication",
									"gotoMimeGroup");	

	/* Class vars */
	var $ui;
	var $cn									              = "";					
	var $gotoMimeLeftClickAction			    = "I";
	var $gotoMimeLeftClickAction_I		    = true;
	var $gotoMimeLeftClickAction_E		    = false;
	var $gotoMimeLeftClickAction_Q		    = false;
	var $gotoMimeIcon						          = "*removed*";
	var $description						          = "";
	var $gotoMimeFilePattern				      = array();
	var $gotoMimeApplication				      = array();
	var $gotoMimeEmbeddedApplication	    = array();
	var $gotoMimeGroup						        = "";
	var $iconData							            = NULL;	
	var $base								              = "";
  var $ApplicationList                  = array();  


  /* To prevent errors when using the 'apply' button, we use this variables 
      to manage array attributes */
	var $use_gotoMimeIcon					        = NULL;
	var $use_gotoMimeFilePattern			    = array();
	var $use_gotoMimeApplication			    = array();
	var $use_gotoMimeEmbeddedApplication	= array();

  /* divLists */
  var $DivPatterns						          = NULL;
  var $DivApps							            = NULL;
  var $DivEApps							            = NULL;

  /* Mime type release mode */
  var $isReleaseMimeType 					      = false;

  /* These vars will be copied too, if you use copy&paste mode */
  var $CopyPasteVars        = array("use_gotoMimeFilePattern","use_gotoMimeApplication","use_gotoMimeEmbeddedApplication","iconData",
                                    "gotoMimeLeftClickAction_I","gotoMimeLeftClickAction_E","gotoMimeLeftClickAction_Q","use_gotoMimeIcon");

  var $view_logged  = FALSE;

  /* Select options */
  var $MimeGroups			      = array("application","audio","chemical","image","inode","message","model",
                                      "multipart","text","video","x-conference","x-world");

  /* Orig_Dn is used to verify if this object is new or only edited */		
  var $orig_dn			= "";

  function mimetype(&$config,$dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Save original dn */
    $this->orig_dn = $dn;	

    /* get gotoMimeLeftClickActions I/E/Q */
    if(isset($this->gotoMimeLeftClickAction)){
      $str = $this->gotoMimeLeftClickAction;
      for($i = 0 ; $i < strlen($str) ; $i ++ ){
        $varna = "gotoMimeLeftClickAction_". $str[$i];
        if(isset($this->$varna)){
          $this->$varna = true;
        }
      }
    }

    /* Create list of defined applications in GOsa */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaApplication)(cn=*))",array('cn'));
    $this->ApplicationList = array();
    while($attrs = $ldap->fetch()){
      $this->ApplicationList[$attrs['cn'][0]] = $attrs['cn'][0];
    }

    /* If both radio buttons arn't set, set option I */
    if(!$this->gotoMimeLeftClickAction_I && !$this->gotoMimeLeftClickAction_E){
      $this->gotoMimeLeftClickAction_I = true;
    }

    /* Get list of array attributes */
    foreach(array("gotoMimeFilePattern") as $attr){
      $this->$attr = array();
      if(isset($this->attrs[$attr])){
        $tmp = array();
        for($i = 0 ; $i < $this->attrs[$attr]['count'] ; $i ++){
          $str = $this->attrs[$attr][$i];
          $tmp[] = $str;
        }
        $use_attr = "use_".$attr;
        $this->$use_attr = $tmp;
      }
    }

    /* Get list of array attributes with priority tag ( Test|32 )*/
    foreach(array("gotoMimeApplication","gotoMimeEmbeddedApplication") as $attr){
      $this->$attr = array();
      if(isset($this->attrs[$attr])){
        $tmp = array();
        for($i = 0 ; $i < $this->attrs[$attr]['count'] ; $i ++){
          $str = $this->attrs[$attr][$i];
          $tmp2= split("\|",$str);

          if(count($tmp2) == 2){
            $name = $tmp2[0];
            $prio = $tmp2[1];
            $tmp[$prio] = $name;
          }
        }
        ksort($tmp);
        $use_attr = "use_".$attr;
        $this->$use_attr = $tmp;
      }
    }

    /* Check if release Management is enabled */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)) {
      $this->isReleaseMimeType= true;
    }

    /* Set base */	
    if ($this->dn == "new"){
      if(session::is_set('CurrentMainBase')){
        $this->base= session::get('CurrentMainBase');
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,/", "", $this->dn);
    }

    /* Get icon data */
    if(isset($this->attrs['gotoMimeIcon'])){
      $ldap = $this->config->get_ldap_link();
      $this->iconData = $ldap->get_attribute($this->dn,"gotoMimeIcon");
      $this->saved_attributes['gotoMimeIcon'] = $this->iconData;
    }
    if ($this->iconData == ""){
      $this->set_new_picture("");
    }
    session::set('binary',$this->iconData);
    session::set('binarytype',"image/jpeg");
  }


  function execute()
  {
    $smarty = get_smarty();

    if(!$this->view_logged){
      $this->view_logged =TRUE;
      new log("view","mimetypes/".get_class($this),$this->dn);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
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

        /* A new base was selected, check if it is a valid one */
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$this->dialog->isSelected()])){
          $this->base = $this->dialog->isSelected();
        }

        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Check Posts */
    $posts = array(	
        "/^Pattern_SortUp_/" 	=> array("Action" => "Pattern_SortUp",	"Func" => "ArrayUp",	"Attr" => "use_gotoMimeFilePattern"),
        "/^Pattern_SortDown_/" 	=> array("Action" => "Pattern_SortDown","Func" => "ArrayDown",	"Attr" => "use_gotoMimeFilePattern"),
        "/^Pattern_Remove_/" 	=> array("Action" => "Pattern_Remove",	"Func" => "ArrayRemove","Attr" => "use_gotoMimeFilePattern"),
        "/^Apps_SortUp_/" 		=> array("Action" => "Apps_SortUp",		"Func" => "ArrayUp",    "Attr" => "use_gotoMimeApplication"),
        "/^Apps_SortDown_/" 	=> array("Action" => "Apps_SortDown",	"Func" => "ArrayDown",  "Attr" => "use_gotoMimeApplication"),
        "/^Apps_Remove_/" 		=> array("Action" => "Apps_Remove",		"Func" => "ArrayRemove","Attr" => "use_gotoMimeApplication"),
        "/^EApps_SortUp_/" 		=> array("Action" => "EApps_SortUp",	"Func" => "ArrayUp",    "Attr" => "use_gotoMimeEmbeddedApplication"),
        "/^EApps_SortDown_/" 	=> array("Action" => "EApps_SortDown",	"Func" => "ArrayDown",  "Attr" => "use_gotoMimeEmbeddedApplication"),
        "/^EApps_Remove_/" 		=> array("Action" => "EApps_Remove",	"Func" => "ArrayRemove","Attr" => "use_gotoMimeEmbeddedApplication"));		
    $once = true;

    /* Walk through posts and try to find some commands for us. */
    foreach($_POST as $name => $value){

      /* Walk through possible commands */
      foreach($posts as $regex => $action){

        /* Check if there is a command posted */
        if(preg_match($regex,$name) && $once){
          $once = false;

          /* Get action vars */
          $func 		= $action['Func'];		// Get function name 
          $attr 		= $action['Attr'];		// Get attribute name

          /* Get entry id */
          $s_entry  	= preg_replace($regex,"",$name);
          $s_entry 	= preg_replace("/_[xy]$/","",$s_entry); 

          /* Execute a command with the given attribute and entry 
             e.g. $this->gotoMimeFilePattern = $this->ArrayUp(3,$this->gotoMimeFilePattern) */
          if($this->acl_is_writeable($attr)){
            $this->$attr= $this->$func($s_entry,$this->$attr,true);					 
          }
        }
      }
    }

    /* Set a new icon was requested */
    if(isset($_POST['update_icon']) && (isset($_FILES['picture_file']['name']))){
      $this->set_new_picture($_FILES['picture_file']['tmp_name']);		
    }

    /* Add gotoMimeFilePattern */
    if(isset($_POST['AddNewFilePattern']) && isset($_POST['NewFilePattern'])){
      if($this->acl_is_writeable("gotoMimeFilePattern")){
        $str = $_POST['NewFilePattern'];
        if(!empty($str)){
          $this->use_gotoMimeFilePattern[] = $str;
        }
      }
    }	

    /* Add gotoMimeFilePattern */
    if(isset($_POST['AddNewApplication']) && 
            (isset($_POST['NewApplication']) || isset($_POST['NewApplicationSelect']))){
      if($this->acl_is_writeable("gotoMimeApplication")){
        $str = "";
        if(isset($_POST['NewApplicationSelect']) && !empty($_POST['NewApplicationSelect'])){
          $str = get_post("NewApplicationSelect");
        }
        if(isset($_POST['NewApplication']) && !empty($_POST['NewApplication'])){
          $str = get_post("NewApplication");
        }
        if(!empty($str) && !in_array($str,$this->use_gotoMimeApplication)){
          $this->use_gotoMimeApplication[] = $str;
        }
      }
    }	

    /* Add embedded application 
     *  - From input or from select box 
     */
    if(isset($_POST['AddNewEmbeddedApplication']) && 
            (isset($_POST['NewEmbeddedApplication']) || isset($_POST['NewEmbeddedApplicationSelect']))){
      if($this->acl_is_writeable("gotoMimeEmbeddedApplication")){
        $str = "";
        if(isset($_POST['NewEmbeddedApplicationSelect']) && !empty($_POST['NewEmbeddedApplicationSelect'])){
          $str = get_post('NewEmbeddedApplicationSelect');
        }
        if(isset($_POST['NewEmbeddedApplication']) && !empty($_POST['NewEmbeddedApplication'])){
          $str = get_post('NewEmbeddedApplication');
        }
        if(!empty($str) && !in_array($str,$this->use_gotoMimeEmbeddedApplication)){
          $this->use_gotoMimeEmbeddedApplication[] = $str;
        }
      }
    }	

    /* Create divlists */
    $DivPatterns 	= new divSelectBox("gotoMimePatterns");
    $DivApps 		= new divSelectBox("gotoMimeApplications");
    $DivEApps 		= new divSelectBox("gotoMimeEmbeddedApplications");
    $DivPatterns	-> SetHeight(100);
    $DivApps		-> SetHeight(100);
    $DivEApps		-> SetHeight(100);


    if($this->acl_is_writeable("gotoMimeFilePattern")){
      $Pattern_Actions= " <input type='image' src='images/sort_up.png' 	class='center'	name='Pattern_SortUp_%s' >&nbsp;
      <input type='image' src='images/sort_down.png' 	class='center'  name='Pattern_SortDown_%s'>&nbsp;
      <input type='image' src='images/edittrash.png'	class='center'  name='Pattern_Remove_%s'>";
    }else{
      $Pattern_Actions= "";	
    }

    if($this->acl_is_writeable("gotoMimeApplication")){
      $Apps_Actions 	= " <input type='image' src='images/sort_up.png' 	class='center'	name='Apps_SortUp_%s' >&nbsp;
      <input type='image' src='images/sort_down.png' 	class='center'  name='Apps_SortDown_%s'>&nbsp;
      <input type='image' src='images/edittrash.png'	class='center'  name='Apps_Remove_%s'>";
    }else{
      $Apps_Actions= "";	
    }

    if($this->acl_is_writeable("gotoMimeEmbeddedApplication")){
      $EApps_Actions 	= " <input type='image' src='images/sort_up.png' 	class='center'	name='EApps_SortUp_%s' >&nbsp;
      <input type='image' src='images/sort_down.png' 	class='center'  name='EApps_SortDown_%s'>&nbsp;
      <input type='image' src='images/edittrash.png'	class='center'  name='EApps_Remove_%s'>";
    }else{
      $EApps_Actions= "";	
    }

    /* Before adding some entries check acls */
    if($this->acl_is_readable("gotoMimeFilePattern")){
      foreach($this->use_gotoMimeFilePattern as $key => $pattern){
        $field1 = array("string" => $pattern);
        $field2 = array("string" => preg_replace("/%s/",$key,$Pattern_Actions),"attach"=>"style='border-right:0px;width:50px;'");
        $fields = array($field1,$field2);
        $DivPatterns -> AddEntry($fields);
      }			
    }

    if($this->acl_is_readable("gotoMimeApplication")){
      foreach($this->use_gotoMimeApplication as $key => $pattern){
        $field1 = array("string" => $pattern);
        $field2 = array("string" => preg_replace("/%s/",$key,$Apps_Actions),"attach"=>"style='border-right:0px;width:50px;'");
        $fields = array($field1,$field2);
        $DivApps -> AddEntry($fields);
      }			
    }			
    if($this->acl_is_readable("gotoMimeEmbeddedApplication")){
      foreach($this->use_gotoMimeEmbeddedApplication as $key => $pattern){
        $field1 = array("string" => $pattern);
        $field2 = array("string" => preg_replace("/%s/",$key,$EApps_Actions),"attach"=>"style='border-right:0px;width:50px;'");
        $fields = array($field1,$field2);
        $DivEApps -> AddEntry($fields);
      }
    }			
    $smarty->assign("bases", 						$this->get_allowed_bases());		
    $smarty->assign("base_select", 					$this->base);		
    $smarty->assign("isReleaseMimeType",			$this->isReleaseMimeType);
    $smarty->assign("gotoMimeFilePatterns",			$DivPatterns->DrawList());
    $smarty->assign("gotoMimeApplications",			$DivApps->DrawList());
    $smarty->assign("gotoMimeEmbeddedApplications",	$DivEApps->DrawList());

    $smarty->assign("ApplicationList",$this->ApplicationList);
  
    /* Assign class vars to smarty */
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }	

    /* Assign additional vars that are not included in attributes*/
    foreach(array("gotoMimeLeftClickAction_I","gotoMimeLeftClickAction_E","gotoMimeLeftClickAction_Q") as $attr){
      $smarty->assign($attr,$this->$attr);
    }	

    /* Assign select box options */
    $smarty->assign("gotoMimeGroups",$this->MimeGroups);
    $smarty->assign("gotoMimeIcon"	,$this->get_picture());
    return($smarty->fetch(get_template_path("generic.tpl",TRUE,dirname(__FILE__))));
  }


  function save_object()
  {
    if(isset($_POST['MimeGeneric'])){

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;
  
      /* Only save base if we are not in release mode */
      if(!$this->isReleaseMimeType){

        /* Set new base if allowed */
        $tmp = $this->get_allowed_bases();
        if(isset($_POST['base'])){
          if(isset($tmp[$_POST['base']])){
            $this->base= $_POST['base'];
          }
        }

      }

      /* Save radio buttons */
      if($this->acl_is_writeable("gotoMimeLeftClickAction")){
        if(isset($_POST['gotoMimeLeftClickAction_IE'])){
          $chr = $_POST['gotoMimeLeftClickAction_IE'];
          if($chr == "E"){
            $this->gotoMimeLeftClickAction_E = true;
            $this->gotoMimeLeftClickAction_I = false;
          }else{
            $this->gotoMimeLeftClickAction_E = false;
            $this->gotoMimeLeftClickAction_I = true;
          }
        }
        if(isset($_POST['gotoMimeLeftClickAction_Q'])){
          $this->gotoMimeLeftClickAction_Q = true;
        }else{
          $this->gotoMimeLeftClickAction_Q = false;
        }
      }
    }
  }


  /* save current changes */
  function save()
  {
    /* Create gotoMimeLeftClickAction out of checkboxes and radio buttons */	
    $arr = array ("E","I","Q");
    $str = "";
    foreach ($arr as $Chr){
      $var = "gotoMimeLeftClickAction_".$Chr;
      if($this->$var){
        $str .= $Chr;
      }
    }
    $this->gotoMimeLeftClickAction = $str;

    /* Create array entries with priority tag ( Test|3 )*/
    foreach(array("gotoMimeEmbeddedApplication","gotoMimeApplication") as $attr){
      $i 				= 0;
      $use_attr 		= "use_".$attr;
      $tmp 			= array();
      $this->$attr 	= array();
      foreach($this->$use_attr as $entry){
        $tmp[] = $entry."|".$i ++;
      }
      $this->$attr = $tmp;
    }

    /* Create array entries */
    foreach(array("gotoMimeFilePattern") as $attr){
      $i 				= 0;
      $use_attr 		= "use_".$attr;
      $tmp 			= array();
      $this->$attr 	= array();
      foreach($this->$use_attr as $entry){
        $tmp[] = $entry;
      }
      $this->$attr = $tmp;
    }

    /* Remove Icon if requested  */
    if($this->use_gotoMimeIcon != "*removed*"){
      $this->gotoMimeIcon = $this->iconData;
    }else{
      $this->gotoMimeIcon = "";
    }

    plugin::save();

    /* If this is a newly created object, skip storing those 
       attributes that contain an empty array */
    if($this->orig_dn == "new"){
      foreach(array("gotoMimeEmbeddedApplication","gotoMimeApplication","gotoMimeFilePattern") as $attr){
        if(!count($this->$attr)){
          unset($this->attrs[$attr]);
        }
      }
    }

    $ldap = $this->config->get_ldap_link();
    $ldap-> cd ( $this->config->current['BASE']);
    $ldap->cat($this->dn);
    if($ldap->count()){
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify($this->attrs);
      new log("modify","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      new log("create","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of mime type/generic with dn '%s' failed."),$this->dn));
  }


  /* Remove current mime type */
  function remove_from_parent()
  {
    plugin::remove_from_parent();
    $ldap = $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of mime type/generic with dn '%s' failed."),$this->dn));
    new log("remove","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
      show_ldap_error($ldap->get_error(), sprintf(_("Removing mime type from objectgroup '%s' failed"), $og->dn));
    }
  }


  /* Check given values */
  function check()
  {
    $message = plugin::check();
    if(empty($this->cn)){
      $message[] = _("Please specify a valid name for this mime type.");
    }
    if(!count($this->use_gotoMimeFilePattern)){
      $message[] = _("Please specify at least one file pattern.") ;
    }

    /* Check if there is already a mime type with this cn */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current["BASE"]);

    $mimefilter = session::get('mimefilter');
  
    if($this->isReleaseMimeType && (isset($mimefilter['release']))){
      $baseDn = str_replace($this->config->current['BASE'],$this->base,$mimefilter['release']);
      $baseDn = preg_replace("/".get_ou('mimetypeou').".*/",get_ou('mimetypeou').$this->base,$mimefilter['release']);
      $ldap->ls("(&(objectClass=gotoMimeType)(cn=".$this->cn."))",$baseDn,array("cn"));
      if($ldap->count()){
        $attrs = $ldap->fetch();
        if($this->dn != $attrs['dn']) {
          $message[]= _("There's already a mime type with this 'Name'.");
        }
      }
    }else{
      $ldap->ls("(&(objectClass=gotoMimeType)(cn=".$this->cn."))",get_ou('mimetypeou').$this->base,array("cn"));
      if ($ldap->count()){
        $attrs = $ldap->fetch();
        if($this->dn != $attrs['dn']) {
          $message[]= _("There's already an mime with this 'Name'.");
        }
      }
    }

    return($message);
  }

  /** Helper functions **/

  /* Set a new picture */	
  function set_new_picture($filename)
  {
    if (empty($filename)){
      $filename= "./images/default_icon.png";
      $this->use_gotoMimeIcon= "*removed*";
    }else{
      $this->use_gotoMimeIcon= $filename;
    }

    if (file_exists($filename)){
      $fd = fopen ($filename, "rb");
      $this->iconData= fread ($fd, filesize ($filename));
      session::set('binary',$this->iconData);
      session::set('binarytype',"image/jpeg");
      fclose ($fd);
    }
  }

  /* Get picture link */
  function get_picture()
  {
    session::set('binary',$this->iconData);
    session::set('binarytype',"image/jpeg");
    return("getbin.php");
  }

  /* Transports the given Arraykey one position up*/
  function ArrayUp($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == 1))){
      $before = array_slice($attrs,0,($pos-2));
      $mitte  = array_reverse(array_slice($attrs,($pos-2),2));
      $unten  = array_slice($attrs,$pos);
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* Transports the given Arraykey one position down*/
  function ArrayDown($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == $cn))){
      $before = array_slice($attrs,0,($pos-1));
      $mitte  = array_reverse(array_slice($attrs,($pos-1),2));
      $unten  = array_slice($attrs,($pos+1));
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* return the position of the element in the array */
  function getpos($atr,$attrs)
  {
    $i = 0;
    foreach($attrs as $attr => $name)    {
      $i++;
      if($attr == $atr){
        return($i);
      }
    }
    return(-1);
  }


  /* Remove this element from array */
  function ArrayRemove($key,$array,$reorder = false)	
  {
    if(isset($array[$key])){
      unset($array[$key]);
      if($reorder){
        $tmp = array();
        foreach($array as $entry){
          $tmp[] = $entry;
        }
        $array = $tmp;
      }
    }	
    return($array);
  }


  /* Combine new array */
  function combineArrays($ar0,$ar1,$ar2)
  {
    $ret = array();
    if(is_array($ar0))
      foreach($ar0 as $ar => $a){
        $ret[]=$a;
      }
    if(is_array($ar1))
      foreach($ar1 as $ar => $a){
        $ret[]=$a;
      }
    if(is_array($ar2))
      foreach($ar2 as $ar => $a){
        $ret[]=$a;
      }
    return($ret);
  }


  /* Return a dialog with all fields that must be changed, 
     if we want to copy this entry */
  function getCopyDialog()
  {
    $str = "";

    $smarty = get_smarty();
    $smarty->assign("cn",     		$this->cn);
    $smarty->assign("description",  $this->description);
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));

    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }


  /* Save all */
  function saveCopyDialog()
  {
    $attrs = array("cn","description");
    foreach($attrs as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }
  }

  /* Return plugin informations for acl handling  */ 
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Generic"),
          "plDescription" => _("Mime type generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("mimetypes" => array("description"  => _("Mime types"),
                                                        "objectClass"  => "gotoMimeType")),
          "plProvidedAcls"=> array(
            "cn"                          => _("Name"),
            "gotoMimeGroup"               => _("Mime group"),
            "description"                 => _("Description"),
            "base"                        => _("Base"),
            "gotoMimeApplication"         => _("Application"),
            "gotoMimeLeftClickAction"     => _("Left click action"),
            "gotoMimeIcon"                => _("Icon"),
            "gotoMimeFilePattern"         => _("File patterns"),
            "gotoMimeEmbeddedApplication" => _("Embedded applications"))
          ));

  }

  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $source_o = new mimetype($this->config,$source['dn'],$this->parent);

    foreach(array("gotoMimeLeftClickAction_Q","gotoMimeEmbeddedApplication","gotoMimeApplication","gotoMimeFilePattern","gotoMimeIcon","iconData") as $name){
      $use_attr     = "use_".$name;
      if(isset($this->$use_attr)){
        $this->$use_attr= $source_o->$use_attr;
      }
      $this->$name = $source_o->$name;
    } 
    foreach($this->attributes as $name){
      $this->$name = $source_o->$name;
    }

    if($this->iconData){
      $this->use_gotoMimeIcon ="Not emtpy, causes icon to be written.";
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
