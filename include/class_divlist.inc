<?php

class divlist {

	var $a_entries;
	var $a_header;
	var $b_displayPageNums; 	
	var $summary;
	var $cols;
	var $pageid;

	// Members for page managment
	var $i_currentPage;
	var $i_entriesPerPage;

	// Added php 4 constructor
	function divlist($pageid){

		$this->i_currentPage    = 0;
		$this->i_entriesPerPage = 10;
		$this->s_summary                = "";
		$this->a_entries                = array();
		$this->a_header                 = array();
		$this->b_displayPageNums= true;
		$this->cols                     = 0;
		$this->pageid                   = $pageid ;

	}

	function __destruct(){
	
	}

	function AddEntry($a_entriedata) {
		$this->a_entries[] = $a_entriedata;
	}

	function DisablePageNums() {
		$this->b_displayPageNums =false;
	}

	function SetHeader($a_headerdata) {
		$this->a_header[] = $a_headerdata ;
	}

	function SetEntriesPerPage($i_num){
		$this->i_entriesPerPage=$i_num;
	}
	
	function DrawList(){
		$s_return = "";
		$s_return.= "<table summary='".$this->s_summary."' width='600' height='500' cellspacing='0'>";
    if($this->i_entriesPerPage==0) {
      // TODO: Find a more convenient way to use the same functionality in both
      // MSIE and non-MSIE browsers. ATM we need to manually set entriesPerPage to
      // a non-zero value, because MSIE won't scroll the tbody properly.
      $s_browser = isset($_SERVER['HTTP_USER_AGENT'])?$_SERVER['HTTP_USER_AGENT']:"";
      if(is_numeric(strpos($s_browser,'Mozilla')) &&
         !is_numeric(strpos($s_browser,'MSIE')) &&
         !is_numeric(strpos($s_browser,'KHTML'))
        ){
        $s_return.= "<thead>".$this->_generateHeader()."</thead>";
        // Define an alternate style for IE (not used ATM)
        // $s_return.= "<!--[if IE]><style type='text/css'>tbody.scrollcontent {overflow-y:scroll;max-height:480px;}</style><![endif]-->";
        $s_return.= "<tbody style='max-height:480px;overflow:auto;overflow:-moz-scrollbars-vertical;'>".$this->_generatePage()."</tbody>";
      } else {
        // Call ourself with i_entriesPerPage set to a default value
        $this->SetEntriesPerPage(20);
        $s_return.=$this->_generateHeader();
        $s_return.=$this->_generatePage();
      }
    } else {
        $s_return.=$this->_generateHeader();
        $s_return.=$this->_generatePage();
    }
		//$s_return.= nl2br(htmlentities($this->_generatePage()));
		
		$s_return.= "</table>";
		
		return ($s_return);
	}
	
	function _numpages(){
		$cnt = count($this->a_entries);
		
		$tmp 	= $cnt % $this->i_entriesPerPage;
		$pages 	= (int) ($cnt / $this->i_entriesPerPage);
		if($tmp) $pages ++;
		
		return $pages;
	}
	
	function _numentries(){
		$cnt = count($this->a_entries);
		return $cnt;
	}
	
	function _generateHeader(){
		
		$s_return = "";
		
		$s_value 	= "";
		$s_key 		= "";
				
		$s_return .= "\n<tr>";

    $i_count = count($this->a_header[0])-1;
		foreach($this->a_header[0] as $s_key => $s_value ){
			if(!isset($s_value['attach'])){
				$s_value['attach'] = "";
			}
	
      if($i_count == 0) {
  			$s_return .= "\n<td class='listheader' style='border-right:0px;border-bottom:1px solid #b0b0b0;' ".$s_value['attach'].">".$s_value['string']."</td>";
      } else {
  			$s_return .= "\n<td class='listheader' style='border-bottom:1px solid #b0b0b0;'".$s_value['attach'].">".$s_value['string']."</td>";
      }
      $i_count--;
		}
		
    // Only create additional column if we're using scrollbars
    if($this->i_entriesPerPage==0) {
      $s_return .= "\n<td class='listheader' style='border-right:0px;padding:0px;border-bottom:1px solid #b0b0b0;width:16px;'>&nbsp;</td>";
    }
		$s_return .= "\n</tr>";
		return $s_return;
	}
	
