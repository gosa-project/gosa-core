<?php

/*! \brief   Implements a sample ObjectListEntryFilter interface.
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class converts entries in lists to upper case.
 */
class ObjectListEntryFilter_uppercase implements ObjectListEntryFilter {
  static public function filter($string) {
    return strtoupper($string);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
