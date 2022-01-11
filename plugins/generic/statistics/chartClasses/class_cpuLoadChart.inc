<?php

class cpuLoadChart extends memoryUsageChart
{
    protected $title = 'CPU load';
    protected $enableCaching = TRUE;

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();

        // Generate graph which displays the memory usage over time
        $this->series = array(
                'max_load' => _('Maximum'),
                'avg_load' => _('Average'),
                'min_load' => _('Minimum'));
        $this->dataName = "usagePerInterval";
    }
}
?>
