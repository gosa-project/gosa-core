<?php

/**
 * GOsa Registry of static class instances that are accessible
 * from all locations.
 *
 * Creates a new instance of the requested class if required.
 */
class Registry {

	static private $registry = array();

	private function __construct(){}

	/**
	 * Returns a static instance of $class
	 * Creates a new instance of $class if required.
	 * @param string $class
	 * @return obj $$class
	 */
	static function getInstance($class){
		if (!isset(Registry::$registry[$class])){
			Registry::$registry[$class]= new $class;
		}
		return Registry::$registry[$class];
	}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
