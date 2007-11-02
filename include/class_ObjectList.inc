<?php

/*! \brief   Exception implementation for ObjectList
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles the exceptions occuring in ObjectList.
 */
class ObjectListException extends Exception {
	public function __construct($message, $code = 0) {
		parent::__construct($message, $code);
	}
}
 

/*! \brief   Implementation for keeping a list of objects
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    The class ObjectList handles a list of objects found in the database
    based on an optional filter modules. This objects can be iterated
    directly.

    \sa ObjectListIterator
 */
class ObjectList implements IteratorAggregate {

  /*!
    \brief Container for objects

    This variable stores the list of objects.
   */
	private $objects;

  /*!
    \brief List headline

    String that keeps the desired headline. Returned by ObjectList::getHeadline.

    \sa ObjectList::getHeadline
   */
  private $headline;

  /*!
    \brief List footer

    String that keeps the desired footer. Returned by ObjectList::getFooter.

    \sa ObjectList::getFooter
   */
  private $footer;

  /*!
    \brief Config

    Config array that keeps the classes we do lists for, filter information, etc.
   */
  private $config;


  /*! \brief ObjectList constructor

    The ObjectList is initialized by a list of classes we're interested
    in. Rest is done by instances of Filter registered in the Registry.

    \param config Config section that is used to configure this ObjectList
    \sa Registry
    \sa Filter
   */
	public function __construct(&$config){

    /* Save current config */
    $this->config= &$config;

    /* Load and instanciate classes, extract filter, icons, view hooks, etc. */
    $this->load();

  }


	/*! \brief Function to initialy load object list

	  Internally loads the relevant list of objects depending on eventually
	  defined filter modules. Handles sorting, too.
	 */
	private function load(){
    # Crap filling
    $this->objects= array( array("i", "Testobjekt mit was auch immer", "UPS"),
                           array("i", "Noch ein Testobjekt", "UPS"),
                           array("d", "Ein drittes Testobjekt", "UL"));
    $this->headline= "|{16px}|{90%}Name|{64px}Actions|";

    $this->footer= "Statistical footer";
	}


	/*! \brief Function to reload object list in case of external changes

	  Triggers a reload the relevant list of objects depending on eventually
	  defined filter modules. Handles sorting, too.
	 */
  public function reload(){
  }


	/*! \brief Function to return the iterator object for this class.

      This function is used internally by PHP to produce an iterator.

      \return Iterator object

      \sa ObjectListIterator
	 */
  public function getIterator() {
    return new ObjectListIterator($this->objects);
  }


	/*! \brief Function to get the desired headline

      Return the property of headline to the public world.

      \return headline for current list
	 */
  public function getHeadline() {
    return $this->headline;
  }


	/*! \brief Function to get the desired footer

      Return the property of footer to the public world.

      \return headline for current list
	 */
  public function getFooter() {
    return $this->footer;
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
