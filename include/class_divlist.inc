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
	
	
	function __construct($pageid){
		$this->i_currentPage	= 0;
		$this->i_entriesPerPage	= 10;
		$this->s_summary		= "";
		$this->a_entries		= array();
		$this->a_header			= array();
		$this->b_displayPageNums= true;
		$this->cols 			= 0;
		$this->pageid 			= $pageid ;
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
		
		$s_return.= "<table summary='".$this->s_summary."' width='600' cellspacing='0'>";
		
		$s_return.= $this->_generateHeader();
		
		$s_return.= $this->_generatePage();
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
		foreach($this->a_header[0] as $s_key => $s_value ){
			if(!isset($s_value['attach'])){
				$s_value['attach'] = "";
			}
			
			$s_return .= "\n<td class='listheader' ".$s_value['attach'].">".$s_value['string']."</td>";
		}
		
		$s_return .= "\n</tr>";
		return $s_return;
	}
	
	function	 SetSummary($msg){
		$this->s_summary = $msg;
	}

	function  _generatePage(){
		
		$s_value 	= "";
		$s_key 		= "";
		$s_return 	= "";
		
		$i_alternate=0;
	
		if(isset($_GET['start'])){
			$_SESSION['start'.$this->pageid]=$_GET['start'];
			$start = $_GET['start'];
		}else{
			if(isset($_SESSION['start'.$this->pageid])){
				$start = $_SESSION['start'.$this->pageid];
			}else{
				$start=0;
			}
		}
		
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
					$s_return.="<td>&nbsp;</td>";
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
						$s_return.="<td class='list1' style='border:0px;' height='26'>&nbsp; </td>";
					}else{
						$s_return.="<td class='list1' height='26'>&nbsp; </td>";
					}
				}
				$s_return.="</tr>";
			}	
		}

		if($this->b_displayPageNums){
			$s_return .= "<tr><td colspan='".$this->cols."' align='center'>".range_selector($this->_numentries(),$start,$this->i_entriesPerPage)."</td></tr>";
		}

		return $s_return;
	}
	
	
}
?>
