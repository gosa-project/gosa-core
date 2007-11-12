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
class ObjectListViewport implements GOsaGuiElement {

  /* Internal variable for color alternation */
  protected $colorAlternator= 0;

  private $headline;
  private $footer;
  private $entryFormat;

  /* Dummy here ----> */
  private $attributes= array('cn', '_icon', '_actions', 'dn');
  /* <---- Dummy here */

  private $displayHeaderFlag= TRUE;
  private $displayFooterFlag= TRUE;
  private $numberOfCols= 0;

  /*!
    \brief Container for objects

    This variable stores the ObjectList object to be displayed.
   */
	private $objects;

  /*!
    \brief Switch to handle multiselect or not
   */
	private $multiselect;

  /*! \brief ID used to identify objects of same list */
  private $id = "";

  
  /*! \brief ObjectListViewport constructor

    The ObjectListViewport class renders/handles the ObjectList defined by $config.

    \param config Config section that is used to configure this ObjectListViewport
   */
	public function __construct($config, $multiselect= TRUE, $displayHeader= TRUE, $displayFooter= FALSE){

    /* Transfer initialization values */
    $this->displayFooterFlag= $displayFooter;
    $this->displayHeaderFlag= $displayHeader;
    $this->multiselect= $multiselect;

    /* Load list configuration from ConfigManager */
    $cr= Registry::getInstance("ConfigManager");
    $cr->setSection($config);
    $this->parseHeadline($cr->getValue("headline"));
    $this->footer= $cr->getValue("footer");
    $this->entryFormat= $cr->getValue("entryFormat");

    /* Load and instanciate classes, extract filter, icons, view hooks, etc. */
    $this->objects= new ObjectList($config);

    /* generate an unique id */
    $this->id= uniqid();
  }


	/*! \brief Handles _POST / _GET events

	    Processes the list of registered plugins to do their eventHandler and adapt
      internal objectlist according to this.
	 */
  public function eventHandler(){
    /* Reloads the list if things have changed interally */
    $this->objects->reload();
  }


	/*! \brief Processes post events

	    Processes all post events and acts as needed.

      \return bool for if changes are present or not
	 */
  public function process(){

    /* TODO: process input */
    return FALSE;
  }


	/*! \brief Renders headline into a string

	    Gets the headline description from the ObjectList object and renders it.

      \return HTML rendered headline
	 */
  private function renderHeadline(){
    $tpl =" <tr>
              <td class='ObjectListViewport_TD_Header' id='ObjectListViewport_TD_Header_".$this->id."'>
                <table class='ObjectListViewport_Header_Table' id='ObjectListViewport_Header_Table_".$this->id."'>
                  <tr>
                    {content}
                  </tr>
                </table>
             </td>
           </tr>";
    $buffer ="";
    foreach($this->headline as $key => $value){
      $buffer .= "<td class='ObjectListViewport_Header_Cell' style='".$value['style']."'>".$value['name']."</td>\n";
    }
    return(preg_replace("/\{content\}/", $buffer,$tpl));
  }


	/*! \brief Renders footer into a string

	    Gets the footer description from the ObjectList object and renders it.

      \return HTML rendered footer
	 */
  private function renderFooter(){
    $buffer ="<tr>
              <td class='ObjectListViewport_TD_Footer' id='ObjectListViewport_TD_Footer_".$this->id."'>
                <table class='ObjectListViewport_Footer_Table'>
                  <tr>
                    <td class='ObjectListViewport_Footer_Cell' colspan='".count($this->headline)."'>".$this->footer."</td>
                  </tr>
                </table>     
              </td>
           </tr>";
    return $buffer;
  }


  /* TODO: replace this -> not our task */
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

    /* define entry template */
    $tpl = "<td class='ObjectListViewport_Entry_Cell' style='{style_1}'>
              <div style='{style_2}'>
                {content}
              </div>
            </td>
            ";
    /* Template vriables to replace */
    $attrs = array("/\{style_1\}/","/\{style_2\}/","/\{content\}/");

