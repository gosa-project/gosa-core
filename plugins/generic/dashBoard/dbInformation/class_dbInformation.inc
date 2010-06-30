<?php

class dbInformation extends plugin
{
    function __construct($config)
    {
        parent::__construct($config, NULL);
    }

    function execute()
    {
        $smarty = get_smarty();
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
