<?php


class jsonROP
{
    private $config;

    function __construct($config){
        $this->config = &$config;
    }

    function getObject($objectID)
    {
        
        $dL = $this->config->configRegistry->getProperty('core', 'debugLevel');
        $dL->setValue(0);   
        $dL->save();

        $rpc = $this->config->getRpcHandle('http://10.3.64.59:8088','','');
        $res = $rpc->openObject($objectID, 12);

        $classDef = $res['__jsonclass__'][1];

        $type = $classDef[0];
        $uuid = $classDef[1];
        $object_id = $classDef[2];
        $methods = $classDef[3];
        $properties = $classDef[4];
        $values = array();

        $object = new remoteObject($rpc, $type, $properties, $values, $methods, $object_id, $uuid);

        $object->set_sn("Wurst"); 
        echo $object->get_sn();

        $object->sn = "Timmay";
        echo $object->sn;
    }
}


class remoteObject
{
    private $rpcHandle;
    private $properties;
    private $methods;
    private $type;
    private $object_id;
    private $uuid;
    private $values;

    function __construct(&$rpcHandle, $type, $properties, $values, $methods, $object_id, $uuid)
    {
        $this->rpcHandle = $rpcHandle;
        $this->properties = $properties;
        $this->methods = $methods;
        $this->type = $type;
        $this->uuid = $uuid;
        $this->object_id = $object_id;
        $this->values = $values;
    }


    // Enables calls like  get_property() and set_property()
    //  and allow to call the dynamic methods
    function __call($name, $args)
    {
        // Check if such an attribute is registered
        if(preg_match("/^get_/", $name)){
            $varName = preg_replace("/^get_/","", $name);
            if(in_array($varName, $this->properties)){
                return($this->__getProperty($varName));
            } 
        }elseif(preg_match("/^set_/", $name)){
            $varName = preg_replace("/^set_/","", $name);
            if(in_array($varName, $this->properties)){
                return($this->__setProperty($varName, $args[0]));
            }
        }

        echo "<br><b>Calling: {$name}</b>";
        #return($this->rpcHandle->$name($args));
    }


    // Enables calls like  $object->mailAddress = 'test';
    function __set($varName, $value)
    {
        // Set property value
        if(in_array($varName, $this->properties)){
            return($this->__setProperty($varName, $value));
        }

        // Set class member value
        if(isset($this->$varName)){
            $this->$varName = $value;
            return(TRUE);
        }

        trigger_error("No attribute '{$varName}' defined!");
        return(FALSE);
    }


    function __get($varName)
    {
        if(in_array($varName, $this->properties)){
            return($this->__getProperty($varName));
        }

        // Set class member value
        if(isset($this->$varName)){
            return($this->$varName);
        }

        trigger_error("No attribute '{$varName}' defined!");
        return(NULL);
    }

    
    function __setProperty($name, $value)
    {
        $this->rpcHandle->setObjectProperty($this->uuid, $name,$value);
        if($this->rpcHandle->success()){
            $this->__addPropValueToCache($name, $value);
            return(TRUE);
        }
        return(FALSE);
    }


    function __getProperty($name)
    {
        if($this->__propIsCached($name)){
            return($this->__getPropFromCache($name));
        }

        $res = $this->rpcHandle->getObjectProperty($this->uuid, $name);
        if(!$this->rpcHandle->success()){
            return(NULL);
        }else{
            $this->__addPropValueToCache($name, $res);
            return($res);
        }
    }

    
    function __addPropValueToCache($name, $value)
    {

    }

    function __getPropFromCache($name, $value)
    {

    }

    function __propIsCached($name)
    {
        return(FALSE);
    }

    function __removePropFromCache($name, $value)
    {
        return(FALSE);
    }

}

?>
