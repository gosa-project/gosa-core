<?php

class dbAdvices extends plugin
{
    function __construct($config)
    {
        parent::__construct($config, NULL);

        // Construct the plugin list.
        $this->adviceList = new sortableListing();
        $this->adviceList->setDeleteable(false);
        $this->adviceList->setEditable(false);
        $this->adviceList->setWidth("100%");
        $this->adviceList->setHeight("200px");
        $this->adviceList->setAcl("rwcdm");
    }

    function execute()
    {
        // Get logs as RSS feed.
        $source = file_get_contents('http://10.3.65.162/gosa/gosa-all/gosa/html/index.html');

        // Read Feeds and sort the results 
        $feeds = rssReader::parseFeedFromSource(array($source));
        $feeds = rssReader::sortFeedResultBy($feeds, 'timestamp');

        // Append the results to the list.
        $data = $lData = array();
        foreach($feeds as $key => $feed){
            $data[$key] = $feed;
            $lData[$key] = array('data'=> array(htmlentities($feed['title'],ENT_QUOTES,'UTF-8')));
        }
        $this->adviceList->setListData($data, $lData);
        $this->adviceList->update();

        // Generate the HTML content
        $smarty = get_smarty();
        $smarty->assign('adviceList', $this->adviceList->render());
        return($smarty->fetch(get_template_path('dbAdvices/contents.tpl', TRUE)));
    }

    function save_object()
    {
        parent::save_object();
        $this->adviceList->save_object();
    }
}

?>
