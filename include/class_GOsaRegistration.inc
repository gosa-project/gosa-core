<?php

class GOsaRegistration 
{
    
    private $config;
    private $server = "http://10.3.64.59:4000";
    private $user = "gosa";
    private $password = "gosa";

    private $isRegistered = NULL;    

    function __construct(&$config)
    {
        // Check if we're registrated 

        $this->config = $config;
    }    

 
    function getRegistrationServer()
    {
        return($this->server);
    }   

    function getConnection($user = NULL, $password ="")
    {
        if($user === NULL){
            return($this->config->getRpcHandle($this->server, $this->user,$this->password, TRUE, FALSE));
        }else{
            return($this->config->getRpcHandle($this->server, $user,$password, TRUE, FALSE));
        }
    }

    function isServerAccessible($force = FALSE)
    {
        // Only request a new status every 2 seconds
        if(isset($this->cache['isServerAccessible']['called']) && !$force){
            if($this->cache['isServerAccessible']['called'] + 2 > time()){
                return($this->cache['isServerAccessible']['result']);
            }
        }

        // Check the connection status by calling a dummy function
        $con = $this->getConnection();
        $res = $con->isInstanceRegistered("dummy");

        // Store the result
        $this->cache['isServerAccessible']['called'] = time();        
        $this->cache['isServerAccessible']['result'] = $con->success();

        return($con->success());
    }

    function registrationNotWanted()
    {
        // While we are registered this will always be FALSE.
        if($this->isInstanceRegistered()) return(FALSE);

        // Check if the registration process was postponed or completely(>=0) canceled (-1)
        $date = $this->config->configRegistry->getPropertyValue('GOsaRegistration','askForRegistration');
        return($date == -1);
    }

    function registrationRequired()
    {
        if($this->isInstanceRegistered()) return(FALSE);

        // Seems that we haven't received an instancePassword yet, this can has two reasons:
        // 1. Not registered yet or registration postponed     2. We do not want to registrate our instance.
        $date = $this->config->configRegistry->getPropertyValue('GOsaRegistration','askForRegistration');
        if($date == -1){

            // We do not want to registrate
            return(FALSE);
        }else{

            // Registration may be postponed.
            return($date < time());
        }
    }

    function isInstanceRegistered()
    {
        if($this->isServerAccessible()){

            // First check if the server is accessible and if the instance is registered.
            return($this->isInstanceRegisteredWithServer());
        }else{

            // Server is down, now check if we've an instancePassword set in our gosa.conf.
            $instancePassword = $this->config->configRegistry->getPropertyValue('GOsaRegistration','instancePassword'); 
            return(!empty($instancePassword));
        }
    }

    function isInstanceRegisteredWithServer()
    {
        $con = $this->getConnection();
        $res = $con->isInstanceRegistered($this->config->getInstanceUuid());
        if($con->success()){
            $this->isRegistered = $res;
        }else{
            return(FALSE);
        }
        return($this->isRegistered);
    }


    static function plInfo()
    {
        return (array(
                    "plProperties"  => array(
                        array(
                            "name"          => "instancePassword",
                            "type"          => "string",
                            "default"       => "",
                            "description"   => "",
                            "check"         => "gosaProperty::isString",
                            "migrate"       => "",
                            "group"         => "registration",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "askForRegistration",
                            "type"          => "integer",
                            "default"       => "0",
                            "description"   => _("UNIX-timestamp pointing to the date GOsa will ask for a registration again (-1 to disable)"), 
                            "check"         => "gosaProperty::isInteger",
                            "migrate"       => "",
                            "group"         => "registration",
                            "mandatory"     => FALSE),
                        )
                    )
                    );
    }
} 

?>
