<?php

class opengwAccount extends plugin
{
  /* Definitions */
  var $plHeadline       = "Opengroupware account";
  var $plDescription    = "This does something";

  /* CLI vars */
  var $cli_summary      = "Manage users webdav account";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("gosaWebdavAccount");

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
  var $serverCon =  false;

  var $handle   = NULL;
  var $is_account  = false;

  function opengwAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->info=array();

    $this->initialy_was_account = false;
    $this->is_account= false;
    if(!is_callable("pg_connect")){
//      print_red(_("Your configuration is missing a postgresql extension. Can't perform any database queries."));
    }elseif(!isset($this->config->data['SERVERS']['OPENGROUPWARE'])){
//      print_red(_("Missing database configuration for opengroupware. Can't get or set any informations."));
    }else{
      $this->serverCon = $this->config->data['SERVERS']['OPENGROUPWARE'];
      $this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);

      if(!$this->handle->connected){
//        print_red(_("Can't connect to specified database. Please check given configuration twice."));
      }else{
        $this->handle->info['login'] = $this->attrs['uid'][0];
        if($this->handle->CheckExistence()){
          $this->info = $this->handle->GetInfos($this->attrs['uid'][0]);
          $this->initialy_was_account = true;
          $this->is_account= true;
        }else{
          $this->initialy_was_account = false;
          $this->is_account= false;

          foreach($this->attrsToUse as $name=>$attr){
            if(isset($this->attrs[$attr][0])){
              $this->info[$name] = $this->attrs[$attr][0];
            }else{
              $this->info[$name] = false;
            }
          } 
          $this->info['description']        = $this->info['firstname']." ".$this->info['name'];
          if($this->info['sex'] == "F"){
            $this->info['sex']                = "female";
          }else{
            $this->info['sex']                = "male";
          }

          /* Selectable in GOsa */
          $this->info['template_user_id']   = 0;
          $this->info['is_locked']          = 0;
          $this->info['LocationTeamID']     = 0;
          $this->info['TeamIDis']           = array();
        }

        if(!is_array($this->info['TeamIDis'])){
          $this->info['TeamIDis'] = array();
        }
        $this->handle->SetInfos($this->info);
      }
    }
  }

  function execute()
  {
    /* Show tab dialog headers */
    $display= "";
    $smarty= get_smarty();
    foreach(array("validLocationTeam","validTemplateUser","validLocationTeams","validTemplateUsers") as $ar){
      $smarty->assign($ar,array());
    }

    $smarty->assign("OGWstate"," disabled ");
    foreach(array("LocationTeam","TemplateUser","is_locked","validTeams","opengwAccount") as $ar){
      $smarty->assign($ar,"");
      $smarty->assign($ar."CHK","");
      $smarty->assign($ar."ACL"," disabled ");
    }

    
    if(!is_callable("pg_connect")){
      print_red(_("OpenGroupware: Your configuration is missing a postgresql extension. Can't perform any database queries."));
    }elseif(!isset($this->config->data['SERVERS']['OPENGROUPWARE'])){
      print_red(_("OpenGroupware: Missing database configuration for opengroupware. Can't get or set any informations."));
    }else{
      $this->serverCon = $this->config->data['SERVERS']['OPENGROUPWARE'];
      $this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
      if(!$this->handle->connected){
        print_red(_("OpenGroupware: Can't connect to specified database. Please check given configuration twice."));
      }else{
        $this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
        /* Show main page */

        $smarty->assign("OGWstate"," disabled ");
        foreach(array("LocationTeam","TemplateUser","is_locked","validTeams","opengwAccount") as $ar){
          $smarty->assign($ar."ACL",chkacl($this->acl,$ar));
        }

        $tmp = array(""=>"none");
        foreach($this->handle->validLocationTeam as $id){
          $tmp[$id['company_id']] = $id['description']; 
        }
        $smarty->assign("validLocationTeam",$tmp);
        $smarty->assign("validLocationTeams",array_flip($tmp));
        $smarty->assign("LocationTeam",$this->info['LocationTeamID']);

        $tmp = array();
        foreach($this->handle->validTemplateUser as $id){
          $tmp[$id['company_id']] = $id['name']; 
        }
        $smarty->assign("validTemplateUser",$tmp);
        $smarty->assign("validTemplateUsers",array_flip($tmp));
        $smarty->assign("TemplateUser",$this->info['template_user_id']);
        

        $this->handle->CheckExistence();
        $str = "";
        if($this->is_account){
          $dis = "";
        }else{
          $dis = " disabled ";
        }

        foreach($this->handle->validTeams as $id){
          if(in_array($id['company_id'],$this->info['TeamIDis'])){
            $str .= "<input ".$dis." type='checkbox' value='".$id['company_id']."' checked name='team_".base64_encode($id['description'])."'>".$id['description']."<br>";
          }else{
            $str .= "<input ".$dis." type='checkbox' value='".$id['company_id']."' name='team_".base64_encode($id['description'])."'>".$id['description']."<br>";
          }
        }
        $smarty->assign("validTeams",$str);

        if($this->is_account){
          $smarty->assign("OGWstate","");
          $smarty->assign("is_account", " checked ");
        }else{
          $smarty->assign("is_account", " ");
          $smarty->assign("OGWstate"," disabled ");
        }

        foreach(array("LocationTeam","TemplateUser","Teams","is_locked") as $atr){
          $smarty->assign($atr."ACL",chkacl($this->acl,$atr));
        }

        if($this->info['is_locked']){
          $smarty->assign("is_lockedCHK", " checked ");
        }else{
          $smarty->assign("is_lockedCHK", "  ");
        }

        $smarty->assign("is_locked", $this->info['is_locked']);
      } 
    }
    $display.= $smarty->fetch (get_template_path('opengw.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if($this->initialy_was_account){
      $this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
      $this->handle->SetInfos($this->info);
      $this->handle->Perform("REMOVE");
    }
  }


  /* Save data to object */
  function save_object()
  {
    if($this->is_account){
      $this->info['TeamIDis'] = array();
      foreach($_POST as $name => $value ){
        if(preg_match("/team_/i",$name)){
          if(!in_array($value,$this->info['TeamIDis'])){
            $this->info['TeamIDis'][]=$value;
          }
        }
      }
      if(isset($_POST['LocationTeam'])){
        $this->info['LocationTeamID'] = $_POST['LocationTeam'];
      }
      
      if(isset($_POST['TemplateUser'])){
        $this->info['template_user_id'] = $_POST['TemplateUser'];
      }
      
      if(isset($_POST['is_locked'])){
        $this->info['is_locked'] = $_POST['is_locked'];
      }else{
        $this->info['is_locked'] = 0;
      }
    }
 
    if(isset($_POST['is_account'])){
      $this->is_account = $_POST['is_account'];
    }else{
      $this->is_account = false;//$_POST['is_account'];
    }

  }


  /* Save to LDAP */
  function save()
  { 
    $this->handle = new ogw($this->serverCon['LOGIN'],$this->serverCon['PASSWORD'],$this->serverCon['SERVER'],$this->serverCon['DB']);
    $this->handle->SetInfos($this->info);
    $this->handle->checkInfos();
    if($this->initialy_was_account){
      $this->handle->Perform("EDIT");
    }else{
      $this->handle->Perform("ADD");
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
