<?php

class goMailServer extends goService{

    var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports mailqueue listings and so on.";
    var $cli_description  = "Some longer text\nfor help";
    var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goMailServer");

    /* This class can't be assigned twice so it conflicts with itsself */
    var $DisplayName      = "";
    var $dn               = NULL;
    var $StatusFlag       = "goMailServerStatus";
    var $attributes       = array("description","postfixHeaderSizeLimit",
            "postfixMailboxSizeLimit","postfixMessageSizeLimit",
            "postfixMyDestinations","postfixMyDomain","postfixMyhostname",
            "postfixMyNetworks","postfixRelayhost","postfixTransportTable",
            "postfixSenderRestrictions","postfixRecipientRestrictions");

    var $goMailServerStatus               ;
    var $postfixHeaderSizeLimit           = 0;
    var $postfixMailboxSizeLimit          = 0;
    var $postfixMessageSizeLimit          = 0;
    var $postfixMyDestinations            = array();
    var $postfixMyDomain                  = "";
    var $postfixMyhostname                = "";
    var $postfixMyNetworks                = array();
    var $postfixRelayhost                 = "";
    var $postfixTransportTable            = array();
    var $postfixSenderRestrictions        = array();
    var $postfixRecipientRestrictions     = array();
    var $description                      = "";
    var $RestrictionFilters               = array();
    var $TransportProtocols               = array(); 
    var $Actions                          = array();
    var $cn                               = "";
    var $conflicts                        = array("goMailServer","kolab");
    var $view_logged  =FALSE;

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("Mail SMTP service (Postfix)");

        $this->Actions = array( SERVICE_STOPPED=>SERVICE_STOPPED,
                SERVICE_STARTED => SERVICE_STARTED,
                SERVICE_RESTARTED=>SERVICE_RESTARTED); 

        /* Fill  RestrictionFilters   TransportProtocols  from external hooks */
        $str = $this->config->data['TABS']['SERVERSERVICE'];
        $this->TransportProtocols =array("smtp"=>"SMTP");
        $this->RestrictionFilters = array("FILTER"=>"FILTER"); 
        foreach(
                array(
                    "postfixRestrictionFilters"=>"RestrictionFilters",
                    "postfixProtocols"        =>"TransportProtocols")
                as $file => $var){
            if($this->config->get_cfg_value("goMailServer",$file) != ""){
                $file = $this->config->get_cfg_value("goMailServer",$file);
                if((isset($file)) && is_readable($file)){
                    $tmp = file_get_contents($file);
                    $tmp2= preg_split("/\n/",$tmp);
                    foreach($tmp2 as $entry){
                        if(empty($entry)) continue;
                        if(preg_match("/:/",$entry)){
                            $tmp3 = explode(":",$entry);
                            $r = $this->$var;
                            $r[$tmp3[0]]=$tmp3[1];
                            $this->$var = $r;
                        }else{
                            $r = $this->$var;
                            $r[$entry] =$entry;
                            $this->$var = $r;
                        }
                    }
                }
            }
        }


        /* Get postfix my networks */
        $this->postfixMyNetworks = array();
        $tmp = array();
        if(isset($this->attrs['postfixMyNetworks'][0])){
            $tmp = explode(",",$this->attrs['postfixMyNetworks'][0]);
            foreach($tmp as $str){
                if(!empty($str)){
                    $this->postfixMyNetworks[base64_encode($str)] = $str;
                }
            }
        }


        /* Create full name */
        if(isset($this->attrs['postfixMyDomain'][0])){
            $this->postfixMyhostname .= ".".$this->attrs['postfixMyDomain'][0];
        }


        /* Get postfix my domains */
        $this->postfixMyDestinations = array(); 
        if(isset($this->attrs['postfixMyDestinations'][0])){
            unset($this->attrs['postfixMyDestinations']['count']);
            foreach($this->attrs['postfixMyDestinations'] as $str){
                $this->postfixMyDestinations[base64_encode($str)] = $str;
            }
        }


