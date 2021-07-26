<?php

class durationTimeChart extends memoryUsageChart
{
    protected $title = 'Plugin duration';
    protected $enableCaching = TRUE;

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();

        // Generate graph which displays the memory usage over time
        $this->series = array(
                'max_dur' => _('Maximum'),
                'avg_dur' => _('Average'),
                'min_dur' => _('Minimum'));
        $this->dataName = "usagePerInterval";
    }
}
?>
