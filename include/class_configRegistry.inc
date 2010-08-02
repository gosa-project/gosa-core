<?php

class configRegistry{

    public $config = NULL;
    public $properties = array();
    public $ldapStoredProperties = array(); 
    public $fileStoredProperties = array(); 
    public $classToName = array(); 

    public $status = 'none';

    // Excludes property values defined in ldap 
    public $ignoreLdapProperties = FALSE;

    // Contains all classes with plInfo
    public $classesWithInfo = array();
    public $pluginRequirements  = array();
    public $categoryToClass  = array();

    public $objectClasses = array();

    public $detectedSchemaIssues = array();
    public $schemaCheckFailed = FALSE;
    public $schemaCheckFinished = FALSE;
    public $pluginsDeactivated = array();

    // Name of enabled plugins found in gosa.conf.
    private $activePlugins = array();


    /*! \brief      Constructs the config registry 
     *  @param      config  The configuration object
     *  @return     
     */
    function __construct($config)
    {
        $this->config = &$config;

        // Detect classes that have a plInfo method 
        global $class_mapping;
        foreach ($class_mapping as $cname => $path){
            $cmethods = get_class_methods($cname);
            if (is_array($cmethods) && in_array_ics('plInfo',$cmethods)){

                // Get plugin definitions
                $def = call_user_func(array($cname, 'plInfo'));;

                // Register Post Events (postmodfiy,postcreate,postremove,checkhook)
                if(count($def)){
                    $this->classesWithInfo[$cname] = $def;
                }
            }
        }

        // (Re)Load properties
        $this->reload();
    }

    
    /*! \brief      Returns a list of plugins used by GOsa.
        @return     Array       An array containing all plugins with theis plInfo data.
     */
    function getListOfPlugins()
    {
        return($this->classesWithInfo);
    }


    /*! \brief      Checks whether the schema check was called in the current session or not.
     *  @return     Boolean     True if check was already called
     */
    function schemaCheckFinished()
    {
        return($this->schemaCheckFinished);
    }


    /*! \brief      Starts the schema validation
     *  @param      Boolean     'force' Force a re-check.
     *  @param      Boolean     'disableIncompatiblePlugins' Disables of incompatible GOsa-plugins.
     *  @return     Boolean     True on success else FALSE
     */
    function validateSchemata($force = FALSE, $disableIncompatiblePlugins = FALSE, $objectClassesToUse = array())
    {
        // Read objectClasses from ldap
        if(count($objectClassesToUse)){
            $this->setObjectClasses($objectClassesToUse);
        }elseif(!count($this->objectClasses)){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $this->setObjectClasses($ldap->get_objectclasses());
        }

        return($this->_validateSchemata($force, $disableIncompatiblePlugins));
    }


    /*! \brief      Sets the list object classes to use while validation the schema. (See 'validateSchemata')
     *              This is called from the GOsa-Setup
     *  @param      Array       The list of object classes (usually LDAP::get_objectlclasses()).
     *  @return     void  
     */
    function setObjectClasses($ocs)
    {
        $this->objectClasses = $ocs;
    }


    /*! \brief      Returns an array which contains all unresolved schemata requirements.
     *  @return     Array       An array containing all errors/issues  
     */
    function getSchemaResults()
    {
        return($this->detectedSchemaIssues);
    }


