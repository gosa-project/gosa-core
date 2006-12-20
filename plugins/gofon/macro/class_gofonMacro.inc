<?php

//!  The Phone Macro Class: Handles Macro Contents, and some attributes. 
/*!
     This class handles the basic information about phone macros, like
     cn base description displayName goFonMacroContent goFonMacroVisible

     This is not the only Class that manages phone Macros, there ist also the class_goFonMacroParameter.
*/
class macro extends plugin
{
  /*! CLI vars */
  var $cli_summary= "Handling of GOsa's macro object";
  /*! CLI vars */
  var $cli_description= "Some longer text\nfor help";
  /*! CLI vars */
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /*! Macro attributes,  */
  var $generate_error= "";
  
  /*! The name of the Macro in the openldap drirectory */
  var $cn               = ""; 
 
  /*! Display error once */
  var $error_shown = false; 

  /*! This ist the variable that contains the description of the macro*/
  var $description      = "";

  /*! The base of the macro, is used to save the macro in the correct directory tree */
  var $base             = "";

  /*! This is the name of the macro which the enduser will see, instead of the cn */
  var $displayName      = "";
    
  /*! Here is the macro content, the real macroscript */
  var $goFonMacroContent= "";
  
  /*! To allow user to use this macro this var must be true, else false */
  var $goFonMacroVisible= 0;

  /*! attribute list for save action */
  var $attributes     = array("cn","base", "description","displayName","goFonMacroContent","goFonMacroVisible");

  var $is_new=false;
  var $orig_cn = ""; 
  /*! Objectclasses that this calls handles */
  var $objectclasses  = array("top", "goFonMacro");

  //! The Konstructor   
  /*!  Konstructor, load class with  attributes of the given dn*/
  function macro ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $ldap= $config->get_ldap_link();

    $this->dn = $dn;

    /* This is always an account */
    $this->is_account= TRUE;


    /* Edit or new one ?*/
    if ($this->dn == "new"){
      if(isset($_SESSION['CurrentMainBase'])){
        $this->base = $_SESSION['CurrentMainBase'];
        $this->is_new = true;
      }else{
        $this->is_new = true;
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {
      $this->is_new = false;
      $this->orig_cn=$this->cn;
      $this->base= preg_replace("/ou=macros,ou=asterisk,ou=configs,ou=systems,/","",dn2base($this->dn));
    }
    $ui= get_userinfo();
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goFonMacro", $ui->dn);
  }

  /*!  Execute this plugin */
  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Variables */
    $vars 	= "";
    $tmp  	= array();
    $number = 0; 

    if(!isset($_SESSION['config']->data['SERVERS']['FON'][0])){
      print_red(_("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
    }

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no phone macro.")."</b>";
      return ($display);
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



    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign all vars to Smarty */
    foreach($this->attributes as $ar){
      $smarty->assign($ar, $this->$ar);
      $smarty->assign($ar."ACL", chkacl($this->acl,$ar));
    }
    /* Checkboxes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("vars", $vars);

    if($this->goFonMacroVisible){
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else{
      $smarty->assign("goFonMacroVisibleChecked","");
    }

    if(isset($_POST['goFonMacroVisible'])) {
      $this->goFonMacroVisible= 1 ;
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else  {
      if(isset($_POST['displayName'])){
        $this->goFonMacroVisible= 0 ;
        $smarty->assign("goFonMacroVisibleChecked","");
      }
    }
    
    if(!$this->is_new){
      $smarty->assign("disable_cn"," disabled ");
      $smarty->assign("cn",$this->orig_cn);
    }else{
      $smarty->assign("disable_cn","  ");
      $smarty->assign("cn",$this->cn);
    }
    $this->generate_mysql_entension_entries();
    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  /*!
  Remove this Object
  */
  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    
    $res = $ldap->search("(&(objectClass=goFonAccount)(objectClass=gosaAccount))", array("goFonMacro"));

    while ($val = $ldap->fetch()){ 
      if(isset($val['goFonMacro'])){
        if(strstr($val['goFonMacro'][0],$this->dn)){ 
          print_red(_("This macro ist still in use. To delete this Macro ensure that nobody has selected this Macro."));
          return false;
        }
      }
    }
  
    $ldap->rmDir($this->dn); 
    show_ldap_error($ldap->get_error(), _("Removing phone macro failed"));
    if(isset($this->orig_cn)){
      $this->generate_mysql_entension_entries(false,true,$this->orig_cn);
    }else{
      $this->generate_mysql_entension_entries(false,true);
    }

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
        $og= new ogroup($this->config, $ldap->getDN());
        unset($og->member[$this->dn]);
        $og->save ();
        show_ldap_error($ldap->get_error(), _("Removing phone macro reverences failed"));
    }
  }

  // Generate MySQL Syntax 
  function generate_mysql_entension_entries($save = false,$delete_only=false,$remove_old_macroname=false){

    if(!isset($_SESSION['config']->data['SERVERS']['FON'][0])){
      return(true);
    }

    if(!is_callable("mysql_connect")){
      if((!$this->error_shown)&&($save)){
        print_red(_("There is no mysql extension available in your php setup, can't save any changes to asterisk db."));
        $this->error_shown = true;
      }
      return(true);
    }

    // Get Configuration for Mysql database Server  
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'][0];

    // Connect to DB server
    $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']); 
  
    // Check if we are  connected correctly 
    if(!$r_con){
      $this->generate_error = sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
          $a_SETUP['SERVER'],$a_SETUP['LOGIN']);
      gosa_log(mysql_error());
      return false;
    }

    // Select database for Extensions 
    $db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

    // Test if we have the database selected correctly
    if(!$db){
      $this->generate_error = sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']);
      gosa_log(mysql_error());
      return false;
    }


    // Context def
    $context  = "macro-".$this->cn;

    // Parse Content if we connected correctly
    if($db &&  $r_con ){

      // Split Content into lines 
      $a_contentLines = split ("\n",$this->goFonMacroContent);

      // Foreach single line ...
      foreach($a_contentLines as $i_linenum => $s_linestr){
    
        // Remove unwanted exten => tag 
        $s_linestr= preg_replace ("/^.*\> /","",$s_linestr);
      
        // Remove  spaces 
        $s_linestr = trim ( $s_linestr);

        // Skip empty lines
        if ($s_linestr == ""){
          continue;
        }

        // If not empty or linebreak at [0]
        if((!empty($s_linestr))&&($s_linestr[0]!=";")&&(ord($s_linestr[0]) !=13)){
  
          // Set general SQL statement
          $SQL[$i_linenum] =  
            "INSERT INTO ".$a_SETUP['EXT_TABLE'].
            " (context,exten,priority,app,appdata) ".
            " VALUES ".
            " (";
      
          // Parse linestr to entry data
          $linecontents = preg_replace("/;.*$/","",$s_linestr) ;

          $tmp  = split(",", preg_replace("/\(.*$/","",$linecontents));
  
          if(!isset($tmp[2])){
            $this->generate_error = sprintf(_("Unable to parse macro contents on line: %s"),$i_linenum);
            return false;
          }    
            $exten = $tmp[0];
            $prio = $tmp[1];
            $app  = $tmp[2];
            $para = $linecontents; 
            $para = preg_replace("/^.*\(/","",$para);
            $para = preg_replace("/\)$/","",$para);

            // Append SQL syntax
            $SQL[$i_linenum].="'".$context."','".$exten."','".$prio."','".$app."','".$para."');";
        }
     }

      if(($save)||($delete_only)){
        $res = mysql_query("SELECT count(*) FROM ".$a_SETUP['EXT_TABLE']." WHERE context= '".$context."'");
        if(!$res){
          $this->generate_error = sprintf(_("Can't perform SELECT query in DB '%s'"),$a_SETUP['DB']);
          gosa_log(mysql_error());
          return false;
        }
        $cnt = mysql_fetch_row($res);
        $cnt = $cnt[0];

        if($cnt != 0) {
          if(!mysql_query("DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE context= '".$context."'")){
            $this->generate_error = sprintf(_("Can't perform DELETE query in DB '%s'"),$a_SETUP['DB']);
            gosa_log(mysql_error());
            return false;
          }
        }

        if($remove_old_macroname!="false"){
          if(!mysql_query("DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE context= 'Makro-".$remove_old_macroname."'")){
            $this->generate_error = sprintf(_("Can't perform DELETE query in DB '%s'"),$a_SETUP['DB']);
            gosa_log(mysql_error());
            return false;
          }
        }



      }

      // We want to save this 
      if(($save)&&(isset($SQL))){
        foreach($SQL as $entry){
          if(!mysql_query($entry)){
            $this->generate_error = sprintf(_("Can't perform INSERT query in DB '%s'"),$a_SETUP['DB']);
            gosa_log(mysql_error());
            return false;
          }
        } 
      }    
    }
    if((isset($r_con))&&($r_con)){
      @mysql_close($r_con);
    }
    return true;
  }




