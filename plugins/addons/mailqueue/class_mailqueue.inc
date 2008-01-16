<?php

class mailqueue extends plugin
{
  /* Definitions */
  var $plHeadline= "Mail queue";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes     = array();
  var $objectclasses  = array();
  var $mailQueueScript= "";    
  var $Server         = "all";
  var $Time           = 0;
  var $Search         = "*";
  var $Page           = 0;
  var $Stat           = "all";
  var $OrderBy        = "Arrival";
  var $SortType       = "up";
  var $disp_header    = false;
  var $range          = 20;   
  var $view_logged = FALSE;

  function mailqueue(&$config, $dn= NULL)
  {
    $this->config           = &$config;
    if (isset($this->config->current['MAILQUEUESCRIPTPATH'])){
      $this->mailQueueScript  = $this->config->current['MAILQUEUESCRIPTPATH'];    
    }

    if(isset($this->config->data['MAIN']['MAILQUEUESCRIPTPATH'])){
       $this->mailQueueScript  = $this->config->data['MAIN']['MAILQUEUESCRIPTPATH'];
    }

    $this->Server           = "all";
  }


  function pass_cmd($str)
  {
    return(shell_exec($str));
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","mailqueue/".get_class($this),$this->dn);
    }

    if(isset($_POST['EntriesPerPage'])){
      $this->range = $_POST['EntriesPerPage'];
    }

    $smarty= get_smarty();
    $error =false;

