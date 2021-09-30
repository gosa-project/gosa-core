<?php
class appgroup extends plugin
{
    var $config;
    var $curbase;

    /* Contains the menu structure in an array.
     */
    var $a_Structure= array();
    var $a_Structure_on_load = array();
    var $Releases;
    var $FAIrelease = 0;
    var $apps = array();
    var $_cache = array();

    var $app_parameter = array();
    var $edit_entry    = array();
    var $enableReleaseManagement = FALSE;

    var $copied_release = ""; 


    public function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct($config,$dn,$parent);
        $this->dn = $dn; 
        $this->_load_menu_structure();
        $this->a_Structure_on_load = $this->a_Structure;

        /* Check if we have relase mangement enabled and prepare group application for release management */
        $this->enableReleaseManagement  = $this->config->pluginEnabled("faiManagement");
        $this->Releases   = $this->getReleases();

        /* Set intial release */
        $this->FAIrelease = $config->get_cfg_value("faiManagement","defaultFaiRelease");
        if(empty($this->FAIrelease) || !isset($this->Releases[$this->FAIrelease])){
            $this->FAIrelease = "/";
        }

        $this->curbase    = $this->config->current['BASE'];
        $this->reload();

        /* If we have at least one assigned application-
           Enable this account.
         */ 
        $this->is_account = FALSE;
        if(count($this->_get_all_entries()) > 1){
            $this->is_account= TRUE;
        }   
        $this->initially_was_account = $this->is_account;

