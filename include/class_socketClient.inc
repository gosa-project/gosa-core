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

	
	private function _is_timeout($start,$stop = 0)
	{
		if($stop == 0){
			$stop = microtime();
		}
		$a = split("\ ",$start);
		$b = split("\ ",$stop);
		$secs = $b[1] - $a[1];
		$msecs= $b[0] - $a[0];
		$ret = (float) ($secs+ $msecs);
		return($ret >= $this->timeout);
	}


	public function read()
	{
		// Output the request results
		$str = "";
		$data = "test";
		socket_set_timeout($this->handle,$this->timeout);			
		stream_set_blocking($this->handle,0);
		$start = microtime();

		while(strlen($str) == 0 || !$this->_is_timeout($start)) {
			usleep(10000);
			$data = fread($this->handle, 1024000);
			if($data && strlen($data)>0) {
				$str .= $data;
			} else {
				break;
			}
		}
		if($this->_is_timeout($start)){
			trigger_error(sprintf("Exceeded timeout %f while reading from socket",$this->timeout));
		}
		$this->bytes_read = strlen($str);
		$this->b_data_send = FALSE;
		$str = $this->decrypt($str);
		return($str);	
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
