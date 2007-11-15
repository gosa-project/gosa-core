<?php

require_once("accept-to-gettext.inc");

class Utils {

  static public function getBrowserLanguage() {

    #TODO: Support users language

    /* Return gettext based string */
    return (al2gt(Utils::getLanguages(), 'text/html'));  
  }


  static public function getLanguages() {
    #TODO: crawl the locale directory for languages

    return array(
        "German" => "de_DE.UTF-8",
        "French" => "fr_FR.UTF-8",
        );
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