    if(empty($this->mailQueueScript)){
      msg_dialog::display(_("Configuration error"), sprintf(_("Missing '%s' directive in configuration!"), "mailqueuescriptpath"), ERROR_DIALOG);
      $error = true;
    }else{

      /* If we have more than one server selected (all), 
         create a string with all servers separated by ', '
       */ 
      if($this->Server=="all"){
        $se = $this->getServer();

        unset($se['all']);
        $se_str = "";
        foreach($se as $server) {
          $se_str .= $server." ";
        }
      }else{
        /* We have only one server selected */
        $se_str = $this->Server;
      }

      /* Check all post that will effect all entries */
      $only_once = true;
      foreach(array("unhold_all","hold_all","del_all","requeue_all") as $attr){
        foreach($_POST as $name => $value){
          if((preg_match("/".$attr."/",$name))&&($only_once)){
            if(!$this->acl_is_readable($attr)){
              $only_once = false;
	      msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to submit a '%s' command!"), $attr), ERROR_DIALOG);
            }else{
              $only_once = false;
              $act = preg_replace("/_.*$/i","",$attr);
              $r_cmd = preg_replace("/%action/" , $act           ,$this->mailQueueScript);
              $r_cmd = preg_replace("/%server/" , $se_str         ,$r_cmd);
              $r_cmd = preg_replace("/%id/"     , "ALL"           ,$r_cmd);
              if($this->pass_cmd($r_cmd)==false){
		msg_dialog::display(_("Configuration error"), sprintf(_("'%s' defined for the '%s' directive cannot be executed!"), $r_cmd, "mailqueuescriptpath"), ERROR_DIALOG);
              }
            }
          }
        }
      }        

      /* Check single entry manipulation  posts */
      $only_once = true;

      /* act specifies the command to execute */
      if(isset($_GET['act'])){
        $opt = $_GET['act'];  
  
        /* The option to exec should be one of these */
        if(in_array($opt,array("unhold","hold","del","requeue","query","header"))){
          $only_once = false;

          if(!$this->acl_is_readable($opt)){
	      msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to submit a '%s' command!"), $opt), ERROR_DIALOG);
          }else{
            /* Create cmd */
            $r_cmd = preg_replace("/%action/" , $opt            ,$this->mailQueueScript);
            $r_cmd = preg_replace("/%server/" , $this->Server   ,$r_cmd);
            $r_cmd = preg_replace("/%id/"     , $_GET['id']     ,$r_cmd);

            /* Execute cmd */
            if(!$str = $this->pass_cmd($r_cmd)){
	      msg_dialog::display(_("Configuration error"), sprintf(_("'%s' defined for the '%s' directive cannot be executed!"), $r_cmd, "mailqueuescriptpath"), ERROR_DIALOG);
            }else{
            
              /* Special handling for option='header' */
              if($opt == "header"){
          
                /* Create table which displays the header informations */
                $this->disp_header ="\n<table width='80%'>";
                foreach(split("\n",$str) as $line){
                  $line = trim($line);
                  if(empty($line)) {
                    continue;
                  }
                  $this->disp_header .= "\n<tr>";
                  $tmp0 = preg_replace("/:.*$/","",$line);
                  $tmp1 = preg_replace("/^.*:/","",$line);
                  $this->disp_header .= "\n<td style='background-color:#EEEEEE;'>".$tmp0."</td><td>".$tmp1."</td>"; 
                  $this->disp_header .= "\n</tr>";
                }              
                $this->disp_header .= "\n</table>";
              }
            }
          }
        }
      }        


      /* Back is posted from the header display page */
      if(isset($_POST['back'])){
        $this->disp_header = false;
      }

      /* If there is a header in disp_header, then display it */
      if($this->disp_header){
        $smarty->assign("header",$this->disp_header);
        return ($smarty->fetch (get_template_path('header.tpl', TRUE)));
      }

      /* tell smarty to display the search results*/
      $smarty->assign("all_ok"      , "true");
      
      /* A single server is selected */
      if($this->Server != "all"){    

        /* Create Query cmd */ 
        $q_cmd = preg_replace("/%action/" ,"query"      ,$this->mailQueueScript);
        $q_cmd = preg_replace("/%server/" ,$this->Server,$q_cmd);
        $q_cmd = preg_replace("/%id/"     ,"all"        ,$q_cmd);

        if(!$this->acl_is_readable("query")){
	  msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to submit a '%s' command!"), "query"), ERROR_DIALOG);
          $mailQueueParser = new parseMailQueue("",$this->Server);
        }else{

          /* Only display this if the query cmd is executeable */
          if($str = @$this->pass_cmd ($q_cmd)){
            /* Parse returned data */
            $mailQueueParser = new parseMailQueue($str,$this->Server);
          }else{
            /* On error/ no return value / false return value */
	    msg_dialog::display(_("Configuration error"), sprintf(_("'%s' defined for the '%s' directive cannot be executed!"), $q_cmd, "mailqueuescriptpath"), ERROR_DIALOG);
            $error = true;
          }
        }
      }else{
        $mailQueueParser = NULL;
        foreach($this->getServer() as $ServerID=>$ServerName){

          /* Don't query the server named all :) */
          if($ServerID == "all") continue;

          /* Prepare query cmd */
          $q_cmd = preg_replace("/%action/" ,"query"      ,$this->mailQueueScript);
          $q_cmd = preg_replace("/%server/" ,$ServerName  ,$q_cmd);
          $q_cmd = preg_replace("/%id/"     ,"ALL"           ,$q_cmd);

          
          if(!$this->acl_is_readable("query")){
	    msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to submit a '%s' command!"), "query"), ERROR_DIALOG);
            $mailQueueParser = new parseMailQueue("",$this->Server);
          }else{

            /* Shell exec this cmd */
            if($str = @$this->pass_cmd ($q_cmd)){

              /* If there is no parser available, create one  */
              if($mailQueueParser === NULL){
                $mailQueueParser = new parseMailQueue($str,$ServerID);
              }else{
                $mailQueueParser->parseAdditionalQueue($str,$ServerID);
              }
              /* On error/ no return value / false return value */
            }else{
	      msg_dialog::display(_("Configuration error"), sprintf(_("'%s' defined for the '%s' directive cannot be executed!"), $q_cmd, "mailqueuescriptpath"), ERROR_DIALOG);
              $error = true;
            }
          }
        }
      }     

      /* Check for existing servers 
          objectClass=goMailServer is required at least for one server. 
         Else display error   */
      $server = $this->getServer();
      if((count($server) == 1 ) && (isset($server['all']))){
	msg_dialog::display(_("Configuration error"), _("No mail servers sepcified!"), ERROR_DIALOG);
        $error = true;
      }

      if(!$error){

        /* Filter data with the given */
        $mailQueueParser->OrderBy($this->OrderBy,$this->SortType);
        $mailQueueParser->OnlyDaysAgo($this->Time);
        $mailQueueParser->CreateDate();
     
        if($this->Stat == "hold"){
          $mailQueueParser->Search(true,array("Hold"),true);
        }
        if($this->Stat == "unhold"){
          $mailQueueParser->Search(false,array("Hold"),true);
        }
        if($this->Stat == "active"){
          $mailQueueParser->Search(true,array("Active"),true);
        }
        if($this->Stat == "nonactive"){
          $mailQueueParser->Search(false,array("Active"),true);
        }

        $mailQueueParser->Search($this->Search,array("MailID","Size","Sender","Recipient","Error","Arrival"));

        /* */
        $entries = $mailQueueParser->GetAll();
  
        if(count($entries) ==0 ){
          $smarty->assign("all_ok",false);
        }
 
        $smarty->assign("entries"       , array_slice($entries,$this->Page,$this->range));
        $smarty->assign("plug"          , "?plug=".$_GET['plug']);
        $smarty->assign("r_stats"       , $this->getStats());
        $smarty->assign("stats"         , array_flip($this->getStats()));
        $smarty->assign("stat"          , $this->Stat);
        $smarty->assign("p_server"      , $this->Server);
        $smarty->assign("p_servers"     , $this->getServer());
        $smarty->assign("p_serverKeys"  , array_flip($this->getServer()));
        $smarty->assign("p_time"        , $this->Time);
        $smarty->assign("p_times"       , $this->getTimes());
        $smarty->assign("p_timeKeys"    , array_flip($this->getTimes()));
        $smarty->assign("search_for"    , $this->Search);
        $smarty->assign("range_selector", range_selector(count($entries), $this->Page, $this->range,"EntriesPerPage")); 
        $smarty->assign("OrderBy"       , $this->OrderBy);

        /* Display sort arrow */
        if($this->SortType == "up"){
          $smarty->assign("SortType","<img src='images/sort_up.png' alt='"._("up")."' border='0'>");
        }else{
          $smarty->assign("SortType","<img src='images/sort_down.png' alt='"._("down")."' border='0'>");
        }
      }
    }

