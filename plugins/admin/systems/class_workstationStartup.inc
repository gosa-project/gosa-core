<?php
class workstartup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal startup options";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Ldap server list */
  var $gotoLdapServers    = array();
  var $gotoLdapServerList = array();
  var $gotoLdap_inherit   = FALSE;

  /* Generic terminal attributes */
  var $bootmode             = "G";
  var $gotoBootKernel       = "default-inherited";
  var $gotoKernelParameters = "";
  var $gotoLdapServer       = "default-inherited";
  var $gotoModules          = array();
  var $gotoAutoFs           = array();
  var $gotoFilesystem       = array();
  var $gotoTerminalPath     = "";
  var $gotoBootKernels      = array();

  /* attribute list for save action */
  var $attributes           = array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters", 
                                    "FAIclass", "FAIstatus", "gotoShare","FAIdebianMirror", "FAIrelease");
  var $objectclasses        = array("GOhard", "FAIobject");

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
  var $FAIclass           = array();  // The currently selected classes 
  var $FAIrelease         = "";
  var $FAIdebianMirror    = "auto";

  var $cache              = array(); // Used as cache in fai mehtods

  var $FAIstatus          = "";
  var $FAIclasses         = array();

  var $view_logged        = FALSE;
  
  /* FAI class selection */
  var $InheritedFAIclass       = array();
  var $InheritedFAIrelease     = "";
  var $InheritedFAIdebianMirror= "auto";

  var $CopyPasteVars    = array("gotoModules","gotoShares");
  var $fai_activated    = FALSE;
  var $o_group_dn       = "";
  var $member_of_ogroup = FALSE;

  function workstartup (&$config, $dn= NULL, $parent= NULL)
  {
    /* Check if FAI is active */
    $tmp= $config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $this->fai_activated = TRUE;
    }else{
      $this->attributes = array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters", "gotoShare");
      $this->objectclasses  = array("GOhard");
    }

    plugin::plugin ($config, $dn, $parent);

    /* Check object group membership */
    if(!isset($this->parent->by_object['ogroup'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".LDAP::prepare4filter($this->dn)."))",array("cn","dn"));
      if($ldap->count()){
        $this->member_of_ogroup = TRUE;
        $attrs = $ldap->fetch();
        $this->o_group_dn = $attrs['dn'];
      }
    }

    /* Creating a list of valid Mirrors 
     * none will not be saved to ldap.
     */
    $ldap   = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    foreach($this->config->data['SERVERS']['LDAP'] as $server) {
      $this->gotoLdapServerList[]= $server; 
    }

    /* Get list of assigned ldap servers 
     */ 
    if(isset($this->attrs['gotoLdapServer'])){
      unset($this->attrs['gotoLdapServer']['count']);
      sort($this->attrs['gotoLdapServer']);
      foreach($this->attrs['gotoLdapServer'] as $value){
        $this->gotoLdapServers[] = preg_replace("/^[0-9]*:/","",$value);
      }
    }
    if(!count($this->gotoLdapServers) && $this->member_of_ogroup){ 
      $this->gotoLdap_inherit = TRUE;
    }

    /* FAI Initialization
       Skip this if FAI is not activated 
     */
    if($this->fai_activated) {

      $this->update_fai_cache(TRUE);

      /* Parse used FAIclasses (stored as string).
       * The single classes are seperated by ' '.
       * There is also the release type given, after first
       *  occurrence of ':'.
       */
      $this->FAIclass =array();
      if(isset($this->attrs['FAIclass'][0])){
        $tmp = split(" ",$this->attrs['FAIclass'][0]);
        $tmp2 =array();  

        foreach($tmp as $class){
          if( ":" == $class[0] ) {
            $this->FAIrelease = trim(substr($class, 1));
          }else{
            $tmp2[$class] = $class;
          }
        }
        $this->FAIclass = $tmp2;
      }
    }

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
  

    $this->orig_dn= $this->dn;

    /* Handle inheritance value "default" */
    if ($this->member_of_ogroup){
      $this->gotoBootKernels= array("default-inherited" => '['._("inherited").']'); 
    }

    /* If we are member in an object group,
     *  we have to handle inherited values.
     * So you can see what is inherited.
     */
    if ($this->member_of_ogroup){

      if(count($this->FAIclass)==0 && $this->FAIrelease == ""){
        $this->FAIdebianMirror = "inherited";
      }

      if($this->fai_activated){
        $map= array("gotoBootKernel","FAIclass","FAIdebianMirror");
      }else{
        $map= array("gotoBootKernel");
      }

      $ldap = $this->config->get_ldap_link();
      $ldap->cat($this->o_group_dn);
      $attrs= $ldap->fetch();

      foreach ($map as $name){
        if (!isset($attrs[$name][0])){
          continue;
        }

        switch ($name){
          case 'gotoBootKernel':
            $this->gotoBootKernels['default-inherited']=  _("inherited").' ['.$attrs[$name][0].']' ;
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


    if($this->fai_activated){

      /* Check if the current mirror is available 
       */
      if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror])){
        if(count($this->FAIclass)){
          print_red(sprintf(_("The selected FAI mirror '%s' is no longer available, the mirror was set to 'auto'."),
                $this->FAIdebianMirror));
        }
        $this->FAIdebianMirror = "auto";
        $this->FAIrelease = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
        $this->cache =array();
        $this->update_fai_cache();
        
      }
  
      /* Check if the current mirror is available 
       */
      if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){
        $new_release = key($this->cache['SERVERS'][$this->FAIdebianMirror]); 
        if(count($this->FAIclass)){
          print_red(sprintf(_("The selected FAI release '%s' is not available for mirror '%s', the release was set to '%s'."),
                $this->FAIrelease,$this->FAIdebianMirror,$new_release));
        }
        $this->FAIrelease = $new_release;
        $this->cache =array();
        $this->update_fai_cache();
      }
    }

    /* Get list of boot kernels */
    if (isset($this->config->data['TABS'])){
      $command= $this->config->search(get_class($this), "KERNELS",array('tabs'));
      if (!check_command($command)){
        $message[]= sprintf(_("Command '%s', specified as KERNELS hook for plugin '%s' doesn't seem to exist."), $command,
            get_class($this));
      } else {
        $fh= popen($command, "r");
        while (!feof($fh)) {
          $buffer= trim(fgets($fh, 256));
          if(!empty($buffer)){
            $name=$value = $buffer;
            if(preg_match("/:/",$buffer)){
              $name = preg_replace("/:.*$/","",$buffer);
              $value= preg_replace("/^.*:/","",$buffer);
              $this->gotoBootKernels[$name]= $name.":".$value;
            }else{
              $this->gotoBootKernels[$name]= $value;
            }
          }
        }
        pclose($fh);
      }
    }

    /* Turn to default, if we've nothing to inherit */
    if (!isset($this->gotoBootKernels['default-inherited']) && $this->gotoBootKernel == "default-inherited"){
      $this->gotoBootKernel= "default";
    }
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

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","workstation/".get_class($this),$this->dn);
    }

    /* Do we need to flip is_account state? */
    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return ($display);
    }

    /* Add module */
    if (isset ($_POST['add_module'])){
      if ($_POST['module'] != "" && $this->acl_is_writeable("gotoModule")){
        $this->add_list ($this->gotoModules, $_POST['module']);
      }
    }

    /* Delete module */
    if (isset ($_POST['delete_module'])){
      if (count($_POST['modules_list']) && $this->acl_is_writeable("gotoModule")){
        $this->del_list ($this->gotoModules, $_POST['modules_list']);
      }
    }

    /* FAI class management */
    if($this->fai_activated){
      if(((isset($_POST['AddClass']))&&(isset($_POST['FAIclassesSel']))) && ($this->acl_is_writeable("FAIclass"))){
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

      /* Move one used class class one position up or down */
      if($this->acl_is_writeable("FAIclass")){
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
      }

      /* Delete selected class from our list */
      if($this->acl_is_writeable("FAIclass")){
        if((isset($_POST['DelClass']))&&(isset($_POST['FAIclassSel']))){
          if(isset($this->FAIclass[$_POST['FAIclassSel']])){
            unset($this->FAIclass[$_POST['FAIclassSel']]);
          }
        }
      }
    }// END fai handling

    /* Show main page */
    $smarty= get_smarty();

    /* Assign ACLs to smarty */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    } 

    $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

    /* In this section server shares will be defined
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */
    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry
     * This entry will be, a combination of mountPoint and sharedefinitions
     */
    if((isset($_POST['gotoShareAdd'])) && ($this->acl_is_writeable("gotoShare"))) {
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
    if(($this->acl_is_writeable("gotoShare"))&& (isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
      unset($this->gotoShares[$_POST['gotoShare']]);
    }

    $smarty->assign("gotoShares",$this->printOutAssignedShares());
    $smarty->assign("gotoSharesCount",count($this->printOutAssignedShares()));
    $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));
    $smarty->assign("gotoBootKernels",$this->gotoBootKernels);

    /* Create divSelectBox for ldap server selection
     */
    $SelectBoxLdapServer = new divSelectBox("LdapServer");
    $SelectBoxLdapServer->SetHeight(130);

    /* Add new ldap server to the list */
    if(!$this->gotoLdap_inherit && isset($_POST['add_ldap_server']) && isset($_POST['ldap_server_to_add'])){
      if(isset($this->gotoLdapServerList[$_POST['ldap_server_to_add']])){
        $to_add = $this->gotoLdapServerList[$_POST['ldap_server_to_add']];
        if(!in_array($to_add,$this->gotoLdapServers)){
          $this->gotoLdapServers[] = $to_add;
        }
      }
    }
    
    /* Move ldap servers up and down */
    if(!$this->gotoLdap_inherit){
      foreach($_POST as $name => $value){
        if(preg_match("/sort_ldap_up_/",$name)){
          $id = preg_replace("/^sort_ldap_up_([0-9]*)_(x|y)$/","\\1",$name);
          $from =  $id;  
          $to   =  $id -1;
          $tmp = $this->array_switch_item($this->gotoLdapServers,$from,$to);
          if($tmp){
            $this->gotoLdapServers = $tmp;
          }
          break;
        }
        if(preg_match("/sort_ldap_down_/",$name)){
          $id = preg_replace("/^sort_ldap_down_([0-9]*)_(x|y)$/","\\1",$name);
          $from =  $id;  
          $to   =  $id +1;
          $tmp = $this->array_switch_item($this->gotoLdapServers,$from,$to);
          if($tmp){
            $this->gotoLdapServers = $tmp;
          }
          break;
        }
        if(preg_match("/gotoLdapRemove_/",$name)){
          $id = preg_replace("/^gotoLdapRemove_([0-9]*)_(x|y)$/","\\1",$name);
          $value = $this->gotoLdapServers[$id];
          $this->gotoLdapServers = array_remove_entries(array($value),$this->gotoLdapServers);
          break;
        }
      } 
    }
  
    /* Add Entries 
     */
    foreach($this->gotoLdapServers as $key => $server){
      if(!in_array($server,$this->gotoLdapServerList)){
        $server = $server."&nbsp;<font style='color:red'>(missing)</font>";
      }

      $SelectBoxLdapServer->AddEntry(
          array(array("string" => $server),
            array("string" => 
              "<input class='center' type='image' src='images/sort_up.png' name='sort_ldap_up_".$key."'>&nbsp;".
              "<input class='center' type='image' src='images/sort_down.png' name='sort_ldap_down_".$key."'>&nbsp;".
              "<input class='center' type='image' src='images/edittrash.png' name='gotoLdapRemove_".$key."'>",
              "attach" => "style='text-align:right;width:40px;border-right:0px;'")));
    }    

    if($this->gotoLdap_inherit){
      $smarty->assign("gotoLdapServerACL_inherit", preg_replace("/w/","",$this->getacl("gotoLdapServer")));;
    }else{
      $smarty->assign("gotoLdapServerACL_inherit", $this->getacl("gotoLdapServer"));
    }
    
    $list = array();
    foreach($this->gotoLdapServerList as $key => $entry){
      if(!in_array($entry,$this->gotoLdapServers)){
        $list[$key] = $entry;
      }
    }
    $smarty->assign("gotoLdapServers",    $SelectBoxLdapServer->DrawList());
    $smarty->assign("gotoLdapServerList", $list);
    $smarty->assign("gotoLdap_inherit",   $this->gotoLdap_inherit);
    $smarty->assign("JS",  session::get('js'));

    foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
      $smarty->assign("$val", $this->$val);
    }

    /* Values */
    foreach(array("gotoBootKernel", "customParameters", "gotoShare","FAIclasses","FAIclass","FAIdebianMirror","FAIrelease") as $val){
      $smarty->assign($val, $this->$val);
    }

    $smarty->assign("fai_activated",$this->fai_activated);

    /* Create FAI output */
    if($this->fai_activated){

      $this->update_fai_cache();

      $smarty->assign("FAIservers"  , $this->cache['SERVERS']);
      $smarty->assign("FAIdebianMirror",$this->FAIdebianMirror);
      $smarty->assign("FAIrelease"  , $this->FAIrelease);
      $smarty->assign("FAIclasses"  , $this->selectable_classes());

      $smarty->assign("InheritedFAIrelease",$this->InheritedFAIrelease);

      $div = new divSelectBox("WSFAIscriptClasses");
      $div -> SetHeight("110");
      $str_up     = " &nbsp;<input type='image' src='images/sort_up.png'    name='sort_up_%s'    value='%s'>";
      $str_down   = " &nbsp;<input type='image' src='images/sort_down.png'  name='sort_down_%s'  value='%s'>";
      $str_remove = " &nbsp;<input type='image' src='images/edittrash.png'  name='fai_remove_%s' value='%s'>";
      $str_empty  = " &nbsp;<img src='images/empty.png' alt=\"\" width='7'>"; 

      /* Get classes */
      if($this->FAIdebianMirror == "inherited"){
        $tmp = $this->InheritedFAIclass;
      }else{
        $tmp = $this->FAIclass;
      }

      /* Get invalid classes */
      $invalid = $this->get_invalid_classes($tmp);

      /* Draw every single entry */
      $i = 1;
      foreach($tmp as $class){

        /* Mark invalid classes. (Not in selected release)
         */
        $marker = "";
        if(in_array_ics($class,$invalid)){
          $marker = "&nbsp;<font color='red'>("._("Not available in current setup").")</font>";
        }

        /* Create up/down priority icons  
         * Skip this, if we have inherited the FAI classes.
         */
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

        /* Get Description tag 
         *  There may be several FAI objects with the same class name, 
         *   use the description from FAIprofile, if possible.
         */  
        $desc = ""; 
        if(isset($this->cache['CLASSES'][$this->FAIrelease][$class])){
          foreach($this->cache['CLASSES'][$this->FAIrelease][$class] as $types ){
            if(isset($types['Desc'])){
              $desc= $types['Desc'];
              if($types['Type'] == "FAIprofile"){
                break;
              }
            }
          }
        }
        if(!empty($desc)){
          $desc = "&nbsp;[".trim($desc)."]";
        }        

        $div->AddEntry(array(
              array("string"=>$class.$desc.$marker),
              array("string"=>preg_replace("/\%s/",base64_encode($class),$str),"attach"=>"style='width:50px;border-right:none;'")
              ));
      }  
      $smarty->assign("FAIScriptlist",$div->DrawList()); 
    }// END FAI output generation 

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

    /* Show main page */
    return($smarty->fetch (get_template_path('workstationStartup.tpl', TRUE,dirname(__FILE__))));
  }


  function remove_from_parent()
  {
    $this->handle_post_events("remove");
    new log("remove","workstation/".get_class($this),$this->dn);
  }


  /* Save data to object */
  function save_object()
  {
    $old_mirror  = $this->FAIdebianMirror;
    plugin::save_object();

    /* Update release */
    if($old_mirror != $this->FAIdebianMirror){
      if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){
        $this->FAIrelease      = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
      }
    }

    if(isset($_POST['WorkstationStarttabPosted'])){
      if(isset($_POST['gotoLdap_inherit'])){
        $this->gotoLdap_inherit = TRUE;
      }else{
        $this->gotoLdap_inherit = FALSE;
      }

      /* Save group radio buttons */
      if ($this->acl_is_writeable("bootmode") && isset($_POST["bootmode"])){
        $this->bootmode= $_POST["bootmode"];
      }

      /* Save kernel parameters */
      if ($this->acl_is_writeable("gotoKernelParameters") && isset($_POST["customParameters"])){
        $this->customParameters= $_POST["customParameters"];
      }
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
      $this->objectclasses = array("gotoWorkstationTemplate");
    }elseif(isset($this->parent->by_object['workgeneric'])){
      $this->objectclasses = array("GOhard");
    }elseif(isset($this->parent->by_object['servgeneric'])){
      $this->objectclasses = array("GOhard","gotoWorkstationTemplate");
    }else{
      print "Object Type Configuration : unknown";
      exit();
    }

    /* Append FAI class */
    if($this->fai_activated){
      $this->objectclasses[]  = "FAIobject";
    }

    /* Find proper terminal path for tftp configuration
       FIXME: This is suboptimal when the default has changed to
       another location! */
    if (($this->gotoTerminalPath == "default")){
      $ldap= $this->config->get_ldap_link();

      /* Strip relevant part from dn, keep trailing ',' */
      $tmp= preg_replace("/^cn=[^,]+,".get_ou('terminalou')."/i", "", $this->dn);
      $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

      /* Walk from top to base and try to load default values for
         'gotoTerminalPath'. Abort when an entry is found. */
      while (TRUE){
        $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

        $ldap->cat("cn=default,".get_ou('terminalou').$tmp.
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

    /* Skip FAI attribute handling if not necessary */
    if($this->fai_activated){
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
    }
  
    /* Add missing arrays */
    foreach (array("gotoFilesystem", "gotoAutoFs", "gotoModules") as $val){
      if (isset ($this->$val) && count ($this->$val) != 0){
    
        $this->attrs["$val"]= array_unique($this->$val);
      }
      if(!isset($this->attrs["$val"])) $this->attrs["$val"]=array();
    }

    /* Prepare list of ldap servers */
    $this->attrs['gotoLdapServer'] = array();
    if(!$this->gotoLdap_inherit){
      $i = 0;
      foreach($this->gotoLdapServers as $server){
        $i ++;
        $this->attrs['gotoLdapServer'][] = $i.":".$server;
      }
    }

    if (($this->attrs['gotoBootKernel'] == "default-inherited") || ($this->attrs['gotoBootKernel'] == "%default%")){
      $this->attrs['gotoBootKernel']= array();
    }

    /* if mirror == none stop saving this attribute */
    if($this->FAIdebianMirror == "none"){
      $this->FAIdebianMirror = "";
    }
   
    /* Get FAIstate from object, the generic tab could have changed it during execute */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);


    /* Skip FAI attribute handling if not necessary */
    if($this->fai_activated){
      $ldap->cat($this->dn,array("FAIstate"));
      $checkFAIstate = $ldap->fetch();

      /* Remove FAI objects if no FAI class is selected */ 
      if((count($this->FAIclass)==0) && (!isset($checkFAIstate['FAIstate']))){
        $this->attrs['FAIclass']        = array();
        $this->attrs['FAIdebianMirror'] = array();
      }
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
    new log("modify","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system workstation/startup with dn '%s' failed."),$this->dn));
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



  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);    
    $source_o = new workstartup ($this->config, $source['dn']);
    foreach(array("FAIclass","gotoModules", "gotoAutoFs", "gotoFilesystem",
          "gotoKernelParameters","gotoShares","customParameters") as $attr){
      $this->$attr = $source_o->$attr;
    }
  }

  
  function array_switch_item($ar,$from,$to)
  {
    if(!is_array($ar)){
      return(false);
    }
    if(!isset($ar[$from])){
      return(false);
    }
    if(!isset($ar[$to])){
      return(false);
    }

    $tmp = $ar[$from];
    $ar[$from] = $ar[$to];    
    $ar[$to] = $tmp;    
    return($ar);
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName"   => _("Startup"),
          "plDescription" => _("System startup"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 9,
          "plSection"     => array("administration"),           
          "plCategory"    => array("workstation","server","ogroups"),

          "plProvidedAcls"=> array(
            "gotoLdapServer"        => _("Ldap server"),
            "gotoBootKernel"        => _("Boot kernel"),
            "gotoKernelParameters"  => _("Kernel parameter"),

            "gotoModules"           => _("Kernel modules"),
            "gotoShare"             => _("Shares"),

            "FAIclass"              => _("FAI classes"),
            "FAIdebianMirror"       => _("Debian mirror"),
            "FAIrelease"            => _("Debian release"),

            "FAIstatus"             => _("FAI status flag")) // #FIXME is this acl realy necessary ?
          ));
  }


  /* Updates release dns 
   *  and reads all classes for the current release, 
   *  if not already done ($this->cache).
   */
  function update_fai_cache($first_call = FALSE)
  {
    $force = FALSE;

    /* Get the list of available servers and their releases. 
     */
    if($force || !isset($this->cache['SERVERS'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(FAIrepository=*)(objectClass=FAIrepositoryServer))",array("FAIrepository"));
      $this->cache['SERVERS'] = array();

      /* Only add inherit option, if we are part in an object group
       */
      if($this->member_of_ogroup){
        $this->cache['SERVERS']['inherited']=array();
      }

      /* Add auto value  
       */
      $this->cache['SERVERS']['auto'] = array();
      $sort_by = array("auto");
      $server_tmp = array("auto"=>array());
      while($attr = $ldap->fetch()){
        if(isset($attr['FAIrepository'])){
          for($i = 0 ; $i < $attr['FAIrepository']['count'] ; $i ++ ){
            $rep = $attr['FAIrepository'][$i];
            $tmp = split("\|",$rep);
            if(count($tmp)==4){
              $sections = split(",",$tmp[3]);
              $release  = $tmp[2];
              $server   = $tmp[1];
              $url      = $tmp[0];
              $server_tmp[$url][$release]=$release;
              $server_tmp['auto'][$release]=$release;
              $sort_by[$url] = $url;
            }
          }
        }
      }
      natcasesort($sort_by);
      foreach($sort_by as $name){
        $releases = $server_tmp[$name];
        natcasesort($releases);
        $this->cache['SERVERS'][$name] = $releases; 
      }
    }

    /* Build up arrays, without checks */
    if(!$first_call){

      /* Check if the selected mirror is available */
      if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror])){
        $this->FAIdebianMirror = "auto";
        $this->FAIrelease      = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
        trigger_error("There was a problem with the selected FAIdebianMirror. This mirror ('".$this->FAIdebianMirror."') is not available");
      }

      /* Check if the selected release is available */
      if($this->FAIdebianMirror != "inherited" && !isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){
        trigger_error("There was a problem with the selected FAIrelease. This release ('".$this->FAIrelease."') is not available");
        $this->FAIrelease = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
      }
    }

    /* Get classes for release from cache. 
     * Or build cache
     */
    if($this->FAIdebianMirror == "inherited"){
      $release = $this->InheritedFAIrelease;
    }else{
      $release = $this->FAIrelease;
    }
    if($force || !isset($this->cache['CLASSES'][$release])){

      /*  Create a list of available releases.
       *  $this->cache['RELEASE_DNS'][ou=siga...,c=de]         = "siga";
       *  $this->cache['RELEASE_DNS'][ou=siga,ou=rc1,...,c=de] = "siga/rc1";
       */
      if($force || !isset($this->cache['RELEASE_DNS'])){
        $this->cache['RELEASE_DNS'] = array();
        $fai_ou_parts = preg_replace("/\/.*$/","",$this->FAIrelease);
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(objectClass=FAIbranch)",array("ou"));
        while($attrs = $ldap->fetch()){
          if(preg_match("/".normalizePreg(get_ou("faiou"))."/",$attrs['dn'])){
            $this->cache['RELEASE_DNS'][$attrs['dn']] = $this->dn_to_release_name($attrs['dn']);
          }
        }
      }

      /* Create list of available classes for the currenlty selected release.
       */
      $base = array_search($release,$this->cache['RELEASE_DNS']);
      $this->cache['CLASSES'][$release] = array();

      if(class_exists("FAI")){
        if(!empty($base)){
          $filter = "(|(objectClass=FAIpackageList)(objectClass=FAItemplate)(objectClass=FAIvariable)".
            "(objectClass=FAIscript)(objectClass=FAIhook)(objectClass=FAIprofile)".
            "(objectClass=FAIpartitionTable))";
          $list = FAI::get_all_objects_for_given_base($base,$filter,TRUE);
          foreach($list as $attrs){
            $info = $this->analyse_fai_object($attrs);
            if(count($info)){
              $this->cache['CLASSES'][$release][$attrs['cn'][0]][] = $info;
            }
          }
        }
      }else{
        print_red(_("Could not load available classes, the fai extension plugin seems not to be installed."));
      }

      /* Add object caught from external hook
       */
      $lines= $this->GetHookElements();
      foreach ($lines as $hline){
        $entries= split(";", $hline);
        $server = $entries['0'];
        $url    = $entries['1'];
        if (!empty($url)){
          
          /* Split releases */
          if (isset($entries[2])){
            $releases= split(",", $entries[2]);

            foreach ($releases as $release_data){
              $release= preg_replace('/:.*$/', '', $release_data);
              $sections = split(':', preg_replace('/^[^:]+:([^|]+)|.*$/', '\1', $release_data));
              $classes  = split('\|', preg_replace('/^[^|]+\|(.*)$/', '\1', $release_data));
              $this->cache['SERVERS'][$url][$release]=$release;
              $this->cache['SERVERS']['auto'][$release]=$release; 
              foreach ($classes as $class){
                if ($class != ""){
                  $this->cache['CLASSES'][$release][$class]= array();
                }
              }
            }
          }
        }
      }
    }
  }


  /* This function return an array containing all 
   *  invalid classes for the selected server/release
   */
  function get_invalid_classes($classes)
  {
    $this->update_fai_cache();
    if($this->FAIdebianMirror == "inherited"){
      $release_classes = $this->cache['CLASSES'][$this->InheritedFAIrelease];
    }else{
      $release_classes = $this->cache['CLASSES'][$this->FAIrelease];
    }


    /* Detect all classes that are not valid 
     *  for the selected release 
     */
    $NA = array();
    foreach($classes as $class){
      if(!isset($release_classes[$class])){
        $NA[] = $class;
      }
    }
    return($NA);
  }  

  
  /* Get all selectable classes for the ui select box
   */
  function selectable_classes()
  {
    $this->update_fai_cache();

    if($this->FAIdebianMirror == "inherited"){
      $classes = $this->cache['CLASSES'][$this->InheritedFAIrelease];
    }else{
      $classes = $this->cache['CLASSES'][$this->FAIrelease];
    }

    $Abbr ="";
    $ret= array();
    foreach($classes as $class_name => $class_types){
      if(!in_array($class_name,$this->FAIclass)){
        foreach($class_types as $type){
          if(!preg_match("/".$type['Abbr']."/",$Abbr)){
            $Abbr .= $type['Abbr']." ";
          }
        }
        $ret[$class_name] = trim($Abbr);
      }
    }
    uksort($ret, 'strnatcasecmp');
    return($ret);
  }


  /* Analyse FAI object and return an array with usefull informations like 
   *  FAIobject type.
   */
  function analyse_fai_object($attr)
  {
    $tmp2 = array();
    if(!isset($attr['description'])){
      $attr['description'][0] ="";
    }
    if(in_array('FAIpackageList',$attr['objectClass'])){
      $tmp2["Type"]   = 'FAIpackageList'; 
      $tmp2["Abbr"]   = 'Pl';
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAItemplate',$attr['objectClass'])){
      $tmp2["Type"]      = 'FAItemplate'; 
      $tmp2["Abbr"]      = 'T'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAIvariable',$attr['objectClass'])){
      $tmp2["Type"]      = 'FAIvariable'; 
      $tmp2["Abbr"]      = 'V'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAIscript',$attr['objectClass'])){
      $tmp2["Type"]        = 'FAIscript'; 
      $tmp2["Abbr"]        = 'S'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAIhook',$attr['objectClass'])){
      $tmp2["Type"]          = 'FAIhook'; 
      $tmp2["Abbr"]          = 'H'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAIpartitionTable',$attr['objectClass'])){
      $tmp2["Type"]= 'FAIpartitionTable'; 
      $tmp2["Abbr"]= 'Pt'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    if(in_array('FAIprofile',$attr['objectClass'])){
      $tmp2["Type"]= 'FAIprofile'; 
      $tmp2["Abbr"]= 'P'; 
      $tmp2["Desc"]  = $attr['description'][0];
    }
    return($tmp2);
  }


  /* Return repository hook output, if possible.
   */
  function GetHookElements()
  {
    $ret = array();
    $cmd= $this->config->search("servrepository", "REPOSITORY_HOOK",array('tabs'));
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


  /* This function creates the release name out of a dn 
   *  e.g. "ou=1.0rc2,ou=siga,ou=fai,..." => "siga/1.0rc2"
   */
  function dn_to_release_name($dn)
  {
    $relevant = preg_replace("/,".normalizePreg(get_ou("faiou")).".*$/","",$dn);
    $parts    = array_reverse(split("\,",$relevant));
    $str ="";
    foreach($parts as $part){
      $str .= preg_replace("/^ou=/","",$part)."/";
    }
    return(preg_replace("/\/$/","",$str)); 
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
