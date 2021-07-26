<?php

class pieChart2 extends pieChart1
{
    protected $title = 'Action usage';
    protected $keyName = "actionsPerPluginAction";

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();
    }
}
?>
