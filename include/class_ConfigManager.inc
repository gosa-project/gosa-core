<?php

/**
 * ConfigManagerException to handle exceptions that happen inside
 * the config manager module.
 */
class ConfigManagerException extends Exception {
	public function __construct($message, $code = 0) {
		parent::__construct($message, $code);
	}
}
 


/**
 * ConfigManager loads and manages GOsa session configurations.
 *
 * This class should be integrated via Registry.
 */
class ConfigManager {

	/* Config container */
	protected $config= NULL;

	public function __construct(){}

	/**
	 * Returns a static instance of $class
	 * Creates a new instance of $class if required.
	 * @param string $class
	 * @return obj $$class
	 */
	public function load($file){
		if (!is_readable($file)){
			throw new ConfigManagerException(sprintf(_("Can't read configuration file '%s'!"), $file));
		}

		/* Capture errors - just the first line is interesting */
		ob_start(); 
		$this->config= parse_ini_file($file, TRUE);
		$ret= preg_replace( array('/^.*\nWarning: /', '/\n.*$/'),
		                    array('', ''),
				    ob_get_contents());
		ob_end_clean(); 

		/* Bail out in case of errors */
		if ($ret != ""){
			throw new ConfigManagerException($ret);
		}

		echo "Test config and load additional configuration data from LDAP\n";
		#$this->test_config();
		#$this->load_servers();        -> Section servers/...;
		#$this->load_plugin_configs(); -> Section pugins/name/value;

		print_r($this->config);
	}


	public function save($file){
	}


	public function set_section($section_name){
	}


	public function &get_section(){
	}


	public function &get_sections(){
	}


	public function &get_value($section){
	}


	public function set_value($section, $value){
	}

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
