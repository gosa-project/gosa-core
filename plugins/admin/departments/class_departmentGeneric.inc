<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003  Cajus Pollmeier

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class department extends plugin
{
	/* department attributes */
	var $ou= "";
	var $description= "";
	var $base= "";
	var $st= "";
	var $l= "";
	var $postalAddress= "";
	var $businessCategory= "";
	var $telephoneNumber= "";
	var $facsimileTelephoneNumber= "";
	var $orig_dn= "";
	var $is_administrational_unit= false;
	var $gosaUnitTag= "";
  var $view_logged = FALSE;
	var $rec_dst=false;	// Destination for recursive move
	var $rec_src=false;	// Source for recursive move 
	var $rec_cpy=false;	// Is recursive move requested ? 

	/* Headpage attributes */
	var $last_dep_sorting= "invalid";
	var $departments= array();

  var $must_be_tagged = false;

	/* attribute list for save action */
	var $attributes= array("ou", "description", "businessCategory", "st", "l", "postalAddress",
			"telephoneNumber", "facsimileTelephoneNumber", "gosaUnitTag");
	var $objectclasses= array("top", "gosaDepartment", "organizationalUnit");
  var $initially_was_tagged = false;

  
  /* Temporary disable the Unit Tag functionalityi.
     This is not used anymore, cause of the new acl implementation. 
     Setting this to TRUE will disable 
      all POST / GET activity, "Department tagging" and the checkbox within the generic template.
   */
  var $UnitTagDiabled     = TRUE;
  var $orig_base = "";
  var $orig_ou = "";

	function department (&$config, $dn)
	{

		plugin::plugin($config, $dn);
		$this->is_account= TRUE;
		$this->ui= get_userinfo();
		$this->dn= $dn;
		$this->orig_dn= $dn;
		$this->orig_ou= $this->ou;
		$this->config= $config;

		/* Set base */
		if ($this->dn == "new"){
			$ui= get_userinfo();
			if(isset($_SESSION['CurrentMainBase'])){
				$this->base= $_SESSION['CurrentMainBase'];
			}else{
				$this->base= dn2base($ui->dn);
			}
		} else {
			$this->base= preg_replace ("/^[^,]+,/", "", $this->dn);
		}

    $this->orig_base = $this->base;

		/* Is administrational Unit? */
		if ($dn != "new" && in_array_ics('gosaAdministrativeUnit', $this->attrs['objectClass'])){
			$this->is_administrational_unit= true;
      $this->initially_was_tagged = true;
		}
	}

	function execute()
	{
		/* Call parent execute */
		plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","department/".get_class($this),$this->dn);
    }

		/* Reload departments */
		$this->config->get_departments($this->dn);
		$this->config->make_idepartments();
		$smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

		/* Base select dialog */
		$once = true;
		foreach($_POST as $name => $value){
			if((preg_match("/^chooseBase/",$name) && $once) && ($this->acl_is_moveable())){
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

		/* Hide all departments, that are subtrees of this department */
    $bases = $this->get_allowed_bases();
		if(($this->dn == "new")||($this->dn == "")){
			$tmp = $bases;
		}else{
			$tmp	= array();	
			foreach($bases as $dn=>$base){
				$fixed = str_replace("/","\\",$this->dn);
				/* Only attach departments which are not a subtree of this one */
				if(!preg_match("/".$fixed."/",$dn)){
					$tmp[$dn]=$base;
				}
			}
		}
		$smarty->assign("bases", $tmp);

		foreach ($this->attributes as $val){
			$smarty->assign("$val", $this->$val);
		}
		$smarty->assign("base_select", $this->base);

    /* Set admin unit flag */
    $smarty->assign("UnitTagDiabled",$this->UnitTagDiabled);
    if(!$this->UnitTagDiabled){
      if ($this->is_administrational_unit) {
        $smarty->assign("unitTag", "checked");
      } else {
        $smarty->assign("unitTag", "");
      }
    }

		return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
	}

	function clear_fields()
	{
		$this->dn   = "";
		$this->base = "";

		foreach ($this->attributes as $val){
			$this->$val= "";
		}
	}

	function remove_from_parent()
	{
		$ldap= $this->config->get_ldap_link();
		$ldap->cd ($this->dn);
		$ldap->recursive_remove();
    new log("remove","department/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of department with dn '%s' failed."),$this->dn));

		/* Optionally execute a command after we're done */
		$this->handle_post_events('remove');
	}

	function must_be_tagged()
	{
		return $this->must_be_tagged;
	}

	function am_i_moved()
	{
		return $this->rec_cpy;
	}


	/* Save data to object */
	function save_object()
	{
		if (isset($_POST['dep_generic_posted'])){

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;

      /* Set new base if allowed */
      $tmp = $this->get_allowed_bases();
      if(isset($_POST['base'])){
        if(isset($tmp[$_POST['base']])){
          $this->base= $_POST['base'];
        }
      }

      /* Save tagging flag */
      if(!$this->UnitTagDiabled){
        if ($this->acl_is_writeable("unitTag")){
          if (isset($_POST['unitTag'])){
            $this->is_administrational_unit= true;
          } else {
            $this->is_administrational_unit= false;
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

		/* Check for presence of this department */
		$ldap= $this->config->get_ldap_link();
    $ldap->ls ("(&(ou=".$this->ou.")(objectClass=organizationalUnit))", $this->base, array('dn'));
    if ($this->orig_dn == "new" && $ldap->count()){
			$message[]= _("Department with that 'Name' already exists.");
		} elseif ($this->orig_dn != $this->dn && !($attrs === FALSE)){
			$message[]= _("Department with that 'Name' already exists.");
		}

		/* All required fields are set? */
		if ($this->ou == ""){
			$message[]= _("Required field 'Name' is not set.");
		}
		if ($this->description == ""){
			$message[]= _("Required field 'Description' is not set.");
		}

    if(is_department_name_reserved($this->ou,$this->base)){
      $message[]= sprintf(_("The field 'Name' contains the reserved word '%s'. Please choose another name."),$this->ou);
    }

		if (preg_match ('/[#+:=>\\\\\/]/', $this->ou)){
			$message[]= _("The field 'Name' contains invalid characters.");
		}
		if (!is_phone_nr($this->telephoneNumber)){
			$message[]= _("The field 'Phone' contains an invalid phone number.");
		}
		if (!is_phone_nr($this->facsimileTelephoneNumber)){
			$message[]= _("The field 'Fax' contains an invalid phone number.");
		}

		return $message;
	}


	/* Save to LDAP */
	function save()
	{
		$ldap= $this->config->get_ldap_link();

    if(!$this->UnitTagDiabled){

      /* Add tag objects if needed */
      if ($this->is_administrational_unit){

        /* If this wasn't tagged before add oc an reset unit tag */
        if(!$this->initially_was_tagged){
          $this->objectclasses[]= "gosaAdministrativeUnit";
          $this->gosaUnitTag= "";

          /* It seams that this method is called twice, 
             set this to true. to avoid adding this oc twice */
          $this->initially_was_tagged = true;
        }

        if ($this->gosaUnitTag == ""){

          /* It's unlikely, but check if already used... */
          $try= 5;
          $ldap->cd($this->config->current['BASE']);
          while ($try--){

            /* Generate microtime stamp as tag */
            list($usec, $sec)= explode(" ", microtime());
            $time_stamp= preg_replace("/\./", "", $sec.$usec);

            $ldap->search("(&(objectClass=gosaAdministrativeUnit)(gosaUnitTag=$time_stamp))",array("gosaUnitTag"));
            if ($ldap->count() == 0){
              break;
            }
          }
          if($try == 0) {
            print_red(_("Fatal error: Can't find an unused tag to mark the administrative unit!"));
            return;
          }
          $this->gosaUnitTag= preg_replace("/\./", "", $sec.$usec);
        }
      }
    }
		plugin::save();

    if(!$this->UnitTagDiabled){

      /* Remove tag information if needed */
      if (!$this->is_administrational_unit){
        $tmp= array();

        /* Remove gosaAdministrativeUnit from this plugin */
        $has_unit_tag= false;
        foreach($this->attrs['objectClass'] as $oc){
          if (!preg_match("/^gosaAdministrativeUnit$/i", $oc)){
            $tmp[]= $oc;
          }
          if (preg_match("/^gosaAdministrativeUnitTag$/i", $oc)){
            $has_unit_tag= true;
          }
        }
        $this->attrs['objectClass']= $tmp;

        if(!$has_unit_tag && isset($this->attrs['gosaUnitTag'])){
          $this->attrs['gosaUnitTag']= array();
        }
        $this->gosaUnitTag = "";
      }
    }

		/* Write back to ldap */
		$ldap= $this->config->get_ldap_link();
		$ldap->cat($this->dn, array('dn'));
		$a= $ldap->fetch();
		$ldap->cd($this->dn);

		if (count($a)){
			$this->cleanup();
			$ldap->modify ($this->attrs); 
      new log("modify","department/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
			$this->handle_post_events('modify');
		} else {
			$ldap->add($this->attrs);
			$this->handle_post_events('add');
      new log("create","department/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
		}
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of department with dn '%s' failed."),$this->dn));


    if(!$this->UnitTagDiabled){
      /* The parameter forces only to set must_be_tagged, and don't touch any objects 
         This will be done later */
      $this->tag_objects(true);

      /* Fix tagging if needed */
      if (!$this->is_administrational_unit){
        $this->handle_object_tagging();
      }
    }
    
    /* Optionally execute a command after we're done */
		$this->postcreate();
    return(false);
	}


  function ShowMoveFrame()
  {
    $smarty = get_smarty();
    $smarty->assign("src","?plug=".$_GET['plug']."&amp;PerformRecMove");
    $smarty->assign("message","As soon as the move operation has finished, you can scroll down to end of the page and press the 'Continue' button to continue with the department management dialog.");
    $display=  $smarty->fetch(get_template_path("dep_iframe.tpl",TRUE));
    return($display);
  }

  function ShowTagFrame()
  {
    $smarty = get_smarty();
    $smarty->assign("src","?plug=".$_GET['plug']."&TagDepartment");
    $smarty->assign("message","As soon as the tag operation has finished, you can scroll down to end of the page and press the 'Continue' button to continue with the department management dialog.");
    $display=  $smarty->fetch(get_template_path("dep_iframe.tpl",TRUE));
    return($display);
  }

	/* Tag objects to have the gosaAdministrativeUnitTag */
	function tag_objects($OnlySetTagFlag = false)
	{
    if(!$OnlySetTagFlag){
      $smarty= get_smarty();
      /* Print out html introduction */
      echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
        <html>
        <head>
        <title></title>
        <style type="text/css">@import url("themes/default/style.css");</style>
        <script language="javascript" src="include/focus.js" type="text/javascript"></script>
        </head>
        <body style="background: none; margin:4px;" id="body" >
        ';
      echo "<h3>".sprintf(_("Tagging '%s'."),"<i>".@LDAP::fix($this->dn)."</i>")."</h3>";
    }

		$add= $this->is_administrational_unit;
		$len= strlen($this->dn);
		$ldap= $this->config->get_ldap_link();
		$ldap->cd($this->dn);
		if ($add){
			$ldap->search('(!(&(objectClass=gosaAdministrativeUnitTag)(gosaUnitTag='.
								$this->gosaUnitTag.')))', array('dn'));
		} else {
			$ldap->search('objectClass=gosaAdministrativeUnitTag', array('dn'));
		}

		while ($attrs= $ldap->fetch()){

			/* Skip self */
			if ($attrs['dn'] == $this->dn){
				continue;
			}

			/* Check for confilicting administrative units */
			$fix= true;
			foreach ($this->config->adepartments as $key => $tag){
				/* This one is shorter than our dn, its not relevant... */
				if ($len >= strlen($key)){
					continue;
				}

				/* This one matches with the latter part. Break and don't fix this entry */
				if (preg_match('/(^|,)'.normalizePreg($key).'$/', $attrs['dn'])){
					$fix= false;
					break;
				}
			}

      /* Fix entry if needed */
      if ($fix){
        if($OnlySetTagFlag){
          $this->must_be_tagged =true;
          return;
        }
				$this->handle_object_tagging($attrs['dn'], $this->gosaUnitTag, TRUE );
			}
		}
    if(!$OnlySetTagFlag){
      echo '<p class="seperator">&nbsp;</p>';
      echo "<div style='width:100%;text-align:right;'><form name='form' method='post' action='?plug=".$_GET['plug']."' target='_parent'>
        <br><input type='submit' name='back' value='"._("Continue")."'>
        </form></div>";
    }
	}


	/* Move/Rename complete trees */
	function recursive_move($src_dn, $dst_dn,$force = false)
	{
    /* If force == false prepare to recursive move this object from src to dst 
        on the next call. */
		if(!$force){
			$this->rec_cpy 	= true;
			$this->rec_src	= $src_dn;
			$this->rec_dst	= $dst_dn;
		}else{

      /* If this is called, but not wanted, abort */
			if(!$this->rec_cpy){ 
				return;
			}

			$src_dn = $this->rec_src;
			$dst_dn = $this->rec_dst;

			/* Print header to have styles included */
			$smarty= get_smarty();

      echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
        <html>
        <head>
        <title></title>
        <style type="text/css">@import url("themes/default/style.css");</style>
        <script language="javascript" src="include/focus.js" type="text/javascript"></script>
        </head>
        <body style="background: none; margin:4px;" id="body" >
        ';
			echo "<h3>".sprintf(_("Moving '%s' to '%s'"),"<i>".@LDAP::fix($src_dn)."</i>","<i>".@LDAP::fix($dst_dn)."</i>")."</h3>";


			/* Check if the destination entry exists */
			$ldap= $this->config->get_ldap_link();

			/* Check if destination exists - abort */
			$ldap->cat($dst_dn, array('dn'));
			if ($ldap->fetch()){
				trigger_error("Recursive_move ".@LDAP::fix($dst_dn)." already exists.",
						E_USER_WARNING);
				echo sprintf("Recursive_move: '%s' already exists", @LDAP::fix($dst_dn))."<br>"; 
				return (FALSE);
			}

			/* Perform a search for all objects to be moved */
			$objects= array();
			$ldap->cd($src_dn);
			$ldap->search("(objectClass=*)", array("dn"));
			while($attrs= $ldap->fetch()){
				$dn= $attrs['dn'];
				$objects[$dn]= strlen($dn);
			}

			/* Sort objects by indent level */
			asort($objects);
			reset($objects);

			/* Copy objects from small to big indent levels by replacing src_dn by dst_dn */
			foreach ($objects as $object => $len){


				$src= str_replace("\\","\\\\",$object);
				$dst= preg_replace("/".str_replace("\\","\\\\",$src_dn)."$/", "$dst_dn", $object);
				$dst= str_replace($src_dn,$dst_dn,$object);

				echo "<b>"._("Object").":</b> ".@LDAP::fix($src)."<br>";

        $this->update_acls($object, $dst,TRUE);

				if (!$this->copy($src, $dst)){
					echo "<font color='#FF0000'><br>".sprintf(_("FAILED to copy %s, aborting operation"),@LDAP::fix($src))."</font>";
					return (FALSE);
				}
        echo "<script language=\"javascript\" type=\"text/javascript\">scrollDown2();</script>" ;
				flush();
			}

			/* Remove src_dn */
			$ldap->cd($src_dn);
			$ldap->recursive_remove();
      $this->dn = $this->rec_dst;
			$this->rec_src = $this->rec_dst = "";
			$this->rec_cpy =false;

			echo '<p class="seperator">&nbsp;</p>';

			echo "<div style='width:100%;text-align:right;'><form name='form' method='post' action='?plug=".$_GET['plug']."' target='_parent'>
				<br><input type='submit' name='back' value='"._("Continue")."'>
				</form></div>";

      echo "<script language=\"javascript\" type=\"text/javascript\">scrollDown2();</script>" ;
			echo "</body></html>";

			return (TRUE);
		}
	}


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array("plShortName"   => _("Generic"),
                  "plDescription" => _("Departments"),
                  "plSelfModify"  => FALSE,
                  "plPriority"    => 0,
                  "plDepends"     => array(),
                  "plSection"     => array("admin"),
                  "plCategory"    => array("department" => array("objectClass" => "gosaDepartment", "description" => _("Departments"))),
            
                  "plProvidedAcls" => array(
                    "description"       => _("Description"),
                    "c"                 => _("Country"),
                    "base"              => _("Base"),
                    "l"                 => _("Location"),
                    "telephoneNumber"   => _("Telephone"),
                    "ou"                => _("Department name"),
                    "businessCategory"  => _("Category"),
                    "st"                => _("State"),
                    "postalAddress"     => _("Address"),
                    "gosaUnitTag"       => _("Administrative settings"),
                    "facsimileTelephoneNumber" => _("Fax"))
                  ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
