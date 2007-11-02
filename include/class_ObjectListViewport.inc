<?php

/*! \brief   Exception implementation for ObjectListViewport
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles the exceptions occuring in ObjectListViewport.
 */
class ObjectListViewportException extends Exception {
	public function __construct($message, $code = 0) {
		parent::__construct($message, $code);
	}
}
 

/*! \brief   Implementation for ObjectListViewport
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles painting of ObjectList objects.

    \sa ObjectList
 */
class ObjectListViewport {

  # DUMMY values ---->
  private $headline= "|{16px}|{90%}Name|{64px}Actions|";
  private $footer= "Statistics with no information currently";
  # <--- DUMMY values.

  /*!
    \brief Container for objects

    This variable stores the ObjectList object to be displayed.
   */
	private $objects;


  /*! \brief ObjectListViewport constructor

    The ObjectListViewport class renders/handles the ObjectList defined by $config.

    \param config Config section that is used to configure this ObjectListViewport
   */
	public function __construct($config){

    /* Load and instanciate classes, extract filter, icons, view hooks, etc. */
    $this->objects= new ObjectList($config);
  }

	/*! \brief Handles _POST / _GET events

	    Processes the list of registered plugins to do their eventHandler and adapt
      internal objectlist according to this.
	 */
  public function eventHandler(){
    /* Reloads the list if things have changed interally */
    $this->objects->reload();
  }


	/*! \brief Renders headline into a string

	    Gets the headline description from the ObjectList object and renders it.

      \return HTML rendered headline
	 */
  private function renderHeadline(){
    # Dummy implementation. Use pre-defined headline.
    $buffer= $this->headline."\n";

    return $buffer;
  }


	/*! \brief Renders footer into a string

	    Gets the footer description from the ObjectList object and renders it.

      \return HTML rendered footer
	 */
  private function renderFooter(){
    # Dummy implementation. Use pre-defined footer.
    $buffer= $this->footer;
    return "|".$buffer."|\n";
  }


	/*! \brief Renders entries from the ObjectList iterator into a string

	    Gets the entry descriptions from the ObjectList object and renders them.

      \return HTML rendered list entries
	 */
  private function renderEntry($entry){

    # Dummy implenetation. It is interested in icon, cn and action
    $buffer= "|".$entry['_icon']."|".$entry['cn']."|".$entry['_actions']."|";

    return $buffer."\n";
  }


	/*! \brief Renders complete ObjectList into a string

      \return HTML rendered list
	 */
  public function render() {
    /* Generate fixed headline */
    $buffer= $this->renderHeadline();

    /* Apply current filter */
    $filterIterator = new ObjectListFilterIterator($this->objects->getIterator(), 'dummy');
    foreach ($this->objects as $value){
      $buffer.= $this->renderEntry($value);
    }

    /* Generate footer */
    $buffer.= $this->renderFooter();

    return ($buffer);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
