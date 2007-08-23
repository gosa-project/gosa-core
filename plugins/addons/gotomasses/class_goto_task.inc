<?php

class goto_task extends plugin
{
  /* Definitions */
  var $plHeadline     = "System mass deployment";
  var $plDescription  = "This does something";
 
  var $parent   = NULL;
 
  var $Minute   = "*";
  var $Hour     = "*";
  var $Day      = "*";
  var $Month    = "*";
  var $Weekday  = "*";
  var $Action   = "install";
  var $Comment  = "";
  var $OGroup   = "keep_current";
  var $OGroups  = array();
  var $Target   = array();
  var $Initial_Target   = array();
  var $Actions  = array();
  var $new      = FALSE;
  var $attributes     = array("OGroup","Minute","Hour","Day","Month","Weekday","Action","Comment","Target","Initial_Target");


  function goto_task($config,$parent,$data = array())
  {
    /* Set parent object */
    $this->parent = $parent;
    $this->acl = $parent->acl;

    /* Intialize plugin */
    $this->config = $config;
    if(count($data)){
    
      $tmp = $this->get_array_values();
      foreach($this->attributes as $attr){
        if(!isset($data[$attr])){
          trigger_error("Missing parameter: '".$attr."' for goto_task contruction.");
        }else{
          $this->$attr = $data[$attr];
        }
      }
    }else{
      $this->new = TRUE;
    }

    /* Create ogroup select list */
    $this->OGroups = array("\"\""     => "["._("Keep current")."]");
    $this->OGroups = array_merge($this->OGroups,$this->parent->get_object_groups());

    /* Prepare list of available actions */
    $this->Actions = $this->parent->get_actions();
  }


