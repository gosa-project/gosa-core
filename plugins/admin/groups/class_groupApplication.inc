<?php
class appgroup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage application groups";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Appgroup attributes */
  var $gosaMemberApplication= array();

  /* Helpers */
  var $departments        = "";       // All departments within $this->curbase;
  var $apps               = array();  // All available applications
  var $used_apps          = array();  // Specifies which applications are currently assigned 

  var $option_name          = array();
  var $option_value         = array();
  var $appoption            = array();

  var $table                  = "";
  var $curbase                = "";
  var $curCatDir                  ;
  var $curCatDepth              =0;         //

  /* attribute list for save action */
  var $attributes               = array("gosaMemberApplication");
  var $objectclasses            = array("gosaApplicationGroup");

  var $Release                  = "/";
  var $enableReleaseManagement  = false;


  function appgroup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Load member applications */
    if (isset ($this->attrs["gosaMemberApplication"][0])){
      $this->gosaMemberApplication = array();
      for ($i= 0; $i<$this->attrs["gosaMemberApplication"]["count"]; $i++){
        $this->gosaMemberApplication[]=
          $this->attrs["gosaMemberApplication"][$i];
      }
    }

    /* Load application options */
    if (isset($this->attrs['gosaApplicationParameter'])){
      for ($i= 0; $i<$this->attrs['gosaApplicationParameter']['count']; $i++){
        $option= preg_replace('/^[^:]+:/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $name= preg_replace('/:.*$/', '',
            $this->attrs['gosaApplicationParameter'][$i]);
        $this->appoption[$name]= $option;
      }
    }

    /* Parse MemberApplication*/
    $tmp = array();
    $tmp2 = array();
    $prios = array();
    $cats = array();

    foreach($this->gosaMemberApplication as $memberApp){
      if(preg_match("/\|/i",$memberApp)){
    
        $tmp = split("\|",$memberApp);
 
        if(!empty($tmp[0])){
          $tmp2[$tmp[1]][$tmp[2]] = array("App"=>$tmp[0]);
        }
        if(!empty($tmp[1])){
          $n = split("/",$tmp[1]);
          $c = count($n);
          $cats [$tmp[1]] = $n[$c-1];
        }
        $prios[$tmp[1]][$tmp[2]] = $tmp[2];
      }else{
        $tmp2[]['App'] = $memberApp;
      }
    }

    $this->Categories = $cats;

    $this->gosaMemberApplication = $tmp2;
    $cats[""]="";
    foreach($cats as $cat ){
      if((isset($prios[$cat]))&&(count($prios[$cat]))){
        $max = max($prios[$cat]);
        $min = 1;//min($prios[$cat]);  
        $last = false;
        for($i = $min ; $i < $max ; $i++){
          if(!isset($prios[$cat][$i])){
            if($last == false){
              $this->gosaMemberApplication[$cat][$i]['App'] = "__SEPARATOR__".$i;
            
              $last = true;
            }
          }else{
            $last = false;
          }
        }
      }
    }

    $tmp = array();
    foreach($this->gosaMemberApplication as $key =>  $entries){
      ksort ($entries);
      foreach($entries as $entry){
        $tmp[$key][]= $entry;
      }
    }
    $this->gosaMemberApplication = $tmp;
    $this->curbase = $this->config->current['BASE'];
    
    /* Check if we should enable the release selection 
     */
    $tmp = search_config($config->data,"faiManagement","CLASS");
    if(!empty($tmp)){
      $this->enableReleaseManagement = true;
      $this->Release = $this->getReleaseName();
    }
  }


  /* Combine new array */
  function combineArrays($ar0,$ar1,$ar2)
  {
    $ret = array();
    if(is_array($ar0))
    foreach($ar0 as $ar => $a){
        $ret[$ar]=$a;
    }
    if(is_array($ar1))
    foreach($ar1 as $ar => $a){
        $ret[$ar]=$a;
    }
    if(is_array($ar2))
    foreach($ar2 as $ar => $a){
        $ret[$ar]=$a;
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


  function catUp($id)
  {
    /* Get all cats depinding on current dir */
    $cats = $this->GetSubdirs($this->curCatDir);
    $newcats =$this->ArrayUp($id,$cats);

    foreach($newcats as $cat => $name){
      unset($this->Categories[$cat]);
    }
    foreach($newcats as $cat => $name){
      $this->Categories[$cat]=$name;
    }
  }

  
  function catDown($id)
  {
   /* Get all cats depinding on current dir */
    $cats = $this->GetSubdirs($this->curCatDir);
     
    $newcats =$this->ArrayDown($id,$cats);

    foreach($newcats as $cat => $name){
      unset($this->Categories[$cat]);
    }
    foreach($newcats as $cat => $name){
      $this->Categories[$cat]=$name;
    }
  }


  function getOneUp($appl)
  { 
    $cat  = $this->curCatDir;
    $apps = $this->gosaMemberApplication[$cat];

    $appsA = array();
    foreach ($apps as $appkey => $name){
      $appsA[$name['App']] =$name['App'];
    }
 
    $result = $this->ArrayUp($appl,$appsA);

    $ret = array();
    foreach($result as $app){
      $ret[]=array("App"=>$app);
    }
    $this->gosaMemberApplication[$cat] = $ret;
  }


  function getOneDown($appl)
  {
    $cat  = $this->curCatDir;
    $apps = $this->gosaMemberApplication[$cat];

    $appsA = array();
    foreach ($apps as $appkey => $name){
      $appsA[$name['App']] =$name['App'];
    }

    $result = $this->ArrayDown($appl,$appsA);

    $ret = array();
    foreach($result as $app){
      $ret[]=array("App"=>$app);
    }
    $this->gosaMemberApplication[$cat] = $ret;
  } 

   
  
  function AddSeperator($id)
  {
    $found  = false;
    $cat    = "";
    $tmp = array();
    foreach($this->gosaMemberApplication[$this->curCatDir] as $appID => $app){  
      $tmp[] = $app;    
      if(($app['App'] == $id)&&(!$found)){
        $cnt = count($this->gosaMemberApplication[$this->curCatDir]);
        $tmp[] = array("App" => "__SEPARATOR__".($cnt+1));
        $found = true;
      }
    }
    if($found){
      $this->gosaMemberApplication[$this->curCatDir]=$tmp;
    }
  }

  function execute()
  {
	  /* Call parent execute */
   	plugin::execute();

    if((isset($_GET['act']))&&($_GET['act']=="depopen")){
      $dep = base64_decode($_GET['depid']);  
      if(isset($this->config->idepartments[$dep])){
        $this->curbase =$dep;
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="open")){
      $this->curCatDir = base64_decode($_GET['id']);
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no appgroup.")."</b>";
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove applications"),
            _("This group has application features enabled. You can disable them by clicking below."));
      } else {
        $display.= $this->show_header(_("Create applications"),
            _("This group has application features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }


    /* Add Categorie */ 
  
    
    if((isset($_POST['AddCat']))&&(isset($_POST['CatName']))&&(!empty($_POST['CatName']))){

      if(preg_match("/[\\\\\/]/i",$_POST['CatName'])){
        print_red(_("Invalid character in category name."));
      }elseif(!in_array($_POST['CatName'],$this->Categories)){ 
        if(empty($this->curCatDir)){
          $this->Categories[$_POST['CatName']]=$_POST['CatName'];
        }else{
          $this->Categories[$this->curCatDir."/".$_POST['CatName']]=$_POST['CatName'];
        }
      }else{
        print_red(_("The specified category already exists."));
      }
    }


    $this->reload();
    $only_once = false;
    foreach($_POST as $name => $value){
      
      if((preg_match("/AddSep_/",$name))&&(!$only_once)){
        $only_once = true;
        $n = preg_replace("/AddSep_/","",$name);
        $val= preg_replace("/_.*$/","",$n);
        $this->AddSeperator($val);
      }

      if((preg_match("/DelApp_/",$name))&&(!$only_once)){
        $only_once = true;
   

        if(preg_match("/DelApp___SEPARATOR__/",$name)) {
          $n=  preg_replace("/DelApp___SEPARATOR__/","",$name);
          $val= "__SEPARATOR__".preg_replace("/_.*$/","",$n);
        }else{
          $n = preg_replace("/DelApp_/","",$name);
          $val= preg_replace("/_.*$/","",$n);
        }

        foreach($this->gosaMemberApplication as $key =>  $cat){
          foreach($cat as $key2 => $app){
            if($app['App'] == $val){
              unset($this->gosaMemberApplication[$key][$key2]);
              if(isset($this->used_apps[$val])){
                unset($this->used_apps[$val]);
              }
            }
          }
        }
      }
  
      if(preg_match("/DelCat_/",$name)){
        $n = preg_replace("/DelCat_/","",$name);
        $app = base64_decode( preg_replace("/_.*$/","",$n));
        foreach($this->Categories as $key =>  $cat){
          if($cat == $app){
            foreach($this->Categories as $p => $n){
              if(preg_match("/^".$key."\/.*/",$p)){
                unset($this->Categories[$p]);    
              }
            }
            unset($this->Categories[$key]);
          }
        }
      }
      
      if((preg_match("/EdiApp_/",$name))&&(!$only_once)){

        $only_once = true;
        $appname = $value;
        $appname = preg_replace("/EdiApp_/","",$name);  
        $appname = preg_replace("/_.*$/","",$appname);

        /* We've got the appname, get parameters from ldap 
         */
        $ldap= $this->config->get_ldap_link();

        $tmp = search_config($this->config->data,"faiManagement","CLASS");
        if(!empty($tmp)){
          $tmp  = array_flip($this->getReleases());
          $base = $tmp[$this->Release];

          $ldap->cd($this->config->current['BASE']);
          $ldap->search("(&(objectClass=gosaApplication)(cn=$appname))",array("gosaApplicationParameter"));
          $found = "";
          while($attrs = $ldap->fetch()) {
            if(preg_match("/cn=".$appname.",".$base."/",$attrs['dn'])){
              $found = $attrs['dn'];
            }
          }
          $ldap->cat($found);
        }else{
          $ldap->cd($this->config->current['BASE']);
          $ldap->search("(&(objectClass=gosaApplication)(cn=$appname))",array("gosaApplicationParameter"));

        }


        if ($ldap->count() != 1){
          print_red (_("The selected application name is not uniq. Please check your LDAP."));
        } else {
          $attrs= $ldap->fetch();
          if(isset($attrs['gosaApplicationParameter'])){
            $this->dialog= TRUE;

            /* Fill name and value arrays */
            for ($i= 0; $i<$attrs['gosaApplicationParameter']['count']; $i++){
              $option= preg_replace('/^[^:]+:/', '',
                  $attrs['gosaApplicationParameter'][$i]);
              $name= preg_replace('/:.*$/', '', 
                  $attrs['gosaApplicationParameter'][$i]);
              $this->option_name[$i]= $name;

              /* Fill with values from application, default should be
                 loaded by the external scripts */
              if (isset($this->appoption[$name])){
                $this->option_value[$i]= $this->appoption[$name];
              }
            }

            /* Create edit field */
            $table= "<table summary=\"\">";
            for ($i= 0; $i < count($this->option_name); $i++){
              if (isset($this->option_value[$i])){
                $value= $this->option_value[$i];
              } else {
                $value= "";
              }
              $table.="<tr><td>".$this->option_name[$i]."</td><td>".
                "<input name=\"value$i\" size=60 maxlength=250 ".
                "value=\"".$value."\"><br></td></tr>";
            }
            $table.= "</table>";
            $this->table= $table;
          } else {
            print_red (_("The selected application has no options."));
          }
        }
      }
    }
    $this->reload();
    /* Add group with post */
    if((isset($_GET['act']))&&($_GET['act']=="add")){
      $this->used_apps[$_GET['id']]= $_GET['id'];
      asort($this->used_apps);
      $this->addApp($_GET['id']);
    }

    /* Add multiple */
    if(isset($_POST['AddApps'])){
      foreach($_POST as $name => $value){
        if(preg_match("/AddApp_/",$name)){
          $app = preg_replace("/AddApp_/","",$name);
          $this->addApp($app);
        }
      }
    }


    /* Cancel edit options? */
    if (isset($_POST['edit_options_cancel'])){
      $this->dialog= FALSE;
    }

    /* Finish edit options? */
    if (isset($_POST['edit_options_finish'])){
      $this->dialog= FALSE;

      /* Save informations passed by the user */
      $this->option_value= array();
      for ($i= 0; $i<count($this->option_name); $i++){
        $this->appoption[$this->option_name[$i]]= $_POST["value$i"];
        $this->is_modified= TRUE;
      }
    }

    /* Prepare templating stuff */
    $smarty= get_smarty();
    $smarty->assign("used_apps", $this->used_apps);
    $apps= array();
    foreach ($this->apps as $key => $value){
      if (!array_key_exists($key, $this->used_apps)){
        $apps["$key"]= "$value";
      }
    }

    $div = new DivSelectBox("appgroup");    

    $div->SetHeight(400);

    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->curbase) ;
    $ldap->ls("(objectClass=gosaDepartment)"); 
    $departments= array();
    $tmp = array();
    while ($value = $ldap->fetch()){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if($value["description"][0]!=".."){
        $departments[$value['dn']]=convert_department_dn($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $departments[$value['dn']]=convert_department_dn($value['dn']);
      }
    }

    /* END NEW LIST MANAGMENT
     */

    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=depopen&amp;depid=%s'>%s</a>";
    $linkadd  = "<a href='?plug=".$_GET['plug']."&amp;act=add&amp;id=%s'>%s</a>";

    $base_back = preg_replace("/^[^,]+,/","",$this->curbase);
    if((strlen($base_back)>= strlen($this->config->current['BASE']))&&($this->curbase!=$this->config->current['BASE'])){
      $div->AddEntry(array(
            array("string"=>sprintf($linkopen,base64_encode($base_back),".. ["._("back")."]"),
              "attach"=>"style='border:0px;'")
            ));
    }
    foreach($departments as $key => $app){
      $div->AddEntry(array(
            array("string"=>"<img class='center' src='images/folder.png' alt='"._("department")."'>&nbsp;".sprintf($linkopen,base64_encode($key),$app),
              "attach"=>"style='border:0px;'")
            ));
    }

    foreach($apps as $key => $app){
      $div->AddEntry(array(
            array("string"=>sprintf("<input class='center' type='checkbox' value='1' name='AddApp_%s'>",$key).
              "<img class='center' src='images/select_application.png' alt='"._("application")."'>&nbsp;".sprintf($linkadd,$key,$app),
              "attach"=>"style='border:0px;'")
            ));
    }

    if((isset($_GET['act']))&&(($_GET['act'] == "cat_up")||($_GET['act']=="cat_down"))){
      if($_GET['act']=="cat_up"){
        $this->catUp(base64_decode($_GET['id']));
      }
      if($_GET['act']=="cat_down"){
        $this->catDown(base64_decode($_GET['id']));
      }
    }

    if((isset($_GET['act']))&&(($_GET['act'] == "one_up")||($_GET['act']=="one_down"))){
      if(isset($_GET['id'])){
        $id   = $_GET['id'];
        $act  = $_GET['act']; 

        if($act == "one_up"){
          $this->getOneUp($id);
        }elseif($act == "one_down")   { 
          $this->getOneDown($id);
        }
      }
    }

    $div2 = new DivSelectBox("appgroup");
    $div2->SetHeight(400);

    $linkopen       = "<img class='center' src='images/folder.png' alt=\"\">&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>%s</a>";
    $catremove      = "&nbsp;<input type='image' src='images/edittrash.png' title='"._("Delete entry")."' name='DelCat_%s' value='%s'>";
    $app            = "<img class='center' src='images/select_application.png' alt=\"\">&nbsp;%s";
    
    $catupdown        = "<a href='?plug=".$_GET['plug']."&amp;act=cat_up&amp;id=%s'>
                       <img align='top' alt=\"\" src='images/sort_up.png' border=0 title='"._("Move up")."'></a>&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=cat_down&amp;id=%s'> 
                       <img alt=\"\" src='images/sort_down.png' title='"._("Move down")."' border=0></a>";

    if(empty($this->curCatDir)){
      $cnt =0;
    }else{
      $cnt = count(split("/",$this->curCatDir));
      $tmp = split("/",$this->curCatDir);
      $bbk = "";
      for($i = 0 ; $i < ($cnt -1 ) ; $i++){
        $bbk .= $tmp[$i];
      }
      $div2 ->AddEntry(array(array("string"=>sprintf($linkopen,base64_encode($bbk),"..")),array("string"=>"&nbsp;","attach"=>"style='border-right:0px;'")));
    }

    $this->GetSubdirs($this->curCatDir);

    foreach($this->GetSubdirs($this->curCatDir) as $path => $name){
      $div2 ->AddEntry(array( 
            array("string"=>sprintf($linkopen,base64_encode($path),$name)),
            array("string"=>preg_replace("/%s/",base64_encode($path),$catupdown.$catremove),
              "attach"=>"align='right' style='width:80px;border-right:0px;'"))); 
    }

    /* Append entries */

    $separator ="<hr size=1>"; 

    $sep = "<input type='image' src='images/back.png' title='"._("Insert seperator")."' value='%s' name='AddSep_%s'>";
  
    $upudown ="<a href='?plug=".$_GET['plug']."&amp;act=one_up&amp;id=%s'>   <img alt='{t}sort{/t}' align='top' src='images/sort_up.png' title='"._("Move up")."' border=0></a>".
      "&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=one_down&amp;id=%s'> <img alt='{t}sort{/t}' src='images/sort_down.png' title='"._("Move down")."' border=0></a>".
      "&nbsp;<input type='image' src='images/edittrash.png' title='"._("Delete entry")."' name='DelApp_%s' value='%s' alt='{t}delete{/t}' >";
    $edit=      "&nbsp;<input type='image' src='images/edit.png' title='"._("Edit entry")."' name='EdiApp_%s' value='%s' alt='{t}edit{/t}' >";

    if(isset($this->gosaMemberApplication[$this->curCatDir])){
      foreach($this->gosaMemberApplication[$this->curCatDir] as $cat => $entry){
        if(preg_match("/__SEPARATOR__/",$entry['App'])){
          $div2 ->AddEntry(array(array("string"=>$separator),
                array("string"=>preg_replace("/\%s/",htmlentities($entry['App']),$upudown),"attach"=>"align='right' style='border-right:0px;'")));
        }else{
          $div2 ->AddEntry(array(array("string"=>sprintf($app,$entry['App'])),
                array("string"=>preg_replace("/\%s/",htmlentities($entry['App']),$sep.$edit.$upudown),"attach"=>"align='right' style='border-right:0px;'")));
        }
      }
    }

    $smarty->assign("UsedApps", $div2->DrawList());
    $smarty->assign("List", $div->DrawList());
    $smarty->assign("apps", $apps);
   
    $smarty->assign("enableReleaseManagement",$this->enableReleaseManagement);
 
    if($this->enableReleaseManagement){
      $smarty->assign("Release", $this->Release);

      $tmp = $this->getReleases();

      $smarty->assign("Releases",   $tmp);

      if(count($this->used_apps)){
        $smarty->assign("ReleaseSelectAble", false);
      }else{
        $smarty->assign("ReleaseSelectAble", true);
      }
    }

    /* Show main page */
    if ($this->dialog){
      $smarty->assign("table", $this->table);
      $display.= $smarty->fetch (get_template_path('application_options.tpl', TRUE));
    } else {
      $display.= $smarty->fetch (get_template_path('application.tpl', TRUE));
    }
    return ($display);
  }

  function getReleases()
  {
  
    if(isset($this->base)){
      $base = $this->base;
    }else{
      $base = $this->parent->by_object['group']->base;
    }

    $dn   = "ou=apps,".$base;
    $ret  = array();

    $ret ["ou=apps,".$base] = "/";

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($dn);

    $ldap->search("objectClass=organizationalUnit",array("ou"));

    while($attrs = $ldap->fetch()){
      $str = str_replace($dn,"",$attrs['dn']);
      $tmp = array_reverse( split("ou=",$str));
      $str = "";
      foreach($tmp as $val){
        $val = trim(preg_replace("/,/","",$val));
        if(empty($val)) break;
        $str .= "/".$val;
      }
      if(!empty($str)){
        $ret[$attrs['dn']]= preg_replace("/^\//","",$str);
      }
    }
    return($ret);
  }
 
  function save_object()
  {
    if(isset($_POST ['Release'])){
      $this->Release = $_POST['Release'];
    }
  }
 

  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $this->attrs["gosaMemberApplication"]= array();

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
$ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Copy members */
    $this->Categories[""]=""; 
    $this->attrs["gosaMemberApplication"]= array();
    foreach($this->Categories as $name => $cats){
      $i =0;
      if(isset($this->gosaMemberApplication[$name])){
        foreach($this->gosaMemberApplication[$name] as $entry){
          if(!preg_match("/__SEPARATOR__/",$entry['App'])){
            $this->attrs["gosaMemberApplication"][]= $entry['App']."|".$name."|".$i;
          }
          $i ++;
        }
      }
      if(($i==0)&&(!empty($name))){
        $this->attrs["gosaMemberApplication"][]= "|".$name."|".$i;
      }
    }

    /* Are there application parameters to be saved */
    $this->attrs['gosaApplicationParameter']= array();
    foreach($this->appoption as $name => $value){
      if ($value != ""){
        $this->attrs['gosaApplicationParameter'][]= "$name:$value";
      }
    }

    /* Write back to LDAP */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
$ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }

  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    return ($message);
  }


  function reload()
  {
    /* Generate applist */
    $this->apps= array();

    /* Special handling for release managed apps 
     */
    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if(!empty($tmp)){
      $this->enableReleaseManagement = true;

      $tmp = array_flip($this->getReleases());
      if(isset($tmp[$this->Release])){
        $base =  $tmp[$this->Release];
      }else{

        $k = key($tmp);
        $r = $tmp[$k];
    
        print_red(sprintf(_("Can't resolve the release name '%s', setting release name to '%s'. "),$this->Release,$k));
        $this->Release = $k;
        $base = $r; 
      }

      $base = preg_replace("/ou=apps,.*$/","ou=apps,".$this->curbase,$base);
    
      $ldap   = $this->config->get_ldap_link();
      $ldap->cd ($base);
      $ldap->ls ("(objectClass=gosaApplication)",$base);
    }else{
      $ldap   = $this->config->get_ldap_link();
      $ldap->cd ("ou=apps,".$this->curbase);
      $ldap->ls ("(objectClass=gosaApplication)","ou=apps,".$this->curbase);
    }
    
    while ($attrs= $ldap->fetch()){
      if (isset($attrs["description"][0])){    
        $this->apps[$attrs["cn"][0]]=
          $attrs["cn"][0]." (".
          $attrs["description"][0].")";
      } else {
        $this->apps[$attrs["cn"][0]]=
          $attrs["cn"][0];
      }
    }
    natcasesort ($this->apps);
    reset ($this->apps);

    if(is_array($this->gosaMemberApplication))
      foreach ($this->gosaMemberApplication as $cat){   
        if(is_array($cat))
          foreach($cat as $entry){
            $this->used_apps[$entry['App']]= $entry['App'];
          }
      }
  }


  function addApp($cn)
  {
    if((isset($this->gosaMemberApplication[$this->curCatDir]))&&(is_array($this->gosaMemberApplication[$this->curCatDir]))){
      foreach($this->gosaMemberApplication[$this->curCatDir] as $entry){
        if($entry['App'] == $cn) return;
      }
    }
    $this->gosaMemberApplication[$this->curCatDir][]= array("App"=>$cn);
    $this->used_apps[$cn]=$cn;
    $this->is_modified= TRUE;
  }


  function removeApp($cn)
  {
    $temp= array();
    foreach ($this->gosaMemberApplication as $value){
      if ($value != $cn){
        $temp[]= $value;
      }
    }
    $this->gosaMemberApplication= $temp;
    $this->is_modified= TRUE;
  }

  function GetSubdirs($dir)
  {
    $ret = array();
    $tmp1 = split("/",$this->curCatDir);
 
    foreach($this->Categories as $path=>$cat){
      $tmp2 = split("/",$path);
      
      if((empty($this->curCatDir))&&(!preg_match("/\//",$path))){
        $abort = false;
      }elseif(((count($tmp1))+1) == (count($tmp2))){
        $abort = false;
        for($i = 0 ; $i < count($tmp1) ; $i++){
          if($tmp1[$i] != $tmp2[$i]){
            $abort = true;
          }
        }
      }else{
        $abort= true;
      }
      if(!$abort){
        $ret[$path]=$cat;
      } 
    }
  return($ret);
  }


  /* Get selected release name.
   *  it is written in the description tag 
   *  within ... (.*)$
   */
  function getReleaseName()
  {
  
    if(isset($this->parent)){
      $base = $this->parent->by_object['group'];
    }else{
      $base = $this;
    }

    if(isset($base->attrs['description'][0])){
      $desc = $base->attrs['description'][0];
    }else{
      $desc = "";
    }
 
    if(preg_match("/\(.*\)$/",$desc)){
      $str = preg_replace("/^.*\((.*)\)$/","\\1",$desc);
    }else{
      $str = "/";
    }
    return($str);  
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
