<?php

restore_error_handler();

/* Basic setup, remove eventually registered sessions */
require_once ("../../../include/php_setup.inc");
require_once ("functions.inc");
session::start();
session::global_set('errorsAlreadyPosted',array());


/* Logged in? Simple security check */
if (!session::global_is_set('ui')){
  new log("security","unknown","",array(),"Error: getbin.php called without session") ;
  header ("Location: index.php");
  exit;
}

if(!isset($_GET['id'])) return;

if(session::is_set('statistics::graphFile'.preg_replace("/[^0-9]/","",$_GET['id']))){
    header('Content-Type: image/png');
    $graphFile = session::get('statistics::graphFile'.preg_replace("/[^0-9]/","",$_GET['id']));
    echo file_get_contents($graphFile);
}

?>
