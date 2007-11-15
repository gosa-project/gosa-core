<?php

/* Provide Smarty capabilities */
require(BASE_DIR."/include/smarty/Smarty.class.php");

/*! \brief   Exception implementation for ViewPortController
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles the exceptions occuring in ObjectList.
 */
class ViewportControllerException extends Exception {
	public function __construct($message, $code = 0) {
		parent::__construct($message, $code);
	}
}
 

/*! \brief   Implementation for rendering the main view
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    The class ViewportController handles the rendering of the main view port,
    manages the location for images and templates based on themes.
 */
class ViewportController {

  private $theme;
  private $smarty;
  private $language= '';
  private $timezone;
  private $mainTemplate= 'main';

  /*! \brief ViewportController constructor

    The ViewportController loads the 'display' config option from the
    ConfigRegistry and acts depending on these settings.
   */
	public function __construct(){

    /* Do language setup during initialization, so we can really be sure
       that most stuff gets translated in the beginning. */
    $this->languageSetup();

    /* Get configuration instance */
    $config= Registry::getInstance("ConfigManager");
    $config->setSection('display');

    /* Set timezone */
    $this->timezone= $config->getValue('timezone', 'GMT');
    if (!date_default_timezone_set($this->timezone)){
      throw new ViewportControllerException(_("Timezone '%s' is not valid!"), $this->timezone);
    }

    /* Do Smarty setup */
    $this->Smarty= new Smarty;
    $this->Smarty->template_dir= BASE_DIR.'/templates/';
    $this->Smarty->caching= $config->getValue('cache-templates', FALSE);
    $this->Smarty->php_handling= SMARTY_PHP_REMOVE;

    /* Set template compile directory */
    $this->Smarty->compile_dir= $config->getValue('compile-directory', '/var/spool/gosa');

    /* Check if our template directory is accessible */
    if (!(is_dir($this->Smarty->compile_dir) && is_writable($this->Smarty->compile_dir))){
      throw new ViewportControllerException(sprintf(_("Directory '%s' specified as compile-directory is not accessible!"), $this->Smarty->compile_dir));
    }

    /* Get more display settings */
    $this->theme= $config->getValue('theme', 'default');
    
    /* Check for old files in compile directory */
    $this->cleanSmartyCompileDir();
  }


  private function cleanSmartyCompileDir() {
    #TODO: Clean compile dir
  }


  private function languageSetup() {

    /* Get the browser language if */
    $lang= Utils::getBrowserLanguage();
    if ($this->language != $lang){
      #TODO: Emit EventObject for changed language
      $this->language= $lang;
    }

    /* Get the browser language */
    putenv("LANGUAGE=");
    putenv("LANG=$this->language");
    setlocale(LC_ALL, $this->language);

    /* Set the text domain as 'messages' */
    $domain= 'messages';
    bindtextdomain($domain, BASE_DIR.'/locale');
    textdomain($domain);

    /* Set global Smarty variable to language and */
    $GLOBALS['t_language']= $this->language;
    $GLOBALS['t_gettext_message_dir']= BASE_DIR.'/locale/';
  }


  public function getSmartyInstance() {
    return $this->Smarty;
  }


  public function getTemplatePath($name) {
  	return "horst";
  }
  
  
  public function render() {
    #Header generator from template
    echo $this->getTemplatePath('main');
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