    /*! \brief      This method checks if the installed ldap-schemata matches the plugin requirements.
     *  @param      Boolean     'force' Force a re-check.
     *  @param      Boolean     'disableIncompatiblePlugins' Disables of incompatible GOsa-plugins.
     *  @return     String  
     */
    private function _validateSchemata($force = FALSE, $disableIncompatiblePlugins = FALSE)
    {
        // We cannot check without readable schema info
        if(!count($this->objectClasses)){
            return(TRUE); 
        }

        // Don't do things twice unless forced
        if($this->schemaCheckFinished && !$force) return($this->schemaCheckFailed); 

        // Prepare result array
        $this->detectedSchemaIssues = array();
        $this->detectedSchemaIssues['missing'] = array();
        $this->detectedSchemaIssues['versionMismatch'] = array();

        // Clear last results 
        $this->pluginsDeactivated = array();

        // Collect required schema infos
        $this->pluginRequirements = array('ldapSchema' => array());
        $this->categoryToClass = array();

        // Walk through plugins with requirements, but only check for active plugins.
        foreach($this->classesWithInfo as $cname => $defs){
            if(isset($defs['plRequirements'])){

                // Check only if required plugin is enabled in gosa.conf
                // Normally this is the class name itself, but may be overridden
                //  in plInfo using the plRequirements::activePlugin statement.
                $requiresActivePlugin = $cname;
                if(isset($defs['plRequirements']['activePlugin'])){
                    $requiresActivePlugin = $defs['plRequirements']['activePlugin'];
                }

                // Only queue checks for active plugins. 
                if(isset($this->activePlugins[strtolower($requiresActivePlugin)])){
                    $this->pluginRequirements[$cname] = $defs['plRequirements'];
                }else{
                    if($cname == $requiresActivePlugin){
                        new log("debug","","Skipped schema check for '{$cname}' plugin is inactive!",
                                array(),'');
                    }else{
                        new log("debug","","Skipped schema check for class '{$cname}' skipped,".
                                    " required plugin '{$requiresActivePlugin}' is inactive!",
                                array(),'');
                    }
                }
            }
        }

        // Check schema requirements now        $missing = $invalid = array();
        foreach($this->pluginRequirements as $cname => $requirements){

            // Check LDAP schema requirements for this plugins
            $failure = FALSE;
            if(isset($requirements['ldapSchema'])){
                foreach($requirements['ldapSchema'] as $oc => $version){
                    if(!$this->ocAvailable($oc)){
                        $this->detectedSchemaIssues['missing'][$oc] = $oc;
                    
                        $this->schemaCheckFailed = TRUE;
                        $failure = TRUE;

                        new log("debug","","LDAP objectClass missing '{$oc}'!",
                                array(),'');

                    }elseif(!empty($version)){
                        $currentVersion = $this->getObjectClassVersion($oc);
                        if(!empty($currentVersion) && !$this->ocVersionMatch($version, $currentVersion)){
                            if($currentVersion == -1){
                                $currentVersion = _("unknown");
                            }
                            $this->detectedSchemaIssues['versionMismatch'][$oc] = 
                                sprintf(_("%s has version %s but %s is required!"), bold($oc),bold($currentVersion),bold($version));
                            $this->schemaCheckFailed = TRUE;
                            $failure = TRUE;

                            new log("debug","","LDAP objectClass version mismatch '{$oc}' ".
                                    "has '{$currentVersion}' but {$version} required!",
                                    array(),'');
                        }
                    }
                }
            }

            // Display corresponding plugins now 
            if($disableIncompatiblePlugins && $failure && isset($requirements['onFailureDisablePlugin'])){
                foreach($requirements['onFailureDisablePlugin'] as $name){
                    $this->pluginsDeactivated[$name] = $name;
                } 
            }
        }
        $this->schemaCheckFinished =TRUE;
        session::un_set('plist');
        return(!$this->schemaCheckFailed);
    }

    
    /*! \brief      The function 'validateSchemata' may has disabled some GOsa-Plugins, 
     *               the list of disabled plugins will be returned here.
     *  @return     Array       The list of plugins disabled by 'validateSchemata'
     */
    function getDisabledPlugins()
    {
        return($this->pluginsDeactivated);
    }

        
    /*! \brief      Displays an error message with all issues detect during the schema validation.
     */
    function displayRequirementErrors()
    {
        $message = "";
        if(count($this->detectedSchemaIssues['missing'])){
            $message.= "<br>".
                _("The following object classes are missing:").
                "<div class='scrollContainer' style='height:100px'>".
                msgPool::buildList(array_values($this->detectedSchemaIssues['missing'])).
                "</div>";
        }    
        if(count($this->detectedSchemaIssues['versionMismatch'])){
            $message.= "<br>".
                _("The following object classes are outdated:").
                "<div class='scrollContainer' style='height:100px'>".
                msgPool::buildList(array_values($this->detectedSchemaIssues['versionMismatch'])).
                "</div>";
        }    
        if($message != ""){
            $message.= "<br>"._("Plugins that require one or more of the object classes above will be disabled until the object classes get updated.");
 
            msg_dialog::display(_("Schema validation error"),$message, ERROR_DIALOG);
        }
    }


