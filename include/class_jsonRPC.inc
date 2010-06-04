<?php
class jsonRPC {

    private $curlHandler = NULL;
    private $config;
    private $id;
    private $lastStats = array();
    private $lastAction = "none";

    public function __construct($config) 
    {
        $this->config = $config;
        $this->id = 0;
        $this->__login();
    }

    private function __login()
    {
        // Get connection data
        $url    =  $this->config->get_cfg_value('core','gosaRpcServer');
        $user   =  $this->config->get_cfg_value('core','gosaRpcUser');
        $passwd =  $this->config->get_cfg_value('core','gosaRpcPassword');

        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($url), "Initiated RPC "); 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($user), "RPC user: "); 
        DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($passwd),"RPC password: "); 

        // Init Curl handler
        $this->curlHandler = curl_init($url);

        // Set curl options
        curl_setopt($this->curlHandler, CURLOPT_URL , $url);
        curl_setopt($this->curlHandler, CURLOPT_COOKIESESSION , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_COOKIEFILE, 'cookiefile.txt'); 
        curl_setopt($this->curlHandler, CURLOPT_POST , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_RETURNTRANSFER , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_HTTPHEADER , array('Content-Type: application/json'));

        // Try to login 
        $this->login($user, $passwd);
       
    }
        

    public function getHTTPstatusCode()
    {
        return((isset($this->lastStats['http_code']))? $this->lastStats['http_code'] : -1 );
    }

    public function get_error()
    {
        if($this->lastStats['http_code'] != 200){
            return($this->getHttpStatusCodeMessage($this->lastStats['http_code']));
        }else{
            return(curl_error($this->curlHandler));
        }
    }

    public function success()
    {
        return(curl_errno($this->curlHandler) == 0 && $this->lastStats['http_code'] == 200);
    }

    public function __destruct()
    {
        if($this->curlHandler){
             curl_close($this->curlHandler);
        }
    }

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
            DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($response['result']), "Result: "); 
        }else{
            DEBUG (DEBUG_RPC, __LINE__, __FUNCTION__, __FILE__,bold($this->get_error())."<br>".$response, "Result (FAILED): "); 
        }

        return($response['result']);
    }

    
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
    
        return($response);
    }
    

    public static function getHttpStatusCodeMessage($code)
    {
        $codes  = array(
                '100' =>_('Continue'),
                '101' =>_('Switching Protocols'),
                '102' =>_('Processing'),
                '200' =>_('OK'),
                '201' =>_('Created'),
                '202' =>_('Accepted'),
                '203' =>_('Non-Authoritative Information'),
                '204' =>_('No Content'),
                '205' =>_('Reset Content'),
                '206' =>_('Partial Content'),
                '207' =>_('Multi-Status'),
                '300' =>_('Multiple Choice'),
                '301' =>_('Moved Permanently'),
                '302' =>_('Found'),
                '303' =>_('See Other'),
                '304' =>_('Not Modified'),
                '305' =>_('Use Proxy'),
                '306' =>_('(reserviert)'),
                '307' =>_('Temporary Redirect'),
                '400' =>_('Bad Request'),
                '401' =>_('Unauthorized'),
                '402' =>_('Payment Required'),
                '403' =>_('Forbidden'),
                '404' =>_('Not Found'),
                '405' =>_('Method Not Allowed'),
                '406' =>_('Not Acceptable'),
                '407' =>_('Proxy Authentication Required'),
                '408' =>_('Request Time-out'),
                '409' =>_('Conflict'),
                '410' =>_('Gone'),
                '411' =>_('Length Required'),
                '412' =>_('Precondition Failed'),
                '413' =>_('Request Entity Too Large'),
                '414' =>_('Request-URI Too Long'),
                '415' =>_('Unsupported Media Type'),
                '416' =>_('Requested range not satisfiable'),
                '417' =>_('Expectation Failed'),
                '421' =>_('There are too many connections from your internet address'),
                '422' =>_('Unprocessable Entity'),
                '423' =>_('Locked'),
                '424' =>_('Failed Dependency'),
                '425' =>_('Unordered Collection'),
                '426' =>_('Upgrade Required'),
                '500' =>_('Internal Server Error'),
                '501' =>_('Not Implemented'),
                '502' =>_('Bad Gateway'),
                '503' =>_('Service Unavailable'),
                '504' =>_('Gateway Time-out'),
                '505' =>_('HTTP Version not supported'),
                '506' =>_('Variant Also Negotiates'),
                '507' =>_('Insufficient Storage'),
                '509' =>_('Bandwidth Limit Exceeded'),
                '510' =>_('Not Extended'));
        return((isset($codes[$code]))? $codes[$code] : sprintf(_("Unknown HTTP status code '%s'!"), $code));
    }
}
?>
