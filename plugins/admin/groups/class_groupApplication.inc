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

  var $option_name  = array();
  var $option_value = array();
  var $appoption    = array();

  var $table          = "";
  var $curbase        = "";
  var $curCatDir      ;
  var $curCatDepth    =0;         //

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("gosaApplicationGroup");

  function appgroup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Load member applications */
    if (isset ($this->attrs["gosaMemberApplication"][0])){
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
  }

  function catDown($id){
   /* Get all cats depinding on current dir */
    $cats = $this->GetSubdirs($this->curCatDir);
    $ret = array();
    if((in_array($id,array_flip($cats)))&&(count($cats)>1)){
      if(count($cats) == 2){
        $ret = array_reverse($cats);
      }else{
        $cnt = 0;
        $hit = 0;
        foreach($cats as $cat=>$name){
          if($cat  == $id){
            $hit = $cnt;
          }
          $cnt ++;
        }
        if($hit == 0){ 
          $first  = array_slice($cats,0,0);
          $middle = array_slice($cats,0,2);
          $last   = array_slice($cats,$hit+2,$cnt);
        }else{
          $first  = array_slice($cats,0,$hit);
          $middle = array_slice($cats,$hit,2);
          $last   = array_slice($cats,$hit+2,$cnt);
        }
        foreach($first as $cat => $name){
          $ret[$cat]=$name;
        }
        foreach(array_reverse($middle) as $cat => $name){
          $ret[$cat]=$name;
        }
        foreach($last as $cat => $name){
          $ret[$cat]=$name;
        }
      }
      foreach($cats as $cat => $name){
        unset($this->Categories[$cat]);
      }
      foreach($ret as $cat => $name){
        $this->Categories[$cat]=$name;
      }
    }
  }

  function catUp($id)
  {
    /* Get all cats depinding on current dir */
    $cats = $this->GetSubdirs($this->curCatDir);

    $ret = array();

    if((in_array($id,array_flip($cats)))&&(count($cats)>1)){
      if(count($cats) == 2){
        $ret = array_reverse($cats);
      }else{
        $cnt = 0;
        $hit = 0;
        foreach($cats as $cat => $name){
          $cnt ++;
          if($cat  == $id){
            $hit = $cnt;
          }
        }

        if($hit>1){
          if($hit == 2){ 
            $first  = array_slice($cats,0,$hit-2);
            $middle = array_slice($cats,$hit-2,$hit);
            $last   = array_slice($cats,$hit,$cnt); 
          }else{
            $first  = array_slice($cats,0,$hit-2);
            $middle = array_slice($cats,$hit-2,2);
            $last   = array_slice($cats,$hit,$cnt); 
          }
          foreach($first as $cat => $name){
            $ret[$cat]=$name;
          }
          foreach(array_reverse($middle) as $cat => $name){
            $ret[$cat]=$name;
          }
          foreach($last as $cat => $name){
            $ret[$cat]=$name;
          }
        }
      }

      foreach($cats as $cat => $name){
        unset($this->Categories[$cat]);
      } 
      foreach($ret as $cat => $name){
        $this->Categories[$cat]=$name;
      }
    }
    

  }

  function getOneUp($appl)
  {
    $cat = $this->curCatDir;
    if(!isset($this->gosaMemberApplication[$cat][($appl-1)])) return;
    $f = 1;
    if($appl == 1 ){
      $f = 0;
    }
    $first  = array_slice($this->gosaMemberApplication[$cat],0,($appl-1));
    $middle = array_slice($this->gosaMemberApplication[$cat],$appl-1,(($appl+1)-$f));
    $last   = array_slice($this->gosaMemberApplication[$cat],($appl+1));
    $tmp = array();
    foreach($first as $entry){
      $tmp[] = $entry;
    }
    foreach(array_reverse($middle) as $entry){
      $tmp[] = $entry;
    }
    foreach($last as $entry){
      $tmp[] = $entry;
    }
    $this->gosaMemberApplication[$cat] =$tmp;
  }
  
  function getOneDown($appl){
    $cat = $this->curCatDir;
    if(!isset($this->gosaMemberApplication[$cat][($appl+1)])) return;
    $f = 0;
    if($appl == 0 ){
      $f = 1;
    }
    $first  = array_slice($this->gosaMemberApplication[$cat],0,($appl));
    $middle = array_slice($this->gosaMemberApplication[$cat],$appl,(($appl+1+$f)));
    $last   = array_slice($this->gosaMemberApplication[$cat],($appl+2));
  
    $tmp = array();
    foreach($first as $entry){
      $tmp[] = $entry;
    }
    foreach(array_reverse($middle) as $entry){
      $tmp[] = $entry;
    }
    foreach($last as $entry){
      $tmp[] = $entry;
    }
    $this->gosaMemberApplication[$cat] =$tmp;
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
        $this->curbase = $dep;
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="open")){
      $this->curCatDir = $_GET['id'];
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
      
      if(preg_match("/[^0-9a-z,\.-;:_#\+\- ]/i",$_POST['CatName'])){
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
    foreach($_POST as $name => $value){
      if(preg_match("/AddSep_/",$name)){
        $this->AddSeperator($value);
      }
    

      if(preg_match("/DelApp_/",$name)){
        $app = $value; 

        foreach($this->gosaMemberApplication as $key =>  $cat){
          foreach($cat as $key2 => $app){
            if($app['App'] == $value){
              unset($this->gosaMemberApplication[$key][$key2]);
              if(isset($this->used_apps[$value])){
                unset($this->used_apps[$value]);
              }
            }
          }
        }
      }
      if(preg_match("/DelCat_/",$name)){
        $app = $value; 
        foreach($this->Categories as $key =>  $cat){
          if($cat == $value){
            unset($this->Categories[$key]);
          }
        }
      }
      if(preg_match("/EdiApp_/",$name)){
        $appname = $value;
        /* We've got the appname, get parameters from ldap */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaApplication)(cn=$appname))");
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
            array("string"=>"<img src='images/folder.png' alt='"._("department")."'>&nbsp;".sprintf($linkopen,base64_encode($key),$app),
              "attach"=>"style='border:0px;'")
            ));
    }

    foreach($apps as $key => $app){
      $div->AddEntry(array(
            array("string"=>sprintf("<input type='checkbox' value='1' name='AddApp_%s'>",$key).
              "<img src='images/select_application.png' alt='"._("application")."'>&nbsp;".sprintf($linkadd,$key,$app),
              "attach"=>"style='border:0px;'")
            ));
    }

    if((isset($_GET['act']))&&(($_GET['act'] == "cat_up")||($_GET['act']=="cat_down"))){
      if($_GET['act']=="cat_up"){
        $this->catUp($_GET['id']);
      }
      if($_GET['act']=="cat_down"){
        $this->catDown($_GET['id']);
      }
    }

    if((isset($_GET['act']))&&(($_GET['act'] == "one_up")||($_GET['act']=="one_down"))){
      if(isset($_GET['id'])){
        $id   = $_GET['id'];
        $act  = $_GET['act']; 
        $found = -1;
        if(isset($this->gosaMemberApplication[$this->curCatDir])){
          foreach($this->gosaMemberApplication[$this->curCatDir] as $key =>  $member){
            if($id == $member['App']){
              $found = $key;
            }
          }
        }

        if($found != -1){
          if($act == "one_up"){
            $this->getOneUp($found);
          }elseif($act == "one_down")   { 
            $this->getOneDown($found);
          }
        }
      }
    }

    $div2 = new DivSelectBox("appgroup");
    $div2->SetHeight(400);

    $linkopen       = "<img src='images/folder.png'>&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>%s</a>";
    $catremove      = "&nbsp;<input type='image' src='images/edittrash.png' name='DelCat_%s' value='%s'>";
    $app            = "<img src='images/select_application.png'>&nbsp;%s";
    
    $catupdown        = "<a href='?plug=".$_GET['plug']."&amp;act=cat_up&id=%s'>
                       <img src='images/sort_up.png' border=0></a>&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=cat_down&id=%s'> 
                       <img src='images/sort_down.png' border=0></a>";

    if(empty($this->curCatDir)){
      $cnt =0;
    }else{
      $cnt = count(split("/",$this->curCatDir));
      $tmp = split("/",$this->curCatDir);
      $bbk = "";
      for($i = 0 ; $i < ($cnt -1 ) ; $i++){
        $bbk .= $tmp[$i];
      }
      $div2 ->AddEntry(array(array("string"=>sprintf($linkopen,$bbk,"..")),array("string"=>"&nbsp;","attach"=>"style='border-right:0px;'")));
    }

    $this->GetSubdirs($this->curCatDir);

    foreach($this->GetSubdirs($this->curCatDir) as $path => $name){
      $div2 ->AddEntry(array( 
            array("string"=>sprintf($linkopen,$path,$name)),
            array("string"=>preg_replace("/%s/",$path,$catupdown.$catremove),
              "attach"=>"align='right' style='width:80px;border-right:0px;'"))); 
    }

    /* Append entries */

    $separator ="<hr height=1  size=1></hr>"; 

    $sep = "<input type='image' src='images/back.png' value='%s' name='AddSep_%s'>";
  
    $upudown ="<a href='?plug=".$_GET['plug']."&amp;act=one_up&id=%s'>   <img src='images/sort_up.png' border=0></a>".
      "&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=one_down&id=%s'> <img src='images/sort_down.png' border=0></a>".
      "&nbsp;<input type='image' src='images/edittrash.png' name='DelApp_%s' value='%s'>";
    $edit=      "&nbsp;<input type='image' src='images/edit.png' name='EdiApp_%s' value='%s'>";

    if(isset($this->gosaMemberApplication[$this->curCatDir])){
      foreach($this->gosaMemberApplication[$this->curCatDir] as $cat => $entry){
        if(preg_match("/__SEPARATOR__/",$entry['App'])){
          $div2 ->AddEntry(array(array("string"=>$separator),
                array("string"=>preg_replace("/\%s/",$entry['App'],$upudown),"attach"=>"align='right' style='border-right:0px;'")));
        }else{
          $div2 ->AddEntry(array(array("string"=>sprintf($app,$entry['App'])),
                array("string"=>preg_replace("/\%s/",$entry['App'],$sep.$edit.$upudown),"attach"=>"align='right' style='border-right:0px;'")));
        }
      }
    }

    $smarty->assign("UsedApps", $div2->DrawList());
    $smarty->assign("List", $div->DrawList());
    $smarty->assign("apps", $apps);

    /* Show main page */
    if ($this->dialog){
      $smarty->assign("table", $this->table);
      $display.= $smarty->fetch (get_template_path('application_options.tpl', TRUE));
    } else {
      $display.= $smarty->fetch (get_template_path('application.tpl', TRUE));
    }
    return ($display);
  }


  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $this->attrs["gosaMemberApplication"]= array();

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
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
    $cats = array();
    foreach($this->Categories as $name => $cats){
      $cats[$name] =0;
      if(isset($this->gosaMemberApplication[$name])){
        foreach($this->gosaMemberApplication[$name] as $entry){
          if(!preg_match("/__SEPARATOR__/",$entry['App'])){
            $this->attrs["gosaMemberApplication"][]= $entry['App']."|".$name."|".$cats[$name];
          }
          $cats[$name] = $cats[$name] + 1;
        }
      }
      if(($cats[$name]==0)&&(!empty($name))){
        $this->attrs["gosaMemberApplication"][]= "|".$name."|".$cats[$name];
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
    $ldap->modify($this->attrs);
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
    $message= array();
    return ($message);
  }


  function reload()
  {
    /* Generate applist */
    $this->apps= array();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ("ou=apps,".$this->curbase);

    $ldap->ls ("(objectClass=gosaApplication)","ou=apps,".$this->curbase);
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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
