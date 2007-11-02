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
  private $headline= "|{16px}|{90%}Name|{64px:R}Actions|";
  private $footer= "Statistics with no information currently";
  private $entryFormat= "|{_icon}|{cn} ({_filter(uppercase,{cn})})|{_actions}|";
  private $attributes= array('cn', '_icon', '_actions', 'dn');
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

    #TODO: Make $buffer a proper HTML table output
    return $buffer;
  }


	/*! \brief Renders footer into a string

	    Gets the footer description from the ObjectList object and renders it.

      \return HTML rendered footer
	 */
  private function renderFooter(){
    # Dummy implementation. Use pre-defined footer.
    $buffer= "|".$this->footer."|";

    #TODO: Make $buffer a proper HTML table output
    return $buffer."|\n";
  }


	/*! \brief Renders entries from the ObjectList iterator into a string

	    Gets the entry descriptions from the ObjectList object and renders them.

      \return HTML rendered list entries
	 */
  private function renderEntry($entry){

    /* Copy template */
    $buffer= $this->entryFormat;

    /* Replace set of attributes */
    foreach ($this->attributes as $attribute){
      if (!isset($entry[$attribute])){
        throw new ObjectListViewportException(sprintf(_("Can't locate attribute '%s' to replace in entry!"), $attribute));
      } else {
        $buffer= preg_replace('/\{'.$attribute.'\}/', $entry[$attribute],$buffer);
      }
    }

    /* Execute optional filters */
    preg_match_all ( '/\{_filter\(([^)]+)\)\}/', $buffer, $matches, PREG_SET_ORDER);
    foreach ($matches as $match){
      $filterName= preg_replace('/,.+$/', '', $match[1]);
      $filterParameter= preg_replace('/^[^,]+,/', '', $match[1]);
      $buffer= preg_replace('/\{_filter\('.$match[1].'\)\}/', $this->applyEntryFilter($filterName, $filterParameter), $buffer);
    }

    #TODO: Make $buffer a proper HTML table output

    return $buffer."\n";
  }


	/*! \brief Applies filter to the given entry format string.

	    Instanciates the given ObjectListEntryFilter and calls the method.

      \return rendered output
      \sa ObjectListEntryFilter
	 */
  private function applyEntryFilter($filterName, $string){
    $className= "ObjectListEntryFilter_".$filterName;
    $cl= new $className;
    return $cl->filter("$string");
  }


	/*! \brief Renders complete ObjectList into a string

      \return HTML rendered list
	 */
  public function render() {
    /* Generate fixed headline */
    $buffer= $this->renderHeadline();

    /* Apply current filter */
    $objects= new ObjectListFilterIterator($this->objects->getIterator());
    foreach ($objects as $value){
      $buffer.= $this->renderEntry($value);
    }

    /* Generate footer */
    $buffer.= $this->renderFooter();

    return ($buffer);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
