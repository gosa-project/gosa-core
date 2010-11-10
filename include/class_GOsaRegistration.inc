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

    function registrationRequired()
    {
        $date = $this->config->configRegistry->getPropertyValue('GOsaRegistration','askForRegistration');
        if($date == -1){
            return(FALSE);
        }else{
            return($date < time());
        }
    }

    function isInstanceRegistered()
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
