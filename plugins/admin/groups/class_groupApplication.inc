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
  var $department= "";
  var $apps= array();
  var $used_apps= array();
  var $opt_edit= FALSE;
  var $option_name= array();
  var $option_value= array();
  var $appoption= array();
  var $table= "";
  var $curbase   = "";
  var $curCatDir;
  var $curCatDepth=0;
  var $Categories=array();


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

    $tmp = array();
    $tmp2 = array();
  
    $prios = array();

    foreach($this->gosaMemberApplication as $memberApp){
      if(preg_match("/\|/i",$memberApp)){
        $tmp = split("\|",$memberApp);
        $tmp2[$tmp[0]."|".$tmp[1]]['App'] = $tmp[0];
        $tmp2[$tmp[0]."|".$tmp[1]]['Cat'] = $tmp[1];
        $tmp2[$tmp[0]."|".$tmp[1]]['Pri'] = $tmp[2];
    
        $cats [$tmp[1]] = $tmp[1];
        $prios[$tmp[1]][$tmp[2]] = $tmp[2];
      }else{
        $tmp2[$memberApp."|"]['App'] = $memberApp;
        $tmp2[$memberApp."|"]['Cat'] = "";
        $tmp2[$memberApp."|"]['Pri'] = 1000;
      }
    }

    $this->gosaMemberApplication = $tmp2;
    foreach($cats as $cat ){
      $max = max($prios[$cat]);
      $min = 1;//min($prios[$cat]);  
      for($i = $min ; $i < $max ; $i++){
        if(!isset($prios[$cat][$i])){
          $this->gosaMemberApplication["__SEPARATOR__".$i."|".$cat]['Pri'] = $i;
          $this->gosaMemberApplication["__SEPARATOR__".$i."|".$cat]['Cat'] = $this->curCatDir;
          $this->gosaMemberApplication["__SEPARATOR__".$i."|".$cat]['App'] = "__SEPARATOR__".$i;
        }
      }
    }

    /* Append at last*/
    foreach($this->gosaMemberApplication as $key => $app){
      if($app['Pri'] == 1000){
        $this->gosaMemberApplication[$key]['Pri'] = $this->getNextFree(1,$app['Cat']);
      }
    }
  
  
    $this->curbase = $this->config->current['BASE'];
  }

  function getOneUp($appl)
  {
    $prio = $this->gosaMemberApplication[$appl]['Pri'];
    $prios = array();
    $next = 0;
    $key_use = false;
    foreach($this->gosaMemberApplication as $key => $app){
      if($app['Cat'] != $this->gosaMemberApplication[$appl]['Cat']) continue;
      if($app['Pri']< $prio){
        if($app['Pri'] > $next){
          $next     = $app['Pri'];
          $key_use  = $key;
        }
      }        
    }

    if($next != 0){
      $this->gosaMemberApplication[$appl]['Pri']= $next;
      $this->gosaMemberApplication[$key_use]['Pri']= $prio;
    }
  }
  
  function getOneDown($appl){
    $prio = $this->gosaMemberApplication[$appl]['Pri'];
    $prios = array();
    $next = 10000;
    $key_use = false;
    foreach($this->gosaMemberApplication as $key => $app){
      if($app['Cat'] != $this->gosaMemberApplication[$appl]['Cat']) continue;

      

      if($app['Pri']> $prio){
        if($app['Pri'] < $next){
          $next     = $app['Pri'];
          $key_use  = $key;
        }
      }
    }

    if($next != 0){
      $this->gosaMemberApplication[$appl]['Pri']= $next;
      $this->gosaMemberApplication[$key_use]['Pri']= $prio;
    }
  }
  
  function getNextFree($prio,$cat)
  {
    $ret = $prio;
    $prios =array();
    foreach($this->gosaMemberApplication as $app){
      if($cat == $app['Cat']){
        $prios[$app['Pri']]=$app['Pri'];
      }
    }
    while(isset($prios[$ret])){
      $ret ++;
    }
    return($ret);
  }

  function AddSeperator()
  {
    $i = 1;
    while(isset($this->gosaMemberApplication["__SEPARATOR__".$i."|".$this->curCatDir])){
      $i ++;
    }
    $this->gosaMemberApplication["__SEPARATOR__".$i."|".$this->curCatDir]['Pri'] = $this->getNextFree(1,$this->curCatDir);
    $this->gosaMemberApplication["__SEPARATOR__".$i."|".$this->curCatDir]['Cat'] = $this->curCatDir; 
    $this->gosaMemberApplication["__SEPARATOR__".$i."|".$this->curCatDir]['App'] = "__SEPARATOR__".$i;
  }

  function execute()
  {
	  /* Call parent execute */
   	//plugin::execute();
    if((isset($_GET['act']))&&($_GET['act']=="depopen")){
      $dep = base64_decode($_GET['depid']);  
      if(isset($this->config->idepartments[$dep])){
        $this->curbase = $dep;
      }
    }

    if(isset($_POST['AddSeperator'])){
      $this->AddSeperator();
    }

    if((isset($_GET['act']))&&($_GET['act']=="open")){
      $this->curCatDir = $_GET['id'];

      $found = false;
      foreach($this->Categories as $key => $name ){
        if($this->curCatDir==$name){
          $tmp = array_flip(split("\/",$key));

          $this->curCatDepth = ($tmp[$this->curCatDir]);
          $found= true;
        }
      }
      if(!$found){
        $this->curCatDir = "";
        $this->curCatDepth = 0;
      }
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

    $this->reload();
    foreach($_POST as $name => $value){
      if(preg_match("/DelApp_/",$name)){
        $app = preg_replace("/DelApp_/","",$name); 
        unset($this->used_apps[$app]);
        unset($this->gosaMemberApplication[$app."|".$this->curCatDir]);
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

    $div->SetHeight(300);

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

    if((isset($_GET['act']))&&(($_GET['act'] == "one_up")||($_GET['act']=="one_down"))){
      if(isset($_GET['id'])){
        $id   = $_GET['id'];
        $act  = $_GET['act']; 
        $found = false;
        foreach($this->gosaMemberApplication as $key =>  $member){
          if($id == $member['App']){
            $found = $key;
          }
        }

        if($found != false){
          if($act == "one_up"){
            $this->getOneUp($found);
          }elseif($act == "one_down")   { 
            $this->getOneDown($found);
          }
        }
      }
    }

    $div2 = new DivSelectBox("appgroup");
    $div2->SetHeight(300);

    $menu           = $this->CreateCatMenu();
    $str_noprio     = " %s ";
    $linkopen       = "<img src='images/folder.png'>            &nbsp;<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>%s</a>";
    $app   = "<img src='images/select_application.png'>&nbsp;%s".$str_noprio;

        /* append back entry */
    if($menu["__BACK__"] != false){
        $div2 ->AddEntry(array(
                    array("string"=>sprintf($linkopen,$menu["__BACK__"],".. [ "._("back")." ]")),
                    array("string"=>"&nbsp;","attach"=>"style='border-right:0px;'")
                    ));
    }

    foreach($menu['__CATEGORY__'] as $path => $name){
      $div2 ->AddEntry(array(array("string"=>sprintf($linkopen,$path,$name)),array("string"=>"&nbsp;","attach"=>"style='border-right:0px;'"))); 
    }

      /* Append entries */

    $separator ="<hr height=1  size=1></hr>"; 
 
    $upudown = "<a href='?plug=".$_GET['plug']."&amp;act=one_up&id=%s'>   <img src='images/sort_up.png' border=0></a>".
          "&nbsp;<a href='?plug=".$_GET['plug']."&amp;act=one_down&id=%s'> <img src='images/sort_down.png' border=0></a>".
          "&nbsp;<input type='image' src='images/edittrash.png' name='DelApp_%s' value='%s'>";
    $edit=      "&nbsp;<input type='image' src='images/edit.png' name='EdiApp_%s' value='%s'>";
 
    foreach($menu["__ENTRY__"] as $path => $entry){

      if(preg_match("/__SEPARATOR__/",$path)){
        $div2 ->AddEntry(array(array("string"=>$separator),
                         array("string"=>preg_replace("/\%s/",$entry['name'],$upudown),"attach"=>"style='border-right:0px;'")));

      }else{
        $div2 ->AddEntry(array(array("string"=>sprintf($app,$entry['name'],$entry['prio'])),
                         array("string"=>preg_replace("/\%s/",$entry['name'],$upudown.$edit),"attach"=>"style='border-right:0px;'")));
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


  /* Save data to object */
#  function save_object()
#  {
#    plugin::save_object();
#  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Copy members */
    $this->attrs["gosaMemberApplication"]= array();
    foreach ($this->gosaMemberApplication as $val){
      if(!preg_match("/__SEPARATOR__/i",$val['App'])){
        $this->attrs["gosaMemberApplication"][]= $val['App']."|".$val['Cat']."|".$val['Pri'];
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

    foreach ($this->gosaMemberApplication as $value){
      $this->used_apps[$value['App']]= $value['App'];
    }
  }










  function CreateCatMenu()
  {
    /* The current category */
    $na = $this->curCatDir;

    /* the current folder depth */
    $nd = $this->curCatDepth;

    /* init array */
    $return = array();

    $return["__CATEGORY__"] = array();  // Categories
    $return["__ENTRY__"]    = array();  // Entries in this category
    $return["__BACK__"]     = false;    // The back entry

    $tmp = new xmlParse();

    $this->Categories= array();
    if(!isset($this->config->data['MAIN']['KDE_APPLICATIONS_MENU']))    {
        print_red(_("There is no value for 'KDE_APPLICATIONS_MENU' specified in your gosa.conf."));
    }else{
        $path = $this->config->data['MAIN']['KDE_APPLICATIONS_MENU'];
        if(file_exists($path)){
            if(is_readable($path)){
                $tmp->parseMenu($path);
                $this->Categories = $tmp->GetData();
            }else{
                print_red(sprintf(_("The specified file '%s' for 'KDE_APPLICATIONS_MENU' in your gosa.conf is not accessable."),$path));
            }
        }else{
            print_red(sprintf(_("The specified file '%s' for 'KDE_APPLICATIONS_MENU' in your gosa.conf is not accessable."),$path));
        }
    }


    /* Create new categories to return */
    foreach($this->Categories as $key => $val){

        /* Split categories by | to get name and priority */
        $nk = split("\/",$key);

        /* Check if this category must be shown */
        if((isset($nk[$nd]))&&(is_array($nk))&&($nk[$nd] == $na)){

            /* Add this to the list, if theres is a sub category */
            if(isset($nk[($nd+1)])){
                $return["__CATEGORY__"][$nk[($nd+1)]] = $nk[($nd+1)];
            }

            /* Create back entry */
            if($nd !=0 ){
                $return['__BACK__'] = $nk[($nd-1)];
            }else{
                $return['__BACK__'] = "..";
            }

        }elseif($na==""){
            $return["__CATEGORY__"][$nk[0]] = $nk[0];
        }



    }

    /* If back entry isn't set, set it to false (none) or to ..(base)*/
    if($return["__BACK__"] == false){
        $return['__BACK__'] = "..";
        if($nd == 0 ){
            $return['__BACK__'] = false;
        }
    }

    foreach($this->gosaMemberApplication as $app){
      if($app['Cat'] == $this->curCatDir){
       $return["__ENTRY__"][$app['Pri']][$app['App']]  =  array("name"=>$app['App'],"prio" => $app['Pri']) ;
      }
    }    


    ksort($return["__ENTRY__"]);
 /* recreate array index */
    $tmp = array();
    foreach($return["__ENTRY__"] as $prio => $entries){
        ksort($entries);
        foreach($entries as $entry){
            $tmp[$entry['name']] = $entry;
        }
    }

    /* Assign sorted entries */
    $return['__ENTRY__'] = ($tmp);

    /* Return this all ..*/
    return($return);
  }

  function addApp($cn)
  {
    $this->gosaMemberApplication[$cn."|".$this->curCatDir]= array("App"=>$cn,"Cat"=>$this->curCatDir,"Pri"=>$this->getNextFree(1,$this->curCatDir));
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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
