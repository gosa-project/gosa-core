<?php

class aclResolver extends userinfo
{
    private $attrs = array();

    private $affectingACLs = array();
    private $acl_category = "none";

    function __construct($config, $dn, $parent)
    {
        $this->config = &$config;
        $this->dn = $dn;
        $this->parent = &$parent;

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($dn);
        $this->attrs = $ldap->fetch();
   
        if(isset($this->parent->acl_category) && !empty($this->parent->acl_category)){
            $this->acl_category = preg_replace("/\/$/","",$this->parent->acl_category);
        }
 
        if(isset($attrs['uid'][0])){
            $this->uid = $attrs['uid'][0];
        }
    
        $this->ignoreACL = ($this->config->get_cfg_value("ignoreAcl") == $this->dn);
        $this->reset_acl_cache(); 
        $this->loadACL(); 



        foreach($this->allACLs as $dn => $acls){
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
    }

        
    function getReadableACL() 
    {
        $str = "<table summary='"._("Object permissions")."' width='100%'>";
        
        foreach($this->affectingACLs as $dn => $acls){

            foreach($acls as $acl){
              
                $str.="<tr>"; 
                if(isset($this->config->idepartments[$dn])){
                    $image= image("images/select_department.png");
                }else{
                    $image= image("images/lists/element.png");
                }
                $aclTypes= array("reset" => _("Reset ACLs"),
                        "one" => _("One level"),
                        "base" => _("Current object"),
                        "sub" => _("Complete subtree"),
                        "psub" => _("Complete subtree (permanent)"),
                        "role" => _("Use ACL defined in role"));

                $str.="<td style='width:20px;'>".$image."</td>";
                $str.="<td><b>".$dn."</b></td>";
                $str.="<td>".$aclTypes[$acl['type']]."</td>";
                $str.="</tr><tr>";
                $str.="<td></td><td><b>"._("Members")."</b><ul>";
                foreach($acl['members'] as $type => $name){
                    $str .= "<li>".$name."</li>";
                }
                $str .= "</ul>"; 
                $str .= "</td>"; 
                $str.="";
        
                if($acl['type']!='reset'){
                    $str.="<td><b>"._("Acls")."</b><ul>";
                    foreach($acl['acl'] as $type => $name){
                        $str .= "<li>".$type.": <i>".$this->aclToString(implode(array_unique($name)))."</i></li>";
                    }
                    $str .= "</ul>"; 
                    $str .= "</td>"; 
                    $str .= "</tr>"; 
                }
                $str .= "<tr><td colspan=3><hr></td></tr>"; 
            }
        }
                $str .= "</table>"; 
        return($str);
    }

    function aclToString($acl)
    {
        $str ="";
        if(preg_match("/r/", $acl)) $str.= _("read").', '; 
        if(preg_match("/w/", $acl)) $str.= _("write").', '; 
        if(preg_match("/c/", $acl)) $str.= _("Create").', '; 
        if(preg_match("/d/", $acl)) $str.= _("Remove").', '; 
        if(preg_match("/m/", $acl)) $str.= _("Move").', '; 
        if(preg_match("/s/", $acl)) $str.= _("Owner").', '; 
        return(trim($str,', '));
    }
}

?>
    
