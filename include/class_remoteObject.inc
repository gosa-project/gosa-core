<?php

class remoteObject
{
    private $rpcHandle;
    private $properties;
    private $methods;
    private $type;
    private $object_id;
    private $ref_id;
    private $values;

    private $cache = array();

    function __construct(&$rpcHandle, $type, $properties, $values, $methods, $object_id, $ref_id)
    {
        $this->rpcHandle = $rpcHandle;
        $this->properties = $properties;
        $this->methods = $methods;
        $this->type = $type;
        $this->ref_id = $ref_id;
        $this->object_id = $object_id;
        $this->values = $values;
        $this->cache = $values;
    }


    function getType()
    {
        return($this->type);
    }
    
    function getProperties()
    {
        return($this->properties);
    }
    
    function getReferenceId()
    {
        return($this->red_id);
    }
   
    function clearCache()
    {
        $this->__clearCache();
    }
 

    // Enables calls like  get_property() and set_property()
    //  and allow to call the dynamic methods
    function __call($name, $args)
    {
        // Check if such an attribute is registered
        if(preg_match("/^get_/", $name)){
            $varName = preg_replace("/^get_/","", $name);
            if(in_array($varName, $this->properties)){
                $force = isset($args[0]) && $args[0];
                return($this->__getProperty($varName, $force));
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
        $this->rpcHandle->setObjectProperty($this->ref_id, $name,$value);
        if($this->rpcHandle->success()){
            $this->__addPropValueToCache($name, $value);
            return(TRUE);
        }
        return(FALSE);
    }


    function __getProperty($name, $force = FALSE)
    {
        if(!$force && $this->__propIsCached($name)){
            return($this->__getPropFromCache($name));
        }

        $res = $this->rpcHandle->getObjectProperty($this->ref_id, $name);
        if(!$this->rpcHandle->success()){
            return(NULL);
        }else{
            $this->__addPropValueToCache($name, $res);
            return($res);
        }
    }

    
    function close()
    {
        $res = $this->rpcHandle->closeObject($this->ref_id);
        if($this->success){
            $this->ref_id = "";
        }
        return($this->rpcHandle->success());
    }

    
    function __addPropValueToCache($name, $value)
    {
        $this->cache[$name] = $value;
    }

    function __getPropFromCache($name)
    {
        return($this->cache[$name]);
    }

    function __propIsCached($name)
    {
        return(isset($this->cache[$name]));
    }

    function __clearCache()
    {
        $this->cache = array();
    }

    function __removePropFromCache($name)
    {
        if($this->__propIsCached($name)){
            unset($this->cache[$name]);
        }
    }
}

?>
