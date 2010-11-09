<?php

class GOsaRegistration 
{
    private $config;

    function __construct(&$config)
    {
        $this->config = $config;
    }    

    

    function isInstanceRegistered()
    {
        return(FALSE);
    }
} 

?>
