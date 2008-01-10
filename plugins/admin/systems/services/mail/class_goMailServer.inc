<?php

class goMailServer extends goService{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports mailqueue listings and so on.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goMailServer");

  /* This class can't be assigned twice so it conflicts with itsself */
  var $DisplayName      = "";
  var $dn               = NULL;
  var $StatusFlag       = "goMailServerStatus";
  var $attributes       = array("description","postfixHeaderSizeLimit",
      "postfixMailboxSizeLimit","postfixMessageSizeLimit",
      "postfixMyDestinations","postfixMyDomain","postfixMyhostname",
      "postfixMyNetworks","postfixRelayhost","postfixTransportTable",
      "postfixSenderRestrictions","postfixRecipientRestrictions");

  var $goMailServerStatus               ;
  var $postfixHeaderSizeLimit           = 0;
  var $postfixMailboxSizeLimit          = 0;
  var $postfixMessageSizeLimit          = 0;
  var $postfixMyDestinations            = array();
  var $postfixMyDomain                  = "";
  var $postfixMyhostname                = "";
  var $postfixMyNetworks                = array();
  var $postfixRelayhost                 = "";
  var $postfixTransportTable            = array();
  var $postfixSenderRestrictions        = array();
  var $postfixRecipientRestrictions     = array();
  var $description                      = "";
  var $RestrictionFilters               = array();
  var $TransportProtocols               = array(); 
  var $Actions                          = array();
  var $cn                               = "";
  var $conflicts                        = array("goMailServer","kolab");
  var $view_logged  =FALSE;

  function goMailServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("Mail smtp service (Postfix)");

    $this->Actions = array( SERVICE_STOPPED=>SERVICE_STOPPED,
                            SERVICE_STARTED => SERVICE_STARTED,
                            SERVICE_RESTARTED=>SERVICE_RESTARTED); 

    /* Fill  RestrictionFilters   TransportProtocols  from external hooks */
    $str = $this->config->data['TABS']['SERVERSERVICE'];
    $this->TransportProtocols =array("smtp"=>"SMTP");
    $this->RestrictionFilters = array("FILTER"=>"FILTER"); 
    foreach(array("ADDITIONALRESTRICTIONFILTERS"=>"RestrictionFilters",
          "ADDITIONALPROTOCOLLS"        =>"TransportProtocols") as $file => $var){
      if(isset($this->config->data['MAIN'][$file])){
        $file = $this->config->data['MAIN'][$file];
        if((isset($file)) && is_readable($file)){
          $tmp = file_get_contents($file);
          $tmp2= split("\n",$tmp);
          foreach($tmp2 as $entry){
            if(empty($entry)) continue;
            if(preg_match("/:/",$entry)){
              $tmp3 = split(":",$entry);
              $r = $this->$var;
              $r[$tmp3[0]]=$tmp3[1];
              $this->$var = $r;
            }else{
              $r = $this->$var;
              $r[$entry] =$entry;
              $this->$var = $r;
            }
          }
        }
      }
    }


    /* Get postfix my networks */
    $this->postfixMyNetworks = array();
    $tmp = array();
    if(isset($this->attrs['postfixMyNetworks'][0])){
      $tmp = split(",",$this->attrs['postfixMyNetworks'][0]);
      foreach($tmp as $str){
        if(!empty($str)){
          $this->postfixMyNetworks[base64_encode($str)] = $str;
        }
      }
    }


    /* Create full name */
    if(isset($this->attrs['postfixMyDomain'][0])){
      $this->postfixMyhostname .= ".".$this->attrs['postfixMyDomain'][0];
    }
  

    /* Get postfix my domains */
    $this->postfixMyDestinations = array(); 
    if(isset($this->attrs['postfixMyDestinations'][0])){
      unset($this->attrs['postfixMyDestinations']['count']);
      foreach($this->attrs['postfixMyDestinations'] as $str){
        $this->postfixMyDestinations[base64_encode($str)] = $str;
      }
    }


