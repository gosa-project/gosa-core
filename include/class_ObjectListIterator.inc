<?php

/*! \brief   Implementation for iterating through ObjectList objects
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    The class ObjectList handles a list of objects found in the database
    based on an optional filter modules. This objects can be iterated
    directly by using this iterator class.

    \sa ObjectList
 */
class ObjectListIterator implements Iterator {

  /*!
    \brief Reference container for objects

    This variable stores the list of objects.
   */
	private $objects;

  /*!
    \brief Iterator position

    Keeps the current position inside our ObjectList
   */
  private $position;

  /*! \brief ObjectListIterator constructor

    The ObjectListIterator is initialized by a list of objects from the
    ObjectList object.

    \param objects List of objects to be iterated
   */
	public function __construct(&$objects){
    $this->objects= &$objects;
  }


  /*! \brief Rewind to the begining of the ObjectList */
  public function rewind(){
    $this->position= 0;
  }


  /*! \brief Check if the next object is valid */
  public function valid() {
    return isset($this->objects[$this->position]);
  }


  /*! \brief Return the current key
      \return integer Current position
   */
  public function key() {
    return $this->position;
  }
  

  /*! \brief Return the current value
      \return object Current value
   */
  public function current() {
    return $this->objects[$this->position];
  }


  /*! \brief Go to the next index */
  public function next() {
    $this->position++;
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
