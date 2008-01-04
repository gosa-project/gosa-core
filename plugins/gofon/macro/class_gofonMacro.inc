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
  var $view_logged = FALSE;
  var $orig_cn = ""; 
  /*! Objectclasses that this calls handles */
  var $objectclasses  = array("top", "goFonMacro");

  var $goFonHomeServers = array(); // Contains all available asterisk database server 

  //! The Konstructor   
  /*!  Konstructor, load class with  attributes of the given dn*/
  function macro (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* This is always an account */
    $this->is_account= TRUE;

    /* Edit or new one ?*/
    if ($this->dn == "new"){
      if(session::is_set('CurrentMainBase')){
        $this->base  = session::get('CurrentMainBase');
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {
      $this->orig_cn=$this->cn;
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }

    /* Check server configurations
     * Load all server configuration in $this->goFonHomeServers if available
     */
    $a_SETUP= array();
    if(array_key_exists('config',$_SESSION) &&
       array_key_exists('SERVERS',$_SESSION['config']->data) &&
       count($_SESSION['config']->data['SERVERS']['FON']) && 
       array_key_exists('FON',$_SESSION['config']->data['SERVERS'])) {

      /* Set available server */
      $this->goFonHomeServers = $_SESSION['config']->data['SERVERS']['FON'];
  
      /* Remove default entry, not necessary here */
      if(isset($this->goFonHomeServers[0])){
        unset($this->goFonHomeServers[0]);  
      }
    }
  }


  /*!  Execute this plugin */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","gofonmacro/".get_class($this),$this->dn);
    }

    /* Variables */
    $vars 	= "";
    $tmp  	= array();
    $number = 0; 

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->allowedBasesToMoveTo());
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

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->get_allowed_bases());

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    if($this->acl_is_writeable("base")){
      $smarty->assign("baseSelect",true);
    }else{
      $smarty->assign("baseSelect",false);
    }


    /* Assign all vars to Smarty */
    foreach($this->attributes as $ar){
      $smarty->assign($ar, $this->$ar);
    }
    /* Checkboxes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("vars", $vars);

    if($this->goFonMacroVisible){
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else{
      $smarty->assign("goFonMacroVisibleChecked","");
    }

    $smarty->assign("cnACL",$this->getacl("cn",$this->initially_was_account));
    $smarty->assign("cn",$this->cn);

    /* Ensure that macro content is displayed correctly encoded */
    $smarty->assign("goFonMacroContent",htmlentities(utf8_decode ($this->goFonMacroContent)));

    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  /* This method check if all databases are reachable.  
   *  Returns with error message or an empty string on success.
   * 
   * - Is mysql extension available  
   * - Is every server reachable 
   * - Does the database exists/is accessible
   */
  function check_database_accessibility()
  {
    /* Check if mysql extension is available */
    if(!is_callable("mysql_pconnect")){
      return(_("Can't save any changes to asterisk database, there is currently no mysql extension available in your php setup."));
    }

    /********************
     * Check all home server
     ********************/
    foreach($this->goFonHomeServers as $goFonHomeServer => $cfg_Current){
      $r_current    =  @mysql_pconnect($cfg_Current['SERVER'],$cfg_Current['LOGIN'],$cfg_Current['PASSWORD']);
      if(!$r_current){
        new log("debug","gofonmacro/".get_class($this),"",array(),@mysql_error($r_current));
        return(sprintf(_("The MySQL home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
              $cfg_Current['SERVER'],$cfg_Current['LOGIN']));
      }
      $db_current  =  @mysql_select_db($cfg_Current['DB'],$r_current);
      if(!$db_current){
        new log("debug","gofonmacro/".get_class($this),"",array(),@mysql_error($r_current));
        mysql_close($r_current);
        return( sprintf(_("Can't select database '%s' on home server '%s'."),$cfg_Current['DB'],$cfg_Current['SERVER']));
      }
    }
  }


  /* Remove current macro from all asterisk server.
   * First of all check if we have access to all databases. 
   * - Remove old entries 
   */ 
  function remove_from_database($save)
  {
    /* Check if all databases are reachable */
    $str = $this->check_database_accessibility();
    if($str){
      return($str);
    }

    /* Create query string */
    $context  = addslashes("macro-".$this->cn);

    /* Remove current macro from each server available */ 
    if($save){
      foreach($this->goFonHomeServers as $dn => $Server){
        $query      = "DELETE FROM ".$Server['EXT_TABLE']." WHERE context='".$context."';";
        $r_current  =  @mysql_pconnect($Server['SERVER'],$Server['LOGIN'],$Server['PASSWORD']);
        $db_current =  @mysql_select_db($Server['DB'],$r_current);
        $res = @mysql_query($query,$r_current);
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query");
        if(!$res){
          new log("debug","gofonmacro/".get_class($this),"",array(),@mysql_error($r_current));
          return(sprintf(_("Removing macro from '%s' failed. Check GOsa log for mysql error."),$Server['SERVER']));
        }
        @mysql_close($r_current);
      }
    }
  }

 
  /* Add current macro to all asterisk server.
   * First of all check if we have access to all databases. 
   * - Remove old entries 
   * - Add new entries 
   */ 
  function add_to_database($save)
  {
    /* Check if all databases are reachable */
    $str = $this->check_database_accessibility();
    if($str){
      return($str);
    }

    /* Remove old entries first. Else we got duplicated entries */
    $str = $this->remove_from_database($save);
    if($str){
      return($str);
    }

    /* Create query string */
    $context      = "macro-".$this->cn;

    /************
     * Parse Macro content
     ************/
    $sql = 
      "INSERT INTO %TABLENAME% ".
      " (context,exten,priority,app,appdata) ".
      " VALUES ";
      
    $a_contentLines = split("\n",$this->goFonMacroContent);
    foreach($a_contentLines as $i_linenum => $s_linestr){

      /* Remove the 'exten => ' string in front of the macro content line 
       *  example line  'exten => s,2,GotoIf(${ARG3}?3:5)'
       * Remove comments introduced by ;
       * Skip empty lines 
       */ 
      $s_linestr = trim($s_linestr);
      $s_linestr = preg_replace("/;.*$/","",$s_linestr) ;
      $s_linestr = preg_replace ("/^.*=\> /","",$s_linestr);

      if(empty($s_linestr)){
        continue;
      }

      /* A line that passes the check above should look like this 
       *  s,1,SetLanguage(de)
       * 3 parts seperated by , 
       * If there are more or less parts, abort.
       * The preg_replace exclude parameters from split .. 
       */
      $tmp  = split(",", $s_linestr,3);

      /* Check if there are exactly 2 , */ 
#      if(substr_count($s_linestr,",") !=2){
#        return(sprintf(_("More than two ',' given in line : '%s'. Remember that parameters are seperated by '|'."),$i_linenum));
#      }
      /* Multiple () are not supproted currently ... */  
      if(substr_count($s_linestr,"(") >1 ){
        return(sprintf(_("More than one '(' is currently not supported. Line : '%s'."),$i_linenum));
      }
      if(substr_count($s_linestr,")") >1 ){
        return(sprintf(_("More than one ')' is currently not supported. Line : '%s'."),$i_linenum));
      }
      /* Check if there is an application given */
      if(empty($tmp[1])){
        return(sprintf(_("There is no application given in line : '%s'."),$i_linenum));
      } 
      /* Check if there is an extension given */
      if(empty($tmp[0])){
        return(sprintf(_("There is no extension type given in line : '%s'."),$i_linenum));
      } 

      /* Create extension entry for current line 
       *  and add this line to an array that will be inserted 
       *  to each database.
       */
      $exten  = addslashes($tmp[0]);
      $prio   = addslashes($tmp[1]);
      $app    = addslashes(preg_replace("/\(.*\).*$/","",$tmp[2]));
      $para   = addslashes(preg_replace("/^.*\(/","",$tmp[2]));
      $para   = preg_replace("/\).*$/","",$para);
      $sql.= " ('".$context."','".$exten."','".$prio."','".$app."','".$para."'),";
    }
    
    /* Remove last , from query string */
    $sql = preg_replace("/,$/","",$sql);

    /* Save current changes to the database */
    if($save){
    
      /* Macro are spread to each asterisk server */
      foreach($this->goFonHomeServers as $dn => $cfg){
        $r_con  = @mysql_pconnect($cfg['SERVER'],$cfg['LOGIN'],$cfg['PASSWORD']); 
        $db     = @mysql_select_db($cfg['DB'],$r_con);
        $query  = preg_replace("/%TABLENAME%/",$cfg['EXT_TABLE'],$sql);
        $res    = @mysql_query($query,$r_con);
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query");
        if(!$res){
          new log("debug","gofonmacro/".get_class($this),"",array(),@mysql_error($r_con));
          return(sprintf(_("Insert of new macro failed for server '%s'."),$cfg['SERVER']));
        }
        @mysql_close($r_con);
      }
    }
  }


  function save_object()
  {
    if (isset($_POST['gofonMacroGenericPosted'])){

      $old_cn       = $this->cn;
      $old_visible  = $this->goFonMacroVisible;

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;

      /* Save base, since this is no LDAP attribute */
      $tmp = $this->get_allowed_bases();
      if(isset($_POST['base'])){
        if(isset($tmp[$_POST['base']])){
          $this->base= $_POST['base'];
        }
      }

      /* Restore old cn if we have insuficient acls to change cn ... */
      if(!$this->acl_is_writeable("cn",$this->initially_was_account)){
        $this->cn = $old_cn;
      }

      /* check if we are allowed to toggle visibility */
      if($this->acl_is_writeable("goFonMacroVisible")) {

        /* Checkbox selected ? */
        if(isset($_POST['goFonMacroVisible'])) {
          $this->goFonMacroVisible= 1 ;
        }else  {
          if(isset($_POST['displayName'])){
            $this->goFonMacroVisible= 0 ;
          }
        }
      }else{
        $this->goFonMacroVisible = $old_visible;
      }
    }
  }


  /*! Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!count($this->goFonHomeServers)){
      $message[] = _("There must be at least one server with an asterisk database to save this phone macro.");
    }

    /* Check if insert/replace is possible and all servers are available */
    $str = $this->add_to_database(false);
    if($str){
      $message[] = $str;
    }

    /* Check if cn is already used  */
    if(($this->dn=="new")||($this->orig_cn!=$this->cn)){
      $ldap = $this->config->get_ldap_link();
      $ldap->search("(&(objectClass=goFonMacro)(cn=".$this->cn."))",array("cn"));
      if($ldap->count()>0){
        $message[]=sprintf(_("The given cn '%s' already exists."),$this->cn);
      }
    }
  
    /* Check if display name is set */
    if(empty($this->displayName)){
      $message[] = _("You must specify the 'Display Name' in order to save this macro");
    }  
    /* CN is restricted to 20 chars */
    if(strlen("Makro-".$this->cn)>20 ){
      $message[]=_("The given cn is too long, to create a Makro entry, maximum 20 chars.");
    }
  
    /* If this macro is still in use we should not change the visible for user flag to invisible */
    if(!$this->goFonMacroVisible){
      $ldap = $this->config->get_ldap_link();
      $res = $ldap->search("(&(objectClass=goFonAccount)(objectClass=gosaAccount)(goFonMacro=*))", array("goFonMacro"));
      while ($val = $ldap->fetch()){
        if(strstr($val['goFonMacro'][0],$this->dn)){
          $message[] = _("This macro is still in use. It is necessary to mark this macro as visible for users.");
          return($message);
        }
      }
    }

    /* Macro content must be smaller than 100 lines */
    if(count(split("\n",$this->goFonMacroContent))>100){
      $message[] = _("Makro length must be lower than 100 lines");
    }

    /* Macro content must be smaller than 100 lines */
    if(empty($this->goFonMacroContent)){
      $message[] = _("You can't save an empty macro.");
    }
    return $message;
  }


  /*! Remove makro from all given databases 
   *   and ldap too.
   */
  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();

    /* Skip remove if this macro is still in use */
    $res = $ldap->search("(&(objectClass=goFonAccount)(objectClass=gosaAccount)(goFonMacro=*))", array("goFonMacro"));
    while ($val = $ldap->fetch()){ 
      if(strstr($val['goFonMacro'][0],$this->dn)){ 
        print_red(_("This macro is still in use. To delete this Macro ensure that nobody has selected it."));
        return false;
      }
    }

    /* Try to remove from database */
    if(count($this->goFonHomeServers)){
      $str = $this->remove_from_database(true);
      if($str){ 
        print_red($str);
        return false;
      }
    }else{
      print_red(_("Could not remove the macro entry from asterisk databases. Please check your asterisk database configurations."));
      return false;
    }

    /* Remove phone macro */ 
    $ldap->rmDir($this->dn);
    new log("remove","gofonmacro/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error()); 
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of goFonMacro/generic account with dn '%s' failed."),$this->dn));

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
      show_ldap_error($ldap->get_error(), sprintf(_("Removing of goFonMacro/generic account with dn '%s' failed."),$this->dn));
    }
  }


  /*! Save to LDAP */
  function save()
  {
    plugin::save();
    unset($this->attrs['base']);

    /* Try to add entries to databases */
    $str = $this->add_to_database(true);
    if($str){
      print_red($str);
    }else{
      /* Write back to ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('dn'));
      $a= $ldap->fetch();

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
      show_ldap_error($ldap->get_error(), sprintf(_("Saving of goFonMacro/generic account with dn '%s' failed."),$this->dn));

      /* Log last action */
      if($this->initially_was_account){
        new log("modify","gofonmacro/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }else{
        new log("create","gofonmacro/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }
    }
  }


  function getCopyDialog()
  {
    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }


  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Generic"),
          "plDescription" => _("Asterisk macro management"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("gofonmacro" => array("description" => _("GOfon macro"),
              "objectClass" => "gofonMacro")),

          "plProvidedAcls" => array(
            "cn"                            => _("Macro name"),
            "base"                          => _("Base"),
            "description"                   => _("Description"),
            "displayName"                   => _("Display name"),
            "goFonMacroContent"             => _("Macro content and parameter"),
            "goFonMacroVisible"             => _("Visibility flag"))
          ));
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
