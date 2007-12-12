#!/usr/bin/php5 -q
<?php

error_reporting(E_ALL);

//IP to bind to, use 0 for all 
$bind_ip = 0;

// Port to bind  
$bind_port = 10000;

// Max clients 
$max_clients = 3;

// Rijndal encrypt key 
$enable_encryption = TRUE;
$encrypt_key = "ferdinand_frostferdinand_frostfe";

/* Create Socket - 
 *  AF_INET means IPv4 Socket 
 *  SOCK_STREAM means Fullduplex TCP
 *  SOL_TCP - Protocol type TCP
 */
$socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);

/* Enable reuse of local address */
socket_set_option($socket,SOL_SOCKET,SO_REUSEADDR,1);

/* Bind to socket */
socket_bind($socket,$bind_ip,$bind_port);
socket_listen($socket,$max_clients);

$clients = array('0' => array('socket' => $socket));

echo "\nServer startet on port : $bind_port\n";


/* Open the cipher */
$td = mcrypt_module_open('rijndael-128', '', 'cbc', '');

/* Create the IV and determine the keysize length */
$iv = substr(md5('GONICUS GmbH'),0, mcrypt_enc_get_iv_size($td));
$ks = mcrypt_enc_get_key_size($td);
echo "IV-Size: ".mcrypt_enc_get_iv_size($td)."\n";

/* Create key */
$key = substr(md5('ferdinand_frost'), 0, $ks);
echo "Key: $key\n";

/* Intialize encryption */
mcrypt_generic_init($td, $key, $iv);

/* Accept connections till server is killed */
while(TRUE) {

	/* Create an array of sockets to read from */
	$read[0] = $socket;
	for($i=1;$i<count($clients)+1;$i++) {
		if(isset($clients[$i] ) && $clients[$i] != NULL) {
			$read[$i+1] = $clients[$i]['socket'];
		}
	}

	$ready = socket_select($read,$write=NULL,$except=NULL,0);

	/* Handle incoming connections / Incoming data */
	if(in_array($socket,$read)) {

		/* Check each client slot for a new connection */
		for($i=1;$i<$max_clients+1;$i++) {
		
			/* Accept new connections */
			if(!isset($clients[$i])) {
				$clients[$i]['socket'] = socket_accept($socket);
				socket_getpeername($clients[$i]['socket'],$ip);
				$clients[$i]['ipaddy'] = $ip;

				echo("New client connected: " . $clients[$i]['ipaddy'] . " \n");
				break;
			}
			elseif($i == $max_clients - 1) {
				echo("To many Clients connected! \n");
			}
			if($ready < 1) {
				continue;
			}
		}
	}

	/* Check if there is data to read from the client sockets */
	for($i=1;$i<$max_clients+1;$i++) {

		/* Check if socket has send data to the server */
		if(isset($clients[$i]) && in_array($clients[$i]['socket'],$read)) {

			/* Read socket data */
			$data = @socket_read($clients[$i]['socket'],1024000, PHP_NORMAL_READ);

			/* Client disconnected */
			if ($data === FALSE) {
				unset($clients[$i]);
				echo "Client disconnected!\n";
				continue;
			}

			$data = mdecrypt_generic($td, trim($data));
			echo "Client (".$clients[$i]['ipaddy'].") sent: ".$data."... \n";

			echo "Sending reply... \n";
			socket_write($clients[$i]['socket'],mcrypt_generic($td, $data));

			@socket_close($clients[$i]);
		}
	}
}

?> 
