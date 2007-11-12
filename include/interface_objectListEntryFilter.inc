<?php

/*! \brief   Interface for ObjectListEntryFilter objects.
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This interface describes what we need to filter a string
    for use with list objects.
 */
interface ObjectListEntryFilter {
  static public function filter($string);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
