<?php

class parseMailQueue
{
	var $s_dataToParse;
	var $a_parsedData;
  var $i_count;	

  /* Contructor
   * $s_data specifies the data that shuold be parse
   */
	function parseMailQueue($s_data)
	{
    $this->s_dataToParse = $s_data;		
	  $this->_parse();
  }


  /* Remove all entries which are older than the last x hours
   */
  function OnlyDaysAgo($str)
  {
    /* Get current time */
    $cur = time();
  
    /* Only perform this filter, if the given parameter is valid */
    if((is_numeric($str))&&($str != 0)){

      /* hours are given as parameter */
      $cur = $cur - ($str*(60*60));

      /* Remove old entries */
      foreach($this->a_parsedData as $key => $data){
        if($data['Arrival'] < $cur){
          unset($this->a_parsedData[$key]);
        }
      }
    }
  }


  /* Only keep entries that contains the $filter
   * in any of the given $fields
   */
  function Search($filter,$fields)
  {
    /* Go through all entries */
    foreach($this->a_parsedData as $key => $data){

      /* not found yet */
      $found = false;

      foreach($fields as $attr){
        if(preg_match("/".str_replace("*",".*",$filter)."/i",$data[$attr])){
          $found= true;
        }
      }
  
      /* if nothing found, delete this entry */
      if($found == false){
        unset($this->a_parsedData[$key]);
      }
    }
  }

  /* Convert date from timestamp to human readable */
  function CreateDate()
  {
    foreach($this->a_parsedData as $key => $data){
      $this->a_parsedData[$key]['Arrival'] = date("d.m.Y H:i:s",$data['Arrival']);
    }
  }

  /* Order by specified field */
  function OrderBy($str = "Arrival",$type = "up" )
  {
    $tmp = array();

    /* If the given field is not valid */
    if(!in_array($str,array("MailID","Size","Sender","Recipient","Arrival","Error"))){
      return(false);
    }

    /* Size need special handling, cause it contains numbers 
     */
    if($str == "Size"){
      foreach($this->a_parsedData as $data){
        $struse = "";
        for($i = strlen($data['Size']); $i < 10 ; $i++  ){
          $struse .="0";
        }
        $struse .= $data[$str].$data['MailID'];
        $tmp[$struse]= $data;
      }
    }else{
      foreach($this->a_parsedData as $data){
        $tmp[strtolower($data[$str]).$data['MailID']]= $data;
      }
    } 
    ksort($tmp);
    if($type != "up"){
      $tmp = array_reverse($tmp);
    }
    $this->a_parsedData = array();
    foreach($tmp as $data){
      $this->a_parsedData[$data['MailID']] = $data;
    }
  }
  
  function GetAll()
  {
    return($this->a_parsedData);
  }

  /* Checks if the given MailID exists */
  function IDExists($id)
  {
    return(((isset($this->a_parsedData[$id]))&&(is_array($this->a_parsedData[$id]))));
  }

  /* This function parses the given data 
   * it creates an array with all given queue entries
   */
  function _parse()
  {
    $i              =  0;       // Temp var 
    $entries        = array();  // Contains an array with the raw data for every single entry
    $s_tmp          = "";       // Buffer

    $s_mailID       = "";       // Queue ID 
    $s_Size         = "";       // Mail size 
    $s_Arrival      = "";       // Arrival time
    $s_Sender       = "";       // Sender
    $s_Recipient    = "";       // Recipient 
    $s_Error        = "";       // Occured error

    /* Remove header
     */
    $this->s_dataToParse = preg_replace("/^.*------\n/","",$this->s_dataToParse);

    /* Create array with single entries
     */
    $entries = split("\n\n",$this->s_dataToParse);
  
    /* The last entry in this array is not realy an valid entry, its some kind of status.
     * It would be something like this : -- 795 Kbytes in 124 Requests.
     */
    $this->i_count = (count($entries))-1;
  
    for($i = 0 ; $i < $this->i_count; $i ++ ){
    
      while(strstr($entries[$i],"  ")){
        $entries[$i] = str_replace("  "," ",$entries[$i]);  
      } 
    
      $s_buffer = split("\n",preg_replace("/[\\n\\r\\t]/s","\n",$entries[$i]));
        
      /* Get mailID */
      $tmp = split(" ",$s_buffer[0]);

      /* Get values */
      $s_mailID   = $tmp[0];
      $s_Size     = $tmp[1];
      $s_Sender   = $tmp[6];

      /* Parse time */
      $tmp3 = split(":",$tmp[5]);
      $tmp2 = strtotime($tmp[4]." ".$tmp[3]." ".date("Y"));
      $s_Arrival= mktime($tmp3[0],$tmp3[1],$tmp3[2],date("d",$tmp2),date("m",$tmp2),date("Y",$tmp2));

      $s_Error      = $s_buffer[1];
      $s_Recipient  = $s_buffer[2];

      /* Append data */
      $this->a_parsedData[$s_mailID]['MailID']     = $s_mailID; 
      $this->a_parsedData[$s_mailID]['Size']       = $s_Size; 
      $this->a_parsedData[$s_mailID]['Arrival']    = $s_Arrival; 
      $this->a_parsedData[$s_mailID]['Sender']     = $s_Sender; 
      $this->a_parsedData[$s_mailID]['Recipient']  = $s_Recipient; 
      $this->a_parsedData[$s_mailID]['Error']      = $this->_parseError($s_Error); 
    }
    return($this->a_parsedData);
  }

  /* Parse Error part of the entry */
  function _parseError($str)
  {
    $str   = trim(preg_replace("/[()]/","",$str));
    $tmp2 = split(":",$str);
    $tmp = array_reverse($tmp2);
    $err  = preg_replace("/#.*$/","",$tmp[0]);
    $text = preg_replace("/said$/i","",trim($tmp2[0])); 
    return($err);
  }

}




// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
