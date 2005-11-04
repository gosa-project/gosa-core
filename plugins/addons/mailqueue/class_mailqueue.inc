<?php

class mailqueue extends plugin
{
  /* Definitions */
  var $plHeadline= "Mail queue";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $QueryCommand = "";
  var $RemoveCommand= "";

  var $Server         = "none";
  var $Time           = 0;
  var $Search         = "*";
  var $Page           = 0;
  var $EntriesPerPage = 20;

  var $OrderBy        = "Arrival";
  var $SortType       = "up";

  function mailqueue($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    $this->QueryCommand = search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_QUERY_COMMAND");
    $this->RemoveCommand= search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_REMOVE_COMMAND");

    $this->Server = "none";
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
    $smarty= get_smarty();

    if($str = @shell_exec ($this->QueryCommand)){	
      $mailQueueParser = new parseMailQueue($str);
      $mailQueueParser->OrderBy($this->OrderBy,$this->SortType);
      $mailQueueParser->OnlyDaysAgo($this->Time);
      $mailQueueParser->CreateDate();
      $mailQueueParser->Search($this->Search,array("MailID","Size","Sender","Recipient","Error","Arrival"));

      $entries = $mailQueueParser->GetAll(); 
      $smarty->assign("entries",array_slice($entries,$this->Page,20));
      $smarty->assign("plug","?plug=".$_GET['plug']);
      $smarty->assign("p_server",$this->Server);
      $smarty->assign("p_servers",$this->getServer());
      $smarty->assign("p_time", $this->Time);
      $smarty->assign("p_times",$this->getTimes());
      $smarty->assign("p_timeKeys",array_flip($this->getTimes()));
      $smarty->assign("search_for",$this->Search);
      $smarty->assign("range_selector", range_selector(count($entries), $this->Page, 20)); 
      $smarty->assign("OrderBy",$this->OrderBy);
      if($this->SortType == "up"){
        $smarty->assign("SortType","<img src='images/sort_up.png' alt='"._("up")."' border='0'>");
      }else{
        $smarty->assign("SortType","<img src='images/sort_down.png' alt='"._("down")."' border='0'>");
      }
 
    }else{
      $smarty->assign("entries",array());
      print_red(_("Please check your 'gosa.conf' the value for 'MAILQUEUE_QUERY_COMMAND' can't be executed."));
    }
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

  function getServer()
  {
    $ret= array("none",_("none"));
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);    
    $ldap->search("(objectClass=goMailServer)",array("cn"));
    while($tmp = $ldap->fetch()){
     $ret[$tmp['cn'][0]]= $tmp['cn'][0];
    }
    return($ret);
  }

  function getTimes()
  {
    $ret = array("all"=>_("all"));
    for($i = 0 ; $i < 10 ; $i ++ ){
      if($i == 1){
        $ret[$i] = $i."&nbsp;"._("day ago");
      }else{
        $ret[$i] = $i."&nbsp;"._("days ago");
      }
    }
    return($ret);
  }

  function save_object($save_current= FALSE)
  {
    if(isset($_POST['p_server'])){
      $this->Server = $_POST['p_server'];
    }
    if(isset($_POST['p_time'])){
      $this->Time = $_POST['p_time'];
    }
    if(isset($_POST['search_for'])){
      $this->Search = $_POST['search_for'];
    }
    if((isset($_GET['start']))&&(is_numeric($_GET['start']))&&($_GET['start']>=0)){
      $this->Page = $_GET['start'];
    }

    if((isset($_GET['sort']))&&(!empty($_GET['sort']))){
      $old = $this->OrderBy;
      $this->OrderBy = $_GET['sort'];
      if($this->OrderBy == $old)
      {
        if($this->SortType== "up"){
          $this->SortType = "down";
        }else{
          $this->SortType = "up";
        }
      }
    }

  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
