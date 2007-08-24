<?php

class gotomasses extends plugin
{
  /* Definitions */
  var $plHeadline     = "System deployment";
  var $plDescription  = "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Source file that contains the gotomasses data */
  var $data_file = "Undefined"; #Set in constructor 

  /* Queue tasks */
  var $tasks = array();
  var $current =false;
  var $dialog = FALSE;
  var $ids_to_remove = array();


  function gotomasses($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    /* Define source file */
    $this->data_file = CONFIG_DIR."/gotomasses_machines";
    $file = search_config($this->config->data['MENU'], "gotomasses", "STORAGE_FILE");
    if(!empty($file)){
      $this->data_file = $file;
    }
  
    $this->load_gotomasses_data();
  }


  function execute()
  {
    $smarty = get_smarty();
    /************
     * Handle posts 
     ************/

    $s_entry = $s_action = "";
    $arr = array( "/^edit_task_/"=>"edit","/^remove_task_/"=>"remove",
                  "/^new_task_/"=>"new_task","/^remove_multiple_task_/" => "remove_multiple");
    foreach($arr as $regex => $action){
      foreach($_POST as $name => $value){
        if(preg_match($regex,$name)){
          $s_action = $action;
          $s_entry  = preg_replace($regex,"",$name);
          $s_entry  = preg_replace("/_(x|y)$/","",$s_entry);
        }
      }
    }

    /* Edit posted from list link */
    if(isset($_GET['act']) && $_GET['act'] == "edit" && isset($_GET['id']) && isset($this->tasks[$_GET['id']])){
      $s_action = "edit";
      $s_entry = $_GET['id'];
    }

    /************
     * List posts 
     ************/

    /* Remove multiple */
    if($s_action == "remove_multiple"){
      if(preg_match("/disable/i",chkacl($this->acl,"remove"))){
        print_red(_("You are not allowed to remove a task."));
      }else{
        $this->ids_to_remove = $this->list_get_selected_items();
        $tmp = "";
        foreach($this->ids_to_remove as $key => $id){
          if(isset($this->tasks[$id])){
            $task = $this->tasks[$id];
            $tmp.= "\n".$this->target_to_string($task);
          }else{
            unset($this->ids_to_remove[$key]);
          }
        }
        $smarty->assign("multiple", TRUE); 
        $smarty->assign("info",sprintf(_("Your are about to delete the following tasks: %s"),"<pre>".$tmp."</pre>"));
        $this->current = $s_entry;
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }

    /* Remove specified tasks */
    if(count($this->ids_to_remove) && isset($_POST['delete_multiple_confirm'])){
      foreach($this->ids_to_remove as $id){
        if(isset($this->tasks[$id])){
          unset($this->tasks[$id]);
        }
      }
      $this->save();
    }

    /* Remove entry from list */
    if($s_action == "remove" && isset($this->tasks[$s_entry])){
      if(preg_match("/disable/i",chkacl($this->acl,"remove"))){
        print_red(_("You are not allowed to remove a task."));
      }else{
        $entry = $this->tasks[$s_entry];
        $this->current = $s_entry;
        $smarty->assign("info",sprintf(_("Your are about to delete the following tasks: %s"),
                                    "<pre>".$this->target_to_string($entry)."</pre>"));
        $smarty->assign("multiple", FALSE); 
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }

    /* Remove entry, remove is confirmed */
    if($this->current != -1 && isset($_POST['delete_confirm'])){
      unset($this->tasks[$this->current]);
      $this->current = -1;
      $this->save();
    }

    /* Remove aborted */
    if(isset($_POST['delete_cancel'])){
      $this->ids_to_remove = array();;
    }

    /* Edit selected entry */
    if($s_action == "edit" && isset($this->tasks[$s_entry])){
      $entry = $this->tasks[$s_entry];
      $this->dialog = new goto_task($this->config,$this,$entry);
      $this->current = $s_entry;
    }

    /* New entry */
    if(!preg_match("/disable/i",chkacl($this->acl,"create")) && $s_action== "new_task"){
      $this->dialog = new goto_task($this->config,$this);
      $this->current = -1;
    }
  
    /* Close dialog */
    if(isset($_POST['close_goto_task'])){
      $this->dialog = FALSE;
      $this->current = -1;
    }

    /* Close dialog */
    if((isset($_POST['save_goto_task']) || isset($_POST['apply_goto_task'])) && is_object($this->dialog) ){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);  
        }
      }else{
        if(isset($this->tasks[$this->current]) && $this->current != -1){
          $this->tasks[$this->current] = $this->dialog->save();
        }else{
          $this->tasks[] = $this->dialog->save();
        }
        if(!isset($_POST['apply_goto_task'])){
          $this->dialog = FALSE;
          $this->current = -1;
        }
        $this->save();
      }
    }