        // Prepare lists
        $this->applicationList = new sortableListing();
        $this->applicationList->setDeleteable(false);
        $this->applicationList->setEditable(true);
        $this->applicationList->setWidth("100%");
        $this->applicationList->setHeight("300px");
        $this->applicationList->setColspecs(array('20px','20px','*'));
        $this->applicationList->setHeader(array("&nbsp;","&nbsp;",_("Application")));
        $this->applicationList->setDefaultSortColumn(2);
        $this->applicationList->setAcl('rwcdm'); // All ACLs, we filter on our own here.

    }


    /*! \brief Reload the list of applications for the currently selected release.
      If necessary, maybe the list is already cached.
     */
    function reload()
    {
        $ret = array();
        $release_info = $this->Releases[$this->FAIrelease];

        /* Check if the available application were already been fetched.
           If not, build up a list of available applications.
         */  
        if(!isset($this->_cache['ReleaseApps'][$release_info['suffix']])){

            /* First of all, get all application departments to search in.
             */  
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("ou=apps",array("dn"));
            $app_deps = array();
            while($attrs = $ldap->fetch()){
                $app_deps[] = $attrs['dn'];
            }

            /* Search all release departments for the above fetched application departments
             */
            foreach($app_deps as $dep){
                $ldap->cd($dep);
                $ldap->search("objectClass=FAIbranch",array("dn"));
                while($attrs = $ldap->fetch()){
                    $app_deps[] = $attrs['dn'];
                }
            }

            /* Filter out those releases that match the currently selected release, 
               and fetch all applications from those departments.
             */
            foreach($app_deps as $dep){
                if(preg_match("/^".preg_quote($release_info['suffix'], '/')."/",$dep)){
                    $ret = array_merge($ret,get_list("(objectClass=gosaApplication)","application",$dep,array("*"),GL_NONE));
                }
            }

            /* Create a new array containing all fetch apps for the currently selected release,
               sort it and store results in cache. 
             */ 
            $tmp = array();
            foreach($ret as $key => $app){
                $tmp[$key] = $app['cn'][0];
            }
            natcasesort($tmp);
            $res = array();
            foreach($tmp as $key => $app){
                $res[] = $ret[$key];
            }

            $this->_cache['ReleaseApps'][$release_info['suffix']] = $res;
        } 
        $this->apps = $this->_cache['ReleaseApps'][$release_info['suffix']];
    }



    /*! \brief generate a list of available releases
      @return return an array with all available releases.

      e.g.

      /     "name"    /
      "found"   1
      "parts"   Array (empty)
      "suffix"  ou=apps,

      halut "name"    halut
      "found"   1
      "FAIstate"  
      "dn"      ou=halut,ou=apps,ou=Direktorium,o=Landeshauptstadt München,c=de
      "parts"   0 halut
      "suffix"  ou=halut,ou=apps,

      This will be used as base for the application menu structure.
      If there is a menu assigned for a release, this menu will be 
      appended with the index 'ENTRIES';
     */
    function getReleases()
    {
        $ret =array("/" => array("name" => "/", "found" => TRUE , "parts" => array(),"suffix" => get_ou("application", "applicationRDN")));
        if($this->enableReleaseManagement){

            /* Only display those releases that we are able to read 
             */
            $dn     = get_ou("application", "applicationRDN").$this->config->current['BASE'];
            $filter = "(&(objectClass=organizationalUnit)(objectClass=FAIbranch))";
            $res    = get_sub_list($filter,array("application","fai"), 
                    array(get_ou("application", "applicationRDN"),get_ou("faiManagement", "faiBaseRDN")),$dn, array("ou","FAIstate"), GL_SUBSEARCH);

            /* Go through all departments and check which department is a valid 
               department release.
             */
            foreach($res as $attrs){
                if(preg_match("/".get_ou("application", "applicationRDN")."/i",$attrs['dn'])){

                    /* Parse all returned departments dns into a useable type.
                       (ou=1.0.0,ou=halut,ou=apps  ==> halue/1.0.0)
                     */
                    $bb     = preg_replace("/".preg_quote(get_ou("application", "applicationRDN"), '/').".*/i","",$attrs['dn']);
                    $parts  = array_reverse(explode("ou=",$bb));

                    $str ="";
                    foreach($parts as $key => $part){
                        if(empty($part)) {
                            unset($parts[$key]);
                            continue;
                        }
                        $part = str_replace(",","",$part);
                        $str .= $part."/";
                        $parts[$key] = $part;
                    }
                    $name = preg_replace("/\/$/","",$str);
                    if(empty($name)) {
                        $name ="/";
                    }

                    $FAIstate = "";
                    if(isset($attrs['FAIstate'])){
                        $FAIstate = $attrs['FAIstate'][0];
                    }

                    /* Check if this department has a menu structure assigned 
                     */
                    $all = $this->_get_all_entries();
                    $found = FALSE;
                    foreach($all as $entry){
                        if(isset($entry['DN']) && preg_match("/^".preg_quote($bb, '/')."/",$entry['DN'])){
                            $found =TRUE;
                            break;
                        }
                    }

                    $ret[$name] = array("name"     => $name, 
                            "found"    => $found,
                            "FAIstate" => $FAIstate,
                            "dn"       => $attrs['dn'], 
                            "parts"    => $parts,"suffix" => $bb.get_ou("application", "applicationRDN"));
                }
            }
        }
        ksort($ret);
        return($ret);
    }


    /*! \brief Load the menu structure from ldap and create a multi dimensional array.

      This will create a multidimensional array.
      e.g.:

      $this->a_Structure =

      [0]['TYPE']   = "BASE"
      [0]['ENTRIES']  [0]['TYPE']   = "RELEASE"
      [0]['NAME']   = "halut"
      [0]['ENTRIES']= array()
      ...
      [0]['ENTRIES']  [1]['TYPE']   = "RELEASE"
      [1]['NAME']   = "halut/1.0.0"
      [1]['ENTRIES']  [0]['TYPE']   = "TYPE"
      [0]['NAME']   = "Programme"
      [0]['ENTRIES'][0]['TYPE'] = "ENTRY"
      [0]['NAME'] = "konqueror"
      [1]['TYPE'] = "ENTRY"
      [1]['NAME'] = "firefox"
     */
    function _load_menu_structure()
    {
        /* Create the base object
         */
        $base =  array();
        $base['UNIQID'] = uniqid();
        $base['PARENT'] = 0; 
        $base['NAME']   = "";
        $base['TYPE']   = "BASE";
        $base['ENTRIES']= array();
        $base['STATUS'] = "LOADED";

        $this->a_Structure  = array();
        $this->a_Structure[0] = $base;

        /* Search for all Releases/Menu Folders and Menu Entries,
           to append them to our structure array.
         */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->search("(|(objectClass=gotoSubmenuEntry)(objectClass=FAIbranch)(objectClass=gotoMenuEntry))",array("*"));
        while($attrs = $ldap->fetch()){

            /* Find the correct position where to add this entry.
               e.g. If we have cn=firefox,cn=Programme,ou=halut...

               1. get a pointer to the halut array ($this->a_Structure['0'][ENTRIES''][]['halut'])
               2. then get a pointer to the halut['ENTRIES'][]['Programme'] array. 
               3. append 'firefox' to the 'ENTRIES' of our "Programme" pointer.
             */
            $cur          = &$this->a_Structure[0]['ENTRIES'];
            $parent_id    = $base['UNIQID'];
            $sub_dn       = preg_replace("/,".preg_quote($this->dn, '/')."$/","",$attrs['dn']);
            $sub_dn_array = explode(",",$sub_dn);

            /* Walk through our menu structure array while we have found 
               the correct position where to add this object. 
             */
            $found = true;
            for($i = (count($sub_dn_array)-1) ; $i >= 0 ; $i--){
                $name = preg_replace("/^[^=]*+=/","",$sub_dn_array[$i]);

                /* We haven't found the end node where this object has to be added
                 */
                if($i > 0){
                    $found =FALSE;
                    foreach($cur as $key => $entry){
                        if($entry['NAME'] == $name){
                            $cur = &$cur[$key]['ENTRIES'];
                            $parent_id = $entry['UNIQID'];
                            $found =true;
                            break;
                        }
                    }
                }else{

                    if(!$found){
                        break;
                    }

                    /* Get application priority.
                       And ensure that each priority exists once.
                     */
                    $priority = 1;
                    if(isset($attrs['gosaApplicationPriority'])){
                        $priority= $attrs['gosaApplicationPriority'][0];
                    }
                    while(isset($cur[$priority])){
                        $priority ++;
                    }

                    /* Create the data object that should be added 
                     * Folder
                     * Entry
                     * Release
                     */
                    $data = array();

                    /* Add a menu folder 
                     */
                    if(in_array_strict("gotoSubmenuEntry",$attrs['objectClass'])){
                        $type = "FOLDER";

                        $data['ICON'] = "";
                        if(isset($attrs['gosaApplicationIcon'])){
                            $data['ICON'] = $ldap->get_attribute($attrs['dn'],"gosaApplicationIcon");
                        }

                        /* Add a menu entry 
                         */
                    }elseif(in_array_strict("gotoMenuEntry",$attrs['objectClass'])){

                        $type = "ENTRY";
                        $data['INFO'] = "";
                        $data['PARAMETER'] = array();
                        if(isset($attrs['gosaApplicationParameter'])){
                            for($p = 0 ; $p < $attrs['gosaApplicationParameter']['count'] ; $p ++){
                                if(preg_match("/:/",$attrs['gosaApplicationParameter'][$p])){
                                    $tmp = explode(":",$attrs['gosaApplicationParameter'][$p]);
                                    $data['PARAMETER'][$tmp[0]] = preg_replace('/^[^:]+:/', '', $attrs['gosaApplicationParameter'][$p]);
                                }elseif($attrs['gosaApplicationParameter'][$p] == "*separator*"){
                                    $type = "SEPERATOR";
                                    $data['PARAMETER'] = array();
                                    break;
                                }
                            }
                        }

                        /* Add a release
                         */
                    }elseif(in_array_strict("FAIbranch",$attrs['objectClass'])){
                        $type = "RELEASE";
                        if(isset($attrs['FAIstate'][0])){
                            $data['FAIstate'] = $attrs['FAIstate'][0];
                        }else{
                            $data['FAIstate'] = "";
                        }
                    }

                    /* Create object and append it to the current structure pointer 
                     */
                    $data['LDAP_ATTRS'] = $attrs;
                    $data['DN']       = $attrs['dn'];
                    $data['NAME']     = $name;
                    $data['TYPE']     = $type;
                    $data['PRIORITY'] = $priority;
                    $data['ENTRIES']  = array();
                    $data['UNIQID']   = uniqid();
                    $data['PARENT']   = $parent_id;
                    $data['STATUS']   = "LOADED";
                    $cur[$priority]   = $data;
                    ksort($cur);
                }
            }
        }
    } 


    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if(isset($_GET['r'])) $this->__construct($this->config,$this->dn);

        if (isset($_POST['modify_state'])){
            $this->is_account = !$this->is_account;
        }

        /* Do we represent a valid account? */
        if (!$this->is_account){
            $display= $this->show_disable_header(msgPool::addFeaturesButton(_("Menu")), msgPool::featuresDisabled(_("Menu")));
            return ($display);
        }

        $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Menu")), msgPool::featuresEnabled(_("Menu")));

        if(isset($_GET['send'])){
            $id = $_GET['send'];
            $all = $this->_get_all_entries();
            if(isset($all[$id])){
                send_binary_content($all[$id]['ICON'],$id.".jpg","image/jpeg");
                exit;
            }
        }

        if(isset($_GET['r']))
            $this->__construct($this->config,$this->dn);

        if(count($this->edit_entry)){
            if($this->edit_entry['TYPE'] == "ENTRY"){
                $smarty = get_smarty();
                $smarty->assign("type", "ENTRY");
                $smarty->assign("entry",$this->edit_entry);
                $smarty->assign("paras", set_post($this->app_parameter));
                $display= $smarty->fetch (get_template_path('edit_entry.tpl', TRUE, dirname(__FILE__)));
                return($display);
            }
            if($this->edit_entry['TYPE'] == "FOLDER"){
                $smarty = get_smarty();

                session::set("binarytype" , "image/jpeg");
                session::set("binary" , $this->edit_entry['ICON']);

                $smarty->assign("rand", microtime(TRUE));
                $smarty->assign("image_set" , strlen($this->edit_entry['ICON']) > 0); 
                $smarty->assign("type", "FOLDER");
                $smarty->assign("entry",$this->edit_entry);
                $display= $smarty->fetch (get_template_path('edit_entry.tpl', TRUE, dirname(__FILE__)));
                return($display);
            }
        }

        $smarty = get_smarty();
        $smarty->assign("plug_id" , $_GET['plug']);

        /* Create application list */
        $departments = array();
        if(!$this->enableReleaseManagement){
            $res = get_list("(objectClass=gosaDepartment)", "application", $this->curbase,array("description","cn","ou"),GL_SIZELIMIT);
            foreach($res as $value){
                $fdn = $value['dn'];
                $fdn = preg_replace("/".preg_quote($this->curbase, '/')."/","",$fdn);
                $fdn= LDAP::fix($fdn);
                if($value["description"][0]!=".."){
                    $departments[$value['dn']]= convert_department_dn($fdn)." - [".$value["description"][0]."]";
                }else{
                    $departments[$value['dn']]=convert_department_dn($fdn)." ["._("Back")."]";
                }
            }
        }

        /* Create base back entry */
        $base_back = preg_replace("/^[^,]+,/","",$this->curbase);
        if((strlen($base_back)>= strlen($this->config->current['BASE']))&&($this->curbase!=$this->config->current['BASE'])){
            $departments[$base_back]=".. ["._("back")."]";
        }

        // Append departments for current base 
        $data = $lData = array();
        foreach($departments as $key => $app){
            $img = image('images/lists/folder.png');
            $data[$key] = array('TYPE'=>'DEP','ID'=> $app);
            $lData[$key] = array('data' => array($img,"&nbsp;",$app));

        }

        // Add applications found on this base 
        $used_apps = $this->_get_used_entry_name();
        foreach($this->apps as $key => $app){
            if(in_array_strict($app['cn'][0],$used_apps)) continue;
            if(!preg_match("/".get_ou("application", "applicationRDN").preg_quote($this->curbase, '/')."$/",$app['dn'])){
                continue;
            }

            $name = $app['cn'][0];
            if(isset($app['description'])){
                $name .= "&nbsp;[".$app['description'][0]."]";
            }
            $img = image('plugins/goto/images/select_application.png');
            $chk = "<input class='center' type='checkbox' value='1' name='AddApp_".$key."'>";
            $data[$key] = array('TYPE'=>'APP','ID'=> $app);
            $lData[$key] = array('data' => array($img,$chk,$name));
        }
        $this->applicationList->setListData($data,$lData);
        $this->applicationList->update();

        /* Assign copy / paste values 
         */
        if(!empty($this->copied_release)){
            $smarty->assign("copied", TRUE);
            $smarty->assign("copy_source", $this->copied_release);
        }else{
            $smarty->assign("copied", FALSE);
        }
        $smarty->assign("enableReleaseManagement",$this->enableReleaseManagement);
        $smarty->assign("FAIrelease", set_post($this->FAIrelease));
        $smarty->assign("app_list",$this->applicationList->render());
        $smarty->assign("i",0);
        $smarty->assign("releases", set_post($this->Releases));
        $smarty->assign("folders" , set_post($this->_get_folder_names()));
        $entries = $this->_get_entries_for_release($this->FAIrelease);
        $smarty->assign("entries", set_post($entries));
        $display.= $smarty->fetch (get_template_path('app_list.tpl', TRUE, dirname(__FILE__)));
        return($display);
    }


    /*! \brief Returns all used folder names 
      @return Array  All used folder names.
     */ 
    function _get_folder_names()
    {
        $data = $this->_get_entries_for_release($this->FAIrelease);
        $all    = $this->_get_all_entries();
        $ret = array("BASE" => ".");
        foreach($data as $entry){

            if($entry['TYPE'] == "FOLDER"){
                $str = $entry['NAME'];
                $parent = $entry['PARENT'];
                $i = 10;
                while(isset($all[$parent]) && $i){  
                    $i --;
                    $parent_o = $all[$parent];
                    $str      = $parent_o['NAME']."/".$str;
                    $parent   = $all[$parent_o['UNIQID']]['PARENT'];
                }        
                $ret[$entry['UNIQID']] = $str;
            }
        }
        return($ret);
    }


    /*! \brief return all used applications 
      @return Array  All used applications.
     */ 
    function _get_used_entry_name()
    {
        $data = $this->_get_entries_for_release($this->FAIrelease);
        $ret = array();
        foreach($data as $entry){
            if($entry['TYPE'] == "ENTRY"){
                $ret[] = $entry['NAME'];
            }
        }
        return($ret);
    }


    /*! \brief Returns all folder an entries for the selected release 
      @return Array  Returns the complete menu structure for the given array.
     */ 
    function _get_entries_for_release($release,$cur = NULL)
    {
        $all = $this->_get_all_entries();
        $key = $this->_get_release_key($release);
        if(isset($all[$key]) && count($all[$key]['ENTRIES'])){
            $res = $this->_get_all_entries(TRUE,TRUE,$all[$key]['ENTRIES']);
            return($res);
        } 
        return(array());
    }


    /*! \brief Save the currently edited entry 
     */
    function _save_entry_edit()
    {
        $all    = $this->_get_all_entries();
        $entry  = $this->edit_entry;
        $r_entry= &$all[$entry['UNIQID']];


        if($entry['TYPE'] == "ENTRY"){
            $r_entry['PARAMETER'] = $this->app_parameter;
            if($r_entry['STATUS'] != "ADDED"){
                $r_entry['STATUS'] = "EDITED";
            } 
        }
        if($entry['TYPE'] == "FOLDER"){
            $r_entry['ICON']   = $this->edit_entry['ICON'];
            $r_entry['STATUS'] = "EDITED";
        }
        $this->dialog = FALSE;
        $this->edit_entry = array();
    }


    /*! \brief prepare the entry with the given ID, to be edited.
      Read application Parameter from ldap.
     */
    function _edit_entry_edit($id)
    {
        $all   = $this->_get_all_entries();
        $entry = $all[$id];

        $this->app_parameter = array();
        if($entry['TYPE'] == "ENTRY"){
            $found = FALSE;
            foreach($this->apps as $id => $app){

                if($app['cn'][0] == $entry['NAME']){
                    $found = TRUE;
                    break;
                }
            }
            if($found){

                /* Create a list of editable parameter */
                if(isset($app['gosaApplicationParameter'])){
                    for($i = 0 ; $i < $app['gosaApplicationParameter']['count'] ; $i++) {
                        $para = $app['gosaApplicationParameter'][$i];
                        $tmp  = explode(":",$para);
                        $this->app_parameter[$tmp[0]] = $tmp[1];
                    }
                }

                /* Overwrite parameters with entry parameters */
                foreach($entry['PARAMETER'] as $name => $value){
                    $this->app_parameter[$name] = $value;
                }

                $this->dialog = TRUE;
                $this->edit_entry = $entry;
            }
        }

        if($entry['TYPE'] == "FOLDER"){
            $this->dialog = TRUE;
            $this->edit_entry = $entry;
        }
    }


    /*! \brief Removes the menu structure from ldap 
     */
    function remove_from_parent()
    {
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->ls("(|(objectClass=gotoSubmenuEntry)(objectClass=FAIbranch)(objectClass=gotoMenuEntry))",$this->dn,array("*"));
        $a_remove = array();
        while($attrs = $ldap->fetch()){
            $a_remove[] = $attrs['dn'];
        }
        foreach($a_remove as $remove){
            $ldap->rmdir_recursive($remove);
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
            }
        }
        $this->_load_menu_structure();
    }


    function check()
    {
        $message = plugin::check();
        return($message);
    }


    /*! \brief Create missing releases, if there is a release selected \
      that is currently not part of the menu structure \
      then create this entry
     */
    function _check_missing_release($release)
    {
        $release_info = $this->Releases[$release];

        $parent_id = $this->a_Structure[0]['UNIQID'];
        $cur = &$this->a_Structure[0]['ENTRIES'];
        for($i = 0 ; $i < count($release_info['parts']) ; $i ++){
            $part = $release_info['parts'][$i];
            $found = FALSE;
            foreach($cur as $key => $name){
                if($name['NAME'] == $part){
                    $parent_id = $cur[$key]['UNIQID'];
                    $cur = &$cur[$key]['ENTRIES'];

                    $found =TRUE;
                    break;
                }
            }
            if(!$found){
                $release           =  array();
                $release['UNIQID'] = uniqid();
                $release['PARENT'] = $parent_id;
                $release['NAME']   = $part;
                $release['TYPE']   = "RELEASE";
                $release['ENTRIES']= array();
                $release['STATUS']   = "ADDED";
                $release['FAIstate'] =  $release_info['FAIstate'];
                $cur[] = $release;
                $i --;
            }
        }
    }



    /*! \brief Moves a given object ($id) in a specified direction ($dir).
      @param  String The object ID of the object we want to move
      @dir    String Move "up" or "down"
     */
    function _move_entry($id,$dir)
    {
        $all   = $this->_get_all_entries();
        if($dir == "down"){
            $to = $this->_get_next($id);
        } 
        if($dir == "up"){
            $to = $this->_get_last($id);
        }

        if(!$to){
            return;
        }

        $o_to   = $all[$to];
        $o_from = $all[$id];

        if($o_to['PARENT'] == $o_from['UNIQID'] && $dir == "down"){
            $to    = $this->_get_next($to,$o_from['PARENT']); 
            $o_to  = $all[$to]; 
        }

        /* Target is ENTRY && same BASE, just switch */
        if($o_to['PARENT'] == $o_from['PARENT'] ){
            $parent = $all[$o_to['PARENT']];
            $pos = 0;
            foreach($parent['ENTRIES'] as $entry){
                $pos ++;
                if($entry['UNIQID'] == $to){
                    break;
                }
            }
            if($dir == "up" && $pos > 0){
                $pos --;
            }
            $this->_add_entry($parent['UNIQID'],$o_from,$pos);
            $this->_remove_entry_id($id);
            return(TRUE);
        }
        return(FALSE);
    }



    /*! \brief  Returns the railing object ID of the given object.
      @return String  The id of the trailing object.
     */ 
    function _get_last($id)
    {
        $all_l = array_reverse($this->_get_entries_for_release($this->FAIrelease));
        for($i = 0 ; $i < count($all_l) ; $i ++){
            if(isset($all_l[$i]['UNIQID']) && $all_l[$i]['UNIQID'] == $id){
                $i++;
                break;
            }
        }
        while(isset($all_l[$i]) && !in_array_strict($all_l[$i]['TYPE'],array("ENTRY","FOLDER","CLOSE","OPEN")) && $i < count($all_l)){
            $i++;
        }

        if(!isset($all_l[$i])){
            return(FALSE);
        }

        if(in_array_strict($all_l[$i]['TYPE'],array("CLOSE","OPEN"))){
            return($all_l[$i]['PARENT']);
        }     

        return($all_l[$i]['UNIQID']);
    }


    /*! \brief  Returns the following object ID of the given object.
      @return String  The id of the following object.
     */ 
    function _get_next($id,$parent = 0)
    {
        $all_l = $this->_get_entries_for_release($this->FAIrelease);
        for($i = 0 ; $i < count($all_l) ; $i ++){
            if(isset($all_l[$i]['UNIQID']) && $all_l[$i]['UNIQID'] == $id){
                $i++;
                break;
            }
        }
        if($parent != 0){
            while(isset($all_l[$i]) && $all_l[$i]['PARENT'] != $parent){
                $i++;
            }
        }else{
            while(isset($all_l[$i]) && !in_array_strict($all_l[$i]['TYPE'],array("ENTRY","FOLDER")) && $i < count($all_l)){
                $i++;
            }
        }
        if(!isset($all_l[$i])){
            return(FALSE);
        }
        if(in_array_strict($all_l[$i]['TYPE'],array("CLOSE","OPEN"))){
            return($all_l[$i]['PARENT']);
        }
        return($all_l[$i]['UNIQID']);
    }




    /* !\brief Handle ui POSTS, like sort up/down/delete
     */ 
    function save_object()
    {
        foreach($_POST as $name => $value){

            $value = get_post($name);
            if(preg_match("/^menu_copy/",$name)){
                $this->copied_release = $this->FAIrelease;
                break;
            }
            if(preg_match("/^menu_delete/",$name)){
                $current_rel  = $this->_get_release_key($this->FAIrelease); 
                $this->_remove_entry_id($current_rel);
                break;
            }
            if(preg_match("/^menu_paste/",$name)){
                $source_rel   = $this->_get_release_key($this->copied_release); 
                $current_rel  = $this->_get_release_key($this->FAIrelease);

                $all = $this->_get_all_entries();
                $menu = $all[$source_rel]['ENTRIES'];

                foreach($menu as $entry){
                    if(in_array_strict($entry['TYPE'],array("FOLDER","ENTRY","SEPERATOR"))){
                        $this->_add_entry($current_rel,$entry,-1);
                    }
                }
                $this->copied_release = "";
                break;
            }

            if(preg_match("/del_/",$name)){
                $id = preg_replace("/^del_/","",$name);
                $this->_remove_entry_id($id);
                break;
            }
            if(preg_match("/app_entry_edit/",$name)){
                $id = preg_replace("/^app_entry_edit/","",$name);
                $this->_edit_entry_edit($id);
                break;
            }
            if(preg_match("/up_/",$name)){
                $id = preg_replace("/^up_/","",$name);
                $this->_move_entry($id,"up");
                break;
            }
            if(preg_match("/down_/",$name)){
                $id = preg_replace("/^down_/","",$name);
                $this->_move_entry($id,"down");
                break;
            }
            if(preg_match("/^parameter_/",$name) && 
                    count($this->edit_entry) && $this->edit_entry['TYPE'] == "ENTRY"){
                $name = preg_replace("/^parameter_/","",$name);
                $this->app_parameter[$name] = $value;
            }
        }
        if(isset($_POST['FAIrelease'])){
            $this->FAIrelease = get_post('FAIrelease');
            $this->_check_missing_release($this->FAIrelease);
        }

        $this->applicationList->save_object();
        $action = $this->applicationList->getAction();
        if($action['action'] == "edit"){
            $data= $this->applicationList->getData($action['targets'][0]);
            $dn= $this->applicationList->getKey($action['targets'][0]);
            if($data['TYPE'] == 'DEP'){
                $this->curbase = $dn; 
            }elseif($data['TYPE'] == 'APP' && isset($_POST['folder'])){
                $this->_add_app_id(get_post('folder'),$dn); 
            }
        }
        if(isset($_POST['add_to_folder']) && isset($_POST['folder'])){
            $folder = get_post('folder');
            foreach($_POST as $name => $value){
                if(preg_match("/^AddApp_[0-9]*$/",$name)){
                    $this->_add_app_id($folder,preg_replace("/^AddApp_/","",$name));   
                }
            }
        }

        /* Add seperator */
        if(isset($_POST['add_seperator']) && isset($_POST['menu_folder'])){
            $folder = get_post('menu_folder');
            $this->_add_seperator($folder);
        }

        if(isset($_POST['add_menu_to_folder']) && isset($_POST['menu_folder'])){
            $folder = get_post('menu_folder');
            $name = get_post('menu_folder_name');
            if(strlen($name) > 0 && preg_match("/[a-z ]/i",$name)){
                $this->_add_sub_folder($folder,$name);
            }
        }
        if(isset($_POST['app_entry_save'])){ 
            $this->_save_entry_edit();
        }

        if(isset($_FILES['folder_image']) && isset($_POST['folder_image_upload'])){
            if($_FILES['folder_image']['error'] == 0 && $_FILES['folder_image']['size'] > 0){
                $this->edit_entry['ICON'] = file_get_contents(gosa_file_name($_FILES['folder_image']['tmp_name']));
            }
        }

        if(isset($_POST['edit_reset_image'])){
            $this->edit_entry['ICON'] = "";
        }

        if(isset($_POST['app_entry_cancel'])){
            $this->edit_entry = array();
            $this->dialog = FALSE;
        }
        $this->reload();
    }


    /*! \brief Returns the UNIQID of the currently selected release 
     */ 
    function _get_release_key($release,$add_if_missing = FALSE)
    {
        $release_info = $this->Releases[$release];

        if($release_info['name'] == "/"){
            return($this->a_Structure['0']['UNIQID']);
        }

        $cur = &$this->a_Structure[0]['ENTRIES'];
        $s_key = "";
        $found = FALSE;
        foreach($release_info['parts'] as $name){
            foreach($cur as $key => $obj){
                if($obj['TYPE'] == "RELEASE" && $obj['NAME'] == $name){
                    $s_key = $cur[$key]['UNIQID'];
                    $cur = &$cur[$key]['ENTRIES'];
                    $found = TRUE;
                    break;
                }
                $found = FALSE;
            }
        }
        if($found){
            return($s_key);  
        }  
        return(FALSE);
    }


    /*! \brief Add a new folder folder to the specified folder id
      @param  String $folder The folder id in where we want to add the new folder.
      @param  String $name   The name of the new folder.
     */ 
    function _add_sub_folder($folder,$name)
    {
        $all = $this->_get_all_entries();
        if($folder == "BASE"){
            $folder = $this->_get_release_key($this->FAIrelease,TRUE);
        }

        if(isset($all[$folder])){
            $a_folder = array();
            $a_folder['STATUS'] = "ADDED";
            $a_folder['NAME']   = $name;
            $a_folder['UNIQID'] = uniqid();
            $a_folder['ENTRIES']= array();
            $a_folder['PARENT'] = $folder;      
            $a_folder['TYPE']   = "FOLDER";
            $a_folder['ICON']   = "";
            $all[$folder]['ENTRIES'][] = $a_folder;
        }
    }


    /* !\brief  Remove the given id from the menu structure.
       @param  String  ID to of the entry we want to remove.
       @return Boolean TRUE on success
     */
    function _remove_entry_id($id)
    {
        $all = $this->_get_all_entries();
        if(isset($all[$id])){
            $all[$id]['STATUS'] = "REMOVED";
            $all[$id]['ENTRIES'] = array();
            return(TRUE);
        }
        return(FALSE);
    }


    /* !\brief  Adds an object to a given folder.
       @param  String  The folder where we should add the entry
       @param  Array   The entry we want to add.
       @param  Array   The position in the destination entry array.
     */
    function _add_entry($folder_id,$entry,$pos = 0)
    {
        $all = $this->_get_all_entries();

        /* Do not add removed */
        if($entry['STATUS'] == "REMOVED"){
            return;
        }

        /* Check if the folder exists 
         */
        if(isset($all[$folder_id])){

            /* Check if the entry we want to add, 
               contains su objects.
             */
            if(!isset($entry['ENTRIES'])){
                $entries = array();
            }else{
                $entries = $entry['ENTRIES'];
            }
            $folder  = &$all[$folder_id];

            /* Prepare the entry to be added.
             */
            $entry['UNIQID'] = uniqid();     
            $entry['PARENT'] = $folder_id;
            $entry['ENTRIES']= array();
            $entry['STATUS'] = "ADDED";

            /* Append the ebtry to the given folder 
               and to the given position \$pos
             */ 
            $cnt = 0; 
            $new = array();
            $added =FALSE;
            foreach($folder['ENTRIES'] as $key => $obj){
                if($obj['STATUS'] == "LOADED"){
                    $obj['STATUS'] = "EDITED";
                }
                if($pos == $cnt){
                    $new[] = $entry;
                    $added = TRUE;
                }
                $cnt ++;
                $new[] = $obj;
            }
            if(!$added){
                $new[] = $entry;
            }
            $all[$folder_id]['ENTRIES'] = &$new;

            /* Add sub entries too.
             */ 
            foreach($entries as $sub){
                $this->_add_entry($entry['UNIQID'],$sub,-1);
            }
            return(TRUE);
        }
        return(FALSE);
    }


    /*! \brief Add the application identified by $app_id to folder $folder_id 
      @param  String  folder_id The UNIQID of the folder where we want to add the new folder.
      @param  Integer app_id    The ID of the application which should be added.
     */ 
    function _add_app_id($folder_id,$app_id)
    {
        $all = $this->_get_all_entries();
        if($folder_id == "BASE"){
            $folder_id = $this->_get_release_key($this->FAIrelease);
        }
        if(isset($all[$folder_id]) && isset($this->apps[$app_id])){

            $new = array();
            $new['TYPE']  = "ENTRY";
            $new['NAME']  = $this->apps[$app_id]['cn'][0];
            $new['UNIQID']= uniqid(); 
            $new['PARENT']= $folder_id;
            $new['PARAMETER']= array();
            if(isset($this->apps[$app_id]['description'][0])){
                $new['INFO']  = $this->apps[$app_id]['description'][0];
            }else{
                $new['INFO']  = "";
            }
            $new['STATUS']= "ADDED";
            $all[$folder_id]['ENTRIES'][] = $new;
        }
    }


    /*! \brief Add the application identified by $app_id to folder $folder_id 
      @param  String  folder_id The UNIQID of the folder where we want to add the new folder.
      @param  Integer app_id    The ID of the application which should be added.
     */ 
    function _add_seperator($folder_id)
    {
        $all = $this->_get_all_entries();
        if($folder_id == "BASE"){
            $folder_id = $this->_get_release_key($this->FAIrelease);
        }

        if(isset($all[$folder_id])){
            $new = array();
            $new['TYPE']  = "SEPERATOR";
            $new['NAME']  = "SEPERATOR";
            $new['UNIQID']= uniqid(); 
            $new['PARENT']= $folder_id;
            $new['PARAMETER']= array();
            $new['STATUS']= "ADDED";
            $all[$folder_id]['ENTRIES'][] = $new;
        }
    }


    /*! \brief  Return all entries linear. ($this->a_Structure is a multidimensional array) 
      @param  Boolean   $add_tags  If TRUE, OPEN/CLOSE Tags will be appended.
      Used in the smarty template to display logical sperations.
      @param  &Array    Start here, Pointer to an array.
     */ 
    function _get_all_entries($add_tags = FALSE, $skip_release = FALSE, &$cur = NULL)
    {
        $ret = array();
        if($cur == NULL){
            $cur = &$this->a_Structure;
        }

        /* Walk through all entries and append them to our return array 
         */
        foreach($cur as $key => $entry){
            if($skip_release && $entry['TYPE'] == "RELEASE"){
                continue;
            }    
            if($entry['TYPE'] == "ENTRY"){
                $found = FALSE;
                foreach($this->apps as $app){
                    if($app['cn'][0] == $entry['NAME']){
                        $found = TRUE;
                        if(isset($app['description'][0])){
                            $entry['INFO'] = "[".$app['description'][0]."]";
                        }
                        break;
                    }
                } 
                if(!$found){
                    $entry['INFO'] = "<font color='red'>"._("Not available in release.")."</font>";
                }
            }

            $tmp = $entry;

            /* Recursive resolution of the subentries  
               There are two methods.
               - Just add sub entries  
               - Add sub entries and additionaly add OPEN / CLOSE tags to be able 
               to display logical seperators in the smarty template.
             */ 
            if(!$add_tags){
                $ret[$tmp['UNIQID']] = &$cur[$key];
                if(isset($entry['ENTRIES']) && count($entry['ENTRIES'])){
                    $ret = array_merge($ret,$this->_get_all_entries($add_tags,$skip_release,$cur[$key]['ENTRIES']));
                }
            }else{

                if(isset($tmp['ENTRIES'])){
                    unset($tmp['ENTRIES']);
                }
                if($tmp['STATUS'] != "REMOVED"){
                    $ret[] = $tmp;
                    if(isset($entry['ENTRIES']) && count($entry['ENTRIES'])){
                        $add = false;
                        foreach($entry['ENTRIES'] as $entry){
                            if($entry['STATUS'] != "REMOVED"){
                                $add = TRUE;
                                break;
                            }
                        }

                        if($add){
                            $ret[] = array("TYPE" => "OPEN", "PARENT" => $entry['PARENT']);
                            $ret = array_merge($ret,$this->_get_all_entries($add_tags,$skip_release,$cur[$key]['ENTRIES']));
                            $ret[] = array("TYPE" => "CLOSE" , "PARENT" => $entry['PARENT']);
                        }
                    }
                }
            }
        }
        return($ret);
    }


    /*! \brief Save this plugin data to ldap.
      Save the current menu structure to ldap.
     */
    function save()
    {
        $ldap = $this->config->get_ldap_link();
        $all = $this->_get_all_entries();
        $prio = 0;
        $Actions = array("Remove" => array(),"Edit" => array() , "Add" => array());

        /* Walk through the menu structure and build up the ldap data object, 
           the entry dn and the entry priority.
         */
        $sep_id = 0;
        foreach($all as $entry){
            $prio ++;
            $cur = $entry;
            $dn = "";

            /* Build entry dn
             */
            do{  
                if($cur['TYPE'] == "SEPERATOR"){
                    $sep_id ++;
                    $dn.= "cn=seperator_".$sep_id.",";
                }elseif($cur['TYPE'] == "ENTRY"){
                    $dn.= "cn=".$cur['NAME'].",";
                }elseif($cur['TYPE'] == "FOLDER"){
                    $dn.= "cn=".$cur['NAME'].",";
                }elseif($cur['TYPE'] == "RELEASE"){
                    $dn.= "ou=".$cur['NAME'].",";
                }elseif($cur['TYPE'] == "BASE"){
                }
                if(!isset($all[$cur['PARENT']])){
                    $cur = NULL;
                }else{
                    $cur = $all[$cur['PARENT']];
                }
            }while(is_array($cur));

            $cur_dn = $dn.$this->dn;
            $attrs = array();

            /* Build entry data object.
             */
            switch($entry['TYPE']){
                case "SEPERATOR"    :
                { 
                    $attrs['objectClass'] = array("gotoMenuEntry");
                    $attrs['cn']          = "seperator_".$sep_id;
                    $attrs['gosaApplicationPriority'] = $prio;
                    $attrs['gosaApplicationParameter'] = "*separator*";
                }
                break;
                case "ENTRY"    :
                { 
                    $attrs['objectClass'] = array("gotoMenuEntry");
                    $attrs['cn']          = $entry['NAME'];
                    $attrs['gosaApplicationPriority'] = $prio;
                    $attrs['gosaApplicationParameter'] = array(); 

                    foreach($entry['PARAMETER'] as $name => $value){
                        $attrs['gosaApplicationParameter'][] = $name.":".$value; 
                    }
                    if($entry['STATUS'] == "ADDED" && !count($attrs['gosaApplicationParameter'])){
                        unset($attrs['gosaApplicationParameter']);
                    } 
                }
                break;
                case "FOLDER"   : 
                { 
                    $attrs['objectClass'] = array("gotoSubmenuEntry");
                    $attrs['cn']          = $entry['NAME'];
                    $attrs['gosaApplicationPriority'] = $prio;
                    if($entry['STATUS'] != "ADDED"){
                        $attrs['gosaApplicationIcon'] = array();
                    }

                    if(!empty($entry['ICON'])){
                        $attrs['gosaApplicationIcon']     = $entry['ICON'];
                    }
                }
                break;
                case "RELEASE"  : 
                {
                    $attrs['ou']            = $entry['NAME'];
                    $attrs['objectClass']   = array();
                    $attrs['objectClass'][] = "top";
                    $attrs['objectClass'][] = "organizationalUnit";
                    $attrs['objectClass'][] = "FAIbranch";
                    if(!empty($entry['FAIstate'])){
                        $attrs['FAIstate']      = $entry['FAIstate'];
                    }
                }
                break;
            }

            /* Append missing ObjectClasses,  ...  Tagging 
             */
            if(isset($entry['LDAP_ATTRS'])){
                for($i = 0 ; $i < $entry['LDAP_ATTRS']['objectClass']['count']; $i ++){
                    $oc = $entry['LDAP_ATTRS']['objectClass'][$i];
                    if(!in_array_strict($oc,$attrs['objectClass'])){
                        $attrs['objectClass'][] = $oc;
                    }
                }
            }

            /* Create an array containing all operations sorted by type. (add,remove...)
             */
            if($entry['STATUS'] == "LOADED"){
                continue;
            }
            if($entry['STATUS'] == "REMOVED"){
                if(isset($entry['DN'])){
                    $Actions['Remove'][$entry['DN']] = $entry['DN'];
                }else{
                    $Actions['Remove'][$cur_dn] = $cur_dn;
                }
            }
            if($entry['STATUS'] == "EDITED"){
                $Actions['Edit'][$cur_dn] = $attrs;
            }
            if($entry['STATUS'] == "ADDED"){
                $Actions['Add'][$cur_dn] = $attrs;
            }
        }

        /* First remove entries
         */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        foreach($Actions['Remove'] as $dn){
            $ldap->cd($dn);
            $ldap->cat($dn);
            if($ldap->count()){
                $ldap->rmdir_recursive($dn);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $dn, LDAP_DEL, get_class()));
                }
            }
        }

        /* Add new entries
         */
        foreach($Actions['Add'] as $dn => $data){
            $this->tag_attrs($data,$dn,$this->gosaUnitTag);
            $ldap->cd($dn);
            $ldap->cat($dn);
            if(!$ldap->count()){
                $ldap->add($data);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $dn, LDAP_ADD, get_class()));
                }
            }
        }

        /* Modify entries
         */
        foreach($Actions['Edit'] as $dn => $data){
            $this->tag_attrs($data,$dn,$this->gosaUnitTag);
            $ldap->cd($dn);
            $ldap->cat($dn);
            if($ldap->count()){
                $ldap->modify($data);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $dn, LDAP_MOD, get_class()));
                }
            }
        }

        $this->_load_menu_structure();
    }


    /*! \brief  Return plugin informations for acl handling  
      @return Array containing all plugin ACL informations
     */ 
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Applications"),
                    "plDescription" => _("Group applications"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 7,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("groups","ogroups"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoSubmenuEntry'=>'>=2.7','gotoMenuEntry' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plProvidedAcls"=> array(
                        "gosaMemberApplication"     => _("Application"),
                        "FAIrelease"                => _("Release"),
                        "gosaApplicationParameter"  => _("Application parameter"))
                    ));
    }


    /* \brief   Prepare this plugin to be copied.
       Adapt all required attributes from the source object.
       In this case, update the menu structure too, mark all elements
       as newly added, so they will be saved in save();
     */
    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $tmp = new appgroup($this->config,$source['dn']);
        $this->is_account = TRUE;
        $this->a_Structure = $tmp->a_Structure;
        $all = $this->_get_all_entries();
        foreach($all as &$entry){
            if(isset($entry['STATUS'])){
                $entry['STATUS'] = "ADDED";
            }
        }
    }


    /*! \brief  Save HTML posts in multiple edit mode
     */
    function multiple_save_object()
    {
        if(isset($_POST['group_apps_multi'])){
            $this->save_object(); 
            plugin::multiple_save_object();    

            /* Get posts */
            foreach(array("apps") as $attr){
                if(isset($_POST['use_'.$attr])) {
                    $this->multi_boxes[] = $attr;
                }
            }
        }
    }


    /*! \brief  Return values used in multiple edit mode.
      Some values can be modified for multiple 
      groups at the same time.
      @return Array  All values that support multiple edit.
     */
    function get_multi_edit_values()
    {
        $ret = plugin::get_multi_edit_values();

        if(in_array_strict("apps",$this->multi_boxes)){
            $ret['gosaApplicationParameter'] = $this->gosaApplicationParameter;
            $ret['Categories']               = $this->Categories;
            $ret['gosaMemberApplication']    = $this->gosaMemberApplication;
            $ret['FAIrelease']               = $this->FAIrelease;
            $ret['appoption']                = $this->appoption;
        }
        return($ret);
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
