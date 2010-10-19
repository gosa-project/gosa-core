<?php


class jsonROP
{
    private $config;

    function __construct($config){
        $this->config = &$config;
    }

    function getObject($objectID)
    {
        $rpc = $this->config->getRpcHandle();
        #$res = $rpc->getObject($objectID, 12);
        
        $str = '{"id": "abcdef", "result": {"__jsonro__":{"type":"User", "methods":["notify", "remove", "setPassword"], "properties":["sn", "givenName", "uid", "telephoneNumber"]}, "object_id": "07bb3760-db8e-11df-b5c0-5452005f1250", "givenName": "Cajus", "sn": "Pollmeier", "uid": "cajus"}, "error": null}';

        $res = json_decode($str);
        $result = $res->result;       
        $classDef = $result->__jsonro__;
       
        $object = new remoteObject($rpc, $classDef->type, 
            $classDef->properties, array() ,$classDef->properties, $result->object_id);


        $object->set_sn ('Wurst'); 
        $object->sn = 'Horst';
        echo $object->sn;
        echo $object->get_sn();
    }


}


class remoteObject
{
    private $rpcHandle;
    private $properties;
    private $methods;
    private $type;
    private $oid;
    private $values;

    function __construct(&$rpcHandle, $type, $properties, $values, $methods, $oid)
    {
        $this->rpcHandle = $rpcHandle;
        $this->properties = $properties;
        $this->methods = $methods;
        $this->type = $type;
        $this->oid = $oid;
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
                return($this->rpcHandle->$name($args));
            } 
        }elseif(preg_match("/^set_/", $name)){
            $varName = preg_replace("/^set_/","", $name);
            if(in_array($varName, $this->properties)){
                return($this->rpcHandle->$name($args));
            }
        }
        trigger_error("Invalid method called '{$name}'");
    }


    // Enables calls like  $object->mailAddress = 'test';
    function __set($varName, $args)
    {
        if(in_array($varName, $this->properties)){
            $func = "set_{$varName}";
            $this->rpcHandle->$func($args);
            return;
        }

        if(isset($this->$varName)){
            $this->$varName = $args;
            return;
        }
        trigger_error("No attribute '{$varName}' defined!");
    }

    function __get($varName)
    {
        if(in_array($varName, $this->properties)){
            $func = "get_{$varName}";
            $this->rpcHandle->$func($args);
        }
        trigger_error("No attribute '{$varName}' defined!");
    }

}

?>
