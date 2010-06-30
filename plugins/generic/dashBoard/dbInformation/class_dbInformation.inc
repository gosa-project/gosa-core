<?php

class dbInformation extends plugin
{
    function __construct($config)
    {
        parent::__construct($config, NULL);

        // Construct the plugin list.
        $this->feedList = new sortableListing();
        $this->feedList->setDeleteable(false);
        $this->feedList->setEditable(true);
        $this->feedList->setWidth("100%");
        $this->feedList->setHeight("200px");
        $this->feedList->setAcl("rwcdm");
    }

    function execute()
    {
        // Act on clicks on the feed list.
        $linkOpener = "";
        $action = $this->feedList->getAction();
        if(isset($action['action']) && $action['action'] == 'edit'){
            $data = $this->feedList->getData($action['targets'][0]);
            if(isset($data['link']) && !empty($data['link'])){
                $linkOpener = '
                    <script language="JavaScript" type="text/javascript">
                        window.open("'.$data['link'].'");
                    </script>';
            }
        }
    
        // Read Feeds and sort the results 
        $feeds = rssReader::feadToArray(array(
                    'http://www.uweschwanz.com/de/rss/',
                    'http://www.aviationpower.de/rss_de.php',
                    'http://www.computerbild.de/rssfeed_2261.html?node=10',
                    'http://www.charts-portal.de/games/Computerspiele.xml'));
        $feeds = rssReader::sortFeedResultBy($feeds, 'timestamp');

        // Append the results to the list.
        $data = $lData = array();
        foreach($feeds as $key => $feed){
            $data[$key] = $feed;
            $lData[$key] = array('data'=> array(htmlentities($feed['title'],ENT_QUOTES,'UTF-8')));
        }
        $this->feedList->setListData($data, $lData);
        $this->feedList->update();

        // Generate the HTML content
        $smarty = get_smarty();
        $smarty->assign('feedList', $this->feedList->render());
        return($linkOpener.$smarty->fetch(get_template_path('dbInformation/contents.tpl', TRUE)));
    }

    function save_object()
    {
        parent::save_object();
        $this->feedList->save_object();
    }

    function save()
    {
        parent::save();
    }

    function check()
    {
        return(parent::check());
    }

    function remove_from_parent()
    {
        parent::remove_from_parent();
    }
}

?>
