<?php

class dbInformation extends plugin
{
    function __construct($config)
    {
        parent::__construct($config, NULL);

        // Construct the plugin list.
        $this->feedList = new sortableListing();
        $this->feedList->setDeleteable(false);
        $this->feedList->setEditable(false);
        #$this->feedList->setColspecs(array('130px','*'));
        #$this->feedList->setHeader(array(_("Date"),_("Description")));
        $this->feedList->setWidth("100%");
        #$this->feedList->setDefaultSortColumn(0);
        $this->feedList->setHeight("200px");
        $this->feedList->setAcl("rwcdm");
    }

    function execute()
    {
        $smarty = get_smarty();
        $feeds = rssReader::feadToArray(array(
                    'http://www.computerbild.de/rssfeed_2261.html?node=10',
                    'http://www.charts-portal.de/games/Computerspiele.xml'));
        $feeds = rssReader::sortFeedResultBy($feeds, 'timestamp');
        $data = $lData = array();
        foreach($feeds as $key => $feed){
            $data[$key] = $feed;
            $lData[$key] = array('data'=> array($feed['title']));
        }

        $this->feedList->setListData($data, $lData);
        $this->feedList->update();
        $smarty->assign('feedList', $this->feedList->render());
        return($smarty->fetch(get_template_path('dbInformation/contents.tpl', TRUE)));
    }

    function save_object()
    {
        parent::save_object();
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