        /* Get transport tables */
        $tmp = array();
        $this->postfixTransportTable = array();
        if(isset($this->attrs['postfixTransportTable'])){
            $tmp = array();
            unset($this->attrs['postfixTransportTable']['count']);
            foreach($this->attrs['postfixTransportTable'] as $entry){

                //0: offshore.vip.ms-europa.lhsystems.com smtp:172.28.0.2

                $Number   = preg_replace('/^([^:]+):.*$/', '\\1', $entry);
                $Rest     = trim(preg_replace("/^[0-9]*:/","",$entry));

                $Protocol_Destination = preg_replace("/^.*\ /","",$Rest);
                $Source               = preg_replace("/\ .*$/","",$Rest);

                $Protocol     = preg_replace ('/^([^:]+):.*$/', '\\1' ,trim($Protocol_Destination));
                $Destination  = preg_replace ('/^[^:]+:(.*)$/', '\\1'    ,trim($Protocol_Destination));

                $Destination  = preg_replace ("/[\[\]]/","",$Destination);

                $tmp[$Number]['src'] = $Source;
                $tmp[$Number]['dst'] = $Destination;
                $tmp[$Number]['prt'] = $Protocol;
            }
            ksort($tmp);
            foreach($tmp as $entry){
                $this->postfixTransportTable[] = $entry;
            }
        }


        /* Get sender restrictions */
        $tmp = array();
        $this->postfixSenderRestrictions = array();
        if(isset($this->attrs['postfixSenderRestrictions'])){
            unset($this->attrs['postfixSenderRestrictions']['count']);
            foreach($this->attrs['postfixSenderRestrictions'] as $entry){
                $nr  = preg_replace("/:.*$/","",$entry);
                $rest= trim(preg_replace("/^[^:]+:/","",$entry));
                $src = preg_replace("/ .*$/","",$rest);
                $rest= preg_replace("/^[^ ]+ /","",$rest);
                $dst = preg_replace("/^.* /","",$rest);
                $prt = preg_replace("/ .*$/","",$rest);

                $tmp[$nr]['src'] = $src;
                $tmp[$nr]['dst'] = $dst;
                $tmp[$nr]['filter'] = $prt;
            }
            ksort($tmp);
            foreach($tmp as $entry){
                $this->postfixSenderRestrictions[] = $entry;
            }
        }


        /* Get sender restrictions */
        $tmp = array();
        $this->postfixRecipientRestrictions = array();
        if(isset($this->attrs['postfixRecipientRestrictions'])){
            unset($this->attrs['postfixRecipientRestrictions']['count']);
            foreach($this->attrs['postfixRecipientRestrictions'] as $entry){
                $nr  = preg_replace("/:.*$/","",$entry);
                $rest= trim(preg_replace("/^[^:]+:/","",$entry));
                $src = preg_replace("/ .*$/","",$rest);
                $rest= preg_replace("/^[^ ]+ /","",$rest);
                $dst = preg_replace("/^.* /","",$rest);
                $prt = preg_replace("/ .*$/","",$rest);

                $tmp[$nr]['src'] = $src;
                $tmp[$nr]['dst'] = $dst;
                $tmp[$nr]['filter'] = $prt;
            }
            ksort($tmp);
            foreach($tmp as $entry){
                $this->postfixRecipientRestrictions[] = $entry;
            }
        }

        // Prepare lists
        $this->recipientRestrictionList = new sortableListing(array(),array(), TRUE);
        $this->recipientRestrictionList->setDeleteable(true);
        $this->recipientRestrictionList->setEditable(false);
        $this->recipientRestrictionList->setWidth("100%");
        $this->recipientRestrictionList->setHeight("100px");
        $this->recipientRestrictionList->setDefaultSortColumn(0);
        $this->recipientRestrictionList->setColspecs(array('*','*','*','20px'));
        $this->recipientRestrictionList->setHeader(array(_("Source"),_("Destination"),_("Filter")));

        $this->senderRestrictionList = new sortableListing(array(),array(), TRUE);
        $this->senderRestrictionList->setDeleteable(true);
        $this->senderRestrictionList->setEditable(false);
        $this->senderRestrictionList->setWidth("100%");
        $this->senderRestrictionList->setHeight("100px");
        $this->senderRestrictionList->setDefaultSortColumn(0);
        $this->senderRestrictionList->setColspecs(array('*','*','*','20px'));
        $this->senderRestrictionList->setHeader(array(_("Source"),_("Destination"),_("Filter")));

