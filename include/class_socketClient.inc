<?php

class Socket_Client
{
	private $host 	= "";
	private $port 	= "";
	private $timeout= "";
	private $errno	= "";
	private $errstr	= "";
	private $b_data_send = FALSE;
	private $handle	= NULL;
	private $bytes_read = 0;
	private $error = "";

	/* Crypto information */
	private $td= NULL;
	private $ckey= "";
	private $ks;
	private $iv;


	public function __construct($host, $port, $connect = TRUE, $timeout = 3){
		$this->host= $host;
		$this->port= $port;
		$this->timeout= $timeout;

		/* Connect if needed */
		if($connect){
			$this->open();
		}
	}


	public function setEncryptionKey($key)
	{
		if(!function_exists("mcrypt_get_iv_size")){
			$this->error = _("The mcrypt module was not found. Please install php5-mcrypt.");
			$this->ckey = "";
			return FALSE ;
		}

		if ($this->connected()){
			$this->ckey = substr(md5($key), 0, $this->ks);
			return TRUE;
		}

		return FALSE;
	}


	private function encrypt($data)
	{
		mcrypt_generic_init($this->td, $this->ckey, $this->iv);
		return base64_encode(mcrypt_generic($this->td, $data));
	}


	private function decrypt($data)
	{
		/* decrypt data */
		$data = base64_decode($data);
		mcrypt_generic_init($this->td, $this->ckey, $this->iv);
		return mdecrypt_generic($this->td, $data);
	}


	public function connected()
	{
		return ($this->handle == TRUE);
	}


	public function open()
	{
		$this->handle = @fsockopen($this->host, $this->port, $this->errno, $this->errstr, $this->timeout);
		if(!$this->handle){
			$this->handle = NULL;
			$this->error = $this->errstr;
		}else{
			$this->b_data_send = TRUE;

			/* Open the cipher */
			$this->td = mcrypt_module_open('rijndael-128', '', 'cbc', '');

			/* Create the IV and determine the keysize length */
			$this->iv = substr(md5('GONICUS GmbH'),0, mcrypt_enc_get_iv_size($this->td));
			$this->ks = mcrypt_enc_get_key_size($this->td);
		}
	}


	public function get_error()
	{
		return $this->error;
	}


	public function write($data){
		if($this->handle){
			$data = $this->encrypt($data);
			fputs($this->handle, $data."\n");
			$this->b_data_send = TRUE;
		}else{
			$this->b_data_send = FALSE;
		}

		return $this->b_data_send;
	}


	public function read()
	{
		// Output the request results
		$str = FALSE;
		$data = "test";
		socket_set_timeout($this->handle,$this->timeout);			
		stream_set_blocking($this->handle,1);

		/* Read while last character is Newline, or we exceeded the timelimit 
         */
		$start = microtime();
		while(!preg_match("/\\\n$/",$str) && get_MicroTimeDiff($start,microtime()) < $this->timeout) {
			usleep(10000);
			$data = fread($this->handle, 1024000);
			$str .= $data;
		}
		if(get_MicroTimeDiff($start,microtime()) >= $this->timeout){
			trigger_error(sprintf("Exceeded timeout %f while reading from socket. Time spend for reading was %f.",$this->timeout,get_MicroTimeDiff($start,microtime())));
		}
		$this->bytes_read = strlen($str);
		$this->b_data_send = FALSE;
		$str = $this->decrypt($str);
		return($str);	


#	$str = FALSE;
#	if($this->handle){
#
#		/* Check if there is something to read for us */
#		$read = array("0"=>$this->handle);
#		$write = array();
#		$accept = array();	
#		$start = microtime();		
#		$num = @stream_select($read,$write,$accept,floor($this->timeout), ceil($this->timeout*100000));
#		$str = "";		
#		socket_set_timeout($this->handle,$this->timeout);			
#
#		/* Read data if necessary */
#		while($num && get_MicroTimeDiff($start,microtime()) < $this->timeout){
#			$str.= fread($this->handle, 1024000);
#			$read = array("0"=>$this->handle);	
#			$num = stream_select($read,$write,$accept,0,200000);
#		}
#		$this->bytes_read = strlen($str);
#		$this->b_data_send = FALSE;
#		$str = $this->decrypt($str);
#	}
#	return $str;
	}


	public function bytes_read()
	{
		return $this->bytes_read;
	}


	public function close()
	{
		if($this->handle){
			fclose($this->handle);
		}

		/* Terminate decryption handle and close module */
		@mcrypt_generic_deinit($this->td);
	}
}
?>
