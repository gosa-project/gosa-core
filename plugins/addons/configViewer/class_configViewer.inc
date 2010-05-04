<?php

class configViewer extends plugin
{
    var $plHeadline= "Config viewer";
    var $plDescription= "Allows to modify configuration values";

    var $configRegistry = NULL;    

    function __construct (&$config, $dn= NULL)
    {
       $this->config = &$config;
       $this->configRegistry = &$config->configRegistry;
    }

    function execute()
    {
        $all = $this->configRegistry->getAllProperties();
        $str = "";
        foreach($all as $p){
            $str.="
                <tr>        
                    <td>{$p->getClass()}</td>
                    <td>{$p->getName()}</td>
                    <td>{$p->getDescription()}</td>
                    <td><input type='text' value='{$p->getValue()}'></td>
                    <td>{$p->getStatus()}</td>
                </tr>";
        }

        return("<table cellspacing=0 cellpadding=0>{$str}</table>");
    }
}
?>