        $this->protocolsList = new sortableListing(array(),array(), TRUE);
        $this->protocolsList->setDeleteable(true);
        $this->protocolsList->setEditable(false);
        $this->protocolsList->setWidth("100%");
        $this->protocolsList->setHeight("100px");
        $this->protocolsList->setColspecs(array('*','*','*','20px'));
        $this->protocolsList->setHeader(array(_("Source"),_("Destination"),_("Protocol")));
        $this->protocolsList->setDefaultSortColumn(0);

    }

    function execute()
    { 
        $smarty   = get_smarty();


        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }


        /* Add delete my network entry */
        if($this->acl_is_writeable("postfixMyNetworks")){
            if((isset($_POST['AddpostfixMyNetworks'])) && (!empty($_POST['NewString_postfixMyNetworks']))){
                $str = get_post('NewString_postfixMyNetworks');
                $this->postfixMyNetworks[base64_encode($str)] = $str;
            }
            if((isset($_POST['DelpostfixMyNetworks'])) && 
                    isset($_POST['Select_postfixMyNetworks']) && 
                    (count($_POST['Select_postfixMyNetworks']))){
                foreach(get_post('Select_postfixMyNetworks') as $str ){
                    if(is_array($str)) {
                      unset($this->postfixMyNetworks[$str[0]]);
                    } else {
                      unset($this->postfixMyNetworks[$str]);
                    }
                }
            }
        }


        /* Add delete my domain entry */
        if($this->acl_is_writeable("postfixMyDestinations")){
            if((isset($_POST['AddpostfixMyDestinations'])) && (!empty($_POST['NewString_postfixMyDestinations']))){
                $str = get_post('NewString_postfixMyDestinations');
                $this->postfixMyDestinations[base64_encode($str)] = $str;
            }

            if((isset($_POST['DelpostfixMyDestinations'])) && isset($_POST['Select_postfixMyDestinations']) &&(count($_POST['Select_postfixMyDestinations']))){
                foreach(get_post('Select_postfixMyDestinations') as $str ) {
                    if(is_array($str)) {
                        unset($this->postfixMyDestinations[$str[0]]);
                    } else {
                        unset($this->postfixMyDestinations[$str]);
                    }
                }
            }
        }


        $this->senderRestrictionList->save_object();
        $this->recipientRestrictionList->save_object();
        $this->protocolsList->save_object();

        $action = $this->senderRestrictionList->getAction();
        if($action['action'] == 'reorder'){
            $this->postfixSenderRestrictions = $this->senderRestrictionList->getMaintainedData();
        }
        if($action['action'] == 'delete'){
            $id = $this->senderRestrictionList->getKey($action['targets'][0]);
            unset($this->postfixSenderRestrictions[$id]);
        }

        $action = $this->recipientRestrictionList->getAction();
        if($action['action'] == 'reorder'){
            $this->postfixRecipientRestrictions = $this->recipientRestrictionList->getMaintainedData();
        }
        if($action['action'] == 'delete'){
            $id = $this->recipientRestrictionList->getKey($action['targets'][0]);
            unset($this->postfixRecipientRestrictions[$id]);
        }

        $action = $this->protocolsList->getAction();
        if($action['action'] == 'reorder'){
            $this->postfixTransportTable = $this->protocolsList->getMaintainedData();
        }
        if($action['action'] == 'delete'){
            $id = $this->protocolsList->getKey($action['targets'][0]);
            unset($this->postfixTransportTable[$id]);
        }

        /* Add sender restriction */
        if($this->acl_is_writeable("postfixSenderRestrictions")){
            if(isset($_POST['AddpostfixSenderRestrictions'])){
                $src      = get_post('Source_postfixSenderRestrictions');
                $dst      = get_post('Destination_postfixSenderRestrictions');
                $Filter   = get_post('SenderRestrictionFilter');
                $tmp = array();
                $tmp['src']     = $src;
                $tmp['dst']     = $dst;
                $tmp['filter']  = $Filter;
                $this->postfixSenderRestrictions[] = $tmp;
            }
        }


        /* Add sender restriction */
        if($this->acl_is_writeable("postfixRecipientRestrictions")){
            if(isset($_POST['AddpostfixRecipientRestrictions'])){
                $src      = get_post('Source_postfixRecipientRestrictions');
                $dst      = get_post('Destination_postfixRecipientRestrictions');
                $Filter   = get_post('RecipientRestrictionFilter');
                $tmp = array();
                $tmp['src']     = $src;
                $tmp['dst']     = $dst;
                $tmp['filter']  = $Filter;
                $this->postfixRecipientRestrictions[] = $tmp;
            }
        }

        /* Handle transports */
        if($this->acl_is_writeable("postfixTransportTable")){
            if(isset($_POST['AddpostfixTransportTable'])){
                $src = trim(get_post('Source_postfixTransportTable'));  
                $dst = trim(get_post('Destination_postfixTransportTable'));    
                $prt = trim(get_post('TransportProtocol'));

                $tmp2 = array();
                if((!empty($src)) && (!empty($dst))){
                    if(preg_match("/:/",$dst)){
                        $tmp = explode(":",$dst);
                        $port = trim($tmp[1]);
                        $ip   = trim($tmp[0]);

                        if((tests::is_ip($ip)) && (is_numeric($port))){
                            $dst = "[".$ip."]:".$port;
                        }
                    }
                    if(tests::is_ip($dst)){
                        $dst = "[".$dst."]";
                    }
                    $tmp2 ['src'] = $src;
                    $tmp2 ['dst'] = $dst;
                    $tmp2 ['prt'] = $prt;

                    $this->postfixTransportTable[] = $tmp2;  
                } 
            }
        }

        /* Set attributes */
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }


        /* Create list for translation tables */
        $this->protocolsList->setAcl($this->getacl('postfixTransportTable'));
        $lData = array();
        foreach($this->postfixTransportTable as $key => $entry){
            $lData[$key]=array('data' => array($entry['src'],$entry['dst'],$entry['prt']));
        }
        $this->protocolsList->setListData($this->postfixTransportTable, $lData);
        $this->protocolsList->update();
        $smarty->assign("postfixTransportTableList" ,$this->protocolsList->render());


        /* Create list for sender restrictions */
        $this->senderRestrictionList->setAcl($this->getacl('postfixSenderRestrictions'));
        $lData = array();
        foreach($this->postfixSenderRestrictions as $key => $entry){
            $lData[$key]=array('data' => array($entry['src'],$entry['dst'],$entry['filter']));
        }
        $this->senderRestrictionList->setListData($this->postfixSenderRestrictions, $lData);
        $this->senderRestrictionList->update();
        $smarty->assign("postfixSenderRestrictionsList" ,$this->senderRestrictionList->render());


        /* Create list for translation tables */
        $this->recipientRestrictionList->setAcl($this->getacl('postfixRecipientRestrictions'));
        $lData = array();
        foreach($this->postfixRecipientRestrictions as $key => $entry){
            $lData[$key]=array('data' => array($entry['src'],$entry['dst'],$entry['filter']));
        }
        $this->recipientRestrictionList->setListData($this->postfixRecipientRestrictions, $lData);
        $this->recipientRestrictionList->update();
        $smarty->assign("postfixRecipientRestrictionsList" ,$this->recipientRestrictionList->render());


        /* set new status */
        if(isset($_POST['ExecAction'])){
            if(isset($this->Actions[$_POST['action']])){
                $this->setStatus(get_post('action'));
            }
        }


        $smarty->assign("is_new",                      set_post($this->dn));
        $smarty->assign("is_acc",                      set_post($this->initially_was_account));
        $smarty->assign("TransportProtocols",          set_post($this->TransportProtocols));
        $smarty->assign("Actions",                     set_post($this->Actions));
        $smarty->assign("RestrictionFilters",          set_post($this->RestrictionFilters));
        $smarty->assign("postfixTransportTable" ,      set_post($this->getTransports()));
        $smarty->assign("postfixSenderRestrictions" ,  set_post($this->getSenderRestrictions()));
        $smarty->assign("postfixRecipientRestrictions",set_post($this->getRecipientRestrictions()));

        return($smarty->fetch(get_template_path("goMailServer.tpl",TRUE,dirname(__FILE__))));
    }


    /* return transports formated for select box */ 
    function getTransports()
    {
        $ret = array();
        foreach($this->postfixTransportTable as $key => $vals){
            $ret[$key] = $vals['src']." -> ".$vals['prt'].":".$vals['dst'];
        }
        return($ret);
    }


    /* return sender restriction formated for select box */ 
    function getSenderRestrictions()
    {
        $ret = array();
        foreach($this->postfixSenderRestrictions as $key => $vals){
            $ret[$key] = $vals['src']." ".$vals['filter']." ".$vals['dst'];
        }
        return($ret);
    }


    /* return recipient restriction formated for select box */ 
    function getRecipientRestrictions()
    {
        $ret = array();
        foreach($this->postfixRecipientRestrictions as $key => $vals){
            $ret[$key] = $vals['src']." ".$vals['filter']." ".$vals['dst'];
        }
        return($ret);
    }


    /* Return list entry */
    function getListEntry()
    {
        $fields               = goService::getListEntry();
        $fields['Message']    = _("Mail SMTP service (Postfix)");
#$fields['AllowEdit']  = true;
        return($fields);
    }


    function save()
    {
        $this->postfixMyDomain   = preg_replace("/^[^\.]+\./","",$this->postfixMyhostname);
        $this->postfixMyhostname = preg_replace("/\..*$/","",$this->postfixMyhostname);

        plugin::save();

        /* Fix transport table*/
        $i = 0 ; 
        $this->attrs['postfixTransportTable'] = array();
        foreach($this->postfixTransportTable as $key => $entry){
            $this->attrs['postfixTransportTable'][] = $i.": ".$entry['src']." ".$entry['prt'].":".$entry['dst'];
            $i ++;
        }


        /* Fix sender restrictions */
        $i = 0;
        $this->attrs['postfixSenderRestrictions'] =array();
        foreach($this->postfixSenderRestrictions as $key => $entry){
            $this->attrs['postfixSenderRestrictions'][] = $i.": ".$entry['src']." ".$entry['filter']." ".$entry['dst']; 
            $i ++;  
        }


        /* Fix recipient restrictions */
        $i = 0;
        $this->attrs['postfixRecipientRestrictions'] =array();
        foreach($this->postfixRecipientRestrictions as $key => $entry){
            $this->attrs['postfixRecipientRestrictions'][] = $i.": ".$entry['src']." ".$entry['filter']." ".$entry['dst']; 
            $i ++;  
        }


        /* Fix mydomains */
        $this->attrs['postfixMyDestinations']  = array();
        foreach($this->postfixMyDestinations as $entry){
            $this->attrs['postfixMyDestinations'][] =$entry;
        }


        /* Fix mydomains */
        if(count($this->postfixMyNetworks)){
            $this->attrs['postfixMyNetworks']  = "";
            foreach($this->postfixMyNetworks as $entry){
                $this->attrs['postfixMyNetworks'] .=$entry.",";
            }
            $this->attrs['postfixMyNetworks'] = preg_replace("/,$/","",$this->attrs['postfixMyNetworks']);
        }else{
            $this->attrs['postfixMyNetworks']  = array();
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
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

        if($this->initially_was_account){
            $this->handle_post_events("modify");
            new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
        }else{
            $this->handle_post_events("add");
            new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
        }

    }


    function check()
    { 
        $message =plugin::check();
        if(!is_numeric($this->postfixHeaderSizeLimit)){
            $message[] = msgPool::invalid(_("Header size limit"),$this->postfixHeaderSizeLimit,"/[0-9]/");
        }
        if(!is_numeric($this->postfixMailboxSizeLimit)){
            $message[] = msgPool::invalid(_("Mailbox size limit"),$this->postfixMailboxSizeLimit,"/[0-9]/");
        }
        if(!is_numeric($this->postfixMessageSizeLimit)){
            $message[] = msgPool::invalid(_("Message size limit"),$this->postfixMessageSizeLimit,"/[0-9]/");
        }
        return $message;
    }


    function save_object()
    {
        plugin::save_object();  
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $source_o = new goMailServer($this->config,$source['dn']);
        foreach(array("postfixMyDomain","postfixMyhostname","postfixMyNetworks","postfixTransportTable","postfixSenderRestrictions","postfixRecipientRestrictions","postfixMyDestinations") as $attr){
            $this->$attr = $source_o->$attr;
        }
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Mail SMTP (Postfix)"),
                    "plDescription" => _("Mail SMTP - Postfix")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 98,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('goMailServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),

                    "plProperties"  => array(
                        array(
                            "name"          => "postfixProtocols",
                            "type"          => "file",
                            "default"       => "",
                            "description"   => _("File containing user defined protocols.")."

                            File syntax:
                            name1:Description1
                            name2:Description2",

                            "check"         => "gosaProperty::isReadableFile",
                            "migrate"       => "",
                            "group"         => "mail",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "postfixRestrictionFilters",
                            "type"          => "file",
                            "default"       => "",
                            "description"   => _("File containing user defined restriction filters.")."

                            File syntax:
                            name1:Description1
                            name2:Description2",
                            "check"         => "gosaProperty::isReadableFile",
                            "migrate"       => "",
                            "group"         => "mail",
                            "mandatory"     => FALSE),
                        ),

                        "plProvidedAcls"=> array(

                                "start"           => _("Start"),
                                "stop"            => _("Stop"),
                                "restart"         => _("Restart"),
                                "postfixMyhostname"           => _("Visible fully qualified host name"),
                                "description"                 => _("Description"), 
                                "postfixHeaderSizeLimit"      => _("Header size limit"), 
                                "postfixMailboxSizeLimit"     => _("Max mailbox size"), 
                                "postfixMessageSizeLimit"     => _("Max message size"), 
                                "postfixMyDestinations"       => _("Domains to accept mail for"), 
                                "postfixMyNetworks"           => _("Local networks"), 
                                "postfixRelayhost"            => _("Relay host"), 
                                "postfixTransportTable"       => _("Transport table"), 
                                "postfixSenderRestrictions"   => _("Restrictions for sender"), 
                                "postfixRecipientRestrictions"=> _("Restrictions for recipient"))
                            ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