    /* Display dialogs if currently opened */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /************
     * Handle Divlist 
     ************/

    $plug = $_GET['plug'];
    $divlist = new MultiSelectWindow($this->config,"GotoMasses","gotomassses");
    $divlist->SetSummary(_("List of system deployment tasks"));
    $divlist->SetHeadpageMode();
    $divlist->EnableCloseButton(FALSE);
    $divlist->EnableSaveButton(FALSE);
    $divlist->SetInformation(_("This menu allows you to add, remove and change the properties of system deployment tasks."));
 
    /* Display add button if allowed */ 
    $header = "<div style='padding:5px'>";
    if(!preg_match("/disable/i",chkacl($this->acl,"create"))){
      $header .= "<input type='image' src='images/gotomasses_new_task.png' name='new_task' class='center'>&nbsp;";
    }

    /* Display add button if allowed */ 
    if(!preg_match("/disable/i",chkacl($this->acl,"remove"))){
      $header .= "<input type='image' src='images/edittrash.png' name='remove_multiple_task' class='center'>&nbsp;";
    }
    $header .= "</div>";

    /* Create divlist */
    $divlist->SetListHeader($header);
 /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $divlist->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));

    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=object'>"._("Target")."</a>"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=schedule'>"._("Schedule")."</a>",
                                      "attach"=>"style='width:100px;'"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=action'>"._("Type")."</a>",
                                      "attach"=>"style='width:80px;'"));
    $divlist->AddHeader(array("string"=>_("Action"),
                                      "attach"=>"style='border-right:0px;width:40px;'"));

    /* Create edit link */
    $edit_link = "<div style='width:100%;overflow:hidden;'><nobr><a href='?plug=".$_GET['plug']."&act=edit&id=%key%'>%str%</nobr></div>";
    foreach($this->tasks as $key => $task){
      $action = "<input type='image' src='images/edit.png' name='edit_task_".$key."' class='center' alt='"._("Edit")."'>";
      if(!preg_match("/disable/i",chkacl($this->acl,"remove"))){
        $action.= "<input type='image' src='images/edittrash.png' name='remove_task_".$key."' class='center' alt='"._("Remove")."'>";
      }
      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
          "attach" => "style='width:20px;'");

      $field1 = array("string" => preg_replace(array("/%key%/","/%str%/"),array($key,$this->target_to_string($task)),$edit_link));
      $field2 = array("string" => $this->time_to_string($task),"attach" => "style='width:100px;'");
      $field3 = array("string" => $this->action_to_string($task),"attach" => "style='width:80px;'");
      $field4 = array("string" => $action,"attach" => "style='text-align:right;width:40px;border-right:0px;'");
      $divlist->AddElement(array($field0,$field1,$field2,$field3,$field4));
    }

    return($divlist->Draw());
  }

  
  function target_to_string($data)
  {
    $ret = "";
    if($data['Action'] == "initial_install"){
      foreach($data['Initial_Target'] as $target){
        $ret .= $target['MAC'].", ";
      } 
    }else{
      foreach($data['Target'] as $target){
        $ret .= $target.", ";
      } 
    }
    return(preg_replace("/, $/","",$ret));
  }

  
  function time_to_string($data)
  {
    return($data['Minute']." ".$data['Hour']." ".$data['Day']." ".$data['Month']." ".$data['Weekday']);
  }

  
  function action_to_string($data)
  {
    $tmp = $this->get_actions();
    if(isset($tmp[$data['Action']])){
      return($tmp[$data['Action']]);
    }else{
      return(_("Unknown"));
    }
  }

  
  function load_gotomasses_data()
  {
    $ui = get_userinfo();

    /* Check file existence */
    if(!file_exists($this->data_file) || !is_readable($this->data_file)){
      print_red(sprintf(_("Can't locate or read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* check if file is readable */
    $fp = @fopen($this->data_file,"r");
    if(!$fp){
      print_red(sprintf(_("Can't read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* Get file contents */
    $data ="";
    while(!feof($fp)){
      $data.= fread($fp,512);
    }

    /* Get lines from file */
    $this->tasks  = array(); 
    $comment      = "";
    $rows         = split("\n",$data);

    /* Walk trough rows and parse data */
    foreach($rows as $row){

      /* Skip empty lines */
      $row = trim($row);
      if(empty($row)){
        continue;
      }

      /* Get comment, if available */     
      if(preg_match("/^#/",$row)){
        $comment = preg_replace("/^#/","",$row);
        continue;
      }

      /* Split row into minutes/ hours ...*/ 
      $row    = preg_replace('/[\t ]/umi'," ",$row);
      $row    = preg_replace('/  */umi'," ",$row);
      $parts  = split(" ",$row);

      if(count($parts) != 8){
        print_red(_("Entry broken, skipped."));
      }else{

        $entry = array();
        $entry['Minute']  = $parts[0];  
        $entry['Hour']    = $parts[1];  
        $entry['Day']     = $parts[2];  
        $entry['Month']   = $parts[3];  
        $entry['Weekday'] = $parts[4];  
        $entry['Action']  = $parts[5];  
        $entry['OGroup']  = $parts[6];  
        if($entry['Action'] == "initial_install"){
          $tmp2 = split(";",$parts[7]);
          foreach($tmp2 as $target){
            $tmp = split(",",$target);
            $entry['Initial_Target'][]  = array(
                          "MAC"     => $tmp[0],
                          "IP"      => $tmp[1],
                          "NAME"    => $tmp[2],
                          "ZONE"    => $tmp[3],
                          "SECTION" => $tmp[4]);
          }
          $entry['Target']  = array();
        }else{
          $entry['Initial_Target']  = array();
          $entry['Target']  = split(";",$parts[7]);
        }
        $entry['Comment'] = $comment;  
        $this->tasks []   = $entry;
      }
    } 
  }


  function save_gotomasses_data()
  {
    $str = "#GOsa generated file, please just modify if you realy know what you do.";
    foreach($this->tasks as $task){
      $str .= "\n#".trim($task['Comment']);
      $str .= "\n";
      if($task['Action'] == "initial_install"){
        $str .= "*     *     *     *     *     ";
      }else{
        $str .= str_pad($task['Minute'] ,5," ")." ";
        $str .= str_pad($task['Hour']   ,5," ")." ";
        $str .= str_pad($task['Day']    ,5," ")." ";
        $str .= str_pad($task['Month']  ,5," ")." ";
        $str .= str_pad($task['Weekday'],5," ")." ";
      }
      $str .= str_pad($task['Action'] ,5," ")." ";
      $str .= str_pad($task['OGroup'] ,5," ")." ";
      if($task['Action'] == "initial_install"){
        foreach($task['Initial_Target'] as $target){
          $str .= trim($target['MAC']).",".trim($target['IP']).",".trim($target['NAME']).",".
                  trim($target['ZONE']).",".trim($target['SECTION']).";";
        }
      }else{
        foreach($task['Target'] as $target){
          $str .= $target.";";
        }
      }
      $str = preg_replace("/;$/","",$str);
    }

    /* check file existence*/
    $ui = get_userinfo();
    if(!file_exists($this->data_file) || !is_writeable($this->data_file)){
      print_red(sprintf(_("Can't locate or write gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* check permissions */
    $fp = @fopen($this->data_file,"w");
    if(!$fp){
      print_red(sprintf(_("Can't read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* Write contents */
    $str .= "\n";
    fwrite($fp,$str);
    fclose($fp);
  }


  function save_object()
  {
  }


  /* Return list of object groups */
  function get_object_groups()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
    while($attrs = $ldap->fetch()){
      $ret[$attrs['cn'][0]] = $attrs['cn'][0];
    }
    return($ret); 
  }

  
  function save()
  {
    $this->save_gotomasses_data();
  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  function get_actions()
  {
    /* Prepare list of available actions */
    $actions = array(       "reboot"          => _("Reboot"),
                            "localboot"       => _("Localboot"),
                            "halt"            => _("Halt system"),
                            "initial_install" => _("Initial installation"),
                            "update"          => _("Update"),
                            "reinstall"       => _("(Re)Install"),
                            "rescan"          => _("Rescan"),
                            "wake"            => _("Wake"),
                            "memcheck"        => _("Memory check"));
    return($actions);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