    /* Get transport tables */
    $tmp = array();
    $this->postfixTransportTable = array();
    if(isset($this->attrs['postfixTransportTable'])){
      $tmp = array();
      unset($this->attrs['postfixTransportTable']['count']);
      foreach($this->attrs['postfixTransportTable'] as $entry){

        //0: offshore.vip.ms-europa.lhsystems.com smtp:172.28.0.2

        $Number   = preg_replace('/^([^:]+):.*$/', '\\1', $entry);
        $Rest     = trim(preg_replace("/^[0-9]*:/","",$entry));

        $Protocol_Destination = preg_replace("/^.*\ /","",$Rest);
        $Source               = preg_replace("/\ .*$/","",$Rest);

        $Protocol     = preg_replace ('/^([^:]+):.*$/', '\\1' ,trim($Protocol_Destination));
        $Destination  = preg_replace ('/^[^:]+:(.*)$/', '\\1'    ,trim($Protocol_Destination));

        $Destination  = preg_replace ("/[\[\]]/","",$Destination);

        $tmp[$Number]['src'] = $Source;
        $tmp[$Number]['dst'] = $Destination;
        $tmp[$Number]['prt'] = $Protocol;
      }
      ksort($tmp);
      foreach($tmp as $entry){
        $this->postfixTransportTable[] = $entry;
      }
    }
   

