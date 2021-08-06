<?php

class gospamserver extends goService{

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goSpamServer");
    var $attributes       = array("saRewriteHeader","saTrustedNetworks","saRequiredScore","saFlags","saRule");
    var $StatusFlag       = "saStatus";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("goSpamServer");
    var $Flags            = array("B","b","C","R","D","P");

    var $DisplayName      = "";
    var $dn               = NULL;
    var $cn               = "";
    var $saStatus         = "";

    var $saRewriteHeader  = "";
    var $saTrustedNetworks= array();
    var $TrustedNetworks  = array();
    var $saRequiredScore  = 0;
    var $saFlags          = "";
    var $Rules            = array();
    var $saRule           = array();

    var $saFlagsB         = false;
    var $saFlagsb         = false;
    var $saFlagsC         = false;
    var $saFlagsR         = false;
    var $saFlagsD         = false;
    var $saFlagsP         = false;

    var $ui               = NULL;
    var $acl              = NULL;
    var $view_logged  =FALSE;

    function __construct(&$config,$dn, $parent= NULL)
    {
        /* Init class */
        goService::__construct($config,$dn, $parent);
        $this->DisplayName = _("Spamassassin");

        /* Get userinfo & acls */
        $this->ui = get_userinfo();

        /* Get Flags */
        foreach($this->Flags as $flag){
            $var = "saFlags".$flag;
            if(preg_match("/".$flag."/",$this->saFlags)){
                $this->$var = TRUE;
            }
        }

        /* Get trusted networks */
        $this->TrustedNetworks = array();
        if(isset($this->attrs['saTrustedNetworks']) && is_array($this->attrs['saTrustedNetworks'])){
            $var = $this->attrs['saTrustedNetworks'];
            for($i = 0 ; $i < $var['count'] ; $i ++ ){
                $var2 = $this->attrs['saTrustedNetworks'][$i];
                $this->TrustedNetworks[ $var2 ] = $var2; 
            }
        }

        /* Get rules */
        $this->Rules = array();
        if(isset($this->attrs['saRule']) && is_array($this->attrs['saRule'])){  
            $var = $this->attrs['saRule'];
            for($i = 0 ; $i < $var['count'] ; $i ++ ){
                $var2 = $this->attrs['saRule'][$i];
                $name = preg_replace("/:.*$/","",$var2);
                $value= base64_decode(preg_replace("/^.*:/","",$var2));
                $this->Rules[ $name ] = $value;
            }
        }

        // Prepare lists
        $this->ruleList = new sortableListing();
        $this->ruleList->setDeleteable(true);
        $this->ruleList->setInstantDelete(true);
        $this->ruleList->setEditable(true);
        $this->ruleList->setWidth("100%");
        $this->ruleList->setHeight("170px");
        $this->ruleList->setHeader(array(_("Rule")));
        $this->ruleList->setDefaultSortColumn(0);
        $this->ruleList->setColspecs(array('*','40px'));
    }


    function execute()
    {
        $display ="";
        $smarty = get_smarty(); 

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        /* If displayed, it is ever true*/
        $this->is_account =true;

        /* Get acls */
        $tmp = $this->plinfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        /* Add new trusted network */
        if(isset($_POST['AddNewTrust']) && ($this->acl_is_writeable("saTrustedNetworks"))){
            $this->AddTrust(get_post('NewTrustName'));
        }

        /* Delete selected trusted network */
        if(isset($_POST['DelTrust']) && ($this->acl_is_writeable("saTrustedNetworks"))){
            $this->DelTrust(get_post('TrustedNetworks'));
        }

        /* Add a new rule */
        if(isset($_POST['AddRule']) && $this->acl_is_writeable("saRule")){
            $this->dialog = new goSpamServerRule($this->config,$this->dn);
            $this->dialog->acl_base = $this->acl_base;
            $this->dialog->acl_category = $this->acl_category;
        }

        /* Cancel adding/editing specified rule */
        if(isset($_POST['CancelRule'])){
            $this->dialog = FALSE;
        }

        /* Handle post to delete rules */
        $once = true;
        $this->ruleList->save_object();
        $action = $this->ruleList->getAction();
        if($action['action'] == 'delete'){
            $this->Rules = $this->ruleList->getMaintainedData();
        }
        if($action['action'] == 'edit'){
            $id = $this->ruleList->getKey($action['targets'][0]);
            $rule = $this->Rules[$id];
            $this->dialog = new goSpamServerRule($this->config,$this->dn,$id,$rule);
            $this->dialog->acl_base = $this->acl_base;
            $this->dialog->acl_category = $this->acl_category;
        }

        /* Save rules */
        if(isset($_POST['SaveRule']) && $this->dialog instanceOf goSpamServerRule){
            $this->dialog->save_object();
            $msgs = $this->dialog->check();
            if(count($msgs)){
                foreach($msgs as $msg){
                    msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
                }
            }elseif($this->acl_is_writeable("saRule")){
                $ret = $this->dialog->save();
                if((!empty($ret['orig_name'])) && isset($this->Rules[$ret['orig_name']])){
                    unset($this->Rules[$ret['orig_name']]);
                }
                $this->Rules[$ret['name']] = $ret['rule'];
                $this->dialog = FALSE;
            }
        }

        /* Display dialog if available */ 
        if($this->dialog && $this->dialog->config){
            $this->dialog->save_object();
            return($this->dialog->execute());
        }

        /* Assign smarty vars */
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }

        /* Assign checkbox states */
        foreach($this->Flags as $Flag){
            $var = "saFlags".$Flag;
            $smarty->assign("saFlags".$Flag."ACL", $this->getacl($var));
            if($this->$var){
                $smarty->assign("saFlags".$Flag."CHK"," checked " );
            }else{
                $smarty->assign("saFlags".$Flag."CHK","");
            }
        }

        $this->ruleList->setAcl($this->getacl('saRule'));
        $data =$lData= array();
        foreach($this->Rules as $key => $net){
            $lData[$key]=array('data'=> array($key));
        }
        $this->ruleList->setListData($this->Rules, $lData);
        $this->ruleList->update();

        $smarty->assign("ruleList",$this->ruleList->render());
        $smarty->assign("TrustedNetworks",$this->TrustedNetworks); 

        /* Create Spam score select box entries */
        $tmp = array();
        for($i = 0 ; $i <= 20 ; $i ++ ){
            $tmp[$i] = $i;
        }
        $smarty->assign("SpamScore",$tmp);

        return($display.$smarty->fetch(get_template_path("goSpamServer.tpl",TRUE,dirname(__FILE__))));
    }


    /* Add $post to list of configured trusted */
    function AddTrust($post)
    {
        if(!empty($post)){
            if(tests::is_ip($post) || tests::is_domain($post) || (tests::is_ip_with_subnetmask($post))){
                $this->TrustedNetworks[$post] = $post;
            }else{
                msg_dialog::display(_("Error"), msgPool::invalid(_("Trusted network")), ERROR_DIALOG);
            }
        }
    }


    /* Delete trusted network */
    function DelTrust($posts)
    {
        foreach($posts as $post){
            if(isset($this->TrustedNetworks[$post])){
                unset($this->TrustedNetworks[$post]);     
            }
        }
    }

    function save()
    {
        if(!$this->is_account) return;
        plugin::save();

        /* Create Flags */     
        $this->attrs['saFlags'] = array();
        foreach($this->Flags as $flag){
            $var = "saFlags".$flag;
            if($this->$var){
                $this->attrs['saFlags'].=$flag;
            }
        }

        /* Create trusted network entries */
        $this->attrs['saTrustedNetworks'] = array();
        foreach($this->TrustedNetworks as $net){
            $this->attrs['saTrustedNetworks'][] = $net; 
        }    

        /* Rules */
        $this->attrs['saRule'] = array();
        foreach($this->Rules as $name => $rule){
            $this->attrs['saRule'][] = $name.":".base64_encode($rule);
        }

        /* Check if this is a new entry ... add/modify */
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($this->dn,array("objectClass"));
        if($ldap->count()){
            $ldap->cd($this->dn);
            $ldap->modify($this->attrs);
        }else{
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
        }
        if($this->initially_was_account){
            $this->handle_post_events("modify");
            new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            $this->handle_post_events("add");
            new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
    }

    function check()
    { 
        $message = plugin::check();

        /* Check if required score is numeric */
        if(!is_numeric($this->saRequiredScore)){
            $message[] = msgPool::invalid(_("Score"),$this->saRequiredScore,"/[0-9]/");
        }

        return($message);
    }


    function save_object()
    {
        if(isset($_POST['goSpamServer'])){

            plugin::save_object();

            /* Check flags */
            foreach($this->Flags as $flag){
                $var = "saFlags".$flag;

                if($this->acl_is_writeable($var)){
                    if(isset($_POST[$var])){
                        $this->$var = TRUE;
                    }else{
                        $this->$var = FALSE;
                    }
                }
            }
        }    
    }  


    /* Return plugin informations for acl handling  */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Spamassassin"),
                    "plDescription" => _("Spamassassin")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 89,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('goSpamServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plProvidedAcls"=> array(

                        "saRewriteHeader"   => _("Rewrite header"),
                        "saTrustedNetworks" => _("Trusted networks"),
                        "saRequiredScore"   => _("Required score"),
                        "saRule"            => _("Rules"),


                        "start"           => _("Start"),
                        "stop"            => _("Stop"),
                        "restart"         => _("Restart"),

                        "saFlagsB"           => _("Enable use of Bayes filtering"),
                        "saFlagsb"           => _("Enabled Bayes auto learning"),
                        "saFlagsC"           => _("Enable RBL checks"),
                        "saFlagsR"           => _("Enable use of Razor"),
                        "saFlagsD"           => _("Enable use of DDC"),
                        "saFlagsP"           => _("Enable use of Pyzor"))
                        ));
    }

    /* For newer service management dialogs */
    function getListEntry()
    {
        $fields                 = goService::getListEntry();
        $fields['Message']      = _("Spamassassin");
#$fields['AllowEdit']    = true;
        return($fields);
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