  function execute()
  {
    /********
     * Handle Posts  
     ********/
  
    /* Handle Target options */
    foreach($_POST as $name => $value){
      if(preg_match("/^remove_/",$name)){
        $value = preg_replace("/^remove_([0-9]*)_(x|y)$/i","\\1",$name);
        if(isset($this->Target[$value]) && $this->Action != "initial_install"){
          unset($this->Target[$value]);
        }elseif(isset($this->Initial_Target[$value]) && $this->Action == "initial_install"){
          unset($this->Initial_Target[$value]);
        }
      }
    }

    if($this->Action != "initial_install"){
      
      /* Add target */
      if(isset($_POST['add_target']) && !empty($_POST['target_text'])){
        $target = get_post("target_text");
        if($this->is_valid_target($target) && !in_array($target,$this->Target)){
          $this->Target[] = $target;
        }
      }
    }else{

      /* Add target */
      if(isset($_POST['add_target']) && !empty($_POST['task_MAC'])){
        $MAC = $_POST['task_MAC'];
        $NAME= "";
        $IP  = "";
        if(isset($_POST['task_Name'])){
          $NAME = $_POST['task_Name'];
        }
        if(isset($_POST['task_IP']) && is_ip($_POST['task_IP'])){
          $IP = $_POST['task_IP'];
        }
        if(is_mac($MAC)){
          $this->Initial_Target[] = array("MAC"=>$MAC,"IP"=>$IP,"NAME"=>$NAME);
        }
      }
    }

    /* Add via csv */
    if(isset($_FILES['import_file'])){
      $file = $_FILES['import_file']['tmp_name'];
      if(file_exists($file) && is_readable($file)){
        $str ="";
        $fp = fopen($file,"r");
        while(!feof($fp)){
          $line = fgets($fp,512);
          $tmp = preg_split("/(,|;)/",$line);

          $MAC = $IP = $NAME = "";
          if(isset($tmp[0])){
            $MAC = $tmp[0];
          }
          if(isset($tmp[1])){
            $IP = $tmp[1];
          }
          if(isset($tmp[2])){
            $NAME = $tmp[2];
          }
          if(is_mac($MAC)){
            $this->Initial_Target[] = array("MAC"=>$MAC,"IP"=>$IP,"NAME"=>$NAME);
          }
        }
      }
    }
  
    /********
     * Add target from list 
     ********/

    /* If add from list is was requsted, display this list */ 
    if(isset($_POST['add_from_list'])){
      $this->dialog = new target_list($this->config,$this->Target);
    }

    /* Save selected objects as target */
    if(isset($_POST['SaveMultiSelectWindow'])){
      $this->dialog->save_object();
      $ret = $this->dialog->save();
      foreach($ret as $entry){
        $this->Target[] = $entry['cn'][0];
      }
      $this->dialog = NULL;
    }

    /* Cancel object listing */
    if(isset($_POST['CloseMultiSelectWindow'])){
      $this->dialog = NULL;
    }

    /* Display object Listing */
    if($this->dialog){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    /********
     * Display this plugin  
     ********/

    $divlist = new divlist("goto_task");
    $divlist->SetHeight("160");
    $divlist->SetEntriesPerPage(0);

    if(!preg_match("/disable/i",chkacl($this->parent->acl,"remove"))){
      $field_del = array("string" => "<input type='image' src='images/edittrash.png' name='remove_%KEY%'>" , 
          "attach" => "style='width:44px;border-right:0px;'");
    }else{
      $field_del = array("string" => "",
          "attach" => "style='width:44px;border-right:0px;'");
    }

    /* Add entries to divlist */
    if($this->Action == "initial_install"){
      $divlist->SetHeader(array(
                            array("string" => _("Target systems")),
                            array("string" => _("IP")      , "attach" => "style='width:90px;'"),
                            array("string" => _("Name")    , "attach" => "style='width:150px;'"),
                            array("string" => _("Actions") , "attach" => "style='width:44px;border-right:0px;text-align:right;'")));
      foreach($this->Initial_Target as $key => $target){
        $field1 = array("string" => $target['MAC']  );
        $field2 = array("string" => $target['IP']   ,"attach" => "style='width:90px;'");
        $field3 = array("string" => $target['NAME'] ,"attach" => "style='width:150px;'");
        $divlist->AddEntry(array($field1,$field2,$field3,preg_replace("/%KEY%/",$key,$field_del)));
      } 
    }else{
      $divlist->SetHeader(array(
                            array("string" => "Target", "attach" => "style=''"),
                            array("string" => "Actions" , "attach" => "style='width:44px;border-right:0px;text-align:right;'")));
      foreach($this->Target as $key => $target){
        $field1 = array("string" => $target);
        $divlist->AddEntry(array($field1,preg_replace("/%KEY%/",$key,$field_del)));
      } 
    }

    $smarty = get_smarty();
    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->parent->acl,$attr));
      $smarty->assign($attr,$this->$attr);
    }

    $tmp = $this->get_array_values();
    $smarty->assign("JS"      , $_SESSION['js']);
    $smarty->assign("Minutes" , $tmp['Minute']);
    $smarty->assign("Hours"   , $tmp['Hour']);
    $smarty->assign("Days"    , $tmp['Day']);
    $smarty->assign("Months"  , $tmp['Month']);
    $smarty->assign("Weekdays", $tmp['Weekday']);
    $smarty->assign("OGroups" , $this->OGroups);
    $smarty->assign("Actions"     , $this->Actions);
    $smarty->assign("Target_list" , $divlist->DrawList());
    $smarty->assign("new"      , $this->new);
    return ($smarty->fetch (get_template_path('goto_task.tpl', TRUE)));
  }

  
  /* check given values */
  function check()
  {
    $message = plugin::check();
    $tmp = array(
      "OGroup" => _("Object group") ,"Minute" => _("Minute"),
      "Hour"   => _("Hour")         ,"Day"    => _("Day"),
      "Month"  => _("Month")        ,"Weekday"=> _("Week day"),
      "Action" => _("Action")       ,"Comment"=> _("Description"));
    
    foreach($tmp as $name => $desc){
      if(empty($this->$name)){
        $message[] = sprintf(_("The given value for attribute '%s' is invalid."),$desc);
      }
    }
    if(count($this->Target) == 0 && $this->Action != "initial_install"){
      $message[] = sprintf(_("You must specify at least one target"));
    }
    if(count($this->Initial_Target) == 0 && $this->Action == "initial_install"){
      $message[] = sprintf(_("You must specify at least one target"));
    }
    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goto_task_posted'])){
      plugin::save_object();
      foreach($this->attributes as $attr){
        if(in_array($attr,array("Initial_Target","Target"))){
          continue;
        }
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }
    }
  }  

  
  /* Check if given target is vald.
   * It must either be a valid MAC address or an existing object group
   */
  function is_valid_target($str)
  {
    if(is_mac($str)){
      return(TRUE);
    }else{
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClassgosaGroupOfNames)(cn=".$str."))",array("cn"));
      if($ldap->count()){
        return(TRUE);
      }
    }
  }

  function save()
  {
    $tmp = array();
    foreach($this->attributes as $attr){
      $tmp[$attr] = $this->$attr;
    }
    return($tmp);
  }


  /* Return values for listboxes. 
   */
  function get_array_values()
  {
    $ret = array();    

    /* Create minute array */
    $Minute = array( "*"    => "*",
                      "*/1"  => "*/1",
                      "*/3"  => "*/3",
                      "*/5"  => "*/5",
                      "*/10" => "*/10",
                      "*/15" => "*/15",
                      "*/30" => "*/30",
                      "*/45" => "*/45",
                      "*/60" => "*/60"); 
    for($i = 0; $i < 60 ; $i ++){
      $Minute[$i] = $i;
    }

    /* Create hour array */
    $Hour = array(   "*" => "*");
    for($i = 1 ; $i < 24 ; $i ++ ){
      $Hour["*/".$i] = "*/".$i;
    } 
    for($i = 0 ; $i < 24 ; $i ++ ){
      $Hour[$i] = $i;
    } 

    /* Create hour array */
    $Day = array( "*" => "*");
    for($i = 1 ; $i < 32 ; $i ++ ){
      $Day["*/".$i] = "*/".$i;
    } 
    for($i = 1 ; $i < 32 ; $i ++ ){
      $Day[$i] = $i;
    } 
   
    /* Create month array */
    $Month = array( "*" => "*");
    for($i = 1 ; $i <= 12 ; $i ++ ){
      $Month["*/".$i] = "*/".$i;
    } 
    for($i = 1 ; $i <= 12 ; $i ++ ){
      $Month[$i] = $i;
    } 
   
    /* Create week day array */
    $Weekday = array( "*" => "*");
    for($i = 1 ; $i <= 7 ; $i ++ ){
      $Weekday["*/".$i] = "*/".$i;
    } 
    for($i = 0 ; $i <= 7 ; $i ++ ){
      $Weekday[$i] = $i;
    } 
   
    foreach(array("Minute","Weekday","Hour","Day","Month") as $var){
      $ret[$var] = $$var;
    }
    return($ret);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
