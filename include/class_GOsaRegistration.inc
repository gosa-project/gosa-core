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

    function isServerAccessible()
    {
            $con = $this->getConnection();
            $res = $con->isInstanceRegistered("dummy");
            if($con->success()){
                return(TRUE);
            }else{
                return(FALSE);
            }
    }

    function registrationNotWanted()
    {
        // Check if the registration process was postponed or completely(>=0) canceled (-1)
        $date = $this->config->configRegistry->getPropertyValue('GOsaRegistration','askForRegistration');
        return($date == -1);
    }

    function registrationRequired()
    {
        if($this->isInstanceRegistered) return(FALSE);

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
            return(!empty($this->config->instancePassword));
        }
    }

    function isInstanceRegisteredWithServer()
    {
        if($this->isRegistered === NULL){
            $con = $this->getConnection();
            $res = $con->isInstanceRegistered($this->config->getInstanceUuid());
            if($con->success()){
                $this->isRegistered = $res;
            }else{
                return(FALSE);
            }
        }
        return($this->isRegistered);
    }


    static function plInfo()
    {
        return (array(
                    "plProperties"  => array(
                        array(
                            "name"          => "askForRegistration",
                            "type"          => "integer",
                            "default"       => "0",
                            "description"   => _("Represents a unix-timestamp which points to the date, GOsa will ask for a registration again. -1 = Never, 0 = Now else once we reached the gieven timestamp"), 
                            "check"         => "gosaProperty::isInteger",
                            "migrate"       => "",
                            "group"         => "registration",
                            "mandatory"     => FALSE),
                        ))); 
    }
} 

?>