  /*!
  Save data to object 
  */
  function save_object()
  {
    if (isset($_POST['displayName'])){
      plugin::save_object();
    }
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr= $_POST [$attr];
      }
    }
  }


  /*! 
  Check values 
  */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!$this->generate_mysql_entension_entries()){
      $message[] = $this->generate_error;
    }

    if(($this->dn=="new")||($this->orig_cn!=$this->cn)){
      $ldap = $this->config->get_ldap_link();
      $ldap->search("(&(objectClass=goFonMacro)(cn=".$this->cn."))",array("cn"));
      if($ldap->count()>0){
        $message[]=sprintf(_("The given cn '%s' already exists."),$this->cn);
      }
    }

    if(empty($this->displayName)){
      $message[] = _("You must specify the 'Display Name' in order to save this macro");
    }  
 
    if(strlen("Makro-".$this->cn)>20 ){
      $message[]=_("The given cn is too long, to create a Makro entry, maximum 20 chars.");
    }
 
    foreach($this->attributes as $attr){
      if(chkacl($this->acl,"edit")){
        $str =  sprintf( _("Insufficient permissions, can't change attribute '%s' in goFonMacro"),$attr) ;
        return(array($str));
        }
      }
    
    if(count(split("\n",$this->goFonMacroContent))>100){
      $message[] = _("Makro length must be lower than 100 lines");
    }
   
    /*Some stupid IE fixes again*/
    if(empty ($this->base)) {
      $message[] = _("Please choose a valid  base.");
    }
 
    return $message;
  }


  /*! 
  Save to LDAP 
  */
  function save()
  {
    /* Post checks */
    $this->execute();

    plugin::save();
    unset($this->attrs['base']);

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn, array('dn'));
    $a= $ldap->fetch();
    
    if(isset($this->orig_cn)){
      $this-> generate_mysql_entension_entries(true,false,$this->orig_cn);
    }else{
      $this-> generate_mysql_entension_entries(true);
    }

    if($this->generate_mysql_entension_entries()){
      if (count($a)){
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify ($this->attrs); 

        $this->handle_post_events("modify");
      } else {
        $ldap->cd($this->config->current['BASE']);
        $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        $ldap->cd($this->dn);
        $ldap->add($this->attrs);
        $this->handle_post_events("add");
      }
      show_ldap_error($ldap->get_error(), _("Saving phone macro failed"));
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
