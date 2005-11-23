<?php

class mailqueue extends plugin
{
  /* Definitions */
  var $plHeadline= "Mail queue";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $mailQueueScript = "";    

  var $Server         = "all";
  var $Time           = 0;
  var $Search         = "*";
  var $Page           = 0;
  var $EntriesPerPage = 20;

  var $Stat           = "all";
  var $OrderBy        = "Arrival";
  var $SortType       = "up";

  function mailqueue($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    /* get the query cmd */
    $this->mailQueueScript = $this->config->current['MAILQUEUESCRIPTPATH'];    

    $this->Server = "all";
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
    $smarty= get_smarty();

    $error =false;

    if(empty($this->mailQueueScript)){
      print_red(_("Please check your 'gosa.conf', there is no 'MAILQUEUE_QUERY_COMMAND' specified."));
      $error = true;
    }else{

      /* tell smarty to display the search results*/
      $smarty->assign("all_ok"      , "true");

      /* A single server is selected */
      if($this->Server != "all"){    

        /* Create Query cmd */ 
        $q_cmd = preg_replace("/%action/" ,"query"      ,$this->mailQueueScript);
        $q_cmd = preg_replace("/%server/" ,$this->Server,$q_cmd);
        $q_cmd = preg_replace("/%id/"     ,""           ,$q_cmd);

        /* Only display this if the query cmd is executeable */
        if($str = @shell_exec ($q_cmd)){

          /* Parse returned data */
          $mailQueueParser = new parseMailQueue($str,$this->Server);

        }else{

          /* Can't Execute cmd */
          print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$q_cmd));
          $error = true;
        }

      }else{

        $mailQueueParser = NULL;
        foreach($this->getServer() as $ServerID=>$ServerName){
          if($ServerID == "all") continue;

          $q_cmd = preg_replace("/%action/" ,"query"      ,$this->mailQueueScript);
          $q_cmd = preg_replace("/%server/" ,$ServerName  ,$q_cmd);
          $q_cmd = preg_replace("/%id/"     ,""           ,$q_cmd);

          if($str = @shell_exec ($q_cmd)){
            if($mailQueueParser == NULL){
              $mailQueueParser = new parseMailQueue($str,$ServerID);
            }else{
              $mailQueueParser->parseAdditionalQueue($str,$ServerID);
            }
          }else{
            print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$q_cmd));
            $error = true;
          }
        }

      }     

      if(!$error){
        /* Delete entry if requested */
        if((isset($_GET['act']))&&($_GET['act']=="del")){
          if($mailQueueParser->IDExists($_GET['id'])){
            $r_cmd = preg_replace("/%action/" ,"remove"      ,$this->mailQueueScript);
            $r_cmd = preg_replace("/%server/" ,$_GET['server'] ,$r_cmd);
            $r_cmd = preg_replace("/%id/"     ,$_GET['id']  ,$r_cmd);
            if(!shell_exec($r_cmd)){
              print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
            }
          }
        }

        /* Hold entry if requested */
        if((isset($_GET['act']))&&($_GET['act']=="hold")){
          if($mailQueueParser->IDExists($_GET['id'])){
            $r_cmd = preg_replace("/%action/" ,"hold"          ,$this->mailQueueScript);
            $r_cmd = preg_replace("/%server/" ,$_GET['server'] ,$r_cmd);
            $r_cmd = preg_replace("/%id/"     ,$_GET['id']     ,$r_cmd);
            if(!shell_exec($r_cmd)){
              print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
            }
          }
        }

        /* Requeue entry if requested */
        if((isset($_GET['act']))&&($_GET['act']=="requeue")){
          if($mailQueueParser->IDExists($_GET['id'])){
            $r_cmd = preg_replace("/%action/" ,"requeue"       ,$this->mailQueueScript);
            $r_cmd = preg_replace("/%server/" ,$_GET['server'] ,$r_cmd);
            $r_cmd = preg_replace("/%id/"     ,$_GET['id']     ,$r_cmd);
            if(!shell_exec($r_cmd)){
              print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
            }
          }
        }

        $se = $this->getServer();
        unset($se['all']);
        $se_str = "";
        foreach($se as $server) {
          $se_str .= $server." ";
        }

        if(isset($_POST['requeue_all'])){
          $r_cmd = preg_replace("/%action/" , "requeue"       ,$this->mailQueueScript);
          $r_cmd = preg_replace("/%server/" , $se_str         ,$r_cmd);
          $r_cmd = preg_replace("/%id/"     , "all"           ,$r_cmd);
          if(!shell_exec($r_cmd)){
            print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
          }
        }
        if(isset($_POST['hold_all'])){
          $r_cmd = preg_replace("/%action/" , "hold"          ,$this->mailQueueScript);
          $r_cmd = preg_replace("/%server/" , $se_str         ,$r_cmd);
          $r_cmd = preg_replace("/%id/"     , "all"           ,$r_cmd);
          if(!shell_exec($r_cmd)){
            print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
          }

        }
        if(isset($_POST['unhold_all'])){
          $r_cmd = preg_replace("/%action/" , "unhold"          ,$this->mailQueueScript);
          $r_cmd = preg_replace("/%server/" , $se_str         ,$r_cmd);
          $r_cmd = preg_replace("/%id/"     , "all"           ,$r_cmd);
          if(!shell_exec($r_cmd)){
            print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
          }
        }
        if(isset($_POST['remove_all'])){
          $r_cmd = preg_replace("/%action/" , "remove"        ,$this->mailQueueScript);
          $r_cmd = preg_replace("/%server/" , $se_str         ,$r_cmd);
          $r_cmd = preg_replace("/%id/"     , "all"              ,$r_cmd);
          if(!shell_exec($r_cmd)){
            print_red(sprintf(_("Please check your 'gosa.conf' the given '%s' can't be executed."),$r_cmd));
          }
        }

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
        $smarty->assign("entries"       , array_slice($entries,$this->Page,20));
        $smarty->assign("plug"          , "?plug=".$_GET['plug']);
        $smarty->assign("r_stats"         , $this->getStats());
        $smarty->assign("stats"       , array_flip($this->getStats()));
        $smarty->assign("stat"          , $this->Stat);
        $smarty->assign("p_server"      , $this->Server);
        $smarty->assign("p_servers"     , $this->getServer());
        $smarty->assign("p_serverKeys"  , array_flip($this->getServer()));
        $smarty->assign("p_time"        , $this->Time);
        $smarty->assign("p_times"       , $this->getTimes());
        $smarty->assign("p_timeKeys"    , array_flip($this->getTimes()));
        $smarty->assign("search_for"    , $this->Search);
        $smarty->assign("range_selector", range_selector(count($entries), $this->Page, 20)); 
        $smarty->assign("OrderBy"       , $this->OrderBy);

        /* Display sort arrow */
        if($this->SortType == "up"){
          $smarty->assign("SortType","<img src='images/sort_up.png' alt='"._("up")."' border='0'>");
        }else{
          $smarty->assign("SortType","<img src='images/sort_down.png' alt='"._("down")."' border='0'>");
        }
      }
    }

    if($error){  
      $smarty->assign("all_ok"        , "false");
      $smarty->assign("r_stats"         , $this->getStats());
      $smarty->assign("stats"       , array_flip($this->getStats()));
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

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
