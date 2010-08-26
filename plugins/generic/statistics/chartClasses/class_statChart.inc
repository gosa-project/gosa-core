<?php

class statChart{

    // Graph data 
    protected $statisticData = array();       // Via rpc received stats
    
    // Font used in graphs
    protected $font = "./themes/default/fonts/LiberationSans-Regular.ttf";
    protected $graphData = array();    
    protected $graphID = 0;
    protected $enableCaching = TRUE;
    protected $skipSeries = array();
    protected $graphName = 0;
    protected $seriesList = array();

    function __construct(&$config,$gData = array())
    {
        $this->config = $config;

        // Collect category translations
        $this->catTranslations = array();
        foreach($this->config->configRegistry->getListOfPlugins() as $plugin => $data){
            if(isset($data['plCategory'])){
                foreach($data['plCategory'] as $id => $name){
                    if(!is_numeric($id)){
                        $this->catTranslations[$id] = $name['description'];
                    }
                }
            }
        }
    }


    function getTitle()
    {
        return(_($this->title));
    }    


    function setGraphData($data)
    {
        $this->graphData = $data;
    }


    /*! \brief      This method tries to translate category names.
     *  @param      The category name to translate
     *  @return     String  The translated category names.
     */
    function getCategoryTranslation($name)
    {
        $ret ="";

        // We do not have a category for systems directly, so we've to map all system types to 'System'.
        // If we do not map to _(Systems) the graph legend will be half screen width.
        if($name == "systems"){
            return(_("Systems"));
        }

        // Walk through category names and try to find a translation.
        $cat = trim($name);
        if(isset($this->catTranslations[$cat])){
            $cat = _($this->catTranslations[$cat]);
        }elseif(!empty($cat)){
            $cat = _($cat);
        }
        return($cat); 
    }


    function getGraphID()
    {
        return($this->graphID);
    }

    function getSeriesList()
    {
        return($this->seriesList);
    }

    function getSeriesSelector()
    {
        $str = "<input type='hidden' name='{$this->graphName}_posted' value='1'>";
        $list = $this->getSeriesList();
        foreach($list as $key => $item){
            $checked = (in_array($key, $this->skipSeries))? '': 'checked';
            $str .= "<span style='padding-right:20px'>".
                    "   <input id='addSeries_{$this->graphName}_{$key}' type='checkbox' ".
                    "       name='addSeries_{$this->graphName}_{$key}' value='1'".
                    "       onClick=\"document.mainform.submit();\" ".
                    "       {$checked}><LABEL for='addSeries_{$this->graphName}_{$key}'>{$item}</LABEL>".
                    "   </span>";
        }
        return($str);
    }

    function save_object()
    {
        if(!isset($_POST["{$this->graphName}_posted"])) return;

        // Get series to enable or disable
        $this->skipSeries = array();
        foreach($this->seriesList as $seriesName => $seriesDesc){
            if(!isset($_POST["addSeries_{$this->graphName}_{$seriesName}"])){
                $this->skipSeries[] = $seriesName;
            }
        }
    }
}
?>
