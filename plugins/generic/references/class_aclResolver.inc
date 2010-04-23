<?php

class aclResolver 
{
    private $classMapping = array();
    private $aclTypes = array();
    private $affectingACLs = array();

    private $renderedList = "";

    // The users 'dn' and 'uid' used to verify the collected acls
    private $validateDn;
    private $validateUid;
    private $acl_category;
    private $parent;

    // Used for the autocompleter
    private $userMap;

    function __construct($config, $dn, $parent)
    {
        $this->config = &$config;
        $this->dn = $dn;
        $this->parent = $parent;

        // Replace this with a user defined one later.
        $ui = get_userinfo();
        $this->validateUid = $ui->uid;
        $this->validateDn = $ui->dn;

        // Build class mapping - only once, will not change during session.
        if(!session::is_set('aclConverter::classMapping')){
            $tmp= session::global_get('plist');
            $plist= $tmp->info;
            $map = array();
            $map['all']= _("All categories");
            foreach($plist as $class => $plInfo){
                if(isset($plInfo['plCategory']) && is_array($plInfo['plCategory'])){
                    foreach($plInfo['plCategory'] as $category => $desc){
                        if(!is_numeric($category)){
                            $map[$category] = $desc['description'];
                        }
                    }
                }
            }
            foreach($plist as $class => $plInfo){
                if(isset($plInfo['plCategory']) && is_array($plInfo['plCategory'])){
                    foreach($plInfo['plCategory'] as $category => $desc){
                        if(!is_numeric($category)){
                            $map[$category."/".$class] = $map[$category]." - ".$plInfo['plDescription'];
                        }else{
                            $map[$desc."/".$class] = $map[$desc]." - ".$plInfo['plDescription'];
                        }
                    }

                }
            }
            session::set('aclConverter::classMapping', $map);
        }
        $this->classMapping = session::get('aclConverter::classMapping');

        // Define ACL type translations
        $this->aclTypes= array("reset" => _("Reset ACLs"),
                "one" => _("One level"),
                "base" => _("Current object"),
                "sub" => _("Complete subtree"),
                "psub" => _("Complete subtree (permanent)"),
                "role" => _("Use ACL defined in role"));
    
        // Enforce to reload acl result 
        $this->renderedList = "";
    }

    
    function reload()
    {
        // Go through all ACLs and get those matching the objects dn.
        $ui = get_userinfo();
        $ui->reset_acl_cache();
        $ui->loadACL();

        // Get ACL category for the current object.
        if(isset($this->parent->acl_category) && !empty($this->parent->acl_category)){
            $this->acl_category = preg_replace("/\/$/","",$this->parent->acl_category);
        }

        foreach($ui->allACLs as $dn => $acls){
            if(preg_match("/".preg_quote($dn,'/')."$/i", $this->dn)){

                // Foreach dn there is a collection of ACLs indexed by their priority
                foreach($acls as $prio => $acl){
                    if($acl['type'] == "reset"){
                        $this->affectingACLs[$dn][$prio] = $acl;
                        continue;
                    }else{

                        // Only get those entries with a relevant acl-category
                        foreach($acl['acl'] as $category => $attributes){
                            if(preg_match("/^all($|\/)/", $category) || 
                                    preg_match("/^".$this->acl_category."($|\/)/", $category)){
                                $this->affectingACLs[$dn][$prio] = $acl;
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    


    /*! \brief   Create a human readable HTML result 
     */    
    function getReadableACL() 
    {
        if(isset($_POST['aclTarget'])){
            $d = get_post('aclTarget');
            if(isset($this->userMap[$d])){
                $this->validateDn = $this->userMap[$d]['dn'];
                $this->validateUid = $this->userMap[$d]['uid'][0];
                $this->renderedList = "";
            }
        }
       
        if(empty($this->renderedList)){

            $this->reload();

            // Autocompleter template 
            $autocompleter ="
                <input class='filter_textfield' placeholder='"._("Search")."...' 
                id='aclTarget' name='aclTarget' type='text' value='' 
                onChange=\"\$('aclTargetSubmit').focus();\">

                <div id='autocomplete' class='autocomplete'></div>
                <script type='text/javascript'>
                new Ajax.Autocompleter(
                        'aclTarget', 
                        'autocomplete', 
                        'autocomplete.php', 
                        { minChars: 3, frequency: 0.5 });
                </script>
            ".image("images/lists/submit.png","aclTargetSubmit");

            // Base template - each entry start with this 
            $tpl = 
                "\n <tr class='acl-viewer-head %s'>".
                "\n  <td>%s</td>".
                "\n  <td colspan=2><b>%s</b><div class='right'>%s</div></td>".
                "\n </tr>".
                "\n %s";

            // If the acl consists of a user-object-filter then this template is used.        
            $filter_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Filter")."</b></td>".
                "\n  <td><ul class='acl-viewer-items'><li>%s</li></ul></td>".
                "\n </tr>";

            // Used to display ACL owner of type "group"
            $gmem_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Groups")."</b></td>".
                "\n  <td><ul class='acl-viewer-items'>%s</ul></td>".
                "\n </tr>";

            // Used to display ACL owner of type "user"
            $umem_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Users")."</b></td>".
                "\n  <td><ul class='acl-viewer-items'>%s</ul></td>".
                "\n </tr>";

            // Used to display the acl contents, except 'reset' and 'role'
            $acl_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Acls")."</b></td>".
                "\n  <td><ul class='acl-viewer-items'>%s</ul></td>".
                "\n </tr>";


            $user = "<h3>".sprintf(_("List of effective ACL for '%s'"), $this->validateUid)."</h3>";
            $str  = "<table summary='"._("Object permissions")."'>";
            $str .= " <tr><td>".$user."</td><td>".$autocompleter."</td></tr>";
            $str .= "</table>";
            $str .= "<div class='acl-viewer-container'>";
            $str .= "<table summary='"._("Object permissions")."' cellpadding='0' cellspacing='0' class='acl-viewer'>";
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ui = get_userinfo();
            foreach($this->affectingACLs as $dn => $acls){
                foreach($acls as $acl){

                    // Prepare entry icon (department or element?)
                    $image = (isset($this->config->idepartments[$dn]))? "images/select_department.png":"images/lists/element.png";

                    // The acl type (sub,psub,reset...)
                    $aclType = $this->aclTypes[$acl['type']];

                    // Does the filter match for current object? 
                    $filter ="";
                    $match = TRUE;
                    if(!empty($acl['filter'])){
                        $match = $ldap->object_match_filter($this->dn,$acl['filter']);
                        $filter= $acl['filter'];
                        if(!$match){
                            $filter= "<span>".$filter."</span>";
                        }
                    }

                    // Check membership 
                    $gmem = $umem = "";
                    $users = $groups = array();
                    $gfound = $ufound = FALSE;
                    foreach($acl['members'] as $type => $name){

                        // Check if we're part of the members 
                        if(preg_match("/^U:/", $type)){
                            if(preg_match("/^U:".preg_quote($this->validateDn,'/')."/", $type)){
                                $users[] = $name;
                                $ufound = TRUE;
                                continue;
                            }
                            $users[] = "<span>".$name."</span>";
                        }

                        // Check if we're part of the group members 
                        if(preg_match("/^G/", $type)){
                            if($type == "G:*"){
                                $gfound = TRUE;
                                $groups[] = $name;
                                continue;
                            }
                            if(preg_match("/^G:/", $type)){
                                $gdn = preg_replace("/^G:/","",$type);
                                $ldap->cat($gdn,array('memberUid'));
                                if($ldap->count()){
                                    $attrs = $ldap->fetch();
                                    if(isset($attrs['memberUid']) && in_array($this->validateUid, $attrs['memberUid'])){
                                        $gfound = TRUE;
                                        $groups[] = $name;
                                    }
                                }
                                continue;
                            }

                            // Group member doesn't mactch
                            $groups[] = "<span>".$name."</span>";
                        }
                    }
                    if(!count($groups)) $gfound = TRUE;
                    if(!count($users)) $ufound = TRUE;

                    // Build up ACL definition list 
                    $defs ="";
                    if($acl['type']!='reset'){
                        foreach($acl['acl'] as $type => $acl){
                            if(isset($this->classMapping[$type])){
                                $defs .= "<li>".$this->classMapping[$type].": ".$this->aclToString($acl)."</li>";
                            }else{
                                $defs .= "<li>".$type.": ".$this->aclToString($acl)."</li>";
                            }
                        }
                    }
                   
                    // Display the acl block in a special color if its not matching 
                    $class="";
                    if(!($gfound || $ufound) || !$match){
                        $class = "acl-viewer-blocked";
                    }                    

                    if(!empty($filter)) $filter =sprintf($filter_tpl,$class,$filter);
                    if(!empty($defs)) $defs = sprintf($acl_tpl,$class,$defs);
                    if(count($users))  $umem = sprintf($umem_tpl,$class,"<li>".implode($users,'</li><li>')."</li>");
                    if(count($groups)) $gmem = sprintf($gmem_tpl,$class,"<li>".implode($groups,'</li><li>')."</li>");
                    $str.= sprintf($tpl,$class, image($image), $dn, $aclType, $filter.$gmem.$umem.$defs);
                }
            }
            $str .= "</table>"; 
            $str .= "</div>"; 
            $this->renderedList = $str;
        }
        return($this->renderedList);
    }

    function aclToString($acls)
    {
        $str ="<ul>";
        foreach($acls as $name => $acl){
            if($name == "0") $name = _("All");
            $str .= "<li>".$name;
            $str .= "<ul>";
            if(preg_match("/s/", $acl)){
                $str.="<li><u>"._("Grant permission to owner").'</u></li>'; 
            }
            if(preg_match("/r/", $acl)) $str.="<li>"._("read").'</li>'; 
            if(preg_match("/w/", $acl)) $str.="<li>"._("write").'</li>'; 
            if(preg_match("/c/", $acl)) $str.="<li>"._("Create").'</li>'; 
            if(preg_match("/d/", $acl)) $str.="<li>"._("Remove").'</li>'; 
            if(preg_match("/m/", $acl)) $str.="<li>"._("Move").'</li>'; 
            $str.= "</ul>";
        }
        return($str."</ul>");
    }

    function processAutocomplete()
    {
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(objectClass=gosaAccount)(|(sn=*".get_post('aclTarget')."*)".
                    "(uid=*".get_post('aclTarget')."*)(givenName=*".get_post('aclTarget')."*)))",
                    array('uid','dn','sn','givenName'));
            echo "<ul>";
            while($attrs = $ldap->fetch()){
                $display = $attrs['givenName'][0]." ".$attrs['sn'][0]." [".$attrs['uid'][0]."]";
                echo "<li>{$display}</li>";
                $this->userMap[$display] = $attrs;
            }
            echo "</ul>";
    }
}

?>
