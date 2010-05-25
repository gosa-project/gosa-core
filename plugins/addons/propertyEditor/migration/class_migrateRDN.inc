<?php



class migrateRDN implements propertyMigration
{
    protected $property = NULL;
    protected $config = NULL;
    protected $found = array();
    protected $filter ="";

    // Additional suffixes or prefixes 
    // e.g. for 'faiScriptRDN' (ou=scripts,) moving to new destination lets say 
    //  to 'ou=FAIscripts,' would break stuff with having 'ou=fai,ou=systems,ou=config' 
    // prepended.
    // 
    protected $suffix = ""; 
    protected $prefix = ""; 

    protected $title = "";
    protected $description = "";


    function __construct($config,$property)
    {
        $this->property = &$property;
        $this->config = &$config;

        //Set a dummy title and description
        if(empty($this->title)){
            $this->title = sprintf(_("GOsa migration of property '%s'"), $this->property->getName());
        }
        if(empty($this->description)){
            $this->description = sprintf(_("GOsa has detected objects outside of the configured storage point (%s)."), $this->property->getValue(TRUE));
        }
    }   

    function getChanges()
    {
        return($this->found);
    }

    function checkForIssues()
    {
        $this->found = array('add'=>array(), 'move' => array());
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap2= $this->config->get_ldap_link();
        $ldap2->cd($this->config->current['BASE']);

        // Search for possible release deparments/containers - this enables us to
        //  SKIP release based objects - we cannot move them right now.
        $releases = array();
        $ldap->search("objectClass=FAIbranch");
        while($attrs = $ldap->fetch()){
            $releases[$attrs['dn']] = $attrs['dn'];
        }

        // If the userRDN wasn't empty, then only search for users inside of the old userRDN.
        $initialValue = $this->prefix.$this->property->getValue().$this->suffix;
        $targetValue = $this->prefix.$this->property->getValue(TRUE).$this->suffix;

        if(!empty($initialValue) && !preg_match("/,$/", $initialValue)) $initialValue.=",";
        if(!empty($targetValue) && !preg_match("/,$/", $targetValue)) $targetValue.=",";

        $dnMatch = "";
        if(!empty($initialValue)){
            foreach(preg_split("/,/", $initialValue) as $rdnPart){
                if(empty($rdnPart)) continue;
                list($namingAttrs, $container) = preg_split("/=/",$rdnPart,2);
                $container = trim($container,', ');
                $dnMatch.= "({$namingAttrs}:dn:={$container})";
            }
        }

        // Search for users
        $filter = sprintf($this->filter,$dnMatch);
        $ldap->search($filter,array('dn'));
        $found = FALSE;
        while($attrs = $ldap->fetch()){
            $dn = $attrs['dn'];
            $dnTo = $dn;

            // If there intially was no userDN given then just add the new userRDN to the user dns
            //  and create the new container objects.
            if(empty($initialValue)){
                list($namingAttrs, $container) = preg_split("/=/",$targetValue,2);
                list($name, $container) = preg_split("/,/",$dn,2);

                // Ensure that we handle a valid gosaDepartment container.
                while(!isset($this->config->idepartments[$container])){

                    // This object is part of a FAI release - we better skip it here.
                    if(isset($releases[$container])){
                        break;
                    }

                    $container = preg_replace("/^[^,]*+,/","",$container);
                }
    
                // We haven't found a valid gosaDepartment in this dn, so skip.
                if(isset($this->config->idepartments[$container])){

                    // Queue new containuer to be created.
                    if(!preg_match("/^".preg_quote($targetValue,'/i')."/", $container)){
                        $dnTo = $name.",".$targetValue.$container;
                        if(!$ldap->dn_exists($targetValue.$container)){
                            $this->found['add'][$targetValue.$container] = array(); 
                        }
                        if($dn != $dnTo){
                            $this->found['move'][] = array('from' => $dn, 'to' => $dnTo);
                            $found = TRUE;
                        }
                    }
                }
            }

            // If there intially was a userDN given then replace it with the new one.
            if(!empty($initialValue)){

                list($name, $container) = preg_split("/,/",$dn,2);
                if(preg_match("/^".preg_quote($initialValue,'/i')."/", $container)){
                    $container = preg_replace("/^".preg_quote($initialValue,'/')."/",$targetValue,$container);

                    // Ensure that we handle a valid gosaDepartment container.
                    while(!isset($this->config->idepartments[$container])){

                        // This object is part of a FAI release - we better skip it here.
                        if(isset($releases[$container])){
                            break;
                        }

                        $container = preg_replace("/^[^,]*+,/","",$container);
                    }

                    // We haven't found a valid gosaDepartment in this dn, so skip.
                    if(isset($this->config->idepartments[$container])){


                        $dnTo = $name.",".$targetValue.$container;
                        if(!empty($targetValue) && !$ldap->dn_exists($targetValue.$container)){
                            $this->found['add'][$targetValue.$container] = array(); 
                        }
                        if($dn != $dnTo){
                            $this->found['move'][] = array('from' => $dn, 'to' => $dnTo);
                            $found = TRUE;
                        }
                    }
                }
            }    
        }
        return($found);
    }

    function execute()
    {
        $str = "<h3>".$this->title."</h3>";
        $str.= $this->description;
        $str.= "<hr>";
        if(count($this->found['add'])) {
            $str.= "<br>"._("Objects that will be added");
            $str.= "<ul>";
            foreach($this->found['add'] as $dn => $attrs){
                $str.= "<li>".$dn."</li>";
            }
            $str.= "</ul>";
        }
        if(count($this->found['move'])) {
            $str.= "<br>"._("Objects that will be moved")."<br>";
            $str.= "<ul style='list-style:none; padding: 15px;'>";
            foreach($this->found['move'] as $id => $data){
                $checked = (!isset($_POST["migrateNow".get_class($this)])) ? 'checked':'';
                $str.= "<li>
            
                <span style='white-space:nowrap;'>
                <input $checked type='checkbox' value='1' name='migrateEntry_{$id}'>";
                $str.= sprintf(_("Moving object '%s' to '%s'"), $data['from'], $data['to'])."</span></li>";
            }
            $str.="</ul>";
        }
        $str.= "<button name='migrateNow".get_class($this)."'>"._("Migrate")."</button>";
        return($str);
    }
   
    function save_object()
    {
        if(isset($_POST["migrateNow".get_class($this)])){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);

            // Try to add the new container objects
            foreach($this->found['add'] as $dn => $data){
                $ldap->cd($this->config->current['BASE']);
                $ldap->create_missing_trees(ldap::convert($dn));
            }

            // Now move the objects to the new traget
            $tmp = new plugin($this->config,NULL);
            foreach($this->found['move'] as $id => $data){
                if(isset($_POST["migrateEntry_{$id}"])){
                    if($tmp->move($data['from'], $data['to'])){
                    }elseif($ldap->dn_exists($data['to'])){
                        msg_dialog::display(_("Error"), 
                                sprintf(_("Migration failed for object %s, dn already exists!"), 
                                    bold($data['to'])), ERROR_DIALOG);
                    }else{
                        msg_dialog::display(_("Error"), 
                                sprintf(_("Migration failed for object %s, please check if it already exists!"), 
                                    bold($data['to'])), ERROR_DIALOG);
                    }
                }
            }
            $this->checkForIssues();
        }
    } 

    function check()
    {
        return(array());
    }
} 


?>
