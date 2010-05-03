<?php

class configRegistry{

    private $properties = array();

    function __construct(){
        $this->reload();
    }

    function reload()
    {
        global $class_mapping;
        foreach ($class_mapping as $cname => $path){
            $cmethods = get_class_methods($cname);
            if (is_array($cmethods) && in_array_ics('plInfo',$cmethods)){
                $def = call_user_func(array($cname, 'plInfo'));;
                if(isset($def['plProperties'])){
                    foreach($def['plProperties'] as $property){
                        $this->register(
                                $property['name'],
                                $property['type'],
                                $property['default'],
                                $property['description'],
                                $property['check'],
                                $property['migrate'],
                                $property['mandatory']);
                    }
                }
            }
        }
    }

    function register($name,$type,$default,$description,$check,$migrate,$mandatory)
    {
        $this->properties[$name] = new gosaProperty($name,$type,$default,$description,$check,$migrate,$mandatory);
    }

    function propertyExists($name)
    {
        return(isset($this->properties[$name]));
    }

    function getProperty($name)
    {
        if($this->propertyExists($name)){
            return($this->properties[$name]);
        }
        return(NULL); 
    }

    function getPropertyValue($name)
    {   
        if($this->propertyExists($name)){
            return($this->properties[$name]->getValue());
        }
        return("");
    }
}


class gosaProperty
{
    protected $name = "";
    protected $type = "string";
    protected $default = "";
    protected $description = "";
    protected $check = "";
    protected $migrate = "";
    protected $mandatory = FALSE;

    function __construct($name,$type,$default,$description,$check,$migrate,$mandatory)
    {
        $this->name = $name;
        $this->type = $type;
        $this->default = $default;
        $this->description = $description;
        $this->check = $check;
        $this->migrate = $migrate;
        $this->mandatory = $mandatory;
    }

    function getValue() { return($this->default); }
    function getName() { return($this->name); }
    function getType() { return($this->type); }
    function getDescription() { return($this->description); }
    function getDefault() { return($this->default); }

    function isValid() 
    { 
        return(TRUE);    
    }
}

?>