	function SetSummary($msg){
		$this->s_summary = $msg;
	}

	function _generatePage(){
		
		$s_value 	= "";
		$s_key 		= "";
		$s_return 	= "";
		
		$i_alternate=0;
	
		if(isset($_GET['start'])){
			$_SESSION['start'.$this->pageid]=$_GET['start'];
			$start = $_GET['start'];
		} else {
			if(isset($_SESSION['start'.$this->pageid])){
				$start = $_SESSION['start'.$this->pageid];
			} else {
				$start=0;
			}
		}
		
    if ($this->i_entriesPerPage > 0) {
  		while($start > $this->_numentries()){
  			$start = $start - $this->i_entriesPerPage;
  		}	
  
  		$stop  = $start + $this->i_entriesPerPage;
  
  		$appendempty = ($this->_numentries() -$start);
  
  		for($i = $start ; $i < $stop;$i++){
  			
  			if(isset($this->a_entries[$i])){
  							
  				$s_value = $this->a_entries[$i];
  				
  				
  				if($i_alternate) $i_alternate=0; else $i_alternate=1;
  				
  				$s_return .= "\n<tr>";
  				
  				$cnt = 0; 					
  					
  				foreach($s_value as $s_key2 => $s_value2 ){				
  
  					$this->cols = count($s_value) ;						
  					$cnt++;					
  
  					if(!isset($s_value2['class'])){
  						$class = "list".$i_alternate; 
  					}else{
  						$class = $s_value2['class'];
  					}
  
  					if(!isset($s_value2['attach'])){
  						$style = "";
  					}else{
  						$style = " ".$s_value2['attach']." "	;
  					}
  
  					$s_return .= "\n<td ".$style." class='".$class."'>";
  					$s_return .= $s_value2['string'];
  					$s_return .= "\n</td>";
  				}
  				if($cnt == 0 ){
  					$s_return.="\n<td>&nbsp;</td>";
  				}

  				$s_return .= "\n</tr>";
  			}	
  		}
  
  		if(!(($stop)<$this->_numentries())){
  			$nums = $stop - $this->_numentries();// - $stop;
  			for($i = 0 ; $i < $nums ; $i ++ ){
  				$s_return.="<tr>";
  				$cnt=0;
  				for($a = 0 ; $a < (count($this->a_header[0])) ; $a ++ ) {
  					if($a ==(count($this->a_header[0])-1)){
  						$s_return.="\n<td class='list1' style='border:0px;' height='26'>&nbsp;</td>";
  					} else {
  						$s_return.="\n<td class='list1' height='26'>&nbsp;</td>";
  					}
  				}
  				$s_return.="\n</tr>";
  			}	
  		}
  
  		if($this->b_displayPageNums){
  			$s_return .= "<tr><td colspan='".$this->cols."' align='center'>".range_selector($this->_numentries(),$start,$this->i_entriesPerPage)."</td></tr>";
  		}
    } else {
      // We should display all entries on one page

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
			  $s_return .= "\n</tr>";
		  }
    }

    // if fewer than 22 Entries (list not full), print row to fill empty space
    if($this->_numEntries()<22){
      $fill= "";
      for ($i= 1; $i <= $this->cols; $i++){
        if ($i == $this->cols){
         $fill.= "<td class='list1' style='height:100%;border-right:0px';>&nbsp;</td>";
        } else {
         $fill.= "<td class='list1' style='height:100%';>&nbsp;</td>";
        }
      }
      $s_return.="\n<tr>$fill</tr>";
    }
    return $s_return;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
