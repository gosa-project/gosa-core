<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Blubb</title>
<style type="text/css">@import url('../ObjectListViewport.css');</style>


  <meta name="generator" content="my hands">
  <meta name="description" content="GOsa - Login">
  <meta name="author" lang="de" content="Cajus Pollmeier">

  <meta http-equiv="Expires" content="Mon, 26 Jul 1997 05:00:00 GMT">

  <meta http-equiv="Last-Modified" content="Friday, 09th November 2007 09:14:31 +0000 GMT">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="post-check=0, pre-check=0">
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

  <style type="text/css">@import url('themes/default/style.css');</style>
  <link rel="shortcut icon" href="favicon.ico">

<script language="javascript" src="../ObjectListViewport.js" type="text/javascript"></script>
  <script language="javascript" src="include/png.js" type="text/javascript"></script>
  <script language="javascript" src="include/calendar.js" type="text/javascript"></script>
  <script language="javascript" src="include/focus.js" type="text/javascript"></script>
  <script language="javascript" src="include/warning.js" type="text/javascript"></script>
</head>
<body>
<button onclick='updateObjectListViewportSize();'>Tester</button>
<a href="?d=hf">No Footer &amp; Header</a> - 
<a href="?d=f">No Footer</a> - 
<a href="?d=h">No Header</a> - 
<a href="?d=">Normal</a> 
<?php

/* This is good for testing... */
error_reporting (E_ALL | E_STRICT);


function get_smarty()
{
    echo "Smarty fake in each ./tests/ListTest[1-9]/index.php";
    date_default_timezone_set("europe/berlin");
    $smarty = new smarty;
    /* Set template compile directory */
    $smarty->compile_dir= '/var/spool/gosa/';
    $smarty->template_dir = '../';
    $smarty->caching= false;
    $smarty->php_handling= SMARTY_PHP_REMOVE;
    return($smarty);
}



/* Escape all preg_* relevant characters */
function normalizePreg($input)
{
  return (addcslashes($input, '[]()|/.*+-'));
}


/* Initiate autoloader... */
require_once("../../../include/autoload.inc");
restore_error_handler();
try {

	/* Get new test instance of the Configuration */
	$cr= Registry::getInstance("ConfigManager");
	$cr->load("../../../gosa.conf");

	/* Get a new test instance of ObjectListViewports */
	$vp1= new ObjectListViewport("plugin/sample");
	$vp2= new ObjectListViewport("plugin/sample");
	if(isset($_GET['d']) && preg_match("/f/",$_GET['d'])){
		$vp1->enableFooter(FALSE);
		$vp2->enableFooter(FALSE);
	}
	if(isset($_GET['d']) && preg_match("/h/",$_GET['d'])){
		$vp1->enableHeader(FALSE);
		$vp2->enableHeader(FALSE);
	}
	$content1= $vp1->render();
	$content2= $vp2->render();

	$str = preg_replace("/%LIST%/",$content1,file_get_contents('tpl'));	
	$str = preg_replace("/%LIST2%/",$content2,$str);
	echo $str;	

} catch (Exception $e) {
	echo "\n-GOsa Exception-----------------------------------------------------------\n\n".
		 $e->__toString().
		 "\n\n--------------------------------------------------------------------------\n\n";
}

?>
</body>
</html>
