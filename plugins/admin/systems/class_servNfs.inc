<?php

class servnfs extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array("description","type","charset","path","option", "volume");
  var $objectclasses    = array("whatever");
  var $is_account       = true;

  var $name          ="";  // Name of  
  var $description   ="";  // description
  var $type          ="";  // Type FS/Samba/NCP
  var $charset       ="";  // charset
  var $host       ="";  // hostname
  var $types         =array();  // Array Types NFS/Samba/NCP/netatalk
  var $charsets      =array();  // Array with charsets
  var $path          ="";  // Path
  var $volume        ="";  // Volume
  var $option        ="";  // Options
  var $is_edit           =false;
  var $allow_mounts = false; //do we allow mount entries?
  var $create_mount_init = false; //mount entry set for this entry (while editing)?
  var $create_mount = false; //save mount entry

  function servnfs ($config, $acl, $allow_mounts, $dn= NULL,$entry = false,$mount = false)
  {
    plugin::plugin ($config, $dn);
    
    $this->types   = array("CIFS" => "CIFS", "NFS"=>"NFS","samba"=>"samba","netatalk"=>"netatalk","NCP"=>"NCP");
    if($dn){
      $this->host = substr($dn, 3, strpos($dn, ',')-3);
    }
    
    $this->acl = $acl;
	$this->allow_mounts=$allow_mounts;
	
    $this->charsets = array();

    if(!file_exists(CONFIG_DIR."/encodings")){
      print_red(sprintf(_("The file '%s/encodings' does not exist, can't get supported charsets."),CONFIG_DIR));
    }else{
      if(!is_readable(CONFIG_DIR."/encodings")){
        print_red(sprintf(_("Can't read '%s/encodings', please check permissions."),CONFIG_DIR));
      }else{
        $fp = fopen(CONFIG_DIR."/encodings","r");
        $i = 100;
        while(!feof($fp)&&$i){
          $i -- ;
          $str = trim(fgets($fp,256));
      
          /* Skip comments */
          if(!preg_match("/^#/",$str)){
            $arr = split("\=",$str);
            if(count($arr)==2){
              $this->charsets[$arr[0]]=$arr[1];
            }
          }
        }

    
      }
    }

    if($entry){
      list($this->name, $this->description, $this->type, $this->charset,
           $this->path, $this->option, $this->volume)= split("\|",$entry."|");
      $this->is_edit          = true;

    }else{
      $this->attributes[] = "name";
    }

    
    $this->create_mount_init = $mount;
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
    
    $smarty->assign("allow_mounts", $this->allow_mounts);
    $smarty->assign("mount_checked", "");
    
    
    $appleMountsACL=chkacl($this->acl,"gotoShareAppleMounts");
    $appleMountsACLset=strpos($appleMountsACL, "disabled");
    $smarty->assign("appleMountsACL", $appleMountsACL);
    $smarty->assign("appleMountsACLset", $appleMountsACLset);
  
    if (($this->type == "netatalk") || ($this->type == "NFS")) {
        if ($this->create_mount_init) {
          $smarty->assign("mount_checked", "checked");
        } else {
        $tmp = split(",", $this->dn);
        $clip = $tmp[0] . ",ou=servers,ou=systems,";
        $mountsdn = "cn=mounts," . substr($this->dn, strlen($clip));
          switch ($this->type) {
            case "netatalk" : {
              $mountdn = "cn=".$this->host.":/".$this->name.",".$mountsdn;
              break;
            }
            case "NFS" : {
              $mountdn = "cn=".$this->host.":".$this->path.",".$mountsdn;
              break;
            }
            default : {
              continue;
            }
          }
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($mountdn, array('dn'));
        $attrs = $ldap->fetch();
        if (count($attrs) > 0) {
            $smarty->assign("mount_checked", "checked");
        }
        }        
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
    
    if ((isset($_POST['netatalk_mount'])) && (($this->type == "netatalk") || ($this->type == "NFS"))) {
        $this->create_mount = true;
    } else {
        $this->create_mount = false;
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    // fixme : a check for the path ?  ? 
    if(empty($this->path)){
      $message[]=_("Please specify a valid path for your setup.");
    }

    // Skip if there are diallowed characters 
    if(!$this->is_edit){
      if(!preg_match("/^[a-z0-9\._\-äüö]*$/ui",$this->name)){
        $message[]=_("Please specify a valid name for your share.");
      }
      if(empty($this->name)){
        $message[]=_("Please specify a name for your share.");
      }
    }

    if(preg_match("/^[a-z0-9\._äüö\+ -]*$/ui",$this->description)){
      $message[]=_("Description contains invalid characters.");
    }

    if(preg_match("/^[a-z0-9\._äüö\+ -]*$/ui",$this->volume)){
      $message[]=_("Volume contains invalid characters.");
    }
  
    if(preg_match("/\|/",$this->path)){
      $message[]=_("Path contains invalid characters.");
    }

    if(preg_match("/^[a-z0-9\._äüö,=\+- ]*$/ui",$this->option)){
      $message[]=_("Option contains invalid characters.");
    }

  /* remove a / at the end of the path, we neither need it there nor
   * do we want to check for it later.
   */
  if(substr($this->path, -1, 1) == '/') {
    $this->path=substr($this->path, 0, -1);
  }
  
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goShareServer)", array("goExportEntry"));
    while($test = $ldap->fetch()){
      if($test['dn']==$this->dn)
        continue;
      if(isset($test['goExportEntry'])){
        foreach($test['goExportEntry'] as $entry){
          $tmp = split("\|",$entry);
          if($tmp[0] == $this->name){
            $message[]="Name already in use";
          }
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
    $s_return.= $this->option."|";     
    $s_return.= $this->volume;     

    return(array($this->name=>$s_return));
  }
  
  function should_create_mount() {
    return $this->create_mount;
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