    /* Get sender restrictions */
    $tmp = array();
    $this->postfixSenderRestrictions = array();
    if(isset($this->attrs['postfixSenderRestrictions'])){
      unset($this->attrs['postfixSenderRestrictions']['count']);
      foreach($this->attrs['postfixSenderRestrictions'] as $entry){
        $nr  = preg_replace("/:.*$/","",$entry);
        $rest= trim(preg_replace("/^[^:]+:/","",$entry));
        $src = preg_replace("/ .*$/","",$rest);
        $rest= preg_replace("/^[^ ]+ /","",$rest);
        $dst = preg_replace("/^.* /","",$rest);
        $prt = preg_replace("/ .*$/","",$rest);
 
        $tmp[$nr]['src'] = $src;
        $tmp[$nr]['dst'] = $dst;
        $tmp[$nr]['filter'] = $prt;
      }
      ksort($tmp);
      foreach($tmp as $entry){
        $this->postfixSenderRestrictions[] = $entry;
      }
    }

 
    /* Get sender restrictions */
    $tmp = array();
    $this->postfixRecipientRestrictions = array();
    if(isset($this->attrs['postfixRecipientRestrictions'])){
      unset($this->attrs['postfixRecipientRestrictions']['count']);
      foreach($this->attrs['postfixRecipientRestrictions'] as $entry){
        $nr  = preg_replace("/:.*$/","",$entry);
        $rest= trim(preg_replace("/^[^:]+:/","",$entry));
        $src = preg_replace("/ .*$/","",$rest);
        $rest= preg_replace("/^[^ ]+ /","",$rest);
        $dst = preg_replace("/^.* /","",$rest);
        $prt = preg_replace("/ .*$/","",$rest);
 
        $tmp[$nr]['src'] = $src;
        $tmp[$nr]['dst'] = $dst;
        $tmp[$nr]['filter'] = $prt;
      }
      ksort($tmp);
      foreach($tmp as $entry){
        $this->postfixRecipientRestrictions[] = $entry;
      }
    }

  }

  function execute()
  { 
    $smarty   = get_smarty();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $delAr    = array( 
                    "TranslationDel_"=>"TranslationDel",
                    "SenderRestrictDel_"=>"SenderRestrictDel",
                    "RecipientRestrictDel_"=>"RecipientRestrictDel");
 
    $once     = true;
    $s_action = "";
    $s_entry  = "";
  
    /* Check posts for some intruductions */
    foreach($_POST as $name => $value){
      foreach($delAr as $preg => $type){
        if((preg_match("/^".$preg."/",$name)) && ($once)){
          $once = false;
          $s_action = $type; 
          $s_entry = preg_replace("/^".$preg."/","",$name);
          $s_entry = preg_replace("/_[xy]$/","",$s_entry);
        }
      }
  

      if(preg_match("/^TranslationUp_/",$name) && $once && ($this->acl_is_writeablei("postfixTransportTableACL"))){
        $once = false;
        $key = preg_replace("/^TranslationUp_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixTransportTable = $this->ArrayUp($key,$this->postfixTransportTable) ;
      }
      if(preg_match("/^TranslationDown_/",$name) && $once && ($this->acl_is_writeable("postfixTransportTableACL"))){
        $once = false;
        $key = preg_replace("/^TranslationDown_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixTransportTable = $this->ArrayDown($key,$this->postfixTransportTable) ;
      }
      if(preg_match("/^SenderRestrictUp_/",$name) && $once && ($this->acl_is_writeable("postfixSenderRestrictionsACL"))){
        $once = false;
        $key = preg_replace("/^SenderRestrictUp_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixSenderRestrictions = $this->ArrayUp($key,$this->postfixSenderRestrictions) ;
      }
      if(preg_match("/^SenderRestrictDown_/",$name) && $once && ($this->acl_is_writeable("postfixSenderRestrictionsACL"))){
        $once = false;
        $key = preg_replace("/^SenderRestrictDown_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixSenderRestrictions = $this->ArrayDown($key,$this->postfixSenderRestrictions) ;
      }
      if(preg_match("/^RecipientRestrictUp_/",$name) && $once && ($this->acl_is_writeable("postfixRecipientRestrictionsACL"))){
        $once = false;
        $key = preg_replace("/^RecipientRestrictUp_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixRecipientRestrictions = $this->ArrayUp($key,$this->postfixRecipientRestrictions) ;
      }
      if(preg_match("/^RecipientRestrictDown_/",$name) && $once && ($this->acl_is_writeable("postfixRecipientRestrictionsACL"))){
        $once = false;
        $key = preg_replace("/^RecipientRestrictDown_/","",$name);
        $key = preg_replace("/_[xy]$/","",$key);
        $this->postfixRecipientRestrictions = $this->ArrayDown($key,$this->postfixRecipientRestrictions) ;
      }
    }


    /* Add delete my network entry */
    if((isset($_POST['AddpostfixMyNetworks'])) && (!empty($_POST['NewString_postfixMyNetworks']))){
      $str = $_POST['NewString_postfixMyNetworks'];
      $this->postfixMyNetworks[base64_encode($str)] = $str;
    }

    if((isset($_POST['DelpostfixMyNetworks'])) && isset($_POST['Select_postfixMyNetworks']) &&(count($_POST['Select_postfixMyNetworks']))){
      foreach($_POST['Select_postfixMyNetworks'] as $str ){
        unset($this->postfixMyNetworks[$str]);
      }
    }


    /* Add delete my domain entry */
    if($this->acl_is_writeable("postfixMyDestinations")){
      if((isset($_POST['AddpostfixMyDestinations'])) && (!empty($_POST['NewString_postfixMyDestinations']))){
        $str = $_POST['NewString_postfixMyDestinations'];
        $this->postfixMyDestinations[base64_encode($str)] = $str;
      }

      if((isset($_POST['DelpostfixMyDestinations'])) && isset($_POST['Select_postfixMyDestinations']) &&(count($_POST['Select_postfixMyDestinations']))){
        foreach($_POST['Select_postfixMyDestinations'] as $str ){
          unset($this->postfixMyDestinations[$str]);
        }
      }
    }


    /* Add sender restriction */
    if($this->acl_is_writeable("postfixSenderRestrictions")){
      if(($s_action == "SenderRestrictDel") && (isset($this->postfixSenderRestrictions[$s_entry]))){
        unset($this->postfixSenderRestrictions[$s_entry]);
      }

      if(isset($_POST['AddpostfixSenderRestrictions'])){
        $src      = $_POST['Source_postfixSenderRestrictions'];
        $dst      = $_POST['Destination_postfixSenderRestrictions'];
        $Filter   = $_POST['SenderRestrictionFilter'];
        $tmp['src']     = $src;
        $tmp['dst']     = $dst;
        $tmp['filter']  = $Filter;
        $this->postfixSenderRestrictions[] = $tmp;
      }
    }


    /* Add sender restriction */
    if($this->acl_is_writeable("postfixRecipientRestrictions")){
      if(($s_action == "RecipientRestrictDel") && (isset($this->postfixRecipientRestrictions[$s_entry]))){
        unset($this->postfixRecipientRestrictions[$s_entry]);
      }

      if(isset($_POST['AddpostfixRecipientRestrictions'])){
        $src      = $_POST['Source_postfixRecipientRestrictions'];
        $dst      = $_POST['Destination_postfixRecipientRestrictions'];
        $Filter   = $_POST['RecipientRestrictionFilter'];
        $tmp['src']     = $src;
        $tmp['dst']     = $dst;
        $tmp['filter']  = $Filter;
        $this->postfixRecipientRestrictions[] = $tmp;
      }
    }

    /* Handle transports */
    if($this->acl_is_writeable("postfixTransportTable")){
      if(($s_action == "TranslationDel") && (isset($this->postfixTransportTable[$s_entry]))){
        unset($this->postfixTransportTable[$s_entry]);
      }

      if(isset($_POST['AddpostfixTransportTable'])){
        $src = trim($_POST['Source_postfixTransportTable']);  
        $dst = trim($_POST['Destination_postfixTransportTable']);    
        $prt = trim($_POST['TransportProtocol']);

        if((!empty($src)) && (!empty($dst))){
          if(preg_match("/:/",$dst)){
            $tmp = split("\:",$dst);
            $port = trim($tmp[1]);
            $ip   = trim($tmp[0]);

            if((tests::is_ip($ip)) && (is_numeric($port))){
              $dst = "[".$ip."]:".$port;
            }
          }
          if(tests::is_ip($dst)){
            $dst = "[".$dst."]";
          }
          $tmp2 ['src'] = $src;
          $tmp2 ['dst'] = $dst;
          $tmp2 ['prt'] = $prt;

          $this->postfixTransportTable[] = $tmp2;  
        } 
      }
    }

    /* Set attributes */
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }


    /* Create divList for translation tables */
    $divTranslation = new divSelectBox("TransportProtocols");
    $divTranslation->SetHeight(90);
    foreach($this->postfixTransportTable as $key => $entry){
      $img = "";

      if($key != 0 && $this->acl_is_writeable("postfixTransportTable")){
        $img.= "<input type='image' src='images/sort_up.png' name='TranslationUp_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }
      if(($key+1) < count($this->postfixTransportTable) && $this->acl_is_writeable("postfixTransportTable")){
        $img.= "<input type='image' src='images/sort_down.png' name='TranslationDown_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }

      if($this->acl_is_writeable("postfixTransportTable")){
        $img.= "<input type='image' src='images/edittrash.png' name='TranslationDel_".$key."' class='center'>&nbsp;";      
      }
      $field1 = array("string"=> $entry['src']);
      $field2 = array("string"=> $entry['dst']);
      $field3 = array("string"=> $entry['prt'],"attach"=>"style='width:120px;'");
      $field4 = array("string"=> $img, "attach"=>"style='border-right:0px;width:40px;'");
      $divTranslation->AddEntry(array($field1,$field2,$field3,$field4,));
    }
    $smarty->assign("Div_postfixTransportTable" ,$divTranslation->DrawList());


    /* Create divList for sender restrictions */
    $DivSenderRestrict = new divSelectBox("postfixSenderRestrictions");
    $DivSenderRestrict->SetHeight(90);
    foreach($this->postfixSenderRestrictions as $key => $entry){
      $img ="";

      if($key != 0 && $this->acl_is_writeable("postfixSenderRestrictions")){
        $img.= "<input type='image' src='images/sort_up.png' name='SenderRestrictUp_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }
      if(($key+1) < count($this->postfixSenderRestrictions) && $this->acl_is_writeable("postfixSenderRestrictions")){
        $img.= "<input type='image' src='images/sort_down.png' name='SenderRestrictDown_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }

      if($this->acl_is_writeable("postfixSenderRestrictions")){
        $img.= "<input type='image' src='images/edittrash.png' name='SenderRestrictDel_".$key."' class='center'>&nbsp;";     
      }
 
      $field1 = array("string"=> $entry['src']);
      $field2 = array("string"=> $entry['dst']);
      $field3 = array("string"=> $entry['filter'],"attach"=>"style='width:100px;'");
      $field4 = array("string"=> $img, "attach"=>"style='border-right:0px;width:40px;'");
      $DivSenderRestrict->AddEntry(array($field1,$field2,$field3,$field4,));
    }
    $smarty->assign("Div_postfixSenderRestrictions" ,$DivSenderRestrict->DrawList());


    /* Create divList for translation tables */
    $DivRecipientRestrict = new divSelectBox("postfixRecipientRestrictions");
    $DivRecipientRestrict->SetHeight(90);

    foreach($this->postfixRecipientRestrictions as $key => $entry){
      $img = "";
      if($key != 0 && $this->acl_is_writeable("postfixRecipientRestrictions")){
        $img.= "<input type='image' src='images/sort_up.png' name='RecipientRestrictUp_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }
      if(($key+1) < count($this->postfixRecipientRestrictions) && $this->acl_is_writeable("postfixRecipientRestrictions")){
        $img.= "<input type='image' src='images/sort_down.png' name='RecipientRestrictDown_".$key."' class='center'>&nbsp;";      
      }else{
        $img.= "<img src='images/empty.png' style='width:10px;'>";
      }

      if($this->acl_is_writeable("postfixRecipientRestrictions")){
        $img.= "<input type='image' src='images/edittrash.png' name='RecipientRestrictDel_".$key."' class='center'>&nbsp;";      
      }
      $field1 = array("string"=> $entry['src']);
      $field2 = array("string"=> $entry['dst']);
      $field3 = array("string"=> $entry['filter'],"attach"=>"style='width:100px;'");
      $field4 = array("string"=> $img, "attach"=>"style='border-right:0px;width:40px;'");
      $DivRecipientRestrict->AddEntry(array($field1,$field2,$field3,$field4,));
    }
    $smarty->assign("Div_postfixRecipientRestrictions" ,$DivRecipientRestrict->DrawList());

    
    /* set new status */
    if(isset($_POST['ExecAction'])){
      if(isset($this->Actions[$_POST['action']])){
        $this->setStatus($_POST['action']);
      }
    }


    $smarty->assign("is_new",                       $this->dn);
    $smarty->assign("is_acc",                       $this->initially_was_account);
    $smarty->assign("TransportProtocols",           $this->TransportProtocols);
    $smarty->assign("Actions",                      $this->Actions);
    $smarty->assign("RestrictionFilters",           $this->RestrictionFilters);
    $smarty->assign("postfixTransportTable" ,       $this->getTransports());
    $smarty->assign("postfixSenderRestrictions" ,   $this->getSenderRestrictions());
    $smarty->assign("postfixRecipientRestrictions" ,$this->getRecipientRestrictions());

    return($smarty->fetch(get_template_path("goMailServer.tpl",TRUE,dirname(__FILE__))));
  }

 
  /* return transports formated for select box */ 
  function getTransports()
  {
    $ret = array();
    foreach($this->postfixTransportTable as $key => $vals){
      $ret[$key] = $vals['src']." -> ".$vals['prt'].":".$vals['dst'];
    }
    return($ret);
  }


  /* return sender restriction formated for select box */ 
  function getSenderRestrictions()
  {
    $ret = array();
    foreach($this->postfixSenderRestrictions as $key => $vals){
      $ret[$key] = $vals['src']." ".$vals['filter']." ".$vals['dst'];
    }
    return($ret);
  }


  /* return recipient restriction formated for select box */ 
  function getRecipientRestrictions()
  {
    $ret = array();
    foreach($this->postfixRecipientRestrictions as $key => $vals){
      $ret[$key] = $vals['src']." ".$vals['filter']." ".$vals['dst'];
    }
    return($ret);
  }


  /* Return list entry */
  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("Mail smtp service (Postfix)");
    $fields['AllowEdit']  = true;
    return($fields);
  }


  function save()
  {
    $this->postfixMyDomain   = preg_replace("/^[^\.]+\./","",$this->postfixMyhostname);
    $this->postfixMyhostname = preg_replace("/\..*$/","",$this->postfixMyhostname);

    plugin::save();

    /* Fix transport table*/
    $i = 0 ; 
    $this->attrs['postfixTransportTable'] = array();
    foreach($this->postfixTransportTable as $key => $entry){
      $this->attrs['postfixTransportTable'][] = $i.": ".$entry['src']." ".$entry['prt'].":".$entry['dst'];
      $i ++;
    }


    /* Fix sender restrictions */
    $i = 0;
    $this->attrs['postfixSenderRestrictions'] =array();
    foreach($this->postfixSenderRestrictions as $key => $entry){
      $this->attrs['postfixSenderRestrictions'][] = $i.": ".$entry['src']." ".$entry['filter']." ".$entry['dst']; 
      $i ++;  
    }
  
  
    /* Fix recipient restrictions */
    $i = 0;
    $this->attrs['postfixRecipientRestrictions'] =array();
    foreach($this->postfixRecipientRestrictions as $key => $entry){
      $this->attrs['postfixRecipientRestrictions'][] = $i.": ".$entry['src']." ".$entry['filter']." ".$entry['dst']; 
      $i ++;  
    }


    /* Fix mydomains */
    $this->attrs['postfixMyDestinations']  = array();
    foreach($this->postfixMyDestinations as $entry){
      $this->attrs['postfixMyDestinations'][] =$entry;
    }


    /* Fix mydomains */
    if(count($this->postfixMyNetworks)){
      $this->attrs['postfixMyNetworks']  = "";
      foreach($this->postfixMyNetworks as $entry){
        $this->attrs['postfixMyNetworks'] .=$entry.",";
      }
      $this->attrs['postfixMyNetworks'] = preg_replace("/,$/","",$this->attrs['postfixMyNetworks']);
    }else{
      $this->attrs['postfixMyNetworks']  = array();
    }

 
    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving server services/mail with dn '%s' failed."),$this->dn));
    if($this->initially_was_account){
      $this->handle_post_events("modify");
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
    }else{
      $this->handle_post_events("add");
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
    }

  }


  function check()
  { 
    $message =plugin::check();
  
    if(!is_numeric($this->postfixHeaderSizeLimit)){
      $message[] = _("Please specify a numeric value for header size limit.");
    }

    if(!is_numeric($this->postfixMailboxSizeLimit)){
      $message[] = _("Please specify a numeric value for mailbox size limit.");
    }

    if(!is_numeric($this->postfixMessageSizeLimit)){
      $message[] = _("Please specify a numeric value for message size limit.");
    }

    return $message;
  }


  /* Combine new array */
  function combineArrays($ar0,$ar1,$ar2)
  {
    $ret = array();
    if(is_array($ar0))
    foreach($ar0 as $ar => $a){
        $ret[]=$a;
    }
    if(is_array($ar1))
    foreach($ar1 as $ar => $a){
        $ret[]=$a;
    }
    if(is_array($ar2))
    foreach($ar2 as $ar => $a){
        $ret[]=$a;
    }
    return($ret);
  }


  function getpos($atr,$attrs)
  {
    $i = 0;
    foreach($attrs as $attr => $name)    {
      $i++;
      if($attr == $atr){
        return($i);
      }
    }

    return(-1);
  }


  /* TRansports the geiven Arraykey one position up*/
  function ArrayUp($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == 1))){
      $before = array_slice($attrs,0,($pos-2));
      $mitte  = array_reverse(array_slice($attrs,($pos-2),2));
      $unten  = array_slice($attrs,$pos);
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* TRansports the geiven Arraykey one position up*/
  function ArrayDown($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == $cn))){
      $before = array_slice($attrs,0,($pos-1));
      $mitte  = array_reverse(array_slice($attrs,($pos-1),2));
      $unten  = array_slice($attrs,($pos+1));
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }

  function save_object()
  {
    plugin::save_object();  
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $source_o = new goMailServer($this->config,$source['dn']);
    foreach(array("postfixMyDomain","postfixMyhostname","postfixMyNetworks","postfixTransportTable","postfixSenderRestrictions","postfixRecipientRestrictions","postfixMyDestinations") as $attr){
      $this->$attr = $source_o->$attr;
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Mail smtp (Postfix)"),
          "plDescription" => _("Mail smtp - Postfix")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 98,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "postfixMyhostname"           => _("Visible full qualified hostname"),
            "description"                 => _("Description"), 
            "postfixHeaderSizeLimit"      => _("Header size limit"), 
            "postfixMailboxSizeLimit"     => _("Max mailbox size"), 
            "postfixMessageSizeLimit"     => _("Max message size"), 
            "postfixMyDestinations"       => _("Domains to accept mail for"), 
            "postfixMyNetworks"           => _("Local networks"), 
            "postfixRelayhost"            => _("Relay host"), 
            "postfixTransportTable"       => _("Transport table"), 
            "postfixSenderRestrictions"   => _("Restrictions for sender"), 
            "postfixRecipientRestrictions"=> _("Restrictions for recipient"))
            ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
