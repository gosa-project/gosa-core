<?php

class divSelectBox {

	var $a_entries;
	var $summary;
	var $cols;

	// Members for page managment
  var $width= 300;
  var $height=200;

	// Added php 4 constructor
	function divSelectBox(){
		$this->s_summary                = "";
		$this->a_entries                = array();
		$this->cols                     = 0;
	}

  function setWidth($w){
    $this->width=$w;
  }

  function setHeight($h){
    $this->height=$h;
  }

	function AddEntry($a_entriedata) {
		$this->a_entries[] = $a_entriedata;
	}

  function DrawList(){
    $s_return  = "";
    $s_return .= "<div style=\"height:2px; overflow:hidden;background:#888888;width:".($this->width-18)."px;\"></div>\n";
    $s_return .= "<div style=\"padding-right:1px;padding-bottom:2px;\" height='".($this->height)."' width='".($this->width)."'>\n";
    $s_return .= "<div style=\"overflow: auto;height:".$this->height."px;width:".($this->width)."px;\">\n";
    $s_return .= "<table ".
                    "summary='".$this->s_summary."' ".
                    "width='".($this->width-18)."' ".
                    "height='".($this->height)."' ".
                    "cellspacing='0' ".
                    "style='overflow:scroll; ".
                    "padding-right:1px; ".
                    "padding-bottom:2px; ".
                    "border-left:solid 2px; ".
                    "border-top:solid 0px; ".
                    "border-color:#888888; ".
                    "'>\n";
    $s_return.=$this->_generatePage();
    $s_return.= "</table></div></div>";
    return ($s_return);
  }
	
	function _numentries(){
		$cnt = count($this->a_entries);
		return $cnt;
	}
	
	function SetSummary($msg){
		$this->s_summary = $msg;
	}

	function _generatePage(){
		
		$s_value 	= "";
		$s_key 		= "";
		$s_return 	= "";
		$i_alternate=0;
    $str = "";
	
    /* If divlist is empty, append a single white entry */
    if(count($this->a_entries)==0){
      $str.="<tr><td class='list1' style='height:100%';>&nbsp;</td></tr>";
      return($str);
    }

    $i = $this->_numEntries();
    foreach($this->a_entries as $s_key => $s_value){
      $i--;

      if($i_alternate!=0){ 
        $i_alternate=0; 
      } else {
        $i_alternate=1;
      }

      $s_return .= "\n<tr>";

      $cnt = 0; 					

      foreach($s_value as $s_key2 => $s_value2 ){				
        $this->cols = count($s_value) ;						
        $cnt++;					

        if(!isset($s_value2['class'])){
          $class = "list".$i_alternate; 
        } else {
          $class = $s_value2['class'];
        }

        if(!isset($s_value2['attach'])){
          $style = "";
        } else {
          $style = " ".$s_value2['attach']." "	;
        }

        $s_return .= "\n<td ".$style." class='".$class."'>";
        $s_return .= $s_value2['string'];
        $s_return .= "</td>";
      }
      $s_return.="\n</tr>";
    }
    for($i = 0 ; $i < ($this->cols) ; $i ++){
      if($i >= ($this->cols-1)){
        $s_return .= "<td class='list1' height='100%' style='border:0px;'></td>";
      }else{
        $s_return .= "<td class='list1' height='100%'></td>";
      }
      
    }
    $s_return.="</tr>";
    return $s_return;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
