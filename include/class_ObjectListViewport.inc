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
class ObjectListViewport extends GOsaGuiElement {

  /* Internal variable for color alternation */
  private $colorAlternator= 0;

  protected $headline;
  protected $footer;
  protected $entryFormat;

  /* Dummy here ----> */
  private $attributes= array('cn', '_icon', '_actions', 'dn');
  /* <---- Dummy here */

  protected $displayHeaderFlag= TRUE;
  protected $displayFooterFlag= TRUE;
  private $numberOfCols= 0;

  /*!
    \brief Container for objects

    This variable stores the ObjectList object to be displayed.
   */
	protected $objects;

  /*!
    \brief Switch to handle multiselect or not
   */
	protected $multiselect;

  /*! \brief ObjectListViewport constructor

    The ObjectListViewport class renders/handles the ObjectList defined by $config.

    \param config Config section that is used to configure this ObjectListViewport
   */
	public function __construct($config, $multiselect= TRUE, $displayHeader= TRUE, $displayFooter= FALSE){

    /* Initialize from parents method */
    parent::__construct();

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
  }


	/*! \brief Processes post events

	    Processes all post events and acts as needed.

      \return bool for if changes are present or not
	 */
  protected function __process(){

    /* FIXME: Dummy Action*/
    $this->objects->reload();

    /* TODO: process input */
    return FALSE;
  }


#/*! \brief Renders headline into a string
#
#    Gets the headline description from the ObjectList object and renders it.
#
#    \return HTML rendered headline
# */
#private function renderHeadline(){
#  $tpl =" <tr>
#            <td class='ObjectListViewport_TD_Header' id='ObjectListViewport_TD_Header_".$this->id."'>
#              <table class='ObjectListViewport_Header_Table' id='ObjectListViewport_Header_Table_".$this->id."'>
#                <tr>
#                  {content}
#                </tr>
#              </table>
#           </td>
#         </tr>";
#  $buffer ="";
#  foreach($this->headline as $key => $value){
#    $buffer .= "<td class='ObjectListViewport_Header_Cell' style='".$value['style']."'>".$value['name']."</td>\n";
#  }
#  return(preg_replace("/\{content\}/", $buffer,$tpl));
#}


#/*! \brief Renders footer into a string
#
#    Gets the footer description from the ObjectList object and renders it.
#
#    \return HTML rendered footer
# */
#private function renderFooter(){
#  $buffer ="<tr>
#            <td class='ObjectListViewport_TD_Footer' id='ObjectListViewport_TD_Footer_".$this->id."'>
#              <table class='ObjectListViewport_Footer_Table'>
#                <tr>
#                  <td class='ObjectListViewport_Footer_Cell' colspan='".count($this->headline)."'>".$this->footer."</td>
#                </tr>
#              </table>     
#            </td>
#         </tr>";
#  return $buffer;
#}


  /* FIXME: replace this -> not our task */
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
      $buffer= preg_replace('/\{_filter\('.normalizePreg($match[1]).'\)\}/',
                            $this->applyEntryFilter($filterName, $filterParameter),
                            $buffer);
    }

    #TODO: Make $buffer a proper HTML table output
    $tmp = split("\|",trim($buffer,"|"));  
    $cols = array();

    for($i= 0; $i < $this->numberOfCols; $i++){

      /* If current entry is the last to appen, then skip adding styles */
      if($i == ($this->numberOfCols-1)){
        $cols[$i]['style1'] = $this->headline[$i]['style'];
        $cols[$i]['style2'] = "width:100%;overflow:hidden;";
        $cols[$i]['value']   = $tmp[$i];
      }else{
        $cols[$i]['style1'] = $this->headline[$i]['style'];
        $cols[$i]['style2'] = "width:100%;overflow:hidden;".$this->headline[$i]['style'];
        $cols[$i]['value']   = $tmp[$i];
      }
    }

    /* Add class depending on given id, to alternate background colors */
    if($this->colorAlternator++ & 1){
      $data['row']['class'] = "ObjectListViewport_Entry_Row1";
    }else{
      $data['row']['class'] = "ObjectListViewport_Entry_Row2";
    }

    $data['cols'] = $cols;
    return($data);
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
  protected function __render() {

    /* Apply current filter */
    $entries = "";
    $objects= new ObjectListFilterIterator($this->objects->getIterator());
    foreach ($objects as $value){
      $entries[] = $this->renderEntry($value);
    }


    #Fabian: _POST/_GET Variablen bitte mit $this->createVariable('name');
    #        erstellen.
    #        Damit kann das von der übergreifenden Funktion extrahiert werden
    #        und wir haben keinen doppelten Code dafür.
    #        
    #        Wurde z.B. eine Variable via $this->createVariable('cn') erzeugt und via
    #        smarty eingebunden, dann kann Sie nach einem _POST oder _GET via
    #        $this->getRequestVariable_cn() wieder abgefragt werden.

    $smarty = get_smarty();
    $smarty->assign("OLV_Entries",$entries);
    $smarty->assign("OLV_List_Id",$this->id);

    /* Footer variables */
    $smarty->assign("OLV_Footer_Enabled",$this->displayFooterFlag);
    $smarty->assign("OLV_Footer_Message",$this->footer);
    $smarty->assign("OLV_Num_Cols",$this->numberOfCols);

    /* Assign Headline values */
    $smarty->assign("OLV_Header_Enabled",$this->displayHeaderFlag);
    $smarty->assign("OLV_Header",$this->headline);
 
    return($smarty->fetch("ObjectListViewport.tpl"));
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
