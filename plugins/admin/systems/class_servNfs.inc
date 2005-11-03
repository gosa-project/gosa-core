<?php

class servnfs extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("description","type","charset","path","option");
  var $objectclasses    = array("whatever");
  var $is_account       = true;

  var $name          ="";  // Name of  
  var $description   ="";  // description
  var $type          ="";  // Type FS/Samba/NCP
  var $charset       ="";  // charset
  var $types         =array();  // Array Types NFS/Samba/NCP
  var $charsets      =array();  // Array with charsets
  var $path          ="";  // Path
  var $option        ="";  // Options
  var $is_edit           =false;


  function servnfs ($config, $dn= NULL,$entry = false)
  {
    plugin::plugin ($config, $dn);
    $this->types   = array("NFS"=>"NFS","samba"=>"samba","NCP"=>"NCP");
    $this->charsets = array("UTF-8" => "UTF-8",
        "ISO8859-1"=>"ISO8859-1 (Latin 1)",
        "ISO8859-2"=>"ISO8859-2 (Latin 2)",
        "ISO8859-3"=>"ISO8859-3 (Latin 3)",
        "ISO8859-4"=>"ISO8859-4 (Latin 4)",
        "ISO8859-5"=>"ISO8859-5 (Latin 5)");

    if($entry){
      $tmp = split("\|",$entry);
      $this->name          = $tmp[0];  // Name of NFS
      $this->description   = $tmp[1];  // description
      $this->type          = $tmp[2];  // Type NFS/Samba/NCP
      $this->charset       = $tmp[3];  // charset
      $this->path          = $tmp[4];  // Path
      $this->option        = $tmp[5];  // Options
      $this->is_edit          = true;
    }else{
      $this->attributes[] = "name";
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
    /* Fill templating stuff */
    $smarty= get_smarty();

    $smarty->assign("charsets" ,$this->charsets);
    $smarty->assign("types"    ,$this->types);

    /* attrs to smarty*/
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    $smarty->assign("nameACL","");    
    $smarty->assign("name",$this->name);

    if($this->is_edit){
      $smarty->assign("nameACL"," disabled ");
    }

    $display= $smarty->fetch(get_template_path('servnfs.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object(TRUE);
    if(isset($_POST['path'])){
      foreach($this->attributes as $attr){
        $this->$attr = $_POST[$attr];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    // fixme : a check for the path ?  ? 
    if(empty($this->path)){
      $message[]=_("Please specify a valid path for your setup.");
    }

    // only 0-9a-z
    if(!$this->is_edit){
      if(preg_match("/[^a-z0-9]/i",$this->name)){
        $message[]=_("Please specify a valid name for your setup.");
      }
      if(empty($this->name)){
        $message[]=_("Please specify a name for your setup.");
      }
    }

    if(preg_match("/\|/",$this->description)){
      $message[]=_("Description contains invalid characters.");
    }
  
    if(preg_match("/\|/",$this->path)){
      $message[]=_("Path contains invalid characters.");
    }

    if(preg_match("/\|/",$this->option)){
      $message[]=_("Option contains invalid characters.");
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goShareServer)", array("goExportEntry"));
    while($test = $ldap->fetch()){
      if($test['dn']==$this->dn)
        continue;
      foreach($test['goExportEntry'] as $entry){
        $tmp = split("\|",$entry);
        if($tmp[0] == $this->name){
          $message[]="Name already in use";
        }
      }
    }
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Everything seems perfect, lets 
       generate an new export Entry 
     */

    $s_return = "";

    $s_return.= $this->name."|";     
    $s_return.= $this->description."|";     
    $s_return.= $this->type."|";     
    $s_return.= $this->charset."|";     
    $s_return.= $this->path."|";     
    $s_return.= $this->option;     

    return(array($this->name=>$s_return));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
