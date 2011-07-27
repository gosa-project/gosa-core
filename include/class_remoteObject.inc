<?php


/*! \brief  This class represents a server side object.
 *          All actions preformed on this object will be executed
 *           on the server using RPC calls.
 *          Properties will bestored directly (directStorage=TRUE) or when commit() is called.
 */
class remoteObject
{
    private $rpcHandle;
    private $properties;
    private $methods;
    private $type;
    private $object_id;
    private $ref_id;
    private $values;
    private $success;
    private $lastError;

    private $cache = array();

    // When set to true, property modifications will be 
    //  transfered to the server, immediately.
    // If it is false, changes will be transfered when 
    //  commit() is called.
    private $directStorage=FALSE;


    /*!\brief   Constructs the remoteObject.
     * @param   jsonRPC     The rpc connection handle. 
     * @param   String      The type of the object, e.g. 'user'.
     * @param   Array       A list of available properties.
     * @param   Array       A list of values for the properties.
     * @param   Array       A list of methods provided by this object. 
     * @param   String      A string that represents the object call.
     * @return  String      The server side object ID, used to identify the object.
     */
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


    /*!\brief   Returns the object type. 
     * @return  String  The type of the object. E.g. 'user'.
     */
    function getType()
    {
        return($this->type);
    }


    /*!\brief   Returns a list of available property names.
     * @return  Array   A list of property names.
     */
    function getProperties()
    {
        return($this->properties);
    }


    /*!\brief   Returns the objects reference ID.
     * @return  String the server side object id.
     */
    function getReferenceId()
    {
        return($this->ref_id);
    }


    /*!\brief  Clears all object modification when in not in 
     *          'directStorage' mode.
     */
    function clearCache()
    {
        $this->__clearCache();
    }


    /*!\brief   Catch all method for undefined function calls.
     *          This method detects setter, getter and methods calls
     *           and forwards them to the right object method.
     * @param   String  The name of the function to call. 
     * @param   Array   A list of parameters.
     * @return  Mixed   E.g. The answer from the server.
     */
    function __call($name, $args)
    {

        // Check if such an attribute is registered
        if(preg_match("/^get/", $name)){
            $varName = ucfirst(preg_replace("/^get/","", $name));
            $varName2 = lcfirst($varName);
            if(in_array_strict($varName, $this->properties)){
                $force = isset($args[0]) && $args[0];
                return($this->__getProperty($varName, $force));
            }elseif(in_array_strict($varName2, $this->properties)){
                $force = isset($args[0]) && $args[0];
                return($this->__getProperty($varName2, $force));
            } 
        }elseif(preg_match("/^set/", $name)){
            $varName = ucfirst(preg_replace("/^set/","", $name));
            $varName2 = lcfirst($varName);
            if(in_array_strict($varName, $this->properties)){
                return($this->__setProperty($varName, $args[0]));
            }elseif(in_array_strict($varName2, $this->properties)){
                return($this->__setProperty($varName2, $args[0]));
            }
        }

        // Forward to the call to the backend.
        if(in_array_strict($name, $this->methods)){
            $this->lastError = "";
            $this->success = TRUE;
            $fArgs = array();
            $fArgs[] = $this->ref_id;
            $fArgs[] = $name;
            $fArgs = array_merge($fArgs, $args);
            $res = call_user_func_array(array($this->rpcHandle,"dispatchObjectMethod"), $fArgs);

            if(!$this->rpcHandle->success()){
                $this->success = FALSE;
                $this->lastError = $this->rpcHandle->get_error();
                trigger_error($this->lastError);
            }

            return($res);
        }

        // Show an error, we do not know what to to with this..
        trigger_error("Unknown method '{$name}' called for {$this->object_id}!");
    }


    function success()
    {
        return($this->success);
    }

    function getError()
    {
        return($this->lastError);
    }


    /*!\brief   A catch all method for setter calls. 
     *           
     * @param   String  The name of the property to set. 
     * @param   String  The value to use.
     * @return
     */
    function __set($varName, $value)
    {
        // Set property value
        if(in_array_strict($varName, $this->properties)){
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


    /*!\brief   A catch all method for getter calls. 
     * @param   String  The name of the requested property. 
     * @return  Mixed.
     */
    function __get($varName)
    {
        if(in_array_strict($varName, $this->properties)){
            return($this->__getProperty($varName));
        }

        // Set class member value
        if(isset($this->$varName)){
            return($this->$varName);
        }

        trigger_error("No attribute '{$varName}' defined!");
        return(NULL);
    }


    /*!\brief   Internal method used to set properties. 
     * @param   String  The name of property to set.
     * @param   Mixed   The new value for the property.
     * @return  Boolean true on success else false.
     */
    function __setProperty($name, $value, $forceSave = FALSE)
    {
        if($this->directStorage || $forceSave){
            $this->rpcHandle->setObjectProperty($this->ref_id, $name,$value);
            if($this->rpcHandle->success()){
                $this->__addPropValueToCache($name, $value);
                return(TRUE);
            }else{
                return(FALSE);
            }
        }else{
            $this->__addPropValueToCache($name, $value);
            return(TRUE);
        }
    }


    /*!\brief   Internal method used to get property values. 
     * @param   String  The name of the property. 
     * @return  Mixed.
     */
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
            $this->values[$name] = $res;
            return($res);
        }
    }


    /*!\brief   Closes the object on the server side. 
     * @return  The closing status. 
     */
    function close()
    {
        $res = $this->rpcHandle->closeObject($this->ref_id);
        if($this->success){
            $this->ref_id = "";
        }
        return($this->rpcHandle->success());
    }


    /*!\brief   Internal method used to add property values  to the cache.
     * @param   String  The name of the propterty to add. 
     * @param   String  The value of the property to add. 
     */
    function __addPropValueToCache($name, $value)
    {
        $this->cache[$name] = $value;
    }


    /*!\brief   Internal method used to fetch property values from the cache. 
     * @param   String  The name of the property to fetch. 
     * @return  Mixed.
     */
    function __getPropFromCache($name)
    {
        return($this->cache[$name]);
    }


    /*!\brief   Internal method to check whether a property value is cached or not. 
     * @param   String  The name of the property.
     * @return  Boolean True on success else false
     */
    function __propIsCached($name)
    {
        return(isset($this->cache[$name]));
    }


    /*!\brief   Clears the internal property cache.
     */
    function __clearCache()
    {
        $this->cache = array();
    }

    
    /* \brief   See commit();
     */
    function save()
    {
        return($this->commit());
    }

    
    /*! \brief  Saves property modifications back to the server.
     *          This is only necessary in directStorage mode.
     * @param   Boolean     If set to true all attributes will be saved, even not modified.
     */
    function commit($saveUntouchedPropertiesToo = FALSE)
    {
        foreach($this->properties as $prop){
            if(!$this->__propIsCached($prop)) continue;
            if($saveUntouchedPropertiesToo || $this->values[$prop] != $this->__getPropFromCache($prop)){
                $this->__setProperty($prop, $this->__getPropFromCache($prop), TRUE);
            }
        }
    }


    /*!\brief   Internal method which removes a property from the cache. 
     * @param   String  The name of the property. 
     * @return
     */
    function __removePropFromCache($name)
    {
        if($this->__propIsCached($name)){
            unset($this->cache[$name]);
        }
    }
}

?>
