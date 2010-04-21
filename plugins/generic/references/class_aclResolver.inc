<?php

class aclResolver 
{

    private $classMapping = array();
    private $aclTypes = array();
    private $affectingACLs = array();

    function __construct($config, $dn, $parent)
    {
        $this->config = &$config;
        $this->dn = $dn;

        // Get ACL category for the current object.
        if(isset($parent->acl_category) && !empty($parent->acl_category)){
            $this->acl_category = preg_replace("/\/$/","",$parent->acl_category);
        }

        // Build class mapping
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

        // Go through all ACLs and get those matching out DN.
        $ui = get_userinfo();
        foreach($ui->allACLs as $dn => $acls){
            if(preg_match("/".preg_quote($dn,'/')."$/i", $this->dn)){
                foreach($acls as $prio => $acl){
                    if($acl['type'] == "reset"){
                        $this->affectingACLs[$dn][$prio] = $acl;
                        break;
                    }else{
                        foreach($acl['acl'] as $category => $attributes){
                            if(preg_match("/^all($|\/)/", $category) || 
                                    preg_match("/^".$this->acl_category."($|\/)/", $category)){
                                $this->affectingACLs[$dn][$prio] = $acl;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Define ACL type translations
        $this->aclTypes= array("reset" => _("Reset ACLs"),
                "one" => _("One level"),
                "base" => _("Current object"),
                "sub" => _("Complete subtree"),
                "psub" => _("Complete subtree (permanent)"),
                "role" => _("Use ACL defined in role"));
    }


    /*! \brief   Create a human readable HTML result 
     */    
    function getReadableACL() 
    {
        $tpl = 
            "\n <tr>".
            "\n  <td>%s</td>".
            "\n  <td><b>%s</b></td>".
            "\n  <td rowspan=3>%s</td>".
            "\n </tr>".
            "\n <tr>".
            "\n  <td></td>".
            "\n  <td><b>"._("Group members")."</b><ul>%s</ul></td>".
            "\n </tr>".
            "\n <tr>".
            "\n  <td></td>".
            "\n  <td><b>"._("Members")."</b><ul>%s</ul></td>".
            "\n </tr>".
            "\n <tr>".
            "\n  <td></td>".
            "\n  <td><b>"._("Acls")."</b><ul>%s</ul></td>".
            "\n </tr>".
            "\n <tr>".
            "\n  <td colspan=3><hr></td>".
            "\n </tr>";


        $str = "<table summary='"._("Object permissions")."' width='100%'>";
        foreach($this->affectingACLs as $dn => $acls){
            foreach($acls as $acl){
                $gmem = $umem = $defs = "";
                $image = (isset($this->config->idepartments[$dn]))? "images/select_department.png":"images/lists/element.png";
                $aclType = $this->aclTypes[$acl['type']];
                
                foreach($acl['members'] as $type => $name){
                    if(preg_match("/^G/", $type))
                    $gmem .= "\n        <li>".$name."</li>";
                }
                foreach($acl['members'] as $type => $name){
                    if(!preg_match("/^G/", $type))
                    $umem .= "\n        <li>".$name."</li>";
                }

                if($acl['type']!='reset'){
                    foreach($acl['acl'] as $type => $acl){
                        if(isset($this->classMapping[$type])){
                            $defs .= "<li>".$this->classMapping[$type].": ".$this->aclToString($acl)."</li>";
                        }else{
                            $defs .= "<li>".$type.": ".$this->aclToString($acl)."</li>";
                        }
                    }
                }
                $str.= sprintf($tpl, image($image), $dn, $aclType, $gmem, $umem, $defs);
            }
        }
        $str .= "</table>"; 
        return($str);
    }

    function aclToString($acls)
    {
        $str ="<ul>";
        foreach($acls as $name => $acl){

            if($name == "0") $name = _("All");

            $str .= "<li>".$name.": <i>";

            if(preg_match("/r/", $acl)) $str.= _("read").', '; 
            if(preg_match("/w/", $acl)) $str.= _("write").', '; 
            if(preg_match("/c/", $acl)) $str.= _("Create").', '; 
            if(preg_match("/d/", $acl)) $str.= _("Remove").', '; 
            if(preg_match("/m/", $acl)) $str.= _("Move").', '; 
            if(preg_match("/s/", $acl)) $str.= _("Owner").', '; 
            $str = trim($str,', ');
            $str.= "</i></li>";
        }
        return($str."</ul>");
    }
}

?>

