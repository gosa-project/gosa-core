<?php

class servnfs extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("NFSdescription","NFStype","NFScharset","NFSpath","NFSoption");
  var $objectclasses    = array("whatever");
  var $is_account       = true;

  var $NFSname          ="";  // Name of NFS 
  var $NFSdescription   ="";  // description
  var $NFStype          ="";  // Type NFS/Samba/NCP
  var $NFScharset       ="";  // charset
  var $NFStypes         =array();  // Array Types NFS/Samba/NCP
  var $NFScharsets      =array();  // Array with charsets
  var $NFSpath          ="";  // Path
  var $NFSoption        ="";  // Options
  var $is_edit           =false;


  function servnfs ($config, $dn= NULL,$entry = false)
  {
    plugin::plugin ($config, $dn);
    $this->NFStypes   = array("NFS"=>"NFS","samba"=>"samba","NCP"=>"NCP");
    $this->NFScharsets = array("UTF-8" => "UTF-8",
                        "ISO8859-1"=>"ISO8859-1 (Latin 1)",
                        "ISO8859-2"=>"ISO8859-2 (Latin 2)",
                        "ISO8859-3"=>"ISO8859-3 (Latin 3)",
                        "ISO8859-4"=>"ISO8859-4 (Latin 4)",
                        "ISO8859-5"=>"ISO8859-5 (Latin 5)");

    if($entry){
      $tmp = split("\|",$entry);
      $this->NFSname          = $tmp[0];  // Name of NFS
      $this->NFSdescription   = $tmp[1];  // description
      $this->NFStype          = $tmp[2];  // Type NFS/Samba/NCP
      $this->NFScharset       = $tmp[3];  // charset
      $this->NFSpath          = $tmp[4];  // Path
      $this->NFSoption        = $tmp[5];  // Options
      $this->is_edit          = true;
    }else{
    $this->attributes[] = "NFSname";
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();

    $smarty->assign("NFScharsets" ,$this->NFScharsets);
    $smarty->assign("NFStypes"    ,$this->NFStypes);

    /* attrs to smarty*/
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    
    $smarty->assign("NFSnameACL","");    

    if($this->is_edit){
      $smarty->assign("NFSnameACL"," disabled ");
      $smarty->assign("NFSname","");
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
    if(isset($_POST['NFSpath'])){
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
    if(empty($this->NFSpath)){
      $message[]=_("Please specify a valid path for your setup.");
    }
  
    // only 0-9a-z
    if(!$this->is_edit){
      if(preg_match("/[^a-z0-9]/i",$this->NFSname)){
        $message[]=_("Please specify a valid name for your setup.");
      }
      if(empty($this->NFSname)){
        $message[]=_("Please specify a name for your setup.");
      }
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goShareServer)", array("goExportEntry"));
    while($test = $ldap->fetch()){
      if($test['dn']==$this->dn)
        continue;
      foreach($test['goExportEntry'] as $entry){
        $tmp = split("\|",$entry);
        if($tmp[0] == $this->NFSname){
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
    
    $s_return.= $this->NFSname."|";     
    $s_return.= $this->NFSdescription."|";     
    $s_return.= $this->NFStype."|";     
    $s_return.= $this->NFScharset."|";     
    $s_return.= $this->NFSpath."|";     
    $s_return.= $this->NFSoption;     

    return(array($this->NFSname=>$s_return));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