    /*! \brief      Checks to version strings (e.g. '>=v2.8' and '2.9')
     *  @param      String      The required version with operators (e.g. '>=2.8') 
     *  @param      String      The version to match for withOUT operators (e.g. '2.9') 
     *  @return     Boolean     True if version matches else false.  
     */
    private function ocVersionMatch($required, $installed)
    {
        $operator = preg_replace('/^([=<>]*).*$/',"\\1",$required);
        $required = preg_replace('/^[=<>]*(.*)$/',"\\1",$required);
        return(version_compare($installed,$required, $operator)); 
    }

    
    /*! \brief      Returns the currently installed version of a given object class.
     *  @param      String      The name of the objectClass to check for. 
     *  @return     String      The version string of the objectClass (e.g. v2.7) 
     */
    function getObjectClassVersion($oc)
    {
        if(!isset($this->objectClasses[$oc])){
            return(NULL);
        }else{
            $version = -1; // unknown
            if(preg_match("/(v[^)]*)/", $this->objectClasses[$oc]['DESC'])){
                $version = preg_replace('/^.*\(v([^)]*)\).*$/',"\\1", $this->objectClasses[$oc]['DESC']);
            }
        }
        return($version);
    }
    

    /*! \brief      Check whether the given object class is available or not. 
     *  @param      String      The name of the objectClass to check for (e.g. 'mailAccount') 
     *  @return     Boolean     Returns TRUE if the class exists else FALSE.
     */
    function ocAvailable($name)
    {
        return(isset($this->objectClasses[$name]));
    }


    /*! \brief      Re-loads the list of installed GOsa-Properties. 
     *  @param      Boolean     $force   If force is TRUE, the complete properties list is rebuild..
     */
    function reload($force = FALSE)
    {
        // Do not reload the properties everytime, once we have  
        //  everything loaded and registrered skip the reload.
        // Status is 'finished' once we had a ldap connection (logged in)
        if(!$force && $this->status == 'finished') return;

        // Reset everything
        $this->ldapStoredProperties = array();
        $this->fileStoredProperties = array();
        $this->properties = array();
        $this->mapByName = array();
        $this->activePlugins = array('core'=>'core');

        if(!$this->config) return;

        // Search for config flags defined in the config file (TAB section)
        foreach($this->config->data['TABS'] as $tabname => $tabdefs){
            foreach($tabdefs as $info){

                // Put plugin in list of active plugins
                if(isset($info['CLASS'])){
                    $class = strtolower($info['CLASS']);
                    $this->activePlugins[$class] = $class;
                }

                // Check if the info is valid
                if(isset($info['NAME']) && isset($info['CLASS'])){
                    

                    // Check if there is nore than just the plugin definition
                    if(count($info) > 2){
                        foreach($info as $name => $value){
                            
                            if(!in_array($name, array('CLASS','NAME'))){
                                $class= $info['CLASS'];    
                                $this->fileStoredProperties[$class][strtolower($name)] = $value;
                            }
                        }
                    }
                }
            }
        }

        // Search for config flags defined in the config file (MENU section)
        foreach($this->config->data['MENU'] as $section => $entries){
            foreach($entries as $entry){

                if(isset($entry['CLASS'])){

                    // Put plugin to active plugins list.
                    $class = strtolower($entry['CLASS']);
                    $this->activePlugins[$class] = $class;
                
                    if(count($entry) > 2 ){
                        foreach($entry as $name => $value){
                            if(!in_array($name, array('CLASS','ACL'))){
                                $this->fileStoredProperties[strtolower($class)][strtolower($name)] = $value;
                            }
                        }
                    }
                }
            }
        }

        // Search for config flags defined in the config file (MAIN section)
        foreach($this->config->data['MAIN'] as $name => $value){
            $this->fileStoredProperties['core'][strtolower($name)] = $value;
        }

        // Search for config flags defined in the config file (Current LOCATION section)
        if(isset($this->config->current)){
            foreach($this->config->current as $name => $value){
                $this->fileStoredProperties['core'][strtolower($name)] = $value;
            }
        }

        // Skip searching for LDAP defined properties if 'ignoreLdapProperties' is set to 'true'
        //  in the config. 
        $this->ignoreLdapProperties = FALSE;
        if(isset($this->fileStoredProperties['core'][strtolower('ignoreLdapProperties')]) && 
            preg_match("/(true|on)/i", $this->fileStoredProperties['core'][strtolower('ignoreLdapProperties')])){
            $this->ignoreLdapProperties = TRUE;
        }

        // Search for all config flags defined in the LDAP - BUT only if we ARE logged in. 
        if(!empty($this->config->current['CONFIG'])){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['CONFIG']);
            $ldap->search('(&(objectClass=gosaConfig)(gosaSetting=*))', array('cn','gosaSetting'));
            while($attrs = $ldap->fetch()){
                $class = $attrs['cn'][0];
                for($i=0; $i<$attrs['gosaSetting']['count']; $i++){
                    list($name,$value) = preg_split("/:/",$attrs['gosaSetting'][$i],2);
                    $this->ldapStoredProperties[$class][$name] = $value;
                }
            }
        }

