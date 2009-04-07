<?php

class servapacheVhostEdit extends plugin
{
  /* attribute list for save action */
  var $ignore_account= TRUE;
  
  var $attributes	= array("apacheServerName","apacheDocumentRoot","apacheServerAlias",
  	"apacheServerAdmin","apacheScriptAlias","apacheSuexecUid","apacheSuexecGid");
      
  var $objectclasses  = array("whatever");

  
  var $apacheServerName		= "";
  var $apacheDocumentRoot	= "";
  var $apacheServerAdmin	= "";
  var $apacheSuexecUid		= "";
  var $apacheSuexecGid		= "";
    
//  var $apacheServerAlias                = array();
  var $apacheServerAlias	= ""; 
  var $apacheScriptAlias                = array();

  var $OldApacheServerName              = ""; // To detect changes made with this edit
  var $InitialApacheServerName          = "";

  var $Records                  = array();
  
  var $dialog                   = false;

  var $isNew                    = true;
  var $cn;
  var $VhostObject               = array();

  function servapacheVhostEdit ($config, $dn= NULL,$attrs = array())
  {
    plugin::plugin ($config, $dn);
 
    if(!count($attrs)){
      $this->OldApacheServerName        = "";
      $this->isNew              = true;
      
      $this->InitialApacheServerName    = "";//$attrs['InitialApacheServerName'];
    }else{
      $this->VhostObject         = $attrs;

      $this->OldApacheServerName        = $attrs['apacheServerName'];

      $this->InitialApacheServerName    = $attrs['InitialApacheServerName'];

      $this->isNew                  = false;

      foreach($this->attributes as $value){
        if(isset($attrs[$value])){
          $this->$value = $attrs[$value];
        }
      }

//      if(!isset($attrs['apacheServerAlias'])) $this->apacheServerAlias  = array();
      if(!isset($attrs['apacheScriptAlias'])) $this->apacheScriptAlias  = array();

    }
  }

