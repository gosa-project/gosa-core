<?php

class renderTimeChart extends memoryUsageChart
{
    protected $title = 'Render time';
    protected $enableCaching = TRUE;

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();

        // Generate graph which displays the memory usage over time
        $this->series = array(
                'max_render' => _('Max'),
                'avg_render' => _('Avergae'),
                'min_render' => _('Min'));
        $this->dataName = "usagePerInterval";
    }
}
?>
