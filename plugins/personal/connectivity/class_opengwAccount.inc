<?php

class opengwAccount extends plugin
{
	/* Definitions */
	var $plHeadline       = "Opengroupware account";
	var $plDescription    = "This does something";

	/* attribute list for save action */
	var $attributes       = array();
	var $objectclasses    = array();

  var $SetPassword   = false;

	/* Attribute mapping opengroupware->ldap  */
	var $attrsToUse    = array( "salutation"  =>"vocation",
			"name"        =>"sn",
			"firstname"   =>"givenName",
			"login"       =>"uid",
			"degree"      =>"academicTitle",
			"birthday"    =>"dateOfBirth",
			"sex"         =>"gender",
			"street"      =>"street",
			"zip"         =>"postalCode",
			"value_string"=>"mail",
			"number"      =>"telephoneNumber"
			);

	var $serverCon            =  false;
	var $handle               = NULL;
	var $is_account           = false;
	var $initialy_was_account = false;
  var $OGWPassword          = "";
  var $ReadOnly = false;
  var $view_logged = FALSE;  
  var $uid = "";

	function opengwAccount (&$config, $dn= NULL)
	{
		plugin::plugin ($config, $dn);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

		$this->info=array();

		/* is no account and was no account */
		$this->initialy_was_account = false;
		$this->is_account           = false;

		/* Get configurations */
    if($this->config->search("opengwAccount", 'USERNAME',array('tabs'))){
      $atr = array();
      $atr['LOGIN']	=	  $this->config->search("opengwAccount", 'USERNAME',array('tabs'));
      $atr['PASSWORD']=	$this->config->search("opengwAccount", 'PASSWORD',array('tabs'));
      $atr['SERVER']	=	$this->config->search("opengwAccount", 'DATAHOST',array('tabs'));
      $atr['DB']		=	  $this->config->search("opengwAccount", 'DATABASE',array('tabs'));
      $this->serverCon = $atr;
    }elseif(isset($this->config->data['SERVERS']['OPENGROUPWARE'])){
      $this->serverCon  = $this->config->data['SERVERS']['OPENGROUPWARE'];
    }else{
      $this->serverCon = array();
    }

    /* check if datebase funktions are available, and if database configurations are available */ 
		if(is_callable("pg_connect") && count($this->serverCon)){

			$this->handle     = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);

			/* Check if current configuration allows database connection */
			if($this->handle->connected){

				/* Set login name, to check if this is_account */
				if(isset($this->attrs['uid'][0])){
					$this->handle->info['login'] = $this->attrs['uid'][0];
				}else{
					$this->handle->info['login'] = ""; 
				}

				/* If this is account get data from database first */
				if($this->handle->CheckExistence()){
					$this->info = $this->handle->GetInfos($this->attrs['uid'][0]);

					/* This is an account */
					$this->initialy_was_account = true;
					$this->is_account= true;
				}else{
					/* this is no account */ 
					$this->initialy_was_account = false;
					$this->is_account= false;

					/* Selectable in GOsa */
					$this->info['template_user_id']   = 0;
					$this->info['is_locked']          = 0;
					$this->info['LocationTeamID']     = 0;
					$this->info['TeamIDis']           = array();
				}

				/* Set settings from ldap */
				foreach($this->attrsToUse as $name=>$attr){
					if(isset($this->attrs[$attr][0])){
						$this->info[$name] = $this->attrs[$attr][0];
					}else{
						$this->info[$name] = false;
					}
				} 

				/* Description is displayed as 'Nickname' */
				$this->info['description']          = $this->info['login'];

				if($this->info['sex'] == "F"){
					$this->info['sex']                = "female";
				}else{
					$this->info['sex']                = "male";
				}

			}

			/* Transmit data back to ogw handle */
			$this->handle->SetInfos($this->info);
		}
	}

	function execute()
	{
    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

		/* Show tab dialog headers */
		$display= "";
		$smarty= get_smarty();

    $smarty->assign("serverCon",count($this->serverCon));
    $smarty->assign("validTeams","");
    $smarty->assign("is_account", " ");
    $smarty->assign("OGWstate"," disabled ");
    $smarty->assign("validLocationTeam",array("&nbsp;"));
    $smarty->assign("validLocationTeams",array("&nbsp;"));
    $smarty->assign("LocationTeam",0);
    $smarty->assign("validTemplateUser",array("&nbsp;"));
    $smarty->assign("validTemplateUsers",array("&nbsp;"));
    $smarty->assign("TemplateUser",0);
    $smarty->assign("is_lockedCHK", "  ");
    $smarty->assign("is_locked", "  ");
 
		/* set default values */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $acl => $desc){
      $smarty->assign($acl."ACL",$this->getacl($acl,$this->ReadOnly));
    } 

    if((!$this->ReadOnly)&&(($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable()))) {
      $smarty->assign('opengwAccountACL', "");
    }else{
      $smarty->assign('opengwAccountACL', " disabled ");
    }

    if(count($this->serverCon))

		/* Check database extension */    
		if(!is_callable("pg_connect") && count($this->serverCon)){
			print_red(_("OpenGroupware: Your configuration is missing a postgresql extension. Can't perform any database queries."));
		}elseif(count($this->serverCon)){

			/* Check if config exists */
			if($this->config->search("opengwAccount", 'DATABASE',array('tabs'))){
				print_red(_("OpenGroupware: Missing database configuration for opengroupware. Can't get or set any informations."));
			}else{

				/* Create handle */
				$this->handle     = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);

				/* Check if we are successfully connected to db */
				if(!$this->handle->connected){
					print_red(_("OpenGroupware: Can't connect to specified database. Please check given configuration twice."));
				}else{

					/* Assign LocationTeams */
					$tmp = array(""=>"none");
					foreach($this->handle->validLocationTeam as $id){
						$tmp[$id['company_id']] = $id['description']; 
					}
					$smarty->assign("validLocationTeam",$tmp);
					$smarty->assign("validLocationTeams",array_flip($tmp));
					$smarty->assign("LocationTeam",$this->info['LocationTeamID']);

					/* Assign TemplateUsers*/
					$tmp = array();
					foreach($this->handle->validTemplateUser as $id){
						$tmp[$id['company_id']] = $id['name']; 
					}
					$smarty->assign("validTemplateUser",$tmp);
					$smarty->assign("validTemplateUsers",array_flip($tmp));
					$smarty->assign("TemplateUser",$this->info['template_user_id']);

					/* Create Team membership */ 
					$str = "";
					if(($this->is_account)==""){
						$dis = "";
					}else{
						$dis = " disabled ";
					}

          if($this->acl_is_readable("Teams")){
            foreach($this->handle->validTeams as $id){
              if(in_array($id['company_id'],$this->info['TeamIDis'])){
                $str .= "<input ".$dis." type='checkbox' value='".$id['company_id']."' 
                  checked name='team_".base64_encode($id['description'])."'>".$id['description']."<br>";
              }else{
                $str .= "<input ".$dis." type='checkbox' value='".$id['company_id']."' 
                  name='team_".base64_encode($id['description'])."'>".$id['description']."<br>";
              }
            }
          }
					$smarty->assign("validTeams",$str);

					/* Assign ogw checkbox */
					if($this->is_account){
						$smarty->assign("OGWstate","");
						$smarty->assign("is_account", " checked ");
					}else{
						$smarty->assign("is_account", " ");
						$smarty->assign("OGWstate"," disabled ");
					}

					/* Assign lock status */
					if($this->info['is_locked']){
						$smarty->assign("is_lockedCHK", " checked ");
					}else{
						$smarty->assign("is_lockedCHK", "  ");
					}
					$smarty->assign("is_locked", $this->info['is_locked']);

				} 
			}
    }
		$display.= $smarty->fetch (get_template_path('opengw.tpl', TRUE, dirname(__FILE__)));
		return ($display);
	}

	function remove_from_parent()
	{
		/* remove database entries */
		if($this->initialy_was_account){
			$this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
			$this->handle->SetInfos($this->info);
			$this->handle->Perform("REMOVE");

      new log("remove","users/".get_class($this),$this->dn);
		}
	}


	/* Save data to object */
	function save_object()
	{
		/* get post data */
		if($this->is_account){

      /* Get selected team chkboxes */
      $this->info['TeamIDis'] = array();
      if($this->acl_is_writeable("Teams")) {
        foreach($_POST as $name => $value ){
          if(preg_match("/team_/i",$name)){
            if(!in_array($value,$this->info['TeamIDis'])){
              $this->info['TeamIDis'][]=$value;
            }
          }
        }
      }


			/* Get location Team*/
			if(isset($_POST['LocationTeam']) && $this->acl_is_writeable("LocationTeam")){
				$this->info['LocationTeamID'] = $_POST['LocationTeam'];
			}

			/* Get template user */
			if(isset($_POST['TemplateUser']) && $this->acl_is_writeable("TemplateUser") ){
				$this->info['template_user_id'] = $_POST['TemplateUser'];
			}

      /* get lock status */
      if($this->acl_is_writeable("Locked")){
        if(isset($_POST['is_locked'])){
          $this->info['is_locked'] = $_POST['is_locked'];
        }else{
          $this->info['is_locked'] = 0;
        }
      }
		}

		/* change account status */
		if(isset($_POST['is_account'])){
      if($this->acl_is_createable()){
        $this->is_account = $_POST['is_account'];
      }
    }else{
      if($this->acl_is_removeable()){
        $this->is_account = false;//$_POST['is_account'];
      }
		}
	}


	/* Save to LDAP */
	function save()
	{
		/* Save data */ 
		$this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
		$this->handle->SetInfos($this->info);
		$this->handle->checkInfos();
		if($this->initialy_was_account){
			$this->handle->Perform("EDIT");
      new log("modify","users/".get_class($this),$this->dn);
		}else{
			$this->handle->Perform("ADD");
      new log("create","users/".get_class($this),$this->dn);
		}
	}

  
  function PrepareForCopyPaste($src){
    // Nothing to do, because this object can't be copied ... yet
  }


 /* Return plugin informations for acl handling 
      #FIXME This is only an enable/disable checkbox for this account, there is possibly a better solution available later */	
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Opengroupware"),
          "plDescription"   => _("Open groupware account settings")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 26,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "LocationTeam"  => _("Location team"),
            "TemplateUser"  => _("Template user"),
            "Locked"        => _("Locked"),
            "Teams"         => _("Teams"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
