<?php

/*! \brief   Implements a list filter for ObjectList objects
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles filtering of ObjectList iterators.
 */
class ObjectListFilterIterator extends FilterIterator {
  private $filter;
 
  public function __construct(ObjectListIterator $objectListIterator, $filter) {
    parent::__construct($objectListIterator);
    $this->filter = $filter;
  }
 
  public function accept() {
    $current = $this->getInnerIterator()->current();
 
    return TRUE;
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