        // Register plugin properties.
        foreach ($this->classesWithInfo as $cname => $def){

            // Detect class name
            $name = $cname;
            $name = (isset($def['plShortName'])) ? $def['plShortName'] : $cname;
            $name = (isset($def['plDescription'])) ? $def['plDescription'] : $cname;

            // Register post events
            $this->classToName[$cname] = $name;
            $data = array('name' => 'postcreate','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'postremove','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'postmodify','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'precreate','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'preremove','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'premodify','type' => 'command');
            $this->register($cname, $data);    
            $data = array('name' => 'check', 'type' => 'command');
            $this->register($cname, $data);    

            // Register properties 
            if(isset($def['plProperties'])){
                foreach($def['plProperties'] as $property){
                    $this->register($cname, $property);
                }
            }
        }

        // We are only finsihed once we are logged in.
        if(!empty($this->config->current['CONFIG'])){
            $this->status = 'finished';
        }
    }

   
    /*! \brief      Returns TRUE if the property registration has finished without any error.
     */ 
    function propertyInitializationComplete()
    {
        return($this->status == 'finished');
    }


    /*! \brief      Registers a GOsa-Property and thus makes it useable by GOsa and its plugins.
     *  @param      String      $class  The name of the class/plugin that wants to register this property.
     *  @return     Array       $data   An array containing all data set in plInfo['plProperty]
     */
    function register($class,$data)
    {
        $id = count($this->properties);
        $this->properties[$id] = new gosaProperty($this,$class,$data);
        $p = strtolower("{$class}::{$data['name']}");
        $this->mapByName[$p] = $id;
    }


    /*! \brief      Returns all registered properties.
     *  @return     Array   A list of all properties.
     */
    public function getAllProperties()
    {
        return($this->properties);
    }


    /*! \brief      Checks whether a property exists or not.
     *  @param      String      $class  The class name (e.g. 'core' or 'mailAccount') 
     *  @param      String      $name   The property name (e.g. 'sessionTimeout' or 'mailMethod')
     *  @return     Boolean     TRUE if it exists else FALSE.
     */
    function propertyExists($class,$name)
    {       
        $p = strtolower("{$class}::{$name}");
        return(isset($this->mapByName[$p]));
    }


    /*! \brief      Returns the id of a registered property.
     *  @param      String      $class  The class name (e.g. 'core' or 'mailAccount') 
     *  @param      String      $name   The property name (e.g. 'sessionTimeout' or 'mailMethod')
     *  @return     Integer     The id for the given property.  
     */
    private function getId($class,$name)
    {
        $p = strtolower("{$class}::{$name}");
        if(!isset($this->mapByName[$p])){
            return(-1);
        }       
        return($this->mapByName[$p]);    
    }


    /*! \brief      Returns a given property, if it exists.
     *  @param      String      $class  The class name (e.g. 'core' or 'mailAccount') 
     *  @param      String      $name   The property name (e.g. 'sessionTimeout' or 'mailMethod')
     *  @return     GOsaPropery     The property or 'NULL' if it doesn't exists.
     */
    function getProperty($class,$name)
    {
        if($this->propertyExists($class,$name)){
            return($this->properties[$this->getId($class,$name)]);
        }
        return(NULL); 
    }


