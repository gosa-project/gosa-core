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

    function __construct($config,$property)
    {
        $this->property = &$property;
        $this->config = &$config;
    }   

    function getChanges()
    {
        return($this->found);
    }

    function checkForIssues()
    {
        $this->found = array();
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap2= $this->config->get_ldap_link();
        $ldap2->cd($this->config->current['BASE']);

        // If the userRDN wasn't empty, then only search for users inside of the old userRDN.
        $initialValue = $this->prefix.$this->property->getValue().$this->suffix;
        $targetValue = $this->prefix.$this->property->getValue(TRUE).$this->suffix;

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
                    $container = preg_replace("/^[^,]*+,/","",$container);
                }

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

            // If there intially was a userDN given then replace it with the new one.
            if(!empty($initialValue)){

                list($name, $container) = preg_split("/,/",$dn,2);
                if(preg_match("/^".preg_quote($initialValue,'/i')."/", $container)){
                    $container = preg_replace("/^".preg_quote($initialValue,'/')."/",$targetValue,$container);

                    // Ensure that we handle a valid gosaDepartment container.
                    while(!isset($this->config->idepartments[$container])){
                        $container = preg_replace("/^[^,]*+,/","",$container);
                    }


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
        return($found);
    }

    function execute()
    {
        $str = "";
        if(count($this->found['add'])) {
            $str.= "<br><h3>"._("Objects that will be added")."</h3>";
            foreach($this->found['add'] as $dn => $attrs){
                $str.= $dn."<br>";
            }
        }
        if(count($this->found['move'])) {
            $str.= "<br><h3>"._("Objects that will be moved")."</h3>";
            $str.="<pre>";
            foreach($this->found['move'] as $id => $data){
                $checked = (!isset($_POST["migrateNow".get_class($this)])) ? 'checked':'';
                $str.= "<input $checked type='checkbox' value='1' name='migrateEntry_{$id}'>";
                $str.= sprintf(_("Moving object '%s' to '%s'"), $data['from'], $data['to'])."<br>";
            }
            $str.="</pre>";
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
                    $tmp->move($data['from'], $data['to']);
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
