<?php

/*! \brief   Implements a list filter for ObjectList objects
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles filtering of ObjectList iterators.
 */
class ObjectListFilterIterator extends FilterIterator {
 
  public function __construct(ObjectListIterator $objectListIterator) {
    parent::__construct($objectListIterator);
  }
 
  public function accept() {
    $current = $this->getInnerIterator()->current();

    /* This is a dummy, currently. Accept everything. */
    return TRUE;
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
