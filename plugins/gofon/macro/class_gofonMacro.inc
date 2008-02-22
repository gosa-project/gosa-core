<?php

//!  The Phone Macro Class: Handles Macro Contents, and some attributes. 
/*!
     This class handles the basic information about phone macros, like
     cn base description displayName goFonMacroContent goFonMacroVisible

     This is not the only Class that manages phone Macros, there ist also the class_goFonMacroParameter.
*/
class macro extends plugin
{
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

  var $orig_cn = ""; 
  /*! Objectclasses that this calls handles */
  var $objectclasses  = array("top", "goFonMacro");

  var $goFonHomeServers = array(); // Contains all available asterisk database server 

  var $is_new = FALSE;

  //! The Konstructor   
  /*!  Konstructor, load class with  attributes of the given dn*/
  function macro ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* This is always an account */
    $this->is_account= TRUE;

    /* Edit or new one ?*/
    if ($this->dn == "new"){
      $this->is_new = TRUE;
      if(isset($_SESSION['CurrentMainBase'])){
        $this->base = $_SESSION['CurrentMainBase'];
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

    /* Load acl */
    $ui       = get_userinfo();
    $acl      = get_permissions ($ui->dn, $ui->subtreeACL);
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

    if($this->dn != "new"){
      $smarty->assign("disable_cn"," disabled ");
    }else{
      $smarty->assign("disable_cn","  ");
    }

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
        gosa_log(@mysql_error($r_current));
        return(sprintf(_("The MySQL home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
              $cfg_Current['SERVER'],$cfg_Current['LOGIN']));
      }
      $db_current  =  @mysql_select_db($cfg_Current['DB'],$r_current);
      if(!$db_current){
        gosa_log(@mysql_error($r_current));
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
          gosa_log(@mysql_error($r_current));
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
          gosa_log(@mysql_error($r_con));
          return(sprintf(_("Insert of new macro failed for server '%s'."),$cfg['SERVER']));
        }
        @mysql_close($r_con);
      }
    }
  }


  /*! Save data to object */
  function save_object()
  {
    if (isset($_POST['displayName'])){
      plugin::save_object();

      /* The cn can't be changed if this entry is not new */
      if(!$this->is_new){
        $this->cn = $this->orig_cn;
      }

      if(isset($_POST['goFonMacroVisible'])) {
        $this->goFonMacroVisible= 1 ;
      }else  {
        $this->goFonMacroVisible= 0 ;
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
  
    /* Check permissions */
    foreach($this->attributes as $attr){
      if(chkacl($this->acl,"edit")){
        $str =  sprintf( _("Insufficient permissions, can't change attribute '%s' in goFonMacro"),$attr) ;
        return(array($str));
      }
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
    show_ldap_error($ldap->get_error(), _("Removing phone macro failed"));

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".@LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
      show_ldap_error($ldap->get_error(), _("Removing phone macro reverences failed"));
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
      show_ldap_error($ldap->get_error(), _("Saving phone macro failed"));
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
