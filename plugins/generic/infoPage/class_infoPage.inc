<?php

class infoPage extends plugin
{

    function execute()
    {
        $smarty = get_smarty();
        return($smarty->fetch(get_template_path("infoPage.tpl")));
    }
}

?>
