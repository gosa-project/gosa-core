<?php


class migrate_userRDN implements propertyMigration
{
    private $property = NULL;

    function __construct($property)
    {
        $this->property = &$property;
    }   

    function checkForIssues()
    {
        return(TRUE);
    }

    function execute()
    {
        return("<input type='submit'>");
    }
   
    function save_object()
    {
    
    } 

    function check()
    {
        return(array());
    }
} 

?>
