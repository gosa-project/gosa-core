<?php

/*! \brief   GOsaGuiElement base class
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/12

    This abstract base class defines a GOsaGuiElement.
 */
abstract class GOsaGuiElement implements GOsaGuiElementInteraction {

  protected $errorMessage= "";
  protected $id;


  public function __construct() {
    /* Generate an unique id */
    $this->id= "G".uniqid()."_";
  }


  public function render() {
    #TODO: Add "tooltip" for errorMessage
    if ($this->errorMessage) {
      return "<div class='GOsaGuiElementError'>\n".$this->__render()."</div>\n";
    } else {
      return $this->__render();
    }
  }


  public function process() {
    /* Return original processing results */
    return $this->__process();
  }


  protected function createVariable($name) {
    return ($this->id."_$name");
  }


  protected function __call($method, $parameters) {

    #FIXME: hack should be done by RequestHandler, but it is not ready yet
    if (preg_match('/^getRequestVariable_/', $method)){
      $name= preg_replace('/^[^_]+_/', '', $method); 

      if (isset($_REQUEST[$this->id.$name])){
        return strip_tags(rtrim($_REQUEST[$this->id.$name], "\0"));
      } else {
        return "";
      }
    }

    throw new Exception(sprintf(_("Unknown method '%s' called for '%s'."), $method, get_class()));
  }


  abstract protected function __render();

  abstract protected function __process();
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
