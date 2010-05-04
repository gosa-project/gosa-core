<?php

class configViewer extends plugin
{
    var $plHeadline= "Config viewer";
    var $plDescription= "Allows to modify configuration values";

    function __construct (&$config, $dn= NULL)
    {

    }

    function execute()
    {
        return('Ja');
    }
}
?>
