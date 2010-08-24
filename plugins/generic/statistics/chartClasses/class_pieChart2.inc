<?php

class pieChart2 extends pieChart1
{
    var $title = 'Action usage';
    var $enableCaching = FALSE;

    var $keyName = "actionsPerPluginAction";

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();
    }
}
?>
