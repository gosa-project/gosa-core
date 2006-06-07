<?php
class workstartup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal startup options";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $bootmode             = "G";
  var $goLdapServerList     = array();
  var $gotoBootKernel       = "default";
  var $gotoKernelParameters = "";
  var $gotoLdapServer       = "default";
  var $gotoModules          = array();
  var $gotoAutoFs           = array();
  var $gotoFilesystem       = array();
  var $gotoTerminalPath     = "";
  var $FAIstatus            = "";
  var $gotoBootKernels      = array();

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
 
  /* FAI class selection */ 
  var $FAIclass           = array();
  var $FAIclasses         = array();
  var $FAIclassInfo       = array();
  var $FAIrelease         = "";
  var $FAIdebianMirror    = "auto";

  
  /* FAI class selection */
  var $InheritedFAIclass           = array();
  var $InheritedFAIrelease         = "";
  var $InheritedFAIdebianMirror    = "auto";

  /* Contains all possible server/release/class settings */
  var $FAIServRepConfig   = array();

  function workstartup ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Creating a list of valid Mirrors 
     * none will not be saved to ldap.
     */
    $ldap   = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    foreach($this->config->data['SERVERS']['LDAP'] as $server) {
      $this->goLdapServerList[$server]= $server; // $this->config->data['SERVERS']['LDAP'];
    }
   
    $_SESSION['getAvailableClassesForThisRelease_CACHE'] = array();
    $_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'] = array();

    /* Search all FAI objects */
    $ldap->search("(|(objectClass=FAIpackageList)(objectClass=FAItemplate)(objectClass=FAIvariable)(objectClass=FAIscript)(objectClass=FAIhook)(objectClass=FAIprofile)(objectClass=FAIpartitionTable))",array("cn","objectClass","FAIdebianSection"));
    /* Sort all entries, and attach elementtype.
     * To be able to show the types in the listbox.
     */
    while($attr = $ldap->fetch()){
      $cn = $attr['cn'][0];
    
      $_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'][] = $attr;
 
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

    if(isset($tmp2)){
      $this->FAIclassInfo = $tmp2;
    } else {
      $this->FAIclassInfo = array();
    }

    /* Build up an array like this one :
        [$url]['SERVER'] = 'srv1-002';
        [$url]['RELEASE']['siga/rc9.0.2']
                                           ['SECTIONS'][0] "main";
                                           ['SECTIONS'][1] "non-free";
        [$url]['RELEASE']['siga/rc9.0.2']
                                           ['PACKAGES'][0] "pkg1";
                                           ['PACKAGES'][1] "postfix";
     */

    $ldap->search("(&(FAIrepository=*)(objectClass=FAIrepositoryServer))",array("FAIrepository"));
    $test = array();
    while($attr = $ldap->fetch()){
      if(isset($attr['FAIrepository'])){

        unset($attr['FAIrepository']['count']);

        foreach($attr['FAIrepository'] as $rep){
          $tmp = split("\|",$rep);

          if(count($tmp)==4){
            $sections = split(",",$tmp[3]);
            $release  = $tmp[2];
            $server   = $tmp[1];
            $url      = $tmp[0];
           
            $test[$url]['RELEASE'][$release]['SECTIONS'] = $sections;
    
            /* Result will be cached
             */
            $test[$url]['RELEASE'][$release]['PACKAGES'] = $this->getAvailableClassesForThisRelease($release);
            $test[$url]['SERVER'] = $server;

            /* auto gets all releases/classes 
             */
            $test['auto']['RELEASE'][$release]['SECTION'] = $sections;
            $test['auto']['RELEASE'][$release]['PACKAGES'] = $this->getAvailableClassesForThisRelease($release);
          }
        }
      }
    }
    
    /* Add possible elements from hook */
    $lines= $this->GetHookElements();
    foreach ($lines as $hline){
      $entries= split(";", $hline);
      if (isset($entries[1]) && !isset($test[$entries[1]])){
        $test[$entries[1]]['RELEASE']= array();

        /* Split releases */
        if (isset($entries[2])){
          $releases= split(",", $entries[2]);

          foreach ($releases as $release){
            $rname= preg_replace('/:.*$/', '', $release);
            $sections= split(':', preg_replace('/^[^:]+:([^|]+)|.*$/', '\1', $release));
            $classes= split('\|', preg_replace('/^[^|]+\|(.*)$/', '\1', $release));
            $test[$entries[1]]['RELEASE'][$rname]= array();
            $test[$entries[1]]['RELEASE'][$rname]['SECTION']= $sections;
            foreach ($classes as $class){
              if ($class != ""){
                $test[$entries[1]]['RELEASE'][$rname]['PACKAGES'][$class]= $class;
              }
            }
          }
        }
      }
    }
    
    $this->FAIServRepConfig= $test;

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

    /* Handle inheritance value "default" */
    $this->gotoBootKernels= array("%default%" => '['._("inherited").']');


    /* Load hardware list */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".$this->dn."))");
    if ($ldap->count() == 1){
      $map= array("gotoLdapServer","FAIclass","FAIdebianMirror");
      $attrs= $ldap->fetch();

      foreach ($map as $name){
        if (!isset($attrs[$name][0])){
          continue;
        }

        switch ($name){
          case 'gotoLdapServer':
            $this->goLdapServerList= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']' ), $this->goLdapServerList);
            break;

          case 'gotoBootKernel':
            $this->gotoBootKernels['%default%']=  _("inherited").' ['.$attrs[$name][0].']' ;
            break;

          case 'FAIclass':
            $str = split(":",$attrs[$name][0]);
            $this->InheritedFAIclass    = split("\ ",trim($str[0]));
            $this->InheritedFAIrelease  = trim($str[1]);
            break;

          case 'FAIdebianMirror':
            $this->InheritedFAIdebianMirror = $attrs[$name][0];
            break;
        }
      }
    }

    /* Get list of boot kernels */
    if (isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "KERNELS");

      if (!check_command($command)){
        $message[]= sprintf(_("Command '%s', specified as KERNELS hook for plugin '%s' doesn't seem to exist."), $command,
            get_class($this));
      } else {
        
        $fh= popen($command, "r");
        while (!feof($fh)) {
          $buffer= trim(fgets($fh, 256));
          if ($buffer != ""){
            $this->gotoBootKernels[preg_replace('/:.*$/', '', $buffer)]= $buffer;
          }
        }
        pclose($fh);
      }
    }

    if(count($this->FAIclass)==0 && $this->FAIrelease == ""){
      $this->FAIdebianMirror = "inherited";
    }
  }

  
  /* This class is called by the contrucktor ONLY.
   *   It return the available classes for each 
   *    Server / Release combination ... 
   *   (Release specifies which classes are available) 
   */
  function getAvailableClassesForThisRelease($release)
  {
    /* There could be more than one server providing this release,
        so use cached result if available
     */ 
    if(isset($_SESSION['getAvailableClassesForThisRelease_CACHE'][$release]))  {
      return($_SESSION['getAvailableClassesForThisRelease_CACHE'][$release]);
    }

    /* Create cache with all classes 
     */
    if((!isset($_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'])) ||
       (!is_array($_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'])) ||
       (count($_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES']) ==0 )){

      /* Get ldap connection */
      $ldap   = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);

      /* Get possible classes ... 
         This would be faste with some kind of caching ... 
       */
      $ldap->search("(|(objectClass=FAIpackageList)(objectClass=FAItemplate)(objectClass=FAIvariable)(objectClass=FAIscript)(objectClass=FAIhook)(objectClass=FAIprofile)(objectClass=FAIpartitionTable))",array("cn"),true);
      /* Sort all entries, and attach elementtype.
       * To be able to show the types in the listbox.
       */
      while($attr = $ldap->fetch()){
        $_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'][] = $attr;
      }  
    }

    /* Walk through cache and get out what we need.
     *   
     *   Function od : "$this->generateDNSyn($release)"
     *    It returns an array like this one :
     *       array("ou=packges,ou=rc0.9.2,ou=siga,", 
     *            "ou=scripts.. " 
     *             ...);
     *   This helps us to select the correct classes for each release. 
     *   It prevents errors like:  'siga' is selected as release, but all classes
     *     with ou='siga' in their dn are shown, also ou=rc...,ou=siga...  
     */
    $tmp2 = $this->generateDNSyn($release)     ; 
    $test2 = array();
    foreach($_SESSION['getAvailableClassesForThisRelease_CACHED_CLASSES'] as $attr){  
      foreach($tmp2 as $dns){
        if(preg_match("/".$dns."/",$attr['dn'])){
          $test2[$attr['cn'][0]] = $attr['cn'][0];
        }
      }
    }
    $_SESSION['getAvailableClassesForThisRelease_CACHE'][$release] = $test2;
    return($test2);
  }


  /*  Create array to display available classes/profiles in a selectbox 
   *   This function only displays the available classes.
   *   If a class is available is defined by these facts : 
   *     1. Is this class available for the selected release ?
   *       - if it is available, check if the release is available for the selected server 
   *         (done by $this->getFAIreleases())
   *     2. Is this class currently not assigned to $this->FAIclass
   */
  function selectFriendlyClasses(){
    $tmp=array();

    if($this->FAIdebianMirror == "inherited") return($tmp);

    /* check if the current release exists,
        else select the first one ..
     */
    $tmp2 = $this->getFAIreleases();
    if(!in_array($this->FAIrelease, $tmp2)){  
      $this->FAIrelease = key($tmp2);
    }
 
    /* Get all Packages for this server/release combination
     */
    if(!isset($this->FAIServRepConfig[$this->FAIdebianMirror]['RELEASE'][$this->FAIrelease]['PACKAGES'])){
      $pkgs = array();
      print_red(_("There are packages in your configuration, which can't be resolved with current server/release settings."));
    }else{
      $pkgs = $this->FAIServRepConfig[$this->FAIdebianMirror]['RELEASE'][$this->FAIrelease]['PACKAGES'];
    }

    /* Check each and every single class name 
     */
    foreach($pkgs as $pkg){
  
      /* Class already assigned to the classes list ?
       * If not ... go on
       */
      if(!in_array($pkg,$this->FAIclass)){
        
        /* Create the displayed list entry value
            HKLMOP [-Pl P V T-] or something like that 
         */
        $str = "";
        foreach($this->FAIclassInfo[$pkg] as $entry){
          if(isset($entry['kzl'])){
            $str .= $entry['kzl']." ";
          }
        }
        
        /* Append class if everyting was fine
         */        
        $tmp[$pkg] = $pkg." [-".trim($str)."-]";
      }
    }
    /* Just sort and return new classes list ...
       ( possibly we should cache the result ... )
     */
    natcasesort ($tmp);
    return($tmp);
  }

  function check()
  {
    $messages = array();
    
    /* Call common method to give check the hook */
    $messages= plugin::check();

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
    $smarty->assign("SelectBoxLdapServer","");

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
        if(count($this->gotoAvailableShares)){
          $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
          $s_mount = $_POST['gotoShareMountPoint'];
          /* Preparing the new assignment */
          $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
          $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
        }
      }
    }

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message
     */
    if((isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
      unset($this->gotoShares[$_POST['gotoShare']]);
    }

    $smarty->assign("gotoShares",$this->printOutAssignedShares());
    $smarty->assign("gotoSharesCount",count($this->printOutAssignedShares()));
    $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));
    $smarty->assign("gotoBootKernels",$this->gotoBootKernels);

    /* Arrays */
    $tmp = $this->goLdapServerList;

    /* Create divSelectBox for ldap server selection
     */
    $SelectBoxLdapServer = new divSelectBox("LdapServer");
    $SelectBoxLdapServer->SetHeight(130);

    /* Set first entry as selected, if $this->gotoLdapServer is empty
     *  or given entry is no longer available ... 
     */
    $found = false;
    foreach($tmp as $server){
      if($this->gotoLdapServer==$server){
        $found = true;
      }
    }

    /* Add Entries 
     */
    foreach($tmp as $key => $server){
      $use ="";
      if(($this->gotoLdapServer == $server) || ($found == false)) {
        $found = true;
        $use = " checked ";
      };

      $display = $server;

      $SelectBoxLdapServer->AddEntry(
          array(
            array("string"=>"<input type='radio' name='gotoLdapServer' value='".$key."' ".$use.">",
                  "attach"=>"style='border-left:0px;'"),
            array("string"=>$display)
            ));
    }    

    $smarty->assign("SelectBoxLdapServer",$SelectBoxLdapServer->DrawList());

    $smarty->assign("gotoLdapServerACL", chkacl($this->acl, "gotoLdapServer"));
    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      $smarty->assign("$val", $this->$val);
    }

    /* Values */
    foreach(array("gotoBootKernel", "customParameters", "gotoShare","FAIclasses","FAIclass","FAIdebianMirror","FAIrelease") as $val){
      $smarty->assign($val, $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    $smarty->assign("FAIdebianMirrors",$this->getFAIdebianMirrors());
    $smarty->assign("FAIreleases",$this->getFAIreleases());
    $smarty->assign("FAIrelease",$this->FAIrelease);
    $smarty->assign("FAIclasses",$this->selectFriendlyClasses());
    $smarty->assign("FAIclassesKeys",array_flip($this->selectFriendlyClasses()));
    $smarty->assign("FAIclassKeys",$this->FAIclass);
    $smarty->assign("InheritedFAIrelease",$this->InheritedFAIrelease);
    
    $div = new divSelectBox("WSFAIscriptClasses");
    $div -> SetHeight("110");
    $str_up     = " &nbsp;<input type='image' src='images/sort_up.png'    name='sort_up_%s'    value='%s'>";
    $str_down   = " &nbsp;<input type='image' src='images/sort_down.png'  name='sort_down_%s'  value='%s'>";
    $str_remove = " &nbsp;<input type='image' src='images/edittrash.png'  name='fai_remove_%s' value='%s'>";
    $str_empty  = " &nbsp;<img src='images/empty.png' alt=\"\" width='7'>"; 

    $i = 1;

    if($this->FAIdebianMirror == "inherited"){
      $tmp = $this->InheritedFAIclass;
    }else{
      $tmp = $this->FAIclass;
    }

    foreach($tmp as $class){

      if($this->FAIdebianMirror == "inherited"){
        $str = "";
      }else{
        if($i==1){
          $str = $str_empty.$str_down.$str_remove;
        }elseif($i == count($this->FAIclass)){
          $str = $str_up.$str_empty.$str_remove;
        }else{
          $str = $str_up.$str_down.$str_remove;
        }
      }
      $i ++ ; 

      $div->AddEntry(array(
            array("string"=>$class),
            array("string"=>preg_replace("/\%s/",base64_encode($class),$str),"attach"=>"style='width:50px;border-right:none;'")
            ));
    }  

    $smarty->assign("FAIScriptlist",$div->DrawList()); 

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

  function generateDNSyn($release)
  {
    $str = "";
    $tmp = split("\/",$release);
    $tmp = array_reverse($tmp);

    $base = ",ou=fai,";   
 
    $arr = array("scripts","hooks","disk","variables","templates","profiles","packages");

    foreach($tmp as $departmentname){
      
      $str .= ",ou=".$departmentname;
    }
    $ret = array();
    foreach($arr as $ar){
      $ret[] = ",ou=".$ar.$str.$base;
    }
    return($ret);
  }

  function getFAIdebianMirrors()
  {
    $ret = array();
    $ret['inherited']="["._("inherited")."]";
    $ret['auto']=_("automatic");
    $secs  = array();

    /* Walk through all available servers 
        and check if they support the currently selected classes
        if not, dont't add them to our list
     */
    foreach($this->FAIServRepConfig as $mirror => $rest){

      $use = false;

      if(count($this->FAIclass) == 0){
        $use = true;
      }else{
        $tmp = $this->getFAIreleases();
        foreach($tmp as $release){
          if(isset($rest['RELEASE'][$release])){
            $use =true;
          }
        } 
      }

      /* If current server, doesn't support this class
          remove it from list
       */
      if($use){
        $ret[$mirror] = $mirror;
      }
    }
    return($ret);
  }

  function getFAIreleases() 
  {
    $ret = array();

    if($this->FAIdebianMirror == "inherited") return(array());

    if(!isset($this->FAIServRepConfig[$this->FAIdebianMirror])){
      $this->FAIdebianMirror = "auto";
    }

    $errorClasses = "";  
  
    foreach($this->FAIServRepConfig[$this->FAIdebianMirror]['RELEASE'] as $release => $sections){
      $use = true;
      
      if(!count($this->FAIclass) == 0){
        foreach($this->FAIclass as $class){
          if(!in_array($class, $sections['PACKAGES'])){
            $use = false;
            $errorClasses[$class] = $class;
          }else{
            if(isset($errorClasses[$class])){
              unset($errorClasses[$class]); 
            }
          }
        }
      }
      if($use){
        $ret[$release]=$release;
      }
    } 
    if((count($ret) == 0 ) && ($this->FAIdebianMirror != "auto")){

      $eClasses = " ";
      foreach($errorClasses as $class){
        $eClasses .= $class." ";
      }

      print_red(sprintf(_("Can't resolve one or more of the given FAIclass(es) [%s] in FAI server '%s'. Server was reset to 'auto'."),$eClasses, $this->FAIdebianMirror));
      $this->FAIdebianMirror = "auto";
      return($this->getFAIreleases());
    }elseif((count($ret) == 0 ) && ($this->FAIdebianMirror == "auto")){

      $eClasses = " ";
      foreach($errorClasses as $class){
        $eClasses .= $class." ";
      }
      
      $this->FAIclass= array();
      print_red(sprintf(_("Can't resolve the given FAIclass(es) [%s] anyway, please check your FAI configurations, possibly some classes where deleted or renamed. !All classes have been removed from this account, press cancel if you don't want this to be saved."),$eClasses));
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
      $this->objectclasses = array("gotoWorkstationTemplate", "FAIobject");
    }elseif(isset($this->parent->by_object['workgeneric'])){
      $this->objectclasses = array("GOhard", "FAIobject");
    }elseif(isset($this->parent->by_object['servgeneric'])){
      $this->objectclasses = array("GOhard", "FAIobject");
    }else{
      print "Object Type Configuration : unknown";
      exit();
    }

    /* Find proper terminal path for tftp configuration
       FIXME: This is suboptimal when the default has changed to
       another location! */
    if (($this->gotoTerminalPath == "default")){
      $ldap= $this->config->get_ldap_link();

      /* Strip relevant part from dn, keep trailing ',' */
      $tmp= preg_replace("/^cn=[^,]+,ou=terminals,ou=systems,/i", "", $this->dn);
      $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

      /* Walk from top to base and try to load default values for
         'gotoTerminalPath'. Abort when an entry is found. */
      while (TRUE){
        $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

        $ldap->cat("cn=default,ou=terminals,ou=systems,$tmp".
            $this->config->current['BASE'], array('gotoTerminalPath'));
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
    $this->gotoKernelParameters= "ldap=".base64_encode($this->gotoLdapServer);

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

    if($this->FAIdebianMirror == "inherited"){
  
      $this->attrs['FAIclass'] = $this->attrs['FAIrelease'] =  $this->attrs['FAIdebianMirror'] = array(); 

    }else{

      foreach($this->FAIclass as $class){
        $str .= $class." ";
      }
      $str .= ":" . $this->FAIrelease;
      $this->attrs['FAIclass']= "";
      $this->attrs['FAIclass']= trim($str);

      if(empty($this->attrs['FAIclass'])){
        $this->attrs['FAIclass'] = array();
      }
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
      $this->attrs['gotoLdapServer']= array();
    }

    if (($this->attrs['gotoBootKernel'] == "default") || ($this->attrs['gotoBootKernel'] == "%default%")){
      $this->attrs['gotoBootKernel']= array();
    }

    /* if mirror == none stop saving this attribute */
    if($this->FAIdebianMirror == "none"){
      $this->FAIdebianMirror = "";
    }
   
    /* Get FAIstate from object, the generic tab could have changed it during execute */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("FAIstate"));
    $checkFAIstate = $ldap->fetch();

    /* Remove FAI objects if no FAI class is selected */ 
    if((count($this->FAIclass)==0) && (!isset($checkFAIstate['FAIstate']))){
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

    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving workstation startup settings failed"));
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


  function GetHookElements()
  {
    $ret = array();
    $cmd= search_config($this->config->data['TABS'], "servrepository", "REPOSITORY_HOOK");
    if(!empty($cmd)){
      $res = shell_exec($cmd);
      $res2 = trim($res);
      if((!$res)){
        print_red(sprintf(_("Can't execute specified REPOSITORY_HOOK '%s' please check your gosa.conf."),$cmd));
      }elseif(empty($res2)){
        print_red(sprintf(_("The specified REPOSITORY_HOOK '%s', specified in your gosa.conf, returns an empty string."),$cmd));
      }else{
        $tmp = split("\n",$res);
        foreach($tmp as $line){
          if(empty($line)) continue;
          $ret[]= $line;
        }
      }
    }
    return($ret);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
