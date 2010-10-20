<?php


class jsonRPC {

    private $curlHandler = NULL;
    private $config;
    private $id;
    private $lastStats = array();
    private $lastResult = array();
    private $lastAction = "none";


    private $connectUrl = "";
    private $username = "";
    private $userPassword = "";
    private $authModeDigest = FALSE; 


    /*! \brief  This function is used by the property editor and checks the 
     *           given rpc connection informations.
     */
    public static function testConnectionProperties($message,$class,$name,$value, $type)
    {
        global $config;

        // Get currently used connection usernamem and password.
        // We use the temporary values, due to the fact, that we do not want to test
        //  the current values, we want to test the modified values.
        $user = $config->configRegistry->getProperty('core','gosaRpcUser');
        $username =  $user->getValue($temporaryValue = TRUE);
        $passwd = $config->configRegistry->getProperty('core','gosaRpcPassword');
        $passwdString =  $passwd->getValue($temporaryValue = TRUE);

        $connection = new jsonRPC($config, $value, $username, $passwdString);        
        if(!$connection->success() && $message){
            msg_dialog::display(_("Warning"),
                    sprintf(_("The rpc connection (%s) specified for '%s:%s' is invalid! Error was: %s."),
                        bold($value),bold($class),bold($name), bold($connection->get_error())),
                    WARNING_DIALOG);

        }
        
        return($connection->success());
    }


    /*! \brief      Constructs a new jsonRPC handle which is connected to a given URL.
     *              It can either connect using a rpc method or via auth method digest.
     *  @param      object      The gosa configuration object (class_config)
     *  @param      string      The url to connect to. 
     *  @param      string      The username for authentication
     *  @param      string      The password to use for authentication
     *  @param      boolean     Whether to use DIGEST authentication or not.
     *  @return     
     */
    public function __construct($config, $connectUrl, $username, $userPassword, $authModeDigest=FALSE) 
    {
        $this->config = $config;
        $this->id = 0;

        // Get connection data
        $this->connectUrl     = $connectUrl; 
        $this->username       = $username; 
        $this->userPassword   = $userPassword; 
        $this->authModeDigest = $authModeDigest;

        // Put some usefull info in the logs 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->connectUrl), "Initiated RPC "); 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->username), "RPC user: "); 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->userPassword),"RPC password: "); 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->authModeDigest),"Digest Auth (0: No, 1: Yes): "); 

        $this->__login();
    }


    /*! \brief          
     *  @param      
     *  @return     
     */
    private function __login()
    {
        // Init Curl handler
        $this->curlHandler = curl_init($this->connectUrl);

        // Set curl options
        curl_setopt($this->curlHandler, CURLOPT_URL ,           $this->connectUrl);
        curl_setopt($this->curlHandler, CURLOPT_POST ,          TRUE);
        curl_setopt($this->curlHandler, CURLOPT_RETURNTRANSFER ,TRUE);
        curl_setopt($this->curlHandler, CURLOPT_HTTPHEADER ,    array('Content-Type: application/json'));
        curl_setopt($this->curlHandler, CURLOPT_SSL_VERIFYPEER, FALSE);

        // Try to login 
        if($this->authModeDigest){
            if(!empty($this->username)) 
                curl_setopt($this->curlHandler, CURLOPT_USERPWD , "{$this->username}:{$this->userPassword}");
            curl_setopt($this->curlHandler, CURLOPT_HTTPAUTH , CURLAUTH_ANYSAFE);
        }else{
            curl_setopt($this->curlHandler, CURLOPT_COOKIESESSION , TRUE);
            curl_setopt($this->curlHandler, CURLOPT_COOKIEFILE, 'cookiefile.txt'); 
            if(!empty($this->username)) 
                $this->login($this->username, $this->userPassword);
        }
    }


    /*! \brief      Returns the last HTTP status code.  
     *  @return     int         The last status code.          
     */
    public function getHTTPstatusCode()
    {
        return((isset($this->lastStats['http_code']))? $this->lastStats['http_code'] : -1 );
    }


    /*! \brief      Returns the last error string. 
     *  @return     string      The last error message.
     */
    public function get_error()
    {
        if($this->lastStats['http_code'] != 200){
            $error = $this->getHttpStatusCodeMessage($this->lastStats['http_code']);
            if(isset($this->lastResult['error']['message'])){
                $error .= ": ".$this->lastResult['error']['message']; 
            }
            return($error);
        }else{
            return(curl_error($this->curlHandler));
        }
    }

    

    /*! \brief      Returns TRUE if the last action was successfull else FALSE.
     *  @return     boolean     TRUE on success else FALSE. 
     */
    public function success()
    {
        return(curl_errno($this->curlHandler) == 0 && $this->lastStats['http_code'] == 200);
    }


    /*! \brief      The class destructor, it destroys open rpc handles if needed.
     */
    public function __destruct()
    {
        if($this->curlHandler){
            curl_close($this->curlHandler);
        }
    }


