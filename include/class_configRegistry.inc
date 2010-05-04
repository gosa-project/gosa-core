<?php

class configRegistry{

    public $config = NULL;
    public $properties = array();
    public $mapByClass = array();
    public $mapPropertyToClass = array();
    public $ldapStoredProperties = array(); 
    public $fileStoredProperties = array(); 
    public $classToName = array(); 

    public $status = 'none';

    function __construct($config)
    {
        restore_error_handler();
        $this->config = &$config;
        $this->reload();
    }

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
        $this->mapByClass = array();
        $this->mapPropertyToClass = array();
        
        // Search for config flags defined in the config file
        foreach($this->config->data['TABS'] as $tabname => $tabdefs){
            foreach($tabdefs as $info){

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
            $this->status = 'finished';
        }
 
        global $class_mapping;
        foreach ($class_mapping as $cname => $path){
            $cmethods = get_class_methods($cname);
            if (is_array($cmethods) && in_array_ics('plInfo',$cmethods)){

                // Get plugin definitions
                $def = call_user_func(array($cname, 'plInfo'));;

                // Register Post Events (postmodfiy,postcreate,postremove,checkhook)
                if(isset($def['plShortName'])){
                    $this->classToName[$cname] = $def['plShortName'];
                    $data = array('name' => 'postcreate','type' => 'string');
                    $this->register($cname, $data);    
                    $data = array('name' => 'postremove','type' => 'string');
                    $this->register($cname, $data);    
                    $data = array('name' => 'postmodify','type' => 'string');
                    $this->register($cname, $data);    
                    $data = array('name' => 'checkhook', 'type' => 'string');
                    $this->register($cname, $data);    
                }

                if(isset($def['plProperties'])){
                    foreach($def['plProperties'] as $property){
                        $this->register($cname, $property);
                    }
                }
            }
        }
    }

    function register($class,$data)
    {
        $id = count($this->properties);
        $this->properties[$id] = new gosaProperty($this,$class,$data);
        $this->mapByName[$class][$data['name']] = $id;
        $this->mapByClass[$class][] = $id;
        $this->mapPropertyToClass[$id] = $class;
    }

    public function getAllProperties()
    {
        return($this->properties);
    }

    function propertyExists($class,$name)
    {
        return(isset($this->mapByName[$class][$name]));
    }

    private function getId($class,$name)
    {
        return($this->mapByName[$class][$name]);    
    }

    function getProperty($class,$name)
    {
        if($this->propertyExists($class,$name)){
            return($this->properties[$this->getId($class,$name)]);
        }
        return(NULL); 
    }

    function getPropertyValue($class,$name)
    {   
        if($this->propertyExists($class,$name)){
            $tmp = $this->getProperty($class,$name);
            return($tmp->getValue());
        }
        return("");
    }

    function setPropertyValue($class,$name, $value)
    {   
        if($this->propertyExists($class,$name)){
            $tmp = $this->getProperty($class,$name);
            return($tmp->setValue($value));
        }
        return("");
    }
}


class gosaProperty
{
    protected $name = "";
    protected $class = "";
    protected $value = "";
    protected $type = "string";
    protected $default = "";
    protected $description = "";
    protected $check = "";
    protected $migrate = "";
    protected $mandatory = FALSE;
    protected $group = "default";
    protected $parent = NULL;
    protected $data = array();

    /*!  The current property status
     *     'ldap'       Property is stored in ldap 
     *     'file'       Property is stored in the config file
     *     'undefined'  Property is currently not stored anywhere
     *     'modified'   Property has been modified (should be saved)
     */
    protected $status = 'undefined';

    protected $attributes = array('name','type','default','description','check',
            'migrate','mandatory','group');

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

        // First check for values in the LDAP Database.
        if(isset($this->parent->ldapStoredProperties[$this->class][$this->name])){
            $this->setStatus('ldap');
            $this->value = $this->parent->ldapStoredProperties[$this->class][$this->name];
        }

        // Second check for values in the config file.
        if(isset($this->parent->fileStoredProperties[$this->class][$this->name])){
            $this->setStatus('file');
            $this->value = $this->parent->fileStoredProperties[$this->class][$this->name];
        }

        // If there still wasn't found anything then fallback to the default.
        if($this->getStatus() == 'undefined'){
            $this->value = $this->getDefault();
        }
    }

    function getValue() { return($this->value); }
    function getMigrate() { return($this->migrate); }
    function getCheck() { return($this->check); }
    function getName() { return($this->name); }
    function getClass() { return($this->class); }
    function getGroup() { return($this->group); }
    function getType() { return($this->type); }
    function getDescription() { return($this->description); }
    function getDefault() { return($this->default); }
    function getStatus() { return($this->status); }
    function isMandatory() { return($this->mandatory); }

    function setValue($str) 
    {
        $this->setStatus('modified'); 
        $this->value = $str; 
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
                            'gosaSetting' => $this->name.":".$this->value);

                $ldap->add($data);
                if($ldap->success()){
                    $this->status = 'ldap';
                }else{
                    echo $ldap->get_error();
                }
            }else{
                $attrs = $ldap->fetch();
                $data = array();
                $found = false;
                for($i = 0;$i<$attrs['gosaSetting']['count']; $i ++){
                    $set = $attrs['gosaSetting'][$i];
                    if(preg_match("/^{$this->name}:/", $set)){
                        $set = "{$this->name}:{$this->value}";
                        $found = true;
                    }
                    $data['gosaSetting'][] = $set;
                }
                if(!$found) $data['gosaSetting'][] = "{$this->name}:{$this->value}";
                $ldap->cd($dn);
                $ldap->modify($data);
                if($ldap->success()){
                    $this->status = 'ldap';
                }else{
                    echo $ldap->get_error();
                }
            } 
        }
    }

    private function setStatus($state) 
    {
        if(!in_array($state, array('ldap','file','undefined','modified'))) {
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

?>
