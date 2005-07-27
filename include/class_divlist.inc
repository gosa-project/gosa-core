<?php

class divlist {

	private $a_entries;
	private $a_header;
	private $b_displayPageNums; 	
	private $summary;
	private $cols;

	// Members for page managment
	private $i_currentPage;
	private $i_entriesPerPage;
	
	
	public function __construct(){
		$this->i_currentPage	=0;
		$this->i_entriesPerPage	=10;
		$this->s_summary		="";
		$this->a_entries		=array();
		$this->a_header			=array();
		$this->b_displayPageNums 		=true;
		$this->cols = 0;
	}
	

	public function __destruct(){
	
	}

	public function AddEntry($a_entriedata) {
		$this->a_entries[] = $a_entriedata;
	}

	public function DisablePageNums() {
		$this->b_displayPageNums =false;
	}

	public function SetHeader($a_headerdata) {
		$this->a_header[] = $a_headerdata ;
	}

	public function SetEntriesPerPage($i_num){
		$this->i_entriesPerPage=$i_num;
	}
	
	public function DrawList(){
		
		$s_return = "";
		
		$s_return.= "<table summary='".$this->s_summary."' width='600' cellspacing='0'>";
		
		$s_return.= $this->_generateHeader();
		
		$s_return.= $this->_generatePage();
		//$s_return.= nl2br(htmlentities($this->_generatePage()));
		
		$s_return.= "</table>";
		
		return ($s_return);
	}
	
	private function _numpages(){
		$cnt = count($this->a_entries);
		
		$tmp 	= $cnt % $this->i_entriesPerPage;
		$pages 	= (int) ($cnt / $this->i_entriesPerPage);
		if($tmp) $pages ++;
		
		return $pages;
	}
	
	private function _numentries(){
		$cnt = count($this->a_entries);
		return $cnt;
	}
	
	private function _generateHeader(){
		
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
	
	public function	 SetSummary($msg){
		$this->s_summary = $msg;
	}

	private function  _generatePage(){
		
		$s_value 	= "";
		$s_key 		= "";
		$s_return 	= "";
		
		$i_alternate=0;
	
		if(isset($_GET['start'])){
			$start = $_GET['start'];
		}else{
			$start=0;
		}
		$stop  = $start + $this->i_entriesPerPage;
		

		$appendempty = ($this->_numentries() -$start);



		for($i = $start ; $i <= $stop;$i++){
			
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
