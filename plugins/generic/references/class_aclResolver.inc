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

    function __construct($config, $dn, $parent)
    {
        $this->config = &$config;
        $this->dn = $dn;

        // Replace this with a user defined one later.
        $ui = get_userinfo();
        $this->validateUid = $ui->uid;
        $this->validateDn = $ui->dn;

        // Get ACL category for the current object.
        if(isset($parent->acl_category) && !empty($parent->acl_category)){
            $this->acl_category = preg_replace("/\/$/","",$parent->acl_category);
        }

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

        $this->reload();
    }

    
    function reload()
    {

        // Go through all ACLs and get those matching out DN.
        $ui = get_userinfo();
        $ui->reset_acl_cache();
        $ui->loadACL();

        foreach($ui->allACLs as $dn => $acls){
            if(preg_match("/".preg_quote($dn,'/')."$/i", $this->dn)){
                foreach($acls as $prio => $acl){
                    if($acl['type'] == "reset"){
                        $this->affectingACLs[$dn][$prio] = $acl;
                        continue;
                    }else{
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

        // Enforce to reload acl result 
        $this->renderedList = "";
    }
    


    /*! \brief   Create a human readable HTML result 
     */    
    function getReadableACL() 
    {
        if(empty($this->renderedList)){

            $tpl = 
                "\n <tr class='acl-viewer-head %s'>".
                "\n  <td>%s</td>".
                "\n  <td colspan=2><b>%s</b>&nbsp;-&nbsp;%s</td>".
                "\n </tr>".
                "\n %s".
                "\n <tr>".
                "\n  <td colspan=3><hr></td>".
                "\n </tr>";

            $filter_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Filter")."</b></td>".
                "\n  <td><ul><li>%s</li></ul></td>".
                "\n </tr>";

            $gmem_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Group members")."</b></td>".
                "\n  <td><ul>%s</ul></td>".
                "\n </tr>";

            $umem_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Members")."</b></td>".
                "\n  <td><ul>%s</ul></td>".
                "\n </tr>";

            $acl_tpl = 
                "\n <tr class='%s'>".
                "\n  <td></td>".
                "\n  <td><b>"._("Acls")."</b></td>".
                "\n  <td><ul>%s</ul></td>".
                "\n </tr>";


            $str = "<table summary='"._("Object permissions")."' class='acl-viewer'>";
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ui = get_userinfo();
            foreach($this->affectingACLs as $dn => $acls){
                foreach($acls as $acl){
                    $gmem = $umem = $defs = "";
                    $image = (isset($this->config->idepartments[$dn]))? "images/select_department.png":"images/lists/element.png";
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

                    // Check if we are part of the member list 
                    if($match){
                        $found = FALSE;
                        foreach($acl['members'] as $mem => $desc){
                            if($mem == "U:{$this->validateDn}"){
                                $found = TRUE;
                                break;
                            }
                            if($mem == "G:*"){
                                $found = TRUE;
                                break;
                            }
                            if(preg_match("/^G:/", $mem)){
                                $gdn = preg_replace("/^G:/","",$mem);
                                $ldap->cat($gdn,array('memberUid'));
                                if($ldap->count()){
                                    $attrs = $ldap->fetch();
                                    if(isset($attrs['memberUid']) && in_array($this->validateUid, $attrs['memberUid'])){
                                        $found = TRUE;
                                    }
                                }
                                break;
                            }
                        }
                        $match = $found;
                    }

                    $class = "";
                    if(!$match){
                        $class = "acl-viewer-blocked";
                    }

                    if(!empty($filter)) $filter =sprintf($filter_tpl,$class,$filter);

                    foreach($acl['members'] as $type => $name){
                        if(preg_match("/^G/", $type))
                            $gmem .= "\n        <li>".$name."</li>";
                    }
                    if(!empty($gmem)) $gmem =sprintf($gmem_tpl,$class,$gmem);

                    foreach($acl['members'] as $type => $name){
                        if(!preg_match("/^G/", $type))
                            $umem .= "\n        <li>".$name."</li>";
                    }
                    if(!empty($umem)) $umem = sprintf($umem_tpl,$class,$umem);

                    if($acl['type']!='reset'){
                        foreach($acl['acl'] as $type => $acl){
                            if(isset($this->classMapping[$type])){
                                $defs .= "<li>".$this->classMapping[$type].": ".$this->aclToString($acl)."</li>";
                            }else{
                                $defs .= "<li>".$type.": ".$this->aclToString($acl)."</li>";
                            }
                        }
                        if(!empty($defs)) $defs = sprintf($acl_tpl, $class,$defs);
                    }
                    $str.= sprintf($tpl,$class, image($image), $dn, $aclType, $filter.$gmem.$umem.$defs);
                }
            }
            $str .= "</table>"; 
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
                $str.="<li>"._("Grant permission to owner").'</li>'; 
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
}

?>

