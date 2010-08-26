<?php

class actionSelectChart extends statChart
{
    protected $title = 'Action count per plugin';
    protected $current_action = "";

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();
    }


    /*! \brief  Generates the line-graph which displays the plugin usage over time.
     */ 
    function render()
    {
        $lineMax = 10;
        $gData = $this->graphData;
        $dataSet = new pData;  
        $seriesCnt = 0;
        $pCache = new pCache($this->cachePath);

        // Check if we've received data for the graph we've to render now.
        $seriesValid = count($gData['actionTypeGraph']);
        if($seriesValid){

            // Ensure that we've a valid action type selected.
            if(!isset($gData['actionTypeGraph'][$this->current_action])){
                $this->current_action = key($gData['actionTypeGraph']);
            }

            $dataBase = $gData['actionTypeGraph'][$this->current_action];

            foreach($dataBase as $category => $entriesPerDate){
                if(empty($category) || in_array($category, $this->skipSeries)) continue;

                // Add results to our data set.
                $dataSet->AddPoint($entriesPerDate, $category);
                $dataSet->SetSerieName($this->getCategoryTranslation($category), $category);
                $dataSet->AddSerie($category);

                // Detect maximum value, to adjust the Y-Axis
                $tmpMax = max($entriesPerDate);
                if($tmpMax > $lineMax) $lineMax = $tmpMax;
                $seriesCnt ++;
            }

            // Keep a list of all selecteable data-series, to allow the user to disable
            //  or enable series on demand.
            $this->seriesList = array();
            foreach($dataBase as $key => $data){
                $this->seriesList[$key] = $this->getCategoryTranslation($key); 
            }
        }

        // Add timeline
        $dataSet->AddPoint($gData['dates'], 'date');
        $dataSet->SetAbsciseLabelSerie('date');  

        // Read graph from cache?
        if($this->enableCaching && $pCache->IsInCache(get_class(),$dataSet->GetData())){
            $filename =  $pCache->GetHash(get_class(),$dataSet->GetData());
            $filename = '/var/spool/gosa/'.$filename;    
            if(file_exists($filename) && is_readable($filename)){
                $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE)).rand(0,99999);
                session::set('statistics::graphFile'.$this->graphID,$filename);
                return;
            }
        }

        $chart = new pChart(900,230);  
        $chart->setFixedScale(0,$lineMax);
        $chart->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
        $chart->setGraphArea(50,28,630,200);  
        $chart->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
        $chart->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
        $chart->drawGraphArea(255,255,255,TRUE);  
        $chart->drawGrid(4,TRUE,200,200,200,50);  
        $chart->drawTreshold(0,143,55,72,TRUE,TRUE);  
        $chart->drawTitle(50,22,_($this->title),50,50,50,585);  
        $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, TRUE);     

        // Only draw this graph if we've at least one series to draw! 
        if($seriesCnt){
            $chart->drawFilledLineGraph($dataSet->GetData(),$dataSet->GetDataDescription(),50,TRUE);
            $chart->drawLegend(720,0,$dataSet->GetDataDescription(),255,255,255);
        }

        // Generate new and unique graph id
        $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphID;
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphID,$file);
        $pCache->WriteToCache(get_class(),$dataSet->GetData(),$chart);   

        return;
    }

    function getGraphOptions()
    {
        $current = $this->current_action; 
        $str = "&nbsp;<b>"._("Action").":</b>&nbsp;".
            "<select name='actionSelectChart_action' size='1' onChange='document.mainform.submit();'>";
        foreach(array_keys($this->graphData['actionTypeGraph']) as $name){
            $checked = ($name==$current)? 'selected' : '';
            $str .= "\n<option value='{$name}' {$checked}>"._($name)."</option>";
        }
        $str .= "</select>";
        return($str);
    }

    function save_object()
    {
        parent::save_object();
        if(isset($_POST['actionSelectChart_action'])){
            $this->current_action = get_post('actionSelectChart_action');
        }
    }
}
?>
