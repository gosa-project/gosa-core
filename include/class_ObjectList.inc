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
    $this->objects= array(
                            array("dn" => "cn=Demo client,ou=systems,dc=gonicus,dc=de", "cn" => "Demo client", "_icon" => "s_terminal.png", "_actions" => "ED", "objectClass" => array('gotoWorkstation')),
                            array("dn" => "cn=Demo client2,ou=systems,dc=gonicus,dc=de", "cn" => "Demo client2", "_icon" => "s_terminal.png", "_actions" => "ED", "objectClass" => array('gotoWorkstation')),
                            array("dn" => "cn=Printer,ou=systems,dc=gonicus,dc=de", "cn" => "Printer", "_icon" => "s_printer.png", "_actions" => "ED", "objectClass" => array('gotoPrinter')),
                            array("dn" => "cn=Server,ou=systems,dc=gonicus,dc=de", "cn" => "Server", "_icon" => "s_server.png", "_actions" => "ED", "objectClass" => array("goServer")),
                           );
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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
