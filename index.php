#!/usr/bin/php
<?php


/* This is good for testing... */
error_reporting (E_ALL | E_STRICT);

/* Escape all preg_* relevant characters */
function normalizePreg($input)
{
  return (addcslashes($input, '[]()|/.*+-'));
}



/* Initiate autoloader... */
require_once("include/autoload.inc");
restore_error_handler();
try {

	/* Get new test instance of the Configuration */
	$cr= Registry::getInstance("ConfigManager");
	$cr->load("gosa.conf");

	/* Get a new instance of the main Viewport */
	$vc= new ViewportController();
	echo $vc->render();

} catch (Exception $e) {
	echo "\n-GOsa Exception-----------------------------------------------------------\n\n".
	     $e->__toString().
	     "\n\n--------------------------------------------------------------------------\n\n";
}

exit (0);

?>
