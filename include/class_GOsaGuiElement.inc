<?php

/**
 */
abstract class GOsaGuiElement implements GOsaGuiElementInteraction {

  protected $errorMessage= "asdf";

  public function render() {
    if ($this->errorMessage) {
      return "<div class='GOsaGuiElementError'>\n".$this->__render()."</div>\n";
    } else {
      return $this->__render();
    }
  }

  public function process() {
    return $this->__process();
  }

  abstract protected function __render();

  abstract protected function __process();
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
