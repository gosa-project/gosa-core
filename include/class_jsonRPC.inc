<?php
class jsonRPC {

    private $curlHandler = NULL;
    private $debug;
    private $url;
    private $id;

    public function __construct($url,$debug = false) 
    {

        $this->url = $url;
        $this->debug = $debug;
        $this->id = 0;

        // Init Curl handler
        $this->curlHandler = curl_init($this->url);
        curl_setopt($this->curlHandler, CURLOPT_URL , $this->url);
        curl_setopt($this->curlHandler, CURLOPT_COOKIESESSION , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_COOKIEFILE, 'cookiefile.txt'); 
        curl_setopt($this->curlHandler, CURLOPT_POST , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_RETURNTRANSFER , TRUE);
        curl_setopt($this->curlHandler, CURLOPT_HTTPHEADER , array('Content-Type: application/json'));
    }

    public function get_error()
    {
        return(curl_error($this->curlHandler));
    }

    public function success()
    {
        return(curl_errno($this->curlHandler) == 0);
    }

    public function __destruct()
    {
        curl_close($this->curlHandler);
    }

    public function __call($method,$params) 
    {
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

        // Check responce id
        if ($response['id'] != $this->id){
            #trigger_error('jsonPRC returned incorrect response '.
            #        'id (request id: '.$this->id.', response id: '.$response['id'].')');
        }
        return($response['result']);
    }
}
?>
