<?php

class parseMailQueue
{
	var $s_dataToParse;
	var $a_parsedData;
  var $i_count;	

  /* Contructor
   * $s_data specifies the data that shuold be parse
   */
	function parseMailQueue($s_data,$server)
	{
    $this->s_dataToParse = $s_data;		
	  $this->a_parsedData = array();
    $this->_parse($s_data,$server);
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
  function Search($filter,$fields,$bool = false)
  {
    /* Go through all entries */
    foreach($this->a_parsedData as $key => $data){

      /* not found yet */
      $found = false;

      foreach($fields as $attr){
        if(($bool)&&($data[$attr]==$filter)){
          $found = true;
        }elseif(preg_match("/".str_replace("*",".*",$filter)."/i",$data[$attr])){
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
    if(!in_array($str,array("MailID","Size","Sender","Recipient","Arrival","Error","Server"))){
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
        $struse .= $data[$str].$data['MailID'].$data['Server'];
        $tmp[$struse]= $data;
      }
    }else{
      foreach($this->a_parsedData as $data){
        $tmp[strtolower($data[$str]).$data['MailID']."-".$data['Server']]= $data;
      }
    } 
    ksort($tmp);
    if($type != "up"){
      $tmp = array_reverse($tmp);
    }
    $this->a_parsedData = array();
    foreach($tmp as $data){
      $this->a_parsedData[$data['MailID']."-".$data['Server']] = $data;
    }
    return(true);
  }
  
  function GetAll()
  {
    return($this->a_parsedData);
  }

  /* Checks if the given MailID exists */
  function IDExists($id)
  {
    foreach($this->a_parsedData as $entry){
      if($entry['MailID'] == $id) return(true);
    }
    return(false);
  }

  function parseAdditionalQueue($str, $server)
  {
    $this->_parse($str, $server);
  }

  /* This function parses the given data 
   * it creates an array with all given queue entries
   */
  function _parse($str, $server)
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
    $this->s_dataToParse = preg_replace("/^.*------\n/","",$str);

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

      /*
              *      The message is in the active queue, i.e. the  message  is
                     selected for delivery.

              !      The  message is in the hold queue, i.e. no further deliv-delivery
                     ery attempt will be made until  the  mail  is  taken  off
                     hold.
      */

      $s_Hold = false;
      if(preg_match("/\!/",$s_mailID)){
        $s_mailID = preg_replace("/\!/","",$s_mailID);
        $s_Hold = "true";
      }
      
      $s_Active = false;
      if(preg_match("/\*/",$s_mailID)){
        $s_mailID = preg_replace("/\*/","",$s_mailID);
        $s_Active = true;
      }

      /* Append data */
      $this->a_parsedData[$s_mailID."-".$server]['Server']     = $server; 
      $this->a_parsedData[$s_mailID."-".$server]['MailID']     = $s_mailID; 
      $this->a_parsedData[$s_mailID."-".$server]['Size']       = $s_Size; 
      $this->a_parsedData[$s_mailID."-".$server]['Arrival']    = $s_Arrival; 
      $this->a_parsedData[$s_mailID."-".$server]['Sender']     = $s_Sender; 
      $this->a_parsedData[$s_mailID."-".$server]['Recipient']  = $s_Recipient; 
      $this->a_parsedData[$s_mailID."-".$server]['Hold']       = $s_Hold; 
      $this->a_parsedData[$s_mailID."-".$server]['Active']     = $s_Active; 
      $this->a_parsedData[$s_mailID."-".$server]['Error']      = $this->_parseError($s_Error); 
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
