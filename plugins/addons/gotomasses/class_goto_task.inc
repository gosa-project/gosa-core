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

  var $Zone     = "";
  var $Section  = "";

  var $Zones    = array();
  var $Sections = array();

  var $Comment  = "";
  var $OGroup   = "keep_current";
  var $OGroups  = array();
  var $Target   = array();
  var $Initial_Target   = array();
  var $Actions  = array();
  var $new      = FALSE;
  var $ID = 0;
  var $TASK_ID =0;
  var $attributes     = array("ID","TASK_ID","Zone","Section","OGroup","Minute","Hour","Day",
                              "Month","Weekday","Action","Comment","Target","Initial_Target");

  var $configure_dhcp = FALSE;
  var $configure_dns  = FALSE;


  function goto_task(&$config, &$parent, $data = array())
  {
    /* Set parent object */
    $this->parent = &$parent;

    /* Intialize plugin */
    $this->config = &$config;
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

    if(!empty($this->Zone) && !preg_match("/^\"\"$/",$this->Zone)){
      $this->configure_dns = TRUE;
    }
    if(!empty($this->Section) && !preg_match("/^\"\"$/",$this->Section)){
      $this->configure_dhcp = TRUE;
    }

    /* Set dns and dhcp attributes */
    foreach(getAvailableZones($this->config) as $zone){
      $name = preg_replace("#^[^/]+/#","",$zone);
      $this->Zones[$name] = $zone; 
    }
    $this->Sections = $this->get_dhcp_sections();

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
      if($this->ID == 0 && isset($_POST['add_target']) && !empty($_POST['target_text'])){
        $target = get_post("target_text");
        if($this->is_valid_target($target) && !in_array($target,$this->Target)){
          $this->Target[] = $target;
        }else{
          print_red(_("You must specify a valid MAC address or host name."));
        }
      }
    }else{

      /* Add target */
      if($this->ID ==0 && isset($_POST['add_target']) && !empty($_POST['task_MAC'])){
        $MAC = $_POST['task_MAC'];
        $NAME= "";
        $IP  = "";
        $DNS   = "";
        $DHCP  = "";
        if(isset($_POST['task_Name'])){
          $NAME = $_POST['task_Name'];
        }
        if(isset($_POST['task_IP']) && is_ip($_POST['task_IP'])){
          $IP = $_POST['task_IP'];
        }
        if(isset($_POST['configure_dns']) && isset($_POST['Zone']) && isset($this->Zones[$_POST['Zone']])){
          $DNS = $_POST['Zone'];
        }
        if(isset($_POST['configure_dhcp']) && isset($_POST['Section']) && isset($this->Sections[$_POST['Section']])){
          $DHCP = $_POST['Section'];
        }
        if(is_mac($MAC)){
          $this->Initial_Target[] = array("MAC"=>$MAC,"IP"=>$IP,"NAME"=>$NAME);
        }
      }
    }

    /* Add via csv */
    if($this->ID == 0 && isset($_FILES['import_file'])){
      $file = $_FILES['import_file']['tmp_name'];
      if(file_exists($file) && is_readable($file)){
        $str ="";
        $fp = fopen($file,"r");
        while(!feof($fp)){
          $line = fgets($fp,512);
          $tmp = preg_split("/(,|;)/",$line);

          $MAC = $IP = $NAME; 
          if(isset($tmp[0])){
            $MAC = trim($tmp[0]);
          }
          if(isset($tmp[1])){
            $IP = trim($tmp[1]);
          }
          if(isset($tmp[2])){
            $NAME = trim($tmp[2]);
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
    if(isset($_POST['SaveMultiSelectWindow']) && is_object($this->dialog)){
      $this->dialog->save_object();
      $ret = $this->dialog->save();
      foreach($ret as $entry){
        $this->Target[] = $entry['cn'][0];
      }
      $this->dialog = FALSE;
    }

    /* Cancel object listing */
    if(isset($_POST['CloseMultiSelectWindow'])){
      $this->dialog = FALSE;
    }

    /* Display object Listing */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    /********
     * Display this plugin  
     ********/

    $divlist = new divlist("goto_task");
#    $divlist->SetPluginMode(); 
    $divlist->setHeight(200);
    $divlist->SetWidth("100%");
    $divlist->SetEntriesPerPage(0);

    $acl_target = $this->parent->getacl("Target");
    if(preg_match("/w/i",$acl_target)){
      $field_del = array("string" => "<input type='image' src='images/edittrash.png' name='remove_%KEY%'>" , 
          "attach" => "style='width:44px;border-right:0px;'");
    }else{
      $field_del = array("string" => "",
          "attach" => "style='width:44px;border-right:0px;'");
    }

    /* Add entries to divlist */
    if($this->Action == "initial_install"){
      $divlist->SetHeader(array(
                            array("string" => _("Target systems") ,"attach" => "style='width:120px;'"),
                            array("string" => _("IP")      , "attach" => "style='width:90px;'"),
                            array("string" => _("Name")    , "attach" => "style='width:150px;'"),
                            array("string" => _("Actions") , "attach" => "style='width:44px;border-right:0px;text-align:right;'")));
      foreach($this->Initial_Target as $key => $target){
        $field1 = array("string" => $target['MAC']  ,"attach" => "style='width:120px;'");
        $field2 = array("string" => $target['IP']   ,"attach" => "style='width:90px;'");
        $field3 = array("string" =>  $target['NAME'] ,"attach" => "style='width:150px;'");
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
      $smarty->assign($attr."ACL", $this->parent->getacl($attr));
      $smarty->assign($attr,$this->$attr);
    }

    $smarty->assign("Zones", $this->Zones);
    $smarty->assign("Sections", $this->Sections);

    $smarty->assign("ID",$this->ID);
  
    $smarty->assign("Zone", $this->Zone);
    $smarty->assign("Section", $this->Section);

    $smarty->assign("configure_dhcp", $this->configure_dhcp);
    $smarty->assign("configure_dns", $this->configure_dns);

    $tmp = $this->get_array_values();
    $smarty->assign("JS"      , session::get('js'));
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


  function create_tree($arr,$base,$current = "")
  {
    $ret = array();
    foreach($arr as $r => $name){
      $base_part = str_replace($base,"",$r);
      if(preg_match("/^[a-z]*=".normalizePreg($name)."(|,)$/i",$base_part)){
        $ret[$r] = $current.$name;
        $tmp = $this->create_tree($arr,$r,$current.".&nbsp;");
        foreach($tmp as $sub_key => $sub_name){
          $ret[$sub_key] = $sub_name;
        }
      }
    }
    return($ret);
  }



  function get_dhcp_sections()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=dhcpService)",array("dhcpPrimaryDN"));

    $tmp   = array();
    $tmp2  = array();

    $dhcp_dns = array();
    while($attr = $ldap->fetch()){
      $dhcp_dns[$attr['dn']] = $attr['dhcpPrimaryDN'][0];
    }

    foreach($dhcp_dns as $key => $pri_dns){
      $ldap->cat($pri_dns,array("cn"));
      $tmp = $ldap->fetch();
      $dhcp_dns[$key] = $tmp['cn'][0];
    }

    foreach($dhcp_dns as $dn => $cn){
      $ldap->cd($dn);
      $ldap->search("(|(objectClass=dhcpService)(objectClass=dhcpGroup)".
                    "(objectClass=dhcpSubnet)(objectClass=dhcpSharedNetwork))",array("cn"));
      $tmp = array();
      while($attr = $ldap->fetch()){
        $tmp[$attr['dn']] = $attr['cn'][0];
      }
      $tmp2 = $this->create_tree($tmp,preg_replace("/^[^,]+,/i","",$dn),"(".$cn.")&nbsp;");
    }
    $ret = array();
    foreach($tmp2 as $key => $label){
      $ret[$tmp[$key]] = $label;
    }
    return($ret);
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
      if(empty($this->$name) && $this->$name != 0){
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
  

  /* Map acl function, to be able to use plugin::save_object() */
  function acl_is_writeable($attribute,$skip_write = FALSE)
  {
    return($this->parent->acl_is_writeable($attribute,$skip_write));
  }


  function save_object()
  {
    if(isset($_POST['goto_task_posted'])){

      if($this->Action == "initial_install"){
        if(isset($_POST['configure_dns'])){
          $this->configure_dns = TRUE;
          if(isset($_POST['Zone'])){
            $this->Zone = get_post("Zone");
          }
        }else{
          $this->Zone = "\"\"";
          $this->configure_dns = FALSE;
        }

        if(isset($_POST['configure_dhcp'])){
          $this->configure_dhcp = TRUE;
          if(isset($_POST['Section'])){
            $this->Section = get_post("Section");
          }
        }else{
          $this->configure_dhcp = FALSE;
          $this->Section = "\"\"";
        }
      }
      plugin::save_object();
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

    if($this->Action != "initial_install" || !$this->configure_dns){
      $tmp['Zone'] = "\"\"";
    }
    if($this->Action != "initial_install" || !$this->configure_dhcp){
      $tmp['Section'] = "\"\"";
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
