<?php

class memoryUsageChart extends statChart
{
    var $title = 'Memory usage';
    var $enableCaching = TRUE;

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();

        // Generate graph which displays the memory usage over time
        $this->series = array(
                'max_mem' => _('Max'),
                'avg_mem' => _('Avergae'),
                'min_mem' => _('Min'));
        $this->dataName = "usagePerInterval";
    }


    /*! \brief  Generates the line-graph which displays the plugin usage over time.
     */ 
    function render()
    {
        // Add series data to dataSet        
        $dataSet = new pData;  
        $max = 0;
        foreach($this->series as $seriesName => $seriesDesc){
            if(isset($this->graphData[$this->dataName][$seriesName])){
                $dataSet->AddPoint($this->graphData[$this->dataName][$seriesName],$seriesName);
                $dataSet->SetSerieName($seriesDesc,$seriesName);
                if($max < max($this->graphData[$this->dataName][$seriesName])) $max = max($this->graphData[$this->dataName][$seriesName]);
            }
        }
        $dataSet->AddAllSeries();  
        $dataSet->AddPoint($this->graphData['dates'], 'date');
        $dataSet->SetAbsciseLabelSerie('date');  
       
        echo $this->enableCaching;
 
        // Read graph from cache?
        $pCache = new pCache('/var/spool/gosa/');
        if($this->enableCaching && $pCache->IsInCache(get_class(),$dataSet->GetData())){
            $filename =  $pCache->GetHash(get_class(),$dataSet->GetData());
            $filename = '/var/spool/gosa/'.$filename;    
            if(file_exists($filename) && is_readable($filename)){
                $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE)).rand(0,99999);
                session::set('statistics::graphFile'.$this->graphID,$filename);
                return;
            }
        }

        $chart = new pChart(800,230);  
        $chart->setFixedScale(0.0001,($max*1.1));  
        $chart->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
        $chart->setGraphArea(50,30,585,200);  
        $chart->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
        $chart->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
        $chart->drawGraphArea(255,255,255,TRUE);  
        $chart->drawGrid(4,TRUE,200,200,200,50);  
        $chart->drawTreshold(0,143,55,72,TRUE,TRUE);  
        $chart->drawTitle(50,22,$this->title,50,50,50,585);  
        $chart->drawLegend(650,30,$dataSet->GetDataDescription(),255,255,255);  

        $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, FALSE);
#        $chart->drawFilledCubicCurve($dataSet->GetData(),$dataSet->GetDataDescription(),.1,50); 
        $chart->drawFilledLineGraph($dataSet->GetData(),$dataSet->GetDataDescription(),50,TRUE); 

        // Generate new and unique graph id
        $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphID;
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphID,$file);
        $pCache->WriteToCache(get_class(),$dataSet->GetData(),$chart);   

        return;
    }

}
?>