    /* Append template for each given col */
    $buffer ="";
    for($i= 0; $i < $this->numberOfCols; $i++){

      /* If current entry is the last to appen, then skip adding styles */
      if($i == ($this->numberOfCols-1)){
        $buffer.= preg_replace( $attrs,
            array($this->headline[$i]['style'],"width:100%;overflow:hidden;",$tmp[$i]),$tpl);
      }else{
        $buffer.= preg_replace( $attrs,
            array($this->headline[$i]['style'],"width:100%;overflow:hidden;".$this->headline[$i]['style'],$tmp[$i]),$tpl);
      }
    }

    /* Add class depending on given id, to alternate background colors */
    if($this->colorAlternator++ & 1){
      $a = "class='ObjectListViewport_Entry_Row1'";
    }else{
      $a = "class='ObjectListViewport_Entry_Row2'";
    }

    return "<tr ".$a.">\n".$buffer."</tr>\n";
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

    $header = $footer = "";
    if($this->displayHeaderFlag){
      $header = $this->renderHeadline();
    }
    if($this->displayFooterFlag){
      $footer = $this->renderFooter();
    }

    /* Apply current filter */
    $entries = "";
    $objects= new ObjectListFilterIterator($this->objects->getIterator());
    foreach ($objects as $value){
      $entries .= $this->renderEntry($value);
    }

    /* Generate fixed headline */
    $buffer = "
    <table class='ObjectListViewport' id='ObjectListViewport_".$this->id."' cellspacing=0 cellpadding=0>
      <tr>
        <td>
              <table class='ObjectListViewport_Table' id='ObjectListViewport_Table_".$this->id."' cellpadding=0 cellspacing=0 >
                ".$header."
                <tr>
                  <td class='ObjectListViewport_TD_Entries' id='ObjectListViewport_TD_Entries_".$this->id."'>
                    <div class='ObjectListViewport_Entry_Cover' id='ObjectListViewport_Entry_Cover_".$this->id."'> 
                      <table class='ObjectListViewport_Entry_Table' id='ObjectListViewport_Entry_Table_".$this->id."'>
                        ".$entries."
                      </table> 
                    </div>
                  </td>
                </tr>
                ".$footer."
              </table>

        </td>
      </tr>
    </table>
";

    return ($buffer);
  }


	/*! \brief Parses the given headline format string 

      \return Array with cell properties (width, alignment,name)
	 */
  private function parseHeadline($data)
  {
    /* Each cell definition is seperated by | 
     *  split by and go through each definition
     */
    $this->headline= array();
    $this->numberOfCols= 0;
    $tmp= split("\|", trim($data, "|")); 
    $cell_formats= array();

    foreach($tmp as $key => $dta){

      $width= "";
      $alignment= "";
      $name= preg_replace("/\{[^\}]*+\}/", "", $dta);
      $style= "";
    
      /* Parse format string and detect width & alignment */
      if(preg_match("/\{.*\}/", $dta)){
        $format= preg_replace("/^[^\{]*+\{([^\}]*).*$/", "\\1", $dta);
    
        /* Get aligment */
        if(preg_match("/:/",$format)){
          $al= preg_replace("/^[^:]*+:([a-z]*).*$/i", "\\1", $format);

          if(preg_match("/T/i", $al)){
            $alignment.= "top-";
            $style.= "vertical-align: top;";
          }
          if(preg_match("/B/i", $al)){
            $alignment.= "bottom-";
            $style.= "vertical-align: bottom;";
          }
          if(preg_match("/R/i", $al)){
            $alignment.= "right";
            $style.= "text-align: right;";
          }elseif(preg_match("/L/i", $al)){
            $alignment.= "left";
            $style.= "text-align: left;";
          }elseif(preg_match("/C/i", $al) || preg_match("/M/i", $al) ){
            $alignment.= "center";
            $style.= "text-align: center;";
          }
        }

        /* Get width */
        $width = preg_replace("/^([^:]*).*$/","\\1", $format);
        if(!empty($width)){
          $style.= "width: ".$width.";";
        }
      }

      $cell_formats[$key]= array("name" => $name, "width" => $width, "alignment" => $alignment, "style" => $style);
      $this->numberOfCols++;
    }
    $this->headline= $cell_formats;
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
