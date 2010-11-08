<?php



/* Missing functions, validate registration
 *
 */

class RegistrationDialog extends plugin
{
    protected $selectedRegistrationType = "registered";
    protected $isRegistered = FALSE; 
    protected $step = 0;

    public $attrs = array(
            
            // User table
            'uid','password','password_repeated','restoreQuestion','restoreAnswer',
            'mailAddress','surname','givenName','newsletter',

            // Contact table
            'phone','contact','contact_mail','company','street',
            'postal_code','city','trade','employees','description');


    protected $values = array();

    function __construct(&$config)
    {
        $this->config = $config;    

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

            // Register user 
            print_a($this->rpcHandle->registerUser($this->values['uid'], $this->values));

            // Register Instance with User.
        }

        #$this->step = 200; // Awaiting verification
    }


    function check()
    {
        $message  = array();
        if($this->step == 1){

            // Checking for a valid 'uid'.
            if(empty($this->values['uid'])) {
                $message[] = msgPool::required(_("Account-ID"));
            }elseif(preg_match('/[^a-z0-9_\-]/', $this->values['uid'])){
                $message[] = msgPool::invalid(_("Account-ID"), $this->values['uid'],'/[a-z0-9_-]/');
            }

            // Checking for a valid 'mailAddress'.
            if(empty($this->values['mailAddress'])) {
                $message[] = msgPool::required(_("Mail address"));
            }elseif(!tests::is_email($this->values['mailAddress'])){
                $message[] = msgPool::invalid(_("Mail address"), $this->values['mailAddress']);
            }

            // Checking for a valid 'password'.
            if(empty($this->values['password'])) {
                $message[] = msgPool::required(_("Password"));
            }elseif($this->values['password'] != $this->values['password_repeated']){
                $message[] = _("The given password and repeated password do not match!");
            }

            // Checking for a valid password restore message.
            if(empty($this->values['restoreQuestion'])) {
                $message[] = msgPool::required(_("Password restore question"));
            }

            // Checking for a valid restore answer.
            if(empty($this->values['restoreAnswer'])) {
                $message[] = msgPool::required(_("Password restore answer"));
            }

            // Checking for a valid 'givenName'.
            if(empty($this->values['givenName'])) {
                $message[] = msgPool::required(_("Given name"));
            }

            // Checking for a valid surname.
            if(empty($this->values['surname'])) {
                $message[] = msgPool::required(_("Surname"));
            }
        }
        
        return($message);
    }

   
    function execute()
    {
        // Registration page one filled in, next step requested.
        if(isset($_POST['registerPage2'])){
            $msgs = $this->check();
            if(count($msgs)){
                msg_dialog::displayChecks($msgs); 
            }else{
                $this->step = 2;
            }
        }

        // Registration page one filled in, next step requested.
        if(isset($_POST['registerPage3'])){
            $msgs = $this->check();
            if(count($msgs)){
                msg_dialog::displayChecks($msgs); 
            }else{
                $this->step = 200;
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
        // get Newsletter Checkbox value
        if($this->step == 1){
             $this->values['newsletter'] = isset($_POST['newsletter']);
        }
        foreach($this->attrs as $attr){
            if(isset($_POST[$attr])) $this->values[$attr] = get_post($attr);
        }

        if(isset($_POST['registrationType']))  $this->selectedRegistrationType = get_post('registrationType');
        if(isset($_POST['startRegistration'])) $this->step = 1;
        if(isset($_POST['stepBack'])) $this->step -= 1;

    }
}
?>
