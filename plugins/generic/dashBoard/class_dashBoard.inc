<?php

class dashBoard extends plugin
{
   var $plHeadline = 'dashBoard.png';
   var $plDescription = 'dashBoard.png';
   var $plShortIcon = 'dashBoard.png';
   var $plIcon = 'dashBoard.png';


    function execute()
    {
        $smarty = get_smarty();

        return($smarty->fetch(get_template_path('dashBoard.tpl', TRUE)));
        
    }
}

?>
