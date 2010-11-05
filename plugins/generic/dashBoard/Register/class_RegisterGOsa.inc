<?php



/* Missing functions, validate registration
 *
 */

class GOsaRegistration extends plugin
{
    protected $selectedRegistrationType = "registered";
    protected $isRegistered = FALSE; 
    protected $step = 0;

    public $attrs = array('mailAddress','surname','givenName', 'company', 'clients', 'knownFrom');
    protected $values = array();

    function __construct(&$config)
    {
        $this->config = $config;    
        $this->isRegistered = GOsaRegistration::isRegistered($this->config);

        foreach($this->attrs as $attr) $this->values[$attr] = "";

        // Try to establish a rpc connection.
        $this->rpcConfigured = FALSE;
        if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){
            $this->rpcConfigured = TRUE;
            $this->rpcHandle = $this->config->getRpcHandle(
                    "http://10.3.64.59:4000",
                    "65717fe6-9e3e-11df-b010-5452005f1250",
                    "WyukwauWoid2",
                    TRUE);
        }
    }


    function registerNow()
    {
        if($this->selectedRegistrationType == "registered"){
            print_a($this->rpcHandle->registerUser('cajus', $this->values));
        }

        $this->step = 200; // Awaiting verification
    }

   
    function execute()
    {
        // Registration request.
        if(isset($_POST['registerNow'])){
            $msgs = $this->check();
            if(count($msgs)){
                msg_dialog::displayChecks($msgs); 
            }else{
                $this->registerNow();
            }
        }


        $smarty = get_smarty();
        $smarty->assign("default", $this->selectedRegistrationType);
        $smarty->assign("step", $this->step);

        foreach($this->attrs as $attr) $smarty->assign($attr, set_post($this->values[$attr]));


        return($smarty->fetch(get_template_path("Register/register.tpl", TRUE))); 
    } 


    function save_object()
    {
        if(isset($_POST['registrationType']))  $this->selectedRegistrationType = get_post('registrationType');
        if(isset($_POST['startRegistration'])) $this->step = 1;
        if(isset($_POST['stepBack'])) $this->step -= 1;

        foreach($this->attrs as $attr){
            if(isset($_POST[$attr])) $this->values[$attr] = get_post($attr);
        }
    }
    


    static function registerGOsa($config, $uuid, $type, $userData = array())
    {
        echo "<br>Register GOsa.";
        print_a(func_get_args());
    }


    static function getRegistrationType($config)
    {
        return($config->configRegistry->getPropertyValue('GOsaRegistration','registerStatus'));
    }    


    static function isRegistered($config, $uuid = NULL)
    {
        $status = $config->configRegistry->getPropertyValue('GOsaRegistration','registerStatus');
        if(!$uuid){
            $uuid = $config->configRegistry->getPropertyValue('GOsaRegistration','registeredUUID');
        }       

        // Not yet registered 
        if(!$status) return(FALSE);
        
        // Registered, but backend status not confirmed.
        echo "<br>Validate registration";
    }


    public static function getRegisterStatusValues()
    {
        $ret= array();
        $ret[''] = _("not registered");
        $ret['anonymous'] = _("anonym");
        $ret['registered'] = _("registered");
        return($ret);
    }

   
    public static function checkRegisteredUUID($message,$class,$name,$value, $type)
    {
        // Add check here later 
        $valid = TRUE;

        // Display the reason for failing this check.
        if($message && !$valid){
            msg_dialog::display(_("Warning"),
                    sprintf(_("The given registration UUID '%s' specified for '%s:%s' is invalid!"),
                        bold($value),bold($class),bold($name)),
                    WARNING_DIALOG);
        }

        return($valid);
    }

 
    static function plInfo()
    {
        return (array(
                    "plProperties"  => array(
                        array(
                            "name"          => "registerStatus",
                            "type"          => "switch",
                            "default"       => "",
                            "defaults"      => "GOsaRegistration::getRegisterStatusValues",
                            "description"   => _("The GOsa registration status."),
                            "check"         => "",
                            "migrate"       => "",
                            "group"         => "registerGOsa",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "registeredUUID",
                            "type"          => "string",
                            "default"       => "",
                            "description"   => _("The UUID of the current GOsa installation. This UUID is used to identify the installation, once you've registered GOsa."),
                            "check"         => "GOsaRegistration::checkRegisteredUUID",
                            "migrate"       => "",
                            "group"         => "registerGOsa",
                            "mandatory"     => FALSE)
                        )));
    }
}
?>