    /*! \brief      Returns the value for a given property, if it exists.
     *  @param      String      $class  The class name (e.g. 'core' or 'mailAccount') 
     *  @param      String      $name   The property name (e.g. 'sessionTimeout' or 'mailMethod')
     *  @return     GOsaPropery     The property value or an empty string if it doesn't exists.
     */
    function getPropertyValue($class,$name)
    {   
        if($this->propertyExists($class,$name)){
            $tmp = $this->getProperty($class,$name);
            return($tmp->getValue());
        }
        return("");
    }


    /*! \brief      Set a new value for a given property, if it exists.
     *  @param      String      $class  The class name (e.g. 'core' or 'mailAccount') 
     *  @param      String      $name   The property name (e.g. 'sessionTimeout' or 'mailMethod')
     *  @return     
     */
    function setPropertyValue($class,$name, $value)
    {   
        if($this->propertyExists($class,$name)){
            $tmp = $this->getProperty($class,$name);
            return($tmp->setValue($value));
        }
        return("");
    }


    /*! \brief      Save all temporary made property changes and thus make them useable/effective.
     *  @return     Array       Returns a list of plugins that have to be migrated before they can be saved.
     */
    function saveChanges()
    {
        $migrate = array();
        foreach($this->properties as $prop){

            // Is this property modified
            if(in_array($prop->getStatus(),array('modified','removed'))){

                // Check if we've to migrate something before we can make the changes effective. 
                if($prop->migrationRequired()){
                    $migrate[] = $prop;
                }else{
                    $prop->save();
                }
            }
        }
        return($migrate);
    }
}


class gosaProperty
{
    protected $name = "";
    protected $class = "";
    protected $value = "";
    protected $tmp_value = "";  // Used when modified but not saved 
    protected $type = "string";
    protected $default = "";
    protected $defaults = "";
    protected $description = "";
    protected $check = "";
    protected $migrate = "";
    protected $mandatory = FALSE;
    protected $group = "default";
    protected $parent = NULL;
    protected $data = array();

    protected $migrationClass = NULL;

    /*!  The current property status
     *     'ldap'       Property is stored in ldap 
     *     'file'       Property is stored in the config file
     *     'undefined'  Property is currently not stored anywhere
     *     'modified'   Property has been modified (should be saved)
     */
    protected $status = 'undefined';

    protected $attributes = array('name','type','default','description','check',
            'migrate','mandatory','group','defaults');




    function __construct($parent,$classname,$data)
    {
        // Set some basic infos 
        $this->parent = &$parent;
        $this->class = $classname;
        $this->data  = $data;

        // Get all relevant information from the data array (comes from plInfo)    
        foreach($this->attributes as $aName){
            if(isset($data[$aName])){
                $this->$aName = $data[$aName];
            }
        }      

        // Initialize with the current value
        $this->_restoreCurrentValue(); 

    }

    function migrationRequired()
    {
        // Instantiate migration class 
        if(!empty($this->migrate) && $this->migrationClass == NULL){
            if(!class_available($this->migrate)){
                trigger_error("Cannot start migration for gosaProperty::'{$this->getName()}' class not found ({$this->migrate})!");
            }else{
                $class = $this->migrate;
                $tmp = new $class($this->parent->config,$this);
                if(! $tmp instanceof propertyMigration){ 
                    trigger_error("Cannot start migration for gosaProperty::'{$this->getName()}' doesn't implement propertyMigration!");
                }else{
                    $this->migrationClass = $tmp;
                }
            }
        }
        if(empty($this->migrate) || $this->migrationClass == NULL){
            return(FALSE);
        }
        return($this->migrationClass->checkForIssues());
    }

    function getMigrationClass()
    {
        return($this->migrationClass);
    }

    function check()
    {
        $val = $this->getValue(TRUE);
        $return = TRUE;
        if($this->mandatory && empty($val)){
            $return = FALSE;
        }

        $check = $this->getCheck();
        if(!empty($val) && !empty($check)){
            $res = call_user_func(preg_split("/::/", $this->check),$messages=TRUE, $this->class,$this->name,$val, $this->type);
            if(!$res){
                $return = FALSE;
            }
        }
        return($return);
    }

    static function isBool($message,$class,$name,$value, $type)
    {
        $match = in_array($value,array('true','false',''));

        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The value '%s' specified for '%s:%s' is invalid. A bool value is required here!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }
    
