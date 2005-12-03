<?php

class faiPackage extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description","FAIpackage","FAIdebianRelease","FAIdebianSection","FAIdebianMirror");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIpackageList","FAIrepository");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "";
  var $subClasses       = array("top","FAIclass","FAIscriptEntry");

  /* Class name of the php class which allows us to edit a Sub Object */
  var $subClassName     = "";      

  /* Attributes to initialise for each subObject */
  var $subAttributes    = array("cn","description"); 
  var $sub64coded       = array();

  var $ConfiguredPackages = array();

  /* Specific attributes */
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $SubObjects       = array();  // All leafobjects of this object

  var $FAIdebianRelease          = ""; // The selected release
  var $FAIdebianSection          = ""; // selected section
  var $FAIdebianMirror           = ""; // selected mirror
  var $mirror                    = ""; // selected mirror

  var $servers          = array();  // All available servers
  var $releases         = array();  // All possible releases 
  var $sections         = array();  // All section types

  var $list             = NULL;

  var $mirrors          = array();  // The combination of server/release/section
  var $confDir          = "";
  var $usedPackages     = array();
  var $buffer           = NULL; 
  var $strID            ="";
  var $newDialogShown   =false;

  function faiPackage ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */
    if($dn != "new"){
      $this->dn =$dn;
    }

    if(isset($this->attrs['FAIpackage'])){
      unset($this->attrs['FAIpackage']['count']);
      foreach($this->attrs['FAIpackage'] as $pkg){
        $this->usedPackages[$pkg] = $pkg;
      }
      ksort($this->usedPackages);
    }else{
      $this->usedPackages = array();
    }  
  
    $ldap = $this->config->get_ldap_link();
    foreach($this->usedPackages as $name){
      $ldap->search("(&(objectClass=FAIdebconfInfo)(FAIpackage=".$name."))");
      while($attr = $ldap->fetch()){
     
        $tmp =array(); 
        $tmp['Name']  = $attr['FAIvariable'][0];
        $tmp['Type']  = $attr['FAIvariableType'][0];

        if (isset($attrs['FAIvariableContent'][0])){
          if(!in_array($attr['FAIvariableType'],array("multiselect"))){
            $tmp['Value'] = $attr['FAIvariableContent'][0];
          }else{
            $content = array();        
            unset($attr['FAIvariableContent']['count']);
            foreach($attr['FAIvariableContent'] as $attr){
              $tmp['Value'][] = $attr;
            }
          }
          $this->ConfiguredPackages[$attr['FAIpackage'][0]][$attr['FAIvariable'][0]]=$tmp;
        }
      }
    }

    if (isset($this->attrs['FAIdebianSection']['count'])){
      unset($this->attrs['FAIdebianSection']['count']);
    }
    if((isset($this->attrs['FAIdebianSection']))&&(is_array($this->attrs['FAIdebianSection']))){
      $this->FAIdebianSection = array();
      foreach($this->attrs['FAIdebianSection'] as $sec){
        $this->FAIdebianSection[$sec]=$sec;
      }
    }

    if((isset($this->attrs['FAIdebianSection']))&&(is_string($this->attrs['FAIdebianSection']))){
      $this->FAIdebianSection=array($this->attrs['FAIdebianSection']=>$this->attrs['FAIdebianSection']) ;
    }
    $this->confDir = CONFIG_DIR."/fai/";
    $this->FAIpackage = array();

    /* Extract mirror location */
    $this->mirror= preg_replace("!^[^/]+//([^/]+).*$!", '$1', $this->FAIdebianMirror);
  }

  function execute()
  {
	/* Call parent execute */

	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $this->genMirror();

    /* Check if we exist already - no need to ask for revisions, etc. then */
    if ($this->dn != "new"){
        $this->newDialogShown= true;
    }

    if((!$this->is_account)&&(!$this->newDialogShown)){
      
      if($this->dialog==NULL){
        $this->dialog = new faiPackageNew($this->config, $this->dn,$this->mirrors,$this->servers,$this->sections,$this->releases);
        $this->is_dialog =true;
      }

      /* alert possible missconfigurations */ 
      if((count($this->releases)==0)||(count($this->servers)==0)||(count($this->sections)==0)){
        print_red(_("There is no useable package list defined."));
      }

      /* Assign posible changes, for mirror combinations */
      $this->dialog->save_object();
      $this->dialog->releases = $this->releases;
      $this->dialog->servers  = $this->servers;
      $this->dialog->sections = $this->sections;
      $this->dialog->mirrors  = $this->mirrors;

      /* Assign Repository settings*/ 
      if(isset($_POST['SaveObjectNew'])){
        $this->FAIdebianRelease = $this->dialog->FAIdebianRelease;
        $this->FAIdebianSection = $this->dialog->FAIdebianSection;
        $this->FAIdebianMirror  = $this->dialog->FAIdebianMirror;
        unset($this->dialog);
        $this->dialog = false;
        $this->is_dialog=false;
        $this->newDialogShown = true;
      }

      /* Draw dialog */
      if($this->dialog){
        $display=$this->dialog->execute();
        return($display); 
      }
    }

    /* Assign variables */
    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
    }
  
    /* Set mirror if changed */
    if((isset($_POST['FAIdebianMirrorS']))&&(!empty($_POST['FAIdebianMirrorS']))){
      $this->FAIdebianMirror = $_POST['FAIdebianMirrorS'];
    }
  
    /* check servers matching release and section settings */
    $availableServer = array();
    foreach($this->mirrors as $mir){
      if($mir['release'] == $this->FAIdebianRelease){
        $pass = true;
        foreach($this->FAIdebianSection as $sec){
          if($sec != $mir['section']){
            $pass = false;
          }
        if($pass){
            $availableServer[$mir['mirror']]=$mir['mirror'];
          }
        }
      }
    }
 
    /* Generate package list */
    $this->list=$this->genPkgs();

    /* + was pressed to open the package dialog */
    if(isset($_POST['Addpkg'])){
      $this->dialog = new faiPackageEntry($this->config, $this->dn,$this->list,$this->usedPackages);
      $this->is_dialog =true;
    }

    /* Delte selected package */ 
    if(isset($_POST['Delpkg'])){
      unset($this->usedPackages[$_POST['usedPackages']]);
    }

    /* Abort package selection dialog */ 
    if(isset($_POST['CancelSubObject'])){
      $this->dialog = false;
      $this->is_dialog=false;
    }

    /* attach new packages */
    if(isset($_POST['SaveSubObject'])) {
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msgs){
        print_red($msgs);
        }
      }else{
        $use = $this->dialog->save();
        $this->usedPackages = $use;
        $this->dialog = false;
        $this->is_dialog=false;
        ksort($this->usedPackages);
      }
    }

    /* Configuration dialog open*/
    if((isset($_POST['Conpkg']))&&(isset($_POST['usedPackages']))&&(!empty($_POST['usedPackages']))){
      $path = "/etc/gosa/fai/".preg_replace("#^[^/]+//([^/]+).*$#","\\1",$this->FAIdebianMirror)."/".$this->FAIdebianRelease."/debconf.d";
      $pkg_config = array();
      if(isset($this->ConfiguredPackages[$_POST['usedPackages']])){
        $pkg_config = $this->ConfiguredPackages[$_POST['usedPackages']];
      }
  
      $this->dialog = new faiPackageConfiguration($this->config, $this->dn,$_POST['usedPackages'], $path, $pkg_config);
      $this->is_dialog =true;
    }
  
    /* Save Configuration */
    if(isset($_POST['SaveObjectConfig'])){
      $this->ConfiguredPackages= array_merge($this->ConfiguredPackages,$this->dialog->save());
      $this->dialog = false;
      $this->is_dialog=false;
    }

    /* cancel configuration */     
    if(isset($_POST['CancelObjectConfig'])){
      $this->dialog = false;
      $this->is_dialog=false;
    }

    /* Display dialog */ 
    if($this->is_dialog){
      return $this->dialog->execute();
    }

    /* Assign section to smarty */
    $strsec = "";
    foreach($this->FAIdebianSection as $sec){
       $strsec .= $sec." ";
    }

    $smarty->assign("OptionsACL","");
    if(empty($this->FAIdebianMirror)){
      $smarty->assign("OptionsACL"," disabled ");
    }

    $smarty->assign("mirrors" ,$availableServer);
    $smarty->assign("mirror"  ,$this->FAIdebianMirror);
    $smarty->assign("releases",$this->releases);
    $smarty->assign("release" ,$this->FAIdebianRelease);
    $smarty->assign("sections",$this->sections);
    $smarty->assign("section" ,$strsec);
    $smarty->assign("usedPackages",$this->printUsedPackages());
    $display.= $smarty->fetch(get_template_path('faiPackage.tpl', TRUE));
    return($display);
  }

  function genMirror(){
    $confDir = $this->confDir; 
    if(!is_readable($confDir)){
      print_red(sprintf(_("Can't read configuration folder '%s'."),$confDir));
    }else{
      
      /* Try to catch all available mirrors 
         Possibly check if each server is reachable
       */

      $this->servers= array();
      $this->releases=array();  
      $this->sections=array(); 
      $this->mirrors= array();
      $fd = opendir($confDir);
      while($mirror = readdir($fd)){
        if((is_dir($confDir.$mirror."/"))&&(($mirror != "." )&&($mirror!=".."))){

          $mirrorDir = $confDir.$mirror."/";

          $fe = opendir($mirrorDir);
          while($release = readdir($fe)){
            if((is_dir($mirrorDir.$release))&&(($release != "." )&&($release!=".."))){

              $releaseDir = $mirrorDir.$release."/";

              $ff = opendir($releaseDir);
              while($section = readdir($ff)){
                if((is_file($releaseDir.$section))&&(($section != "." )&&($section!="..")&&(!preg_match("/.*\.in$/",$section)))){
              
                  $this->servers[$mirror]=$mirror;
                  $this->releases[$release]=$release;
                  $this->sections[$section]=$section;

                  $arr=array();
                  $arr['mirror'] = $mirror;
                  $arr['release']= $release; 
                  $arr['section']= $section;
 
                  $this->mirrors[] = $arr ;
                }
              }
              fclose($ff);
            }
          }
          fclose($fe);
        }
      }
      fclose($fd);
    }


  }

  /* Delete me, and all my subtrees
   */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->rmdir_recursive($this->dn);
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    
    plugin::save_object();
    foreach($this->attributes as $attrs){
      if(isset($_POST[$attrs])){
        $this->$attrs = $_POST[$attrs];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
  
    if(count($this->usedPackages)==0){
      $message[]=_("Please select a least one Package.");
    }
  
    if((empty($this->FAIdebianRelease))||(empty($this->FAIdebianSection))||(empty($this->FAIdebianMirror))){
      $message[]=_("Please choose a valid combination for your repository setup.");
    }

    return ($message);
  }

  function printUsedPackages(){
    $a_ret=array(); 
    if(is_array($this->usedPackages)) {
      foreach($this->usedPackages as $usedName){
      
        $config = 0;
   
        foreach($this->ConfiguredPackages as $name => $value){
          if($name == $usedName){
            $config ++;
          }
        }
      
        $c_str ="";
        if($config){
          $c_str = " - "._("package is configured");
        }
     
        if(isset($this->list[$usedName][1])){
          $a_ret[$usedName] = $usedName." [".$this->list[$usedName][1]."]".$c_str;
        }else{
          $a_ret[$usedName] = $usedName.$c_str;
        }
      }
    }
    return($a_ret);
  }

   function genPkgs(){
    /* Generate a list off available packages for this mirror, section and release
     */
    /* Only read this file if it wasn't read before */
    if($this->buffer==NULL){
      $this->buffer=array();
      $a_ret = array();
      foreach($this->FAIdebianSection as $sec){
        $strID= "/etc/gosa/fai/".preg_replace("#^[^/]+//([^/]+).*$#","\\1",$this->FAIdebianMirror)."/".$this->FAIdebianRelease."/".$sec;
        
        if(!is_file($strID)){
          print_red(sprintf(_("Package file '%s' does not exist."),$strID));
          $this->FAIdebianMirror="";
          unset($this->buffer);
          return(array());
        }
        $fp = fopen($strID,"r");

        /* Parse every line and create an array */
        while(!feof($fp)){
          $str= fgets($fp,512);
          $stra= split("\|",$str);
          if(count($stra)==4){
            $a_ret[$stra[0]] = $stra;
          }
        }
        fclose($fp);
          /* Save our Data, to avoid reading it again */
      }
      $this->buffer = $a_ret;
      ksort($a_ret);
      return($a_ret);
    }else{
      return $this->buffer;
    }
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
 
    $ldap = $this->config->get_ldap_link();

    $this->attrs['FAIpackage'] = array();
    foreach($this->usedPackages as $pkg => $obj){
      $this->attrs['FAIpackage'][] = $pkg;
    } 
  
    $this->attrs['FAIdebianSection'] = array();
    foreach($this->FAIdebianSection as $sec){
      $this->attrs['FAIdebianSection'][] = $sec;
    }

    $ldap->cat($this->dn);
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());

      /* Save Package configurations */
    foreach($this->ConfiguredPackages as $pkgname => $attrs){
      foreach($attrs as $name => $attr){

        foreach($attr as $n=>$v){
          if(empty($v)) $v = array();
        }

        /* Set attributes */
        $pkgattrs['objectClass']          = "FAIdebconfInfo";
        $pkgattrs['FAIpackage']           = $pkgname;
        $pkgattrs['FAIvariable']          = $name;
        $pkgattrs['FAIvariableType']      = $attr['Type'];
        $pkgattrs['FAIvariableContent']   = $attr['Value'];
        $pkgdn =  "FAIvariable=".$name.",".$this->dn;

        /* cehck if object already exists */
        $ldap->cat($pkgdn);
        if($ldap->count()!=0){
          $ldap->cd($pkgdn);
          $ldap->modify($pkgattrs);
        }else{
          $ldap->cd($this->config->current['BASE']);
          $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $pkgdn));
          $ldap->cd($pkgdn);
          $ldap->add($pkgattrs);
        }
        show_ldap_error($ldap->get_error());
      }
    }


    

  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
