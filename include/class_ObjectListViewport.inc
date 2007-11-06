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

  /* Dummy here ----> */
  private $headline;
  private $footer;
  private $entryFormat;
  private $attributes= array('cn', '_icon', '_actions', 'dn');

  /* <---- Dummy here */

  /*!
    \brief Container for objects

    This variable stores the ObjectList object to be displayed.
   */
	private $objects;

  /*!
    \brief Switch to handle multiselect or not
   */
	private $multiselect;


  /*! \brief ObjectListViewport constructor

    The ObjectListViewport class renders/handles the ObjectList defined by $config.

    \param config Config section that is used to configure this ObjectListViewport
   */
	public function __construct($config, $multiselect= TRUE){

	$this->multiselect= $multiselect;
    /* Dummy here */
    $cr= Registry::getInstance("ConfigManager");
    $cr->setSection($config);
    $this->headline= $this->parseHeadline($cr->getValue("headline"));
    $this->footer= $cr->getValue("footer");
    $this->entryFormat= $cr->getValue("entryFormat");

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
    $buffer ="<tr>\n";
    foreach($this->headline as $key => $value){
      $buffer .= "<td style='".$value['style']."'>".$value['name']."</td>\n";
    }
    $buffer.="</tr>\n";
    return $buffer;
  }


	/*! \brief Renders footer into a string

	    Gets the footer description from the ObjectList object and renders it.

      \return HTML rendered footer
	 */
  private function renderFooter(){

    $buffer ="<tr>\n";
    $buffer .= "<td colspan='".count($this->headline)."'>".$this->footer."</td>\n";
    $buffer.="</tr>\n";
    return $buffer;
  }


  private function getEntryIcon($entry,$alt = ""){
    return("<img src='images/".$entry['_icon']."' alt='".$alt."' class='center'>");
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

        if(preg_match("/_icon/i",$attribute)){
          $buffer= preg_replace('/\{'.$attribute.'\}/', $this->getEntryIcon($entry),$buffer); 
        }else{
          $buffer= preg_replace('/\{'.$attribute.'\}/', $entry[$attribute],$buffer);
        }
      }
    }

    /* Execute optional filters */
    preg_match_all ( '/\{_filter\(([^)]+)\)\}/', $buffer, $matches, PREG_SET_ORDER);
    foreach ($matches as $match){
      $filterName= preg_replace('/,.+$/', '', $match[1]);
      $filterParameter= preg_replace('/^[^,]+,/', '', $match[1]);
      $buffer= preg_replace('/\{_filter\('.normalizePreg($match[1]).'\)\}/', $this->applyEntryFilter($filterName, $filterParameter), $buffer);
    }

    #TODO: Make $buffer a proper HTML table output

    $tmp = split("\|",trim($buffer,"|"));  

    $buffer ="<tr>\n";
    foreach($tmp as $key => $value){
      $buffer .= "<td class='ObjectListViewportEntry' style='".$this->headline[$key]['style']."'>".$value."</td>\n";
    }
    $buffer.="</tr>\n";

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


    return ("<table class='ObjectListViewportTable'>".$buffer."</table>");
  }


	/*! \brief Parses the given headline format string 

      \return Array with cell properties (width, alignment,name)
	 */
  private function parseHeadline($data)
  {
    /* Each cell definition is seperated by | 
     *  split by and go through each definition
     */
    $tmp = split("\|",trim($data,"|"));  
    $cell_formats = array();
    foreach($tmp as $key => $data){

      $s_width    = "";
      $s_alignment= "";
      $s_name     = preg_replace("/\{[^\}]*+\}/","",$data);
      $s_style    = "";
    
      /* Parse format string and detect width & alignment */
      if(preg_match("/\{.*\}/",$data)){
        $s_format=  preg_replace("/^[^\{]*+\{([^\}]*).*$/","\\1",$data);
    
        /* Get aligment */
        if(preg_match("/:/",$s_format)){
          $s_al = preg_replace("/^[^:]*+:([a-z]*).*$/i","\\1",$s_format);

          if(preg_match("/T/i",$s_al)){
            $s_alignment.= "top-"  ;
            $s_style.= "vertical-align: top;";
          }
          if(preg_match("/B/i",$s_al)){
            $s_alignment.= "bottom-"  ;
            $s_style.= "vertical-align: bottom;";
          }
          if(preg_match("/R/i",$s_al)){
            $s_alignment.= "right"  ;
            $s_style.= "text-align: right;";
          }elseif(preg_match("/L/i",$s_al)){
            $s_alignment.= "left"  ;
            $s_style.= "text-align: left;";
          }elseif(preg_match("/C/i",$s_al) || preg_match("/M/i",$s_al) ){
            $s_alignment.= "center"  ;
            $s_style.= "text-align: center;";
          }
        }

        /* Get width */
        $s_width = preg_replace("/^([^:]*).*$/","\\1",$s_format);
        if(!empty($s_width)){
          $s_style = "width: ".$s_width.";";
        }
        
        $cell_formats[$key] = array("name" => $s_name, "width" => $s_width, "alignment" => $s_alignment,"style" => $s_style);
      }
    }
    return($cell_formats);
  }


  
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