        return($match);
    }

    static function isString($message,$class,$name,$value, $type)
    {
        $match = TRUE;
    
        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The value '%s' specified for '%s:%s' is invalid. A string value is required here!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }

        return($match);
    }

    static function isInteger($message,$class,$name,$value, $type)
    {
        $match = is_numeric($value) && !preg_match("/[^0-9]/", $value);

        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The value '%s' specified for '%s:%s' is invalid. A numeric value is required here!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }

        return($match);
    }

    static function isPath($message,$class,$name,$value, $type)
    {
        $match = preg_match("#^(/[^/]*/){1}#", $value);
    
        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The path '%s' specified for '%s:%s' is invalid!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }

        return($match);
    }

    static function isReadablePath($message,$class,$name,$value, $type)
    {
        $match = !empty($value)&&is_dir($value)&&is_writeable($value);
   
        // Display the reason for failing this check.         
        if($message && ! $match){
            if(!is_dir($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The folder '%s' specified for '%s:%s' does not exists!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }elseif(!is_readable($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The folder '%s' specified for '%s:%s' cannot be used for reading!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }
        }

        return($match);
    }

    static function isWriteableFile($message,$class,$name,$value, $type)
    {
        $match = !empty($value) && (is_file($value) || is_writeable($value));
   
        // Display the reason for failing this check.         
        if($message && ! $match){
            if(!is_writeable($value) || !is_file($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The file '%s' specified for '%s:%s' cannot be used for writing!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }
        }

        return($match);
    }

    static function isWriteablePath($message,$class,$name,$value, $type)
    {
        $match = !empty($value)&&is_dir($value)&&is_writeable($value);
   
        // Display the reason for failing this check.         
        if($message && ! $match){
            if(!is_dir($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The folder '%s' specified for '%s:%s' does not exists!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }elseif(!is_writeable($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The folder '%s' specified for '%s:%s' cannot be used for writing!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }
        }

        return($match);
    }

    static function isReadableFile($message,$class,$name,$value, $type)
    {
        $match = !empty($value) && is_readable($value) && is_file($value);

        // Display the reason for failing this check.         
        if($message && ! $match){
                
            if(!is_file($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The file '%s' specified for '%s:%s' does not exists!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }elseif(!is_readable($value)){
                msg_dialog::display(_("Warning"), 
                        sprintf(_("The file '%s' specified for '%s:%s' cannot be read!"), 
                            bold($value),bold($class),bold($name)), 
                        WARNING_DIALOG);
            }
        }

        return($match);
    }

    static function isCommand($message,$class,$name,$value, $type)
    {
        $match = TRUE;

        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The command '%s' specified for '%s:%s' is invalid!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }
        
        return($match);
    }

    static function isDn($message,$class,$name,$value, $type)
    {
        $match = preg_match("/^([a-z]*=[^=,]*,)*[^=]*=[^=]*$/i", $value);

        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The dn '%s' specified for '%s:%s' is invalid!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }
        
        return($match);
    }

    static function isRdn($message,$class,$name,$value, $type)
    {
        $match = preg_match("/^([a-z]*=[^=,]*,)*[^=]*=[^=,]*,?$/i", $value);

        // Display the reason for failing this check.         
        if($message && ! $match){
            msg_dialog::display(_("Warning"), 
                    sprintf(_("The rdn '%s' specified for '%s:%s' is invalid!"), 
                        bold($value),bold($class),bold($name)), 
                    WARNING_DIALOG);
        }
        
        return($match);
    }

    private function _restoreCurrentValue()
    {
        // First check for values in the LDAP Database.
        if(isset($this->parent->ldapStoredProperties[$this->class][$this->name])){
            $this->setStatus('ldap');
            $this->value = $this->parent->ldapStoredProperties[$this->class][$this->name];
            return;
        }

        // Second check for values in the config file.
        if(isset($this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)])){
            $this->setStatus('file');
            $this->value = $this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)];
            return;
        }

        // If there still wasn't found anything then fallback to the default.
        if($this->getStatus() == 'undefined'){
            $this->value = $this->getDefault();
        }
    }

    function getMigrate() { return($this->migrate); }
    function getCheck() { return($this->check); }
    function getName() { return($this->name); }
    function getClass() { return($this->class); }
    function getGroup() { return($this->group); }
    function getType() { return($this->type); }
    function getDescription() { return($this->description); }
    function getDefault() { return($this->default); }
    function getDefaults() { return($this->defaults); }
    function getStatus() { return($this->status); }
    function isMandatory() { return($this->mandatory); }

    function setValue($str) 
    {
        if(in_array($this->getStatus(), array('modified'))){
            $this->tmp_value = $str; 
        }elseif($this->value != $str){
            $this->setStatus('modified'); 
            $this->tmp_value = $str; 
        }
    }

    function getValue($temporary = FALSE) 
    { 
        if($temporary){
            if(in_array($this->getStatus(), array('modified','removed'))){
                return($this->tmp_value); 
            }else{
                return($this->value); 
            }
        }else{ 

            // Do not return ldap values if we've to ignore them.
            if($this->parent->ignoreLdapProperties){
                if(isset($this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)])){
                    return($this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)]);
                }else{
                    return($this->getDefault());
                }
            }else{
                return($this->value); 
            }
        }
    }

    function restoreDefault() 
    {
        if(in_array($this->getStatus(),array('ldap'))){
            $this->setStatus('removed'); 

            // Second check for values in the config file.
            if(isset($this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)])){
                $this->tmp_value = $this->parent->fileStoredProperties[strtolower($this->class)][strtolower($this->name)];
            }else{
                $this->tmp_value = $this->getDefault();
            }
        }
    }

    function save()
    {
        if($this->getStatus() == 'modified'){
            $ldap = $this->parent->config->get_ldap_link();
            $ldap->cd($this->parent->config->current['BASE']);
            $dn = "cn={$this->class},".$this->parent->config->current['CONFIG'];
            $ldap->cat($dn);
            if(!$ldap->count()){
                $ldap->cd($dn);
                $data = array(
                        'cn' => $this->class, 
                        'objectClass' => array('top','gosaConfig'),
                        'gosaSetting' => $this->name.":".$this->tmp_value);

                $ldap->add($data);
                if(!$ldap->success()){
                    echo $ldap->get_error();
                }

            }else{
                $attrs = $ldap->fetch();
                $data = array();
                $found = false;
                if(isset($attrs['gosaSetting']['count'])){
                    for($i = 0;$i<$attrs['gosaSetting']['count']; $i ++){
                        $set = $attrs['gosaSetting'][$i];
                        if(preg_match("/^{$this->name}:/", $set)){
                            $set = "{$this->name}:{$this->tmp_value}";
                            $found = true;
                        }
                        $data['gosaSetting'][] = $set;
                    }
                }
                if(!$found) $data['gosaSetting'][] = "{$this->name}:{$this->tmp_value}";
                $ldap->cd($dn);
                $ldap->modify($data);
                if(!$ldap->success()){
                    echo $ldap->get_error();
                }
            }
            $this->value = $this->tmp_value;
            $this->setStatus('ldap'); 
        }elseif($this->getStatus() == 'removed'){
            $ldap = $this->parent->config->get_ldap_link();
            $ldap->cd($this->parent->config->current['BASE']);
            $dn = "cn={$this->class},".$this->parent->config->current['CONFIG'];
            $ldap->cat($dn);
            $attrs = $ldap->fetch();
            $data = array('gosaSetting' => array());
            for($i = 0;$i<$attrs['gosaSetting']['count']; $i ++){
                $set = $attrs['gosaSetting'][$i];
                if(preg_match("/^{$this->name}:/", $set)){
                    continue;
                }
                $data['gosaSetting'][] = $set;
            }
            $ldap->cd($dn);
            $ldap->modify($data);
            if(!$ldap->success()){
                echo $ldap->get_error();
            }
            $this->_restoreCurrentValue();
        }
    }

    private function setStatus($state) 
    {
        if(!in_array($state, array('ldap','file','undefined','modified','removed'))) {
            trigger_error("Unknown property status given '{$state}' for {$this->class}:{$this->name}!");
        }else{
            $this->status = $state; 
        }
    }

    function isValid() 
    { 
        return(TRUE);    
    }
}



interface propertyMigration
{
    function __construct($config,$property);
}


?>
