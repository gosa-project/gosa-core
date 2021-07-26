<?php

class pieChart1 extends statChart
{
    protected $title = 'Plugin usage';
    protected $keyName = "actionsPerCategory";


    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();
    }

    /*! \brief  Generate the pie-chart which displays the overall-plugin-usage
     */
    function render()
    {
        // Do nothing, if we've no data.
        $data = array();
        if(isset($this->graphData[$this->keyName]) && count($this->graphData[$this->keyName])){
            $data = $this->graphData[$this->keyName];
        }

        // Sort data by usage count and slice array to get 
        //  the eight most used categories
        arsort($data);
        
        // Detect max value and throw out entries less than 1% 
        //  the will else be unreadable

        $max = (count($data)) ? max($data) : 1;
        $pos = 0;
        foreach($data as $key => $count){
            if($pos >=7 || ($count / $max)*100 < 1) break;
            $pos ++;
        }

        $this->graphID = 0;
 
        $dataRes = array_slice($data,0,$pos);

        // Get the rest of categories and combine them 'others'
        $theRest = array_slice($data,$pos);
        $dataRes['remaining'] = array_sum($theRest);

        // Try to find a translation for the given category names
        $values = array_values($dataRes);
        $keys = array_keys($dataRes);
        foreach($keys as $id => $cat){
            $keys[$id] = $this->getCategoryTranslation($cat);
        }

        if(!count($data)) return;

        // Dataset definition   
        $dataSet = new pData;  
        $dataSet->AddPoint($values,"Serie1");  
        $dataSet->AddAllSeries();  
        $dataSet->AddPoint($keys,"Serie2");  
        $dataSet->SetAbsciseLabelSerie("Serie2");  

        // Read graph from cache?
        $pCache = new pCache($this->cachePath);
        if($this->enableCaching && $pCache->IsInCache(get_class(),$dataSet->GetData())){
            $filename =  $pCache->GetHash(get_class(),$dataSet->GetData());
            $filename = '/var/spool/gosa/'.$filename;    
            if(file_exists($filename) && is_readable($filename)){
                $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE)).rand(0,99999);
                session::set('statistics::graphFile'.$this->graphID,$filename);
                return;
            }
        }

        // Set graph area
        $x = 400;
        $y = 200;

        // Initialise the graph  
        $chart = new pChart($x,$y);  
        $chart->setFontProperties($this->font,10); 
        if(count($data)) {
            $chart->drawPieGraph($dataSet->GetData(),$dataSet->GetDataDescription(),($x/3),($y/2)-10,($y/2),PIE_PERCENTAGE,TRUE,50,20,6);  
            $chart->drawPieLegend(($x/3*2),15,$dataSet->GetData(),$dataSet->GetDataDescription(),255,255,255);
        }

        // Store graph data
        $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphID;
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphID,$file);
        $pCache->WriteToCache(get_class(),$dataSet->GetData(),$chart);   
    }
}
?>
