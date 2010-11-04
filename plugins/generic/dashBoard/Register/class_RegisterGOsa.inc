<?php



/* Missing functions, validate registration
 *
 */

class GOsaRegistration extends plugin
{

    protected $isRegistered = FALSE; 

    function __construct(&$config)
    {
        $this->config = $config;    
        $this->isRegistered = GOsaRegistration::isRegistered($this->config);
    }


   
    function execute()
    {
        $smarty = get_smarty();

        return($smarty->fetch(get_template_path("Register/register.tpl", TRUE))); 
    } 


    static function registerGOsa($config, $uuid, $type, $userData = array())
    {

        echo "<br>Register GOsa.";
        print_a(func_get_args());
        
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
