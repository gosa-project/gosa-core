<?php
class workstartup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal startup options";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $bootmode             = "G";
  var $goLdapServerList     = array("default");
  var $gotoBootKernel       = "default";
  var $gotoKernelParameters = "";
  var $gotoLdapServer       = "";
  var $gotoModules          = array();
  var $gotoAutoFs           = array();
  var $gotoFilesystem       = array();
  var $gotoTerminalPath     = "";
  var $FAIstatus            = "";
  var $FAIclass             = array();
  var $FAIclasses           = array();
  var $FAIclassInfo         = array();
  var $FAIdebianMirror      = "auto";


  /* attribute list for save action */
  var $attributes     = array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters", "FAIclass", "FAIstatus", "gotoShare","FAIdebianMirror", "FAIrelease");
  var $objectclasses  = array("GOhard", "FAIobject");

  /* Share */
  var $gotoShares         = array();// Currently Share Option
  var $gotoShare          = "";     // currently selected Share Option
  var $gotoShareSelections= array();// Available Shares for this account in Listbox format
  var $gotoAvailableShares= array();// Available Shares for this account

  /* Helper */
  var $customParameters   = "";
  var $orig_dn            = "";
  var $ignore_account     = TRUE;
  var $FAIdebianMirrors   = array();
  var $FAIrelease         = "";
  var $FAIreleases        = array();

  function workstartup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Creating a list of valid Mirrors 
     * none will not be saved to ldap.
     */
    $this->FAIdebianMirrors   = array();
    $this->FAIreleases        = array();

    $ldap   = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=FAIrepositoryServer)",array("FAIrepository"));
    
    /* attach all attributes with "index => cn" to avoid multiple entries */
    $ret = array();
    $rels = array();
    while($attr = $ldap->fetch()){
      if(isset($attr['FAIrepository'])){
        
        unset($attr['FAIrepository']['count']);

        foreach($attr['FAIrepository'] as $rep){
          $tmp = split("\|",$rep);
          $str = "";
          if(count($tmp)==4){
            $sections = split(",",$tmp[3]);
            $str = $tmp[0];
            $ret[$str]=$sections;
            if( array_key_exists( $str, $rels ) ) {
              if( "" == $this->FAIrelease )
                $this->FAIrelease = $tmp[2];
              array_push( $rels[$str], $tmp[2] );
            }
            else $rels[$str] = array( $tmp[2] );
          }
        }
      }
    }
    ksort($ret);
    ksort($rels);
    $this->FAIdebianMirrors = $ret;
    $this->FAIreleases = $rels;

    /* Get arrays */
    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      if (isset($this->attrs["$val"]["count"])){
        for ($i= 0; $i<$this->attrs["count"]; $i++){
          if (isset($this->attrs["$val"][$i])){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }
      sort ($this->$val);
      $this->$val= array_unique($this->$val);
    }

    /* Parse Kernel Parameters to decide what boot mode is enabled */
    if (preg_match("/ splash=silent/", $this->gotoKernelParameters)){
      $this->bootmode= "G";
    } elseif (preg_match("/ debug/", $this->gotoKernelParameters)){
      $this->bootmode= "D";
    } elseif ($this->gotoKernelParameters == "") {
      $this->bootmode= "G";
    } else {
      $this->bootmode= "T";
    }
    if (preg_match("/ o /", $this->gotoKernelParameters)){
      $this->customParameters= preg_replace ("/^.* o /", "", $this->gotoKernelParameters);
    } else {
      $this->customParameters= "";
    }

    /* Prepare Shares */
    if((isset($this->attrs['gotoShare']))&&(is_array($this->attrs['gotoShare']))){
      unset($this->attrs['gotoShare']['count']);
      foreach($this->attrs['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = split("\|",$share);
        $tmp2['server']      =$tmp[0];
        $tmp2['name']        =$tmp[1];
        $tmp2['mountPoint']  =$tmp[2];
        $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
      }
    }

    $this->gotoShareSelections= $config->getShareList(true);
    $this->gotoAvailableShares= $config->getShareList(false);
    $tmp2 = array();
  
    $ldap   = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Search all FAI objects */
    $ldap->search("(| (objectClass=FAIpackageList)(objectClass=FAItemplate)
                      (objectClass=FAIvariable)(objectClass=FAIscript)(objectClass=FAIhook)(objectClass=FAIprofile)
                      (objectClass=FAIpartitionTable))",array("*"),true);
    /* Sort all entries, and attach elementtype.
     * To be able to show the types in the listbox.
     */
    while($attr = $ldap->fetch()){
      $cn = $attr['cn'][0];
      $tmp2[$cn]['REST'] = $attr;
      if(in_array('FAIpackageList',$attr['objectClass'])){
        $tmp2[$cn]['FAIpackageList']['obj']   = 'FAIpackageList'; 
        $tmp2[$cn]['FAIpackageList']['kzl']   = 'Pl';
        $tmp2[$cn]['FAIpackageList']['sec']   = $attr['FAIdebianSection'];
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAItemplate',$attr['objectClass'])){
        $tmp2[$cn]['FAItemplate']['obj']      = 'FAItemplate'; 
        $tmp2[$cn]['FAItemplate']['kzl']      = 'T'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAIvariable',$attr['objectClass'])){
        $tmp2[$cn]['FAIvariable']['obj']      = 'FAIvariable'; 
        $tmp2[$cn]['FAIvariable']['kzl']      = 'V'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAIscript',$attr['objectClass'])){
        $tmp2[$cn]['FAIscript']['obj']        = 'FAIscript'; 
        $tmp2[$cn]['FAIscript']['kzl']        = 'S'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAIhook',$attr['objectClass'])){
        $tmp2[$cn]['FAIhook']['obj']          = 'FAIhook'; 
        $tmp2[$cn]['FAIhook']['kzl']          = 'H'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAIpartitionTable',$attr['objectClass'])){
        $tmp2[$cn]['FAIpartitionTable']['obj']= 'FAIpartitionTable'; 
        $tmp2[$cn]['FAIpartitionTable']['kzl']= 'Pt'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
      if(in_array('FAIprofile',$attr['objectClass'])){
        $tmp2[$cn]['FAIprofile']['obj']= 'FAIprofile'; 
        $tmp2[$cn]['FAIprofile']['kzl']= 'P'; 
        $this->FAIclasses[$attr['cn'][0]]=$attr['cn'][0];
      }
    }
    if(is_array($this->FAIclasses)){
      natcasesort($this->FAIclasses);
    }

    $this->FAIclassInfo = $tmp2;

    if((isset($this->FAIclass))&&(!is_array($this->FAIclass))){
      $tmp = array();
      $tmp = split(" ",$this->FAIclass);
      $tmp2 =array();  
    
      foreach($tmp as $class){
        if( ":" == $class[0] ) {
          $this->FAIrelease = substr( $class, 1 );
        }
        else
          $tmp2[$class] = $class;
      }
      $this->FAIclass = $tmp2;
    }

    if(!is_array($this->FAIclass)){
      $this->FAIclass =array();
    }

    $this->orig_dn= $this->dn;
  }

  /* Create array to display available classes/profiles in a selectbox */
  function selectFriendlyClasses(){
    $tmp=array();

    $tmp2 = $this->getFAIreleases();
    if(!in_array($this->FAIrelease, $tmp2)){  
      $this->FAIrelease = key($tmp2);
    }


    foreach($this->FAIclasses as $class){
      //  (!preg_match("/ou=".$this->FAIrelease."/",$this->FAIclassInfo[$class]['REST']['dn']))
      if(($this->FAIdebianMirror != "auto") && (stristr($this->FAIclassInfo[$class]['REST']['dn'],"ou=".$this->FAIrelease) == false)){
        continue;
      }
      $str    = "";
      $skip = false;

      if(isset($this->FAIclassInfo[$class])){
        foreach($this->FAIclassInfo[$class] as $objs){
          if (isset($objs['kzl'])){
            $str .= $objs['kzl']." "; 
          }
        }
      }
      $tmp[$class] = $class."&nbsp; [".$str."]";
    }
    return($tmp);
  }

  function check()
  {
    $messages = array();
    /* If there are packages selected, but no mirror show error */   
    if(($this->FAIdebianMirror == "none")&&(count($this->FAIclass)>0)){
      $messages[]=_("Please select a 'FAI server' or remove the 'FAI classes'.");
    }
    return($messages);
  }

  function execute()
  {
  	/* Call parent execute */
  	plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return ($display);
    }

    /* Add module */
    if (isset ($_POST['add_module'])){
      if ($_POST['module'] != "" && chkacl ($this->acl, "gotoModule") == ""){
        $this->add_list ($this->gotoModules, $_POST['module']);
      }
    }

    /* Delete module */
    if (isset ($_POST['delete_module'])){
      if (count($_POST['modules_list']) && chkacl ($this->acl, "gotoModule") == ""){
        $this->del_list ($this->gotoModules, $_POST['modules_list']);
      }
    }

    /* FAI class management */
    if((isset($_POST['AddClass']))&&(isset($_POST['FAIclassesSel']))){
      $found = 0 ; 

      /* If this new class/profile will attach a second partition table
       * to our list of classes, abort and show a message.
       */
      foreach($this->FAIclass as $name){
        if(isset($this->FAIclassInfo[$name])){
          foreach($this->FAIclassInfo[$name] as $atr){
            if(isset($atr['obj'])){
              if($atr['obj'] == "FAIpartitionTable"){
                $found ++ ; 
              }
            }
          }
        }
      }

      if((isset($this->FAIclassInfo[$_POST['FAIclassesSel']]['FAIpartitionTable']))&&($found>0)){
        print_red(_("There is already a profile in your selection that contain partition table configurations."));
      }else{
        $this->FAIclass[$_POST['FAIclassesSel']]=$_POST['FAIclassesSel'];
      }
    }

    $sort = false;
    foreach($_POST as $name => $val){
      
      $sort_type = false;
      if((preg_match("/sort_up/",$name))&&(!$sort)){
        $sort_type = "sort_up_";
      }
      if((preg_match("/sort_down/",$name))&&(!$sort)){
        $sort_type = "sort_down_";
      }
    
      if(($sort_type)&&(!$sort)){
        $value = base64_decode(preg_replace("/_.*$/i","",preg_replace("/".$sort_type."/i","",$name)));
        $sort = true;
        
        $last = -1;
        $change_down  = -1;
 
        /* Create array with numeric index */ 
        $tmp = array();
        foreach($this->FAIclass as $class){
          $tmp [] = $class;
        }

        /* Walk trough array */
        foreach($tmp as $key => $faiName){
          if($faiName == $value){
            if($sort_type == "sort_up_"){
              if($last != -1){
                 $change_down= $last;
              }
            }else{
              if(isset($tmp[$key+1])){
                $change_down = $key;
              }
            }
          }
          $last = $key;
        }
 
        $tmp2 = array();
        $skip = false;    
  
        foreach($tmp as $ky => $vl){

          if($ky == $change_down){
            $skip = $vl;
          }else{
            $tmp2[$vl] = $vl;
          }
          if(($skip != false)&&($ky != $change_down)){
            $tmp2[$skip]  = $skip;
            $skip =false;
          }
        }   
        $this->FAIclass = $tmp2; 
      }
  
      if(preg_match("/fai_remove/i",$name)){
        $value = base64_decode(preg_replace("/_.*$/i","",preg_replace("/fai_remove_/i","",$name)));
        unset($this->FAIclass[$value]);
      }
    }

    /* Delete selected class from our list */
    if((isset($_POST['DelClass']))&&(isset($_POST['FAIclassSel']))){
      if(isset($this->FAIclass[$_POST['FAIclassSel']])){
        unset($this->FAIclass[$_POST['FAIclassSel']]);
      }
    }

    /* Show main page */
    $smarty= get_smarty();

    /* In this section server shares will be defined
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */
    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry
     * This entry will be, a combination of mountPoint and sharedefinitions
     */
    if(isset($_POST['gotoShareAdd'])){
      /* We assign a share to this user, if we don't know where to mount the share */
      if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        print_red(_("You must specify a valid mount point."));
      }else{
        $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
        $s_mount = $_POST['gotoShareMountPoint'];
        /* Preparing the new assignment */
        $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
      }
    }

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message
     */
    if((isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
      unset($this->gotoShares[$_POST['gotoShare']]);
    }

    $smarty->assign("gotoShares",$this->printOutAssignedShares());
    $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));

    /* Arrays */
    $smarty->assign("ldapservers", $this->config->data['SERVERS']['LDAP']);
    $smarty->assign("gotoLdapServer_select", $this->gotoLdapServer);
    $smarty->assign("gotoLdapServerACL", chkacl($this->acl, "gotoLdapServer"));
    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      $smarty->assign("$val", $this->$val);
    }

    /* Values */
    foreach(array("gotoBootKernel", "customParameters", "gotoShare","FAIclasses","FAIclass","FAIdebianMirror","FAIrelease") as $val){
      $smarty->assign($val, $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    $div = new divSelectBox("WSFAIscriptClasses");
    
    $div -> SetHeight("110");

    $str_up     = " &nbsp;<input type='image' src='images/sort_up.png'    name='sort_up_%s'    value='%s'>";
    $str_down   = " &nbsp;<input type='image' src='images/sort_down.png'  name='sort_down_%s'  value='%s'>";
    $str_remove = " &nbsp;<input type='image' src='images/edittrash.png'  name='fai_remove_%s' value='%s'>";
    $str_empty  = " &nbsp;<img src='images/empty.png' alt=\"\" width='7'>"; 

    $i = 1;

    foreach($this->FAIclass as $class){
      if($i==1){
        $str = $str_empty.$str_down.$str_remove;
      }elseif($i == count($this->FAIclass)){
        $str = $str_up.$str_empty.$str_remove;
      }else{
        $str = $str_up.$str_down.$str_remove;
      }
      $i ++ ; 
    
      $div->AddEntry(array(
                       array("string"=>$class),
                       array("string"=>preg_replace("/\%s/",base64_encode($class),$str),"attach"=>"style='width:50px;border-right:none;'")
                    ));
    }  

    $smarty->assign("FAIScriptlist",$div->DrawList()); 
    $smarty->assign("FAIdebianMirrors",$this->getFAIdebianMirrors());
    $smarty->assign("FAIreleases",$this->getFAIreleases());
    $smarty->assign("FAIrelease",$this->FAIrelease);
    $smarty->assign("FAIclasses",$this->selectFriendlyClasses());
    $smarty->assign("FAIclassesKeys",array_flip($this->selectFriendlyClasses()));
    $smarty->assign("FAIclassKeys",$this->FAIclass);

    /* Radio button group */
    if (preg_match("/G/", $this->bootmode)) {
      $smarty->assign("graphicalbootup", "checked");
    } else {
      $smarty->assign("graphicalbootup", "");
    }
    if (preg_match("/T/", $this->bootmode)) {
      $smarty->assign("textbootup", "checked");
    } else {
      $smarty->assign("textbootup", "");
    }
    if (preg_match("/D/", $this->bootmode)) {
      $smarty->assign("debugbootup", "checked");
    } else {
      $smarty->assign("debugbootup", "");
    }

    /* ACL's */
    foreach (array("gotoKernelParameters", "gotoModules", "gotoFilesystem","FAIclass") as $value){
      $smarty->assign($value."ACL", chkacl($this->acl, "$value"));
    }

    /* Show main page */
    return($smarty->fetch (get_template_path('workstationStartup.tpl', TRUE,dirname(__FILE__))));
  }

  function remove_from_parent()
  {
    $this->handle_post_events("remove");
  }


  function getFAIdebianMirrors()
  {
    $ret = array();
    $ret['auto']=_("automatic");
    $secs  = array();
    if(is_array($this->FAIclass)){
      foreach($this->FAIclass as $classes){
        if(isset($this->FAIclassInfo[ $classes]['FAIpackageList']['sec'])){
          if(isset($this->FAIclassInfo[ $classes]['FAIpackageList']['sec']['count'])){
            unset($this->FAIclassInfo[ $classes]['FAIpackageList']['sec']['count']);
          }
          if(is_array($this->FAIclassInfo[ $classes]['FAIpackageList']['sec'])){
            foreach($this->FAIclassInfo[ $classes]['FAIpackageList']['sec'] as $sec => $value){
              $secs[$value]= $value;
            }
          }
        }
      }
    }

    if(is_array($this->FAIdebianMirrors)){
      foreach($this->FAIdebianMirrors as $mirr=>$sections){
        $allok = true;
        foreach($secs as $sec){
          if(!in_array($sec,$sections)){
            $allok = false;
          }
        }
        if(count($this->FAIclass)){
          if(!in_array($this->FAIrelease,$this->FAIreleases[$mirr])){
            $allok = false; 
          }
        }
        if($allok){
          $ret[$mirr]=$mirr;
        }
      }
    }
    return($ret);
  }

  function getFAIreleases() 
  {
    $ret = array();
    if(is_array($this->FAIreleases)) {
      foreach($this->FAIreleases as $mirr=>$releases){
        if( $this->FAIdebianMirror == $mirr ) {
          foreach ($releases as $release) 
            $ret[$release] = $release;
          return($ret);
        }
      }
    }
    return($ret);
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();

    /* Save group radio buttons */
    if (chkacl ($this->acl, "bootmode") == "" && isset($_POST["bootmode"])){
      $this->bootmode= $_POST["bootmode"];
    }

    /* Save kernel parameters */
    if (chkacl ($this->acl, "gotoKernelParameters") == "" && isset($_POST["customParameters"])){
      $this->customParameters= $_POST["customParameters"];
    }
  }


  /* Save to LDAP */
  function save()
  {

    /* Depending on the baseobject (Ogroup / WS) we
     *  use another set of objectClasses
     * In case of WS itself, we use  "array("GOhard", "FAIobject");"
     * if we are currently editing from ogroup menu we use (array("gotWorkstationTemplate","GOhard", "FAIobject"))
     */
    if(isset($this->parent->by_object['ogroup'])){
      $this->objectclasses = array("gotoWorkstationTemplate","GOhard", "FAIobject");
    }elseif(isset($this->parent->by_object['workgeneric'])){
      $this->objectclasses = array("GOhard", "FAIobject");
    }else{
      print "Object Type Configuration : unknown";
      exit();
    }

    /* Find proper terminal path for tftp configuration
       FIXME: This is suboptimal when the default has changed to
       another location! */
    if ($this->gotoTerminalPath == "default"){
      $ldap= $this->config->get_ldap_link();

      /* Strip relevant part from dn, keep trailing ',' */
      $tmp= preg_replace("/^cn=[^,]+,ou=terminals,ou=systems,/i", "", $this->dn);
      $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

      /* Walk from top to base and try to load default values for
         'gotoTerminalPath'. Abort when an entry is found. */
      while (TRUE){
        $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

        $ldap->cat("cn=default,ou=terminals,ou=systems,$tmp".
            $this->config->current['BASE']);
        $attrs= $ldap->fetch();
        if (isset($attrs['gotoTerminalPath'])){
          $this->gotoTerminalPath= $attrs['gotoTerminalPath'][0];
          break;
        }

        /* Nothing left? */
        if ($tmp == ""){
          break;
        }
      }
    }

    /* Add semi automatic values */
    // FIXME: LDAP Server may not be set here...
    $this->gotoKernelParameters= "root=/dev/nfs nfsroot=".
      $this->gotoTerminalPath.
      ",ro,hard,nolock,fg,rsize=8192 ".
      "ip=::::::dhcp ldap=".base64_encode($this->gotoLdapServer);

    switch ($this->bootmode){
      case "D":
        $this->gotoKernelParameters.= " debug";
      break;
      case "G":
        $this->gotoKernelParameters.= " splash=silent";
      break;
    }
    if ($this->customParameters != ""){
      $this->gotoKernelParameters.= " o ".$this->customParameters;
    }

    plugin::save();

    unset( $this->attrs['FAIrelease'] );
    
    $str = "";
    foreach($this->FAIclass as $class){
      $str .= $class." ";
    }
    $str .= ":" . $this->FAIrelease;
    $this->attrs['FAIclass']= "";
    $this->attrs['FAIclass']= trim($str);

    if(empty($this->attrs['FAIclass'])){
      $this->attrs['FAIclass'] = array();
    }

    /* Add missing arrays */
    foreach (array("gotoFilesystem", "gotoAutoFs", "gotoModules") as $val){
      if (isset ($this->$val) && count ($this->$val) != 0){
    
        $this->attrs["$val"]= array_unique($this->$val);
      }
      if(!isset($this->attrs["$val"])) $this->attrs["$val"]=array();
    }
    /* Strip out 'default' values */
    if ($this->attrs['gotoLdapServer'] == "default"){
      unset ($this->attrs['gotoLdapServer']);
    }

    /* if mirror == none stop saving this attribute */
    if($this->FAIdebianMirror == "none"){
      $this->FAIdebianMirror = "";
    }
    
    if((count($this->attrs['FAIclass'])==0)&&(empty($this->FAIdebianMirror))){
      $tmp = array();
      foreach($this->attrs['objectClass'] as $class){
        if($class != "FAIobject"){
          $tmp[] = $class;
        }
      }
      $this->attrs['objectClass']     = $tmp;
      $this->attrs['FAIclass']        = array();
      $this->attrs['FAIdebianMirror'] = array();
    }

    /* prepare share settings */
    $tmp = array();
    foreach($this->gotoShares as $name => $settings){
      $tmp2= split("\|",$name);
      $name = $tmp2[0];
      $tmp[] = $settings['server']."|".$name."|".$settings['mountPoint'];
    }
    $this->attrs['gotoShare']=$tmp;

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
$ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error());
    $this->handle_post_events("modify");
  }

  /* Add value to array, check if unique */
  function add_list (&$array, $value)
  {
    if ($value != ""){
      $array[]= $value;
      sort($array);
      array_unique ($array);
    }
  }


  /* Delete value to array, check if unique */
  function del_list (&$array, $list)
  {
    $tmp= array();
    foreach ($array as $mod){
      if (!in_array($mod, $list)){
        $tmp[]= $mod;
      }
    }
    $array= $tmp;
  }

  /* Generate ListBox frindly output for the defined shares
   * Possibly Add or remove an attribute here,
   */
  function printOutAssignedShares()
  {
    $a_return = array();
    if(is_array($this->gotoShares)){
      foreach($this->gotoShares as $share){
        $a_return[$share['name']."|".$share['server']]= $share['name']." [".$share['server']."]";
      }
    }
    return($a_return);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
