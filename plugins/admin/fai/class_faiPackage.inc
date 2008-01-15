<?php

class faiPackage extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description","FAIpackage","FAIdebianRelease","FAIdebianSection", "FAIinstallMethod");

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
  var $FAIinstallMethod          = "aptitude"; // hard coded
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

  var $FAIstate         = "";

  var $FAIinstallMethods  = array( "install", "ninstall", "remove", 
      "dselect-upgrade", "taskinst", "taskrm",
      "hold", "clean", "aptitude", "aptitude-r",
      "pending", "dpkgc" );

  var $base             = "";
  var $release          = "";
  var $copy_paste_mode  = false;
  var $cut_paste_mode   = false;

  var $CopyPasteVars  = array("ConfiguredPackages","FAIdebianRelease","FAIdebianSection","FAIinstallMethod","mirror","servers","releases","sections","list","mirrors","usedPackages");


  function faiPackage ($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);

    $this->acl ="#all#";

    /* If "dn==new" we try to create a new entry
     * Else we must read all objects from ldap which belong to this entry.
     * First read SubObjects from ldap ... and then the partition definitions for the SubObjects.
     */
    if($dn != "new"){
      $this->dn =$dn;

      /* Set acls
       */
      $ui   = get_userinfo();
      $acl  = get_permissions ($this->dn, $ui->subtreeACL);
      $acli = get_module_permission($acl, "FAIclass", $this->dn);
      $this->acl=$acli;

      /* Get FAIstate
       */
      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }
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


    if($dn != "new"){

      /* Create one filter with all package names, 
         instead of calling $ldap->search for every single package 
       */
      $PackageFilter = "";
      foreach($this->usedPackages as $name){
        $PackageFilter .= "(FAIpackage=".$name.")";
      }
      $PackageFilter = "(&(objectClass=FAIdebconfInfo)(|".$PackageFilter."))";

      /* Search for configuration objects */ 
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $ldap->search($PackageFilter,array("FAIvariable","FAIvariableType","FAIvariableContent","FAIpackage","FAIdebianSection"));

      /* Walk through configurations and append them to our list of ConfiguredPackages */
      while($attr = $ldap->fetch()){
        $tmp =array(); 
        $tmp['Name']  = $attr['FAIvariable'][0];
        $tmp['Type']  = $attr['FAIvariableType'][0];

        if (isset($attr['FAIvariableContent'][0])){
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


    $methods = array();
    foreach($this->FAIinstallMethods as $method){
      $methods[$method] = $method;
    }
    $this->FAIinstallMethods = $methods;
    /* Check if we exist already - no need to ask for revisions, etc. then */
    if ($this->dn != "new"){
      $this->newDialogShown= true;
    }

  }

  function execute()
  {
    /* Call parent execute */

    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $smarty->assign( "FAIinstallMethods", $this->FAIinstallMethods );

    if(!$this->is_account){

      /* Assemble release name */
      $tmp= preg_replace('/,ou=fai,ou=configs,ou=systems,.*$/', '', $_SESSION['faifilter']['branch']);
      $tmp= preg_replace('/ou=/', '', $tmp);
      $rev= array_reverse(split(',', $tmp));
      $this->FAIdebianRelease= "";
      foreach ($rev as $part){
        $this->FAIdebianRelease.= "/$part";
      }
      $this->FAIdebianRelease= preg_replace('#^/#', '', $this->FAIdebianRelease);

      /* Assemble sections */
      $repos= $this->getServerInfos();

      if(isset($repos[$this->FAIdebianRelease])){
        $this->FAIdebianSection= $repos[$this->FAIdebianRelease];
        $this->FAIdebianSection= array_unique($this->FAIdebianSection);
      }else{
        $this->FAIdebianSection = array();
        print_red(sprintf(_("No repository server found that provide this release '%s'."), $this->FAIdebianRelease));
      }

      /* Assign Repository settings*/ 
      $this->is_account     = true;
    }

    /* Assign variables */
    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
    }

    /* Generate package list */
    $this->list= $this->genPkgs();

    /* + was pressed to open the package dialog */
    if(isset($_POST['Addpkg'])){
      $this->dialog = new faiPackageEntry($this->config, $this->dn,$this->list,$this->usedPackages);
      $this->is_dialog =true;
    }

    /* Delte selected package */ 
    if(isset($_POST['Delpkg'])){
      if($this->FAIstate != "freeze"){
        foreach($_POST['usedPackages'] as $del){
          if(isset($this->usedPackages[$del])){
            unset($this->usedPackages[$del]);
          }
        }
      }
    }

    /* Abort package selection dialog */ 
    if(isset($_POST['CancelSubObject'])){
      $this->dialog = false;
      $this->is_dialog=false;
    }

    /* attach new packages */
    if(isset($_POST['SaveSubObject'])) {
      if($this->FAIstate != "freeze"){
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
      }else{
        $this->dialog = false;
        $this->is_dialog=false;
      }
    }

    /* Configuration dialog open*/
    if((isset($_POST['Conpkg']))&&(isset($_POST['usedPackages']))&&(!empty($_POST['usedPackages'][0]))){

      $path = CONFIG_DIR."/fai/".$this->FAIdebianRelease."/debconf.d";
      $pkg_config = array();
      $pkg = $_POST['usedPackages'][0];

      if(isset($this->ConfiguredPackages[$pkg])){
        $pkg_config = $this->ConfiguredPackages[$pkg];
      }

      $this->dialog = new faiPackageConfiguration($this->config, $this->dn,$pkg, $path, $pkg_config);
      $this->dialog ->acl = $this->acl;
      $this->is_dialog =true;
    }

    /* Configuration dialog open*/
    if($this->FAIstate != "freeze"){
      if((isset($_POST['Markpkg']))&&(isset($_POST['usedPackages']))&&(!empty($_POST['usedPackages']))){
        foreach($_POST['usedPackages'] as $pkg){
          if (isset($this->usedPackages[$pkg])){
            unset($this->usedPackages[$pkg]);
            if (preg_match('/^-/', $pkg)){
              $pkg= preg_replace('/^-/', '', $pkg);
            } else {
              $pkg= preg_replace('/^/', '-', $pkg);
            }
            $this->usedPackages[$pkg]= $pkg;
          }
        }
      }
    }    

    /* Save Configuration */
    if(isset($_POST['SaveObjectConfig'])){
      if($this->FAIstate != "freeze"){
        $this->ConfiguredPackages= array_merge($this->ConfiguredPackages,$this->dialog->save());
      }
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

    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    $smarty->assign("OptionsACL","");

    $smarty->assign("releases",$this->releases);
    $smarty->assign("release" ,$this->FAIdebianRelease);
    $smarty->assign("sections",$this->sections);
    $smarty->assign("section" ,$strsec);
    $smarty->assign("usedPackages",$this->printUsedPackages());
    $display.= $smarty->fetch(get_template_path('faiPackage.tpl', TRUE));
    return($display);
  }

  /* Delete me, and all my subtrees
   */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->rmdir_recursive($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing FAI package base failed"));
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {

    if($this->FAIstate == "freeze") return;  
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
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(count($this->usedPackages)==0){
      $message[]=_("Please select a least one Package.");
    }

    if((empty($this->FAIdebianRelease))||(empty($this->FAIdebianSection))){
      $message[]=_("Please choose a valid combination for your repository setup.");
    }

    /* If this is a new script, check if a script with this name already exists */
    if(!empty($this->release) && ($this->copy_paste_mode || $this->cut_paste_mode) ){

      /* Check if current name is already used for fai scripts in selected release */
      $dn = 'cn='.$this->cn.",ou=packages,".$this->release;
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($dn);
      if($ldap->count()){

        $r =convert_department_dn($this->release);;
        $message[] = sprintf(_("Can't insert a fai package list named '%s' in '%s' there is already a package list with the given name."),$this->cn,$r);
      }
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

        /* Adapt used name if we're marked for removal */
        $dsc= "";
        if (preg_match('/^-/', $usedName)){
          $dsc= " - "._("Package marked for removal");
          // Generally a bad idea here, because the toggel triggers on -, not on !
          //$usedName= preg_replace('/^-/', '! ', $usedName);
        }

        if(isset($this->list[$usedName][1])){
          $a_ret[$usedName] = $usedName." [".$this->list[$usedName][1]."]".$c_str.$dsc;
        }else{
          $a_ret[$usedName] = $usedName.$c_str.$dsc;
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
        $strID= CONFIG_DIR."/fai/".$this->FAIdebianRelease."/".$sec;

        if(!is_file($strID)){
          print_red(sprintf(_("Package file '%s' does not exist."),$strID));
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

    /* Copy & Paste : Ensure that FAIstate is copied too */
    if($this->copy_paste_mode && preg_match("/freeze/",$this->FAIstate)){
      $this->attrs['FAIstate'] = $this->FAIstate;
    }

    $this->attrs['FAIpackage'] = array();
    foreach($this->usedPackages as $pkg => $obj){
      $this->attrs['FAIpackage'][] = $pkg;
    } 

    $this->attrs['FAIdebianSection'] = array();
    foreach($this->FAIdebianSection as $sec){
      $this->attrs['FAIdebianSection'][] = $sec;
    }

    //    $this->attrs["FAIinstallMethod"]= "aptitude";

    $ldap->cat($this->dn, array('dn'));
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), _("Saving FAI package base failed"));

    $ldap->cd($this->dn);

    /* Save Package configurations */
    foreach($this->ConfiguredPackages as $pkgname => $attrs){
      foreach($attrs as $name => $attr){
      
        $pkgattrs = array();

        foreach($attr as $n=>$v){
          if(empty($v)) $v = array();
        }

        /* Set attributes */
        $pkgattrs['objectClass'][]        = "FAIdebconfInfo";

        $pkgattrs['FAIpackage']           = $pkgname;
        $pkgattrs['FAIvariable']          = $name;
        $pkgattrs['FAIvariableType']      = $attr['Type'];
        $pkgattrs['FAIvariableContent']   = $attr['Value'];
        $pkgdn =  "FAIvariable=".$name.",".$this->dn;

        /* cehck if object already exists */
        $ldap->cat($pkgdn,array("objectClass"));

	/* Tag object */
	$this->tag_attrs(&$pkgattrs, $sub_dn, $this->gosaUnitTag);
  
        if(in_array($pkgattrs['FAIvariableType'],array("boolean","multiselect","password","select","string","text"))){
          if($ldap->count()!=0){
            $ldap->cd($pkgdn);
            $this->cleanup();
            $ldap->modify ($pkgattrs); 

          }else{
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $pkgdn));
            $ldap->cd($pkgdn);
            $ldap->add($pkgattrs);
          }
          show_ldap_error($ldap->get_error(), _("Saving FAI package entry failed"));
        }

      }
    }
  }
  
  /* return copy & paste dialog
   */
  function getCopyDialog()
  {
    /* Ask for cn */
    $smarty = get_smarty();

    /* Assemble release name */
    $tmp= preg_replace('/,ou=fai,ou=configs,ou=systems,.*$/', '', $_SESSION['faifilter']['branch']);
    $tmp= preg_replace('/ou=/', '', $tmp);
    $rev= array_reverse(split(',', $tmp));
    $this->FAIdebianRelease= "";
    foreach ($rev as $part){
      $this->FAIdebianRelease.= "/$part";
    }
    $this->FAIdebianRelease= preg_replace('#^/#', '', $this->FAIdebianRelease);

    $smarty->assign("cn" ,$this->cn);
    $str = $smarty->fetch(get_template_path("paste_fai_object.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }

  /* Get posted cn */
  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }

  function getServerInfos()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=FAIrepositoryServer)",array("*"));
    while($attrs = $ldap->fetch()){
      if(isset($attrs['FAIrepository'])){
        for($i =0 ; $i < $attrs['FAIrepository']['count']; $i++){
          $obj = $attrs['FAIrepository'][$i];
          $tmp = split("\|",$obj);
          if(count($tmp)==4){
            foreach(split(",",$tmp[3]) as $sec){
              if(!empty($sec)){
                $ret[$tmp[2]][] =  $sec;
              }
            }
          }
        }
      }
    }
    return($ret);
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
