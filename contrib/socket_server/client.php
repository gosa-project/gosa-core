#!/usr/bin/php5 -q
<?php

require_once("../../include/class_socketClient.inc");
error_reporting(E_ALL);

$sock = new Socket_Client("10.89.1.155","9999",TRUE,1);
#$sock = new Socket_Client("169.254.2.248","9999",TRUE,1);
$sock->setEncryptionKey("ferdinand_frost");

if($sock->connected()){
	/* Prepare a hunge bunch of data to be send */
	$data = "<xml><header>ping</header><source>192.168.1.36:10001</source><target>192.168.1.36:10000</target></xml>";
	$sock->write($data);
  
  #$sock->setEncryptionKey("ferdinand_frost");

	$answer = $sock->read();	
  echo "$answer\n";
	$sock->close();	
}else{
	echo "... FAILED!\n";
}

?>