    /* In case of an error */
    if($error){  
      $smarty->assign("all_ok"        , "false");
      $smarty->assign("r_stats"       , $this->getStats());
      $smarty->assign("stats"         , array_flip($this->getStats()));
      $smarty->assign("stat"          , $this->Stat);
      $smarty->assign("plug"          , "?plug=".$_GET['plug']);
      $smarty->assign("p_server"      , $this->Server);
      $smarty->assign("p_servers"     , $this->getServer());
      $smarty->assign("p_serverKeys"  , array_flip($this->getServer()));
      $smarty->assign("p_time"        , $this->Time);
      $smarty->assign("p_times"       , $this->getTimes());
      $smarty->assign("p_timeKeys"    , array_flip($this->getTimes()));
      $smarty->assign("search_for"    , $this->Search);
      $smarty->assign("OrderBy"       , $this->OrderBy);
    }
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }


  /* return selectable server */
  function getServer()
  {
    $ret= array("all"=>_("All"));
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);    
    $ldap->search("(objectClass=goMailServer)",array("cn"));
    while($tmp = $ldap->fetch()){
      $ret[$tmp['cn'][0]]= $tmp['cn'][0];
    }
    return($ret);
  }


  /* Return selectable times*/
  function getTimes()
  {
    $ret = array();
    $ret['nolimit']=_("no limit"); 
    foreach(array(1,2,4,8,12,24,36,48) as $i){
      if($i == 1){
        $ret[$i] = $i."&nbsp;"._("hour");
      }else{
        $ret[$i] = $i."&nbsp;"._("hours");
      }
    }
    return($ret);
  }


  /* Save post values*/
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
    if(isset($_POST['Stat'])){
      $this->Stat = $_POST['Stat'];
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

  /* Return stats */
  function getStats()
  {
    return(array(
          "all"     =>_("All"),
          "hold"    =>_("Hold"),
          "unhold"  =>_("Un hold"),
          "active"  =>_("Active"),
          "nonactive"  =>_("Not active")
          ));
  }

  /* Return plugin informations for acl handling
     #FIXME You can only read attributes within this report plugin */
  static function plInfo()
  {
    return (array(
        "plShortName"   => _("Mailqueue"),
        "plDescription" => _("Mailqueue addon"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 1,
        "plSection"     => array("addon"),
        "plCategory"    => array("mailqueue" => array("objectClass" => "none", "description" => _("Mail queue addon"))),

        "plProvidedAcls" => array(
            "unhold_all"      => _("Unhold all messages"),
            "hold_all"        => _("Hold all messages"),
            "del_all"         => _("Delete all messages"),
            "requeue_all"     => _("Requeue all messages"),
            "unhold"          => _("Unhold message"),
            "hold"            => _("Hold message"),
            "del"             => _("Delete message"),
            "requeue"         => _("Requeue message"),
            "query"           => _("Gathering queue data"),
            "header"          => _("Get header information")
          )
        ));
  }



}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