  /* Transports the given Arraykey one position up*/
  function ArrayUp($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $atr ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == 1)||($pos >$cn))){
      $before = array_slice($attrs,0,($pos-2));
      $mitte  = array_reverse(array_slice($attrs,($pos-2),2));
      $unten  = array_slice($attrs,$pos);
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* Transports the given Arraykey one position down*/
  function ArrayDown($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $atr ;
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


  function execute()
  {
    /* Call parent execute */
    plugin::execute();


    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Open Vhost Entry Edit Dialog
     */
    if(!count($this->VhostObject)){
      $smarty->assign("AllowVhostEdit" , false);
    }else{
      $smarty->assign("AllowVhostEdit" , true);
      if(isset($_POST['EditVhostEntries'])){
        $this->dialog= new apacheVhostEditEntries($this->config,$this->dn,$this->VhostObject);
      }
    }

    /* Save Vhost Entry Edit Dialog
     */
    if(isset($_POST['SaveVhostEntryChanges'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        $msgs = $this->dialog->check();
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $this->dialog->save();
        $this->dialog = false;
      }
    }

    /* Cancel Vhost Entrie Edit Dialog
    */
    if(isset($_POST['CancelVhostEntryChanges'])){
      $this->dialog = false;
    }

    /* Display any type of open dialogs 
     */
    if($this->dialog){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    $once =true;
    foreach($_POST as $name => $value){
      if((preg_match("/^SAup_/",$name)) && ($once)){
        $once = false;

        $id = preg_replace("/^SAup_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
    
        $this->apacheServerAlias = $this->ArrayUp(($id+1),$this->apacheServerAlias);
      }
      if((preg_match("/^SAdown_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^SAdown_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
  
        $this->apacheServerAlias = $this->ArrayDown(($id+1),$this->apacheServerAlias);
      }
      if((preg_match("/^SAdel_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^SAdel_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
        
        unset($this->apacheServerAlias[$id]);

        $tmp  =array();
        foreach($this->apacheServerAlias as $entry){
          $tmp[] = $entry;
        }
 
        $this->apacheServerAlias = $tmp; 
      }

      if((preg_match("/^SCup_/",$name)) && ($once)){
        $once = false;

        $id = preg_replace("/^SCup_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
    
        $this->apacheScriptAlias = $this->ArrayUp(($id+1),$this->apacheScriptAlias);
      }
      if((preg_match("/^SCdown_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^SCdown_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
  
        $this->apacheScriptAlias = $this->ArrayDown(($id+1),$this->apacheScriptAlias);
      }
      if((preg_match("/^SCdel_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^SCdel_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
        
        unset($this->apacheScriptAlias[$id]);

        $tmp  =array();
        foreach($this->apacheScriptAlias as $entry){
          $tmp[] = $entry;
        }
 
        $this->apacheScriptAlias = $tmp; 
      }
      
    }

    if((isset($_POST['AddSARecord'])) && (!empty($_POST['StrSAAlias'])) && (!empty($_POST['StrSADir']))){
      $this->apacheServerAlias[] = trim($_POST['StrSAAlias']." ".$_POST['StrSADir']);      
    }


    
  	if((isset($_POST['AddSCRecord'])) && (!empty($_POST['StrSCAlias'])) && (!empty($_POST['StrSCDir']))){
      $this->apacheScriptAlias[] = trim($_POST['StrSCAlias']." ".$_POST['StrSCDir']);      
    }

    /* Handle Post events */
    $once = true;
    foreach($_POST as $name => $value){

      /* Delete vhosts if requested */
      if((preg_match("/RemoveRecord_/",$name))&&($once)){
        $once = false;
        $id= preg_replace("/RemoveRecord_/","",$name);
        unset($this->Records[$id]);
      }
    }

    /* Add new Vhostrecord */
 /*   if(isset($_POST['AddNewRecord'])){
      $this->Records[] = array("type"=>"aRecord","value"=>"");
    }*/

    /* Fill in values */
    foreach($this->attributes as $name){
      $smarty->assign($name,$this->$name);
    }

    /* Set apacheServerNames without server suffix */
      $smarty->assign("apacheServerName",$this->apacheServerName);

    $div = new DivSelectBox("apacheServerAlias");
    $div->setHeight(120);
    $recs = $this->apacheServerAlias;

    $oneup    = "<input name='SAup_%s'    type='image' src='images/sort_up.png'    title='"._("Up")."'      class='center'>&nbsp;"; 
    $onedown  = "<input name='SAdown_%s'  type='image' src='images/sort_down.png'  title='"._("Down")."'    class='center'>&nbsp;"; 
    $onedel   = "<img src='images/empty.png' width='20' class='center'>
                 <input name='SAdel_%s'   type='image' src='images/edittrash.png'  title='"._("Delete")."'  class='center'>";
    foreach($recs as $key => $rec){
      $div ->AddEntry(array(
            array("string"=>$rec),
/*            array("string"=>$key,
                  "attach"=>"style='width:20px;'"),*/
            array("string"=>str_replace("%s",base64_encode($key),$oneup.$onedown.$onedel),
                  "attach"=>"style='width:70px;border-right:0px;'")
            ));
    }

      $smarty->assign("NotNew", false);

    $smarty->assign("apacheServerAlias",  $div->DrawList());

    $div = new DivSelectBox("apacheScriptAlias");
    $div->setHeight(120);
    $recs = $this->apacheScriptAlias;

    $oneup    = "<input name='SCup_%s'    type='image' src='images/sort_up.png'    title='"._("Up")."'      class='center'>&nbsp;"; 
    $onedown  = "<input name='SCdown_%s'  type='image' src='images/sort_down.png'  title='"._("Down")."'    class='center'>&nbsp;"; 
    $onedel   = "<img src='images/empty.png' width='20' class='center'>
                 <input name='SCdel_%s'   type='image' src='images/edittrash.png'  title='"._("Delete")."'  class='center'>"; 
    
    foreach($recs as $key => $rec){
      $div ->AddEntry(array(
            array("string"=>$rec),
/*            array("string"=>$key,
                  "attach"=>"style='width:20px;'"),*/
            array("string"=>str_replace("%s",base64_encode($key),$oneup.$onedown.$onedel),
                  "attach"=>"style='width:70px;border-right:0px;'")
            ));
    }

      $smarty->assign("NotNew", false);

    $smarty->assign("apacheScriptAlias",  $div->DrawList());    
    
    /* Display template */
    $display.= $smarty->fetch(get_template_path('servApacheVhostEdit.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
  }

  /* Save data to object */
  function save_object()
  {
    //plugin::save_object();
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
        
    /* Check if apacheServerName is already in use */
    $usedVhosts = $this->getUsedServerNames();
    if(($this->isNew == true)||($this->apacheServerName  != $this->InitialApacheServerName)){
/*      if((isset($usedVhosts[$this->apacheServerName]))&&($this->apacheServerName  != $this->InitialApacheServerName)){
        $message[] =_("This apacheServerName is already in use");
      }
*/
    }

    if(!GetVhostsDomain($this->apacheServerName) || empty($this->apacheServerName)){
      $message[] =sprintf(_("Please choose a valid Virtual Host Name."));
    }

    if(!is_path($this->apacheDocumentRoot) || empty($this->apacheDocumentRoot)){
      $message[] = _("Please choose a valid Path as Virtual Host Document Root.");
    }
    
  	if(!is_email($this->apacheServerAdmin) || empty($this->apacheServerAdmin)){
      $message[] = _("Please choose a valid Email Account as Admin Mail address.");
    }

    if($this->apacheServerName != strtolower($this->apacheServerName)){
      $message[] = _("Only lowercase strings are allowed as Virtual Host Name.");
    }
    
  	if($this->apacheDocumentRoot != strtolower($this->apacheDocumentRoot)){
      $message[] = _("Only lowercase strings are allowed as Virtual Host Document Root.");
    }
    
  	if($this->apacheServerAdmin != strtolower($this->apacheServerAdmin)){
      $message[] = _("Only lowercase strings are allowed as Admin Mail address.");
    }

    foreach($this->apacheServerAlias as $key => $line){
		$apacheServerAlias_ar=split(" ",$line);
		$url=$apacheServerAlias_ar[0];
      	if(!is_path($url)){
        	$message[] = sprintf(_("Please choose a valid Path as URL Alias Path in line: %s"),$line);
      	}
    }
    
  	foreach($this->apacheScriptAlias as $key => $line){
		$apacheScriptAlias_ar=split(" ",$line);
		$url=$apacheScriptAlias_ar[0];
      	if(!is_path($url)){
        	$message[] = sprintf(_("Please choose a valid Path as URL Alias Path in line: %s"),$line);
      	}
    }
    
    return ($message);
  }

  /* This funtion returns all used apacheServerNames */
  function getUsedServerNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=apacheConfig)(apacheServerName=*))",array("apacheServerName"));
    while($attr = $ldap->fetch()){
        $ret[$attr['apacheServerName'][0]]="";
    }
    return($ret);
  }

  /* Save to LDAP */
  function save()
  {
    $ret =array();
    foreach($this->attributes as $name){
      $ret[$name] = $this->$name;
    }

    /* Create SA records 
     */
    foreach($this->apacheServerAlias as $key => $rec){
      $rec['value']= $key." ".$rec['value'];
      $this->Records [] = $rec;
    }
    
  /* Create SC records 
     */
    foreach($this->apacheScriptAlias as $key => $rec){
      $rec['value']= $key." ".$rec['value'];
      $this->Records [] = $rec;
    }

    $ret['RECORDS'] = $this->Records; 

    $ret['InitialApacheServerName']   =  $this->InitialApacheServerName;

    return($ret);
  }

}

?>
