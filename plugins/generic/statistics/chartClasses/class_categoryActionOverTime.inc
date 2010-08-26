<?php

class categoryActionsOverTime extends statChart
{
    protected $title = 'Plugin usage over time';

    function __construct(&$config)
    {
        parent::__construct($config);
        $this->graphName = get_class();
    }


    /*! \brief  Generates the line-graph which displays the plugin usage over time.
     */ 
    function render()
    {
        $lineMax = 100;
        $gData = $this->graphData;
        $errorMax = (max($gData['errorsPerInterval']) < 100)? 100:max($gData['errorsPerInterval']);
        $dataSet = new pData;  
        $seriesCnt = 0;
        foreach($gData['actionsGraph'] as $category => $entriesPerDate){
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
        foreach($gData['actionsGraph'] as $key => $data){
            $this->seriesList[$key] = $this->getCategoryTranslation($key); 
        }
        $this->seriesList['errorsPerInterval'] = _("Error");

        // Create a dataSet containing all series 
        $allSeriesDataSet = clone $dataSet;
        if(!in_array('errorsPerInterval', $this->skipSeries)){
            $allSeriesDataSet->AddPoint($gData['errorsPerInterval'], 'Errors');
            $allSeriesDataSet->SetSerieName(_('Error'), 'Errors');
            $allSeriesDataSet->AddSerie('Errors');
        }

        // Add timeline
        $dataSet->AddPoint($gData['dates'], 'date');
        $dataSet->SetAbsciseLabelSerie('date');  

        // Read graph from cache?
        $pCache = new pCache('/var/spool/gosa/');
        if($this->enableCaching && $pCache->IsInCache(get_class(),$allSeriesDataSet->GetData())){
            $filename =  $pCache->GetHash(get_class(),$allSeriesDataSet->GetData());
            $filename = '/var/spool/gosa/'.$filename;    
            if(file_exists($filename) && is_readable($filename)){
                $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE)).rand(0,99999);
                session::set('statistics::graphFile'.$this->graphID,$filename);
                return;
            }
        }

        $chart = new pChart(900,230);  
        $chart->setFixedScale(0.000,$lineMax);
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
        }

        // Do we've to add the errors series?
        // If we have to, then add the error-data-series.
        //  and set the color for the new error-series to red.
        if(!in_array('errorsPerInterval', $this->skipSeries)){

            // Set the color for the error Series to 'red'. 
            // This has to be done before drawing the legend.
            $chart->setColorPalette($seriesCnt,255,0,0);   

            $dataSet->AddPoint($gData['errorsPerInterval'], 'Errors');
            $dataSet->SetSerieName(_('Error'), 'Errors');
            $dataSet->AddSerie('Errors');
        }

        $chart->drawLegend(720,0,$dataSet->GetDataDescription(),255,255,255);

        // Draw the error graph on top of the other graphs now.
        // But remove the category-graph before. 
        if(!in_array('errorsPerInterval', $this->skipSeries)){

            // Remove all graph series and add the error-series, then draw the new graph.
            // (It is not relevant if it was really added before, so we simply remove all series!)
            foreach($gData['actionsGraph'] as $category => $data){
                $dataSet->RemoveSerie($category);
            }
            $chart->setFixedScale(0,$errorMax);
            $chart->drawRightScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,120,150,150,TRUE,0,2, TRUE);
            $chart->drawBarGraph($dataSet->GetData(),$dataSet->GetDataDescription());
        }

        // Generate new and unique graph id
        $this->graphID = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphID;
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphID,$file);
        $pCache->WriteToCache(get_class(),$allSeriesDataSet->GetData(),$chart);   

        return;
    }

}
?>
