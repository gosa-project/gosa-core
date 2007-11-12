<?php

/* This function needs the pre-defined class mapping from autoload-data.inc */
require_once("autoload-data.inc");
require_once("smarty/Smarty.class.php");

/* Set BASE_DIR for the complete code as constant */
define('BASE_DIR', dirname(dirname(__FILE__)));


/**
 * AutoloadException to handle exceptions that happen inside
 * the autoloader module.
 */
class AutoloadException extends Exception {
        public function __construct($message, $code = 0) {
                parent::__construct($message, $code);
        }
}


/**
 * Loads the requested class from the filesystem.
 * @param string $class_name
 * @return -
 */
function __autoload($class_name) {
	global $class_mapping;

	/* Load data for class locations */

	if (isset($class_mapping[$class_name])){
		require_once(BASE_DIR."/".$class_mapping[$class_name]);
	} else {
		eval("class $class_name {}");
		throw new AutoloadException(sprintf(_("Cannot load class '%s'!"), $class_name));
	}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
