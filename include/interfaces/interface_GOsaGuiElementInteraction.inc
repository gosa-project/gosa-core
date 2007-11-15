<?php

/*! \brief   Interface for GOsaGuiElement objects.
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This interface describes what we need to display an element in the GUI
    for use with list objects.
 */
interface GOsaGuiElementInteraction {

  /**
   * Allows the GOsaGuiElement to process all _POST events that are
   * related to itself.
   *
   * @return TRUE if something has changed
   */
  public function process();


  /**
   * Allows the GOsaGuiElement to render themselves depending on several
   * parameters.
   */
  public function render();

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