    /*! \brief      This is some kind of catch-all method, all unknown method names will 
     *               will be interpreted as rpc request. 
     *              If you call "$this->blafasel" this method will initiate an rpc request 
     *               for method 'blafasel'.
     *  @param      string  method   The rpc method to execute.
     *  @param      params  array    The parameter to use.
     *  @return     mixed            The request result.
     */
    public function __call($method,$params) 
    {
        // Check if handle is still valid!
        if(!$this->curlHandler && $this->lastAction != 'login'){
            $this->__login();
        }

        // Start request
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,"{$method}", "Calling: "); 
        $response = $this->request($method,$params);
        if($this->success()){
            DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,
                    (is_array($response['result']))?$response['result']:bold($response['result']), "Result: "); 
        }else{
            DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->get_error())."<br>".$response, "Result (FAILED): "); 
        }

        global $config;
        $debugLevel = $config->get_cfg_value('core', 'debugLevel'); 
        if($debugLevel & DEBUG_RPC){
            print_a(array('CALLED:' => array($method => $params)));
            print_a(array('RESPONSE' => $response));
        }
        $return = $response['result'];
        
        // Create remote-object handling using the jsonROP class
        if(class_available('jsonROP')){
            $return = jsonROP::inspectJsonResult($return);
        }

        return($return);
    }


    /*! \brief      This method finally initiates the real RPC requests and handles 
     *               the result from the server.
     *  @param      string  method      The method to call 
     *  @param      array   params      The paramter to use.
     *  @return     mixed   The server response. 
     */
    private function request($method, $params)
    {
        // Set last action 
        $this->lastAction = $method;

        // Reset stats of last request.
        $this->lastStats = array();

        // Validate input  values
        if (!is_scalar($method))  trigger_error('jsonRPC::__call requires a scalar value as first parameter!');
        if (is_array($params)) {
            $params = array_values($params);
        } else {
            trigger_error('jsonRPC::__call requires an array value as second parameter!');
        }

        // prepares the request
        $this->id ++;
        $request = json_encode(array('method' => $method,'params' => $params,'id' => $this->id));

        // Set curl options
        curl_setopt($this->curlHandler, CURLOPT_POSTFIELDS , $request);
        $response = curl_exec($this->curlHandler);        
        $response = json_decode($response,true);

        // Set current result stats.
        $this->lastStats = curl_getinfo($this->curlHandler);
        $this->lastResult = $response;

        return($response);
    }


    /*! \brief      Returns the HTTP status message for a given HTTP status code.        
     *  @param      int     code    The status to code to return a message for. 
     *  @return     string  The corresponding status message. 
     */
    public static function getHttpStatusCodeMessage($code)
    {
        $codes  = array(
                '100' =>  'Continue',
                '101' =>  'Switching Protocols',
                '102' =>  'Processing',
                '200' =>  'OK',
                '201' =>  'Created',
                '202' =>  'Accepted',
                '203' =>  'Non-Authoritative Information',
                '204' =>  'No Content',
                '205' =>  'Reset Content',
                '206' =>  'Partial Content',
                '207' =>  'Multi-Status',
                '300' =>  'Multiple Choice',
                '301' =>  'Moved Permanently',
                '302' =>  'Found',
                '303' =>  'See Other',
                '304' =>  'Not Modified',
                '305' =>  'Use Proxy',
                '306' =>  'reserved',
                '307' =>  'Temporary Redirect',
                '400' =>  'Bad Request',
                '401' =>  'Unauthorized',
                '402' =>  'Payment Required',
                '403' =>  'Forbidden',
                '404' =>  'Not Found',
                '405' =>  'Method Not Allowed',
                '406' =>  'Not Acceptable',
                '407' =>  'Proxy Authentication Required',
                '408' =>  'Request Time-out',
                '409' =>  'Conflict',
                '410' =>  'Gone',
                '411' =>  'Length Required',
                '412' =>  'Precondition Failed',
                '413' =>  'Request Entity Too Large',
                '414' =>  'Request-URI Too Long',
                '415' =>  'Unsupported Media Type',
                '416' =>  'Requested range not satisfiable',
                '417' =>  'Expectation Failed',
                '421' =>  'There are too many connections from your internet address',
                '422' =>  'Unprocessable Entity',
                '423' =>  'Locked',
                '424' =>  'Failed Dependency',
                '425' =>  'Unordered Collection',
                '426' =>  'Upgrade Required',
                '500' =>  'Internal Server Error',
                '501' =>  'Not Implemented',
                '502' =>  'Bad Gateway',
                '503' =>  'Service Unavailable',
                '504' =>  'Gateway Time-out',
                '505' =>  'HTTP Version not supported',
                '506' =>  'Variant Also Negotiates',
                '507' =>  'Insufficient Storage',
                '509' =>  'Bandwidth Limit Exceeded',
                '510' =>  'Not Extended');
        return((isset($codes[$code]))? $codes[$code] : sprintf(_("Unknown HTTP status code '%s'!"), $code));
    }
}
?>
