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
  var $iconData;

  /* Headpage attributes */
  var $last_sorting= "invalid";
  var $applications= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gosaApplicationExecute", "gosaApplicationName",
			"gosaApplicationFlags");
  var $objectclasses= array("top", "gosaApplication");

  function application ($config, $dn= NULL)
  {
  	plugin::plugin ($config, $dn);

	/* Load icon */
	$ldap= $config->get_ldap_link();
	$this->iconData= $ldap->get_attribute($dn, "gosaApplicationIcon");
	if ($this->iconData == ""){
		$this->set_picture("");
	}
	$_SESSION['picture']= $this->iconData;
	$this->gosaApplicationIcon= $this->iconData;

	/* This is always an account */
	$this->is_account= TRUE;

        if ($this->dn == "new"){
		$ui= get_userinfo();
		$this->base= dn2base($ui->dn);
	} else {
                $this->base= preg_replace ("/^[^,]+,[^,]+,/", "", $this->dn);
        }
  }

  function execute()
  {
 	/* Do we represent a valid group? */
	if (!$this->is_account && $this->parent == NULL){
		 $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
			 _("This 'dn' is no application.")."</b>";
		 return ($display);
	}
  
	/* Fill templating stuff */
	$smarty= get_smarty();
	$smarty->assign("cn", $this->cn);
	$smarty->assign("bases", $this->config->idepartments);
	if ($this->dn == "new"){
		$smarty->assign("selectmode", "");
		$smarty->assign("namemode", "");
	} else {
		$smarty->assign("namemode", "readonly");
		$smarty->assign("selectmode", "disabled");
	}

	/* Get random number for pictures */
	srand((double)microtime()*1000000);
	$smarty->assign("rand", rand(0, 10000));

	/* Variables */
	foreach(array("description", "gosaApplicationExecute", "gosaApplicationName") as $val){
		$smarty->assign($val, $this->$val);
		$smarty->assign($val."ACL", chkacl($this->acl, $val));
	}

	/* Checkboxes */
	foreach (array("G" => "exec_for_groupmembers", "O" => "overwrite_config",
			"D" => "place_on_desktop", "M" => "place_in_startmenu") as $key => $val){
		if (preg_match("/$key/", $this->gosaApplicationFlags)){
			$smarty->assign("$val", "checked");
		} else {
			$smarty->assign("$val", "");
		}
	}
	$smarty->assign("base_select", $this->base);
	$smarty->assign("gosaApplicationFlagsACL", chkacl($this->acl, "gosaApplicationFlags"));

	/* Show main page */
	return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function remove_from_parent()
  {
	$ldap= $this->config->get_ldap_link();
	$ldap->rmDir($this->dn);

	/* Optionally execute a command after we're done */
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


  /* Save data to object */
  function save_object()
  {
	if (isset($_POST['cn'])){
		plugin::save_object();

		/* Save application flags */
		$flag= "";
		if (isset($_POST['exec_for_groupmembers']) && $_POST['exec_for_groupmembers'] == 1){
			$flag.= "G";
		}
		if (isset($_POST['place_on_desktop']) && $_POST['place_on_desktop'] == 1){
			$flag.= "D";
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
  }


  /* Check values */
  function check()
  {
	$message= array();

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
	if ($this->gosaApplicationExecute == ""){
		$message[]= _("Required field 'Execute' is not filled.");
	}

	/* Check for existing application */
	$ldap= $this->config->get_ldap_link();
	$ldap->cd($this->config->current["BASE"]);
	$ldap->search("(&(objectClass=gosaApplication)(cn=$this->cn))");
	$ldap->fetch();
	if ($ldap->count() != 0 && $this->dn == "new"){
		$message[]= _("There's already an application with this 'Name'.");
	}

	return $message;
  }


  /* Save to LDAP */
  function save()
  {
        plugin::save();
		$this->attrs["gosaApplicationIcon"]= $this->gosaApplicationIcon;

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn);
        $a= $ldap->fetch();
        if (count($a)){
        	$ldap->cd($this->dn);
			$ldap->modify($this->attrs);
			$this->handle_post_events("modify");
        } else {
			$ldap->cd($this->config->current['BASE']);
			$ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        	$ldap->cd($this->dn);
			$ldap->add($this->attrs);
			$this->handle_post_events("add");
        }
        show_ldap_error($ldap->get_error());
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
		$_SESSION['picture']= $this->iconData;
		$this->gosaApplicationIcon= $this->iconData;

		fclose ($fd);
	}
  }


}

?>
