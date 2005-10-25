<?php

class ppdManager
{
	var $path= "";
	var $cachedList= array();

	function ppdManager($path)
	{
		$this->path= $path;
		echo "Path set to $path<br>";
	}


	function findPPD($path)
	{
		$list= array();
		$currentDir= getcwd();

		$dh = opendir($path);
		while(false !== ($file = readdir($dh))){

			/* Skip well known files */
			if( $file == '.' || $file == '..'){
				continue;
			}

			/* Recurse through all "common" directories */
			if(is_dir($path.'/'.$file)){
				$list= array_merge($list, $this->findPPD($path.'/'.$file));
				continue;
			}

			/* Check for PPD extension */
			if (preg_match('/\.ppd$/i', $file)){
				$list[]= $path.'/'.$file;
			}
		}

		closedir($dh);
		chdir ($currentDir);
		return ($list);
	}


	function saveProperties($ppdFile, $properties)
	{
		echo "<pre>";
		print_r ($properties);
		echo "</pre>";
	}
	
	
	function loadProperties($ppdFile)
	{
		$group= "";
		$option= "";
		$properties= array();
		
		$fh= fopen ($ppdFile, 'r');
		while (!feof($fh)) {

			/* Read line */
			$line= fgets($fh, 256);
			if (strlen($line) >= 256){
				trigger_error(_('Parsing PPD file %s failed - line too long. Trailing characters have been ignored'), E_USER_WARNING);
			}

			/* Trigger for option groups */
			if (preg_match('/^\*OpenGroup:/i', $line)){

				/* Sanity checks */
				if ($group != ""){
					trigger_error(_('Nested groups are not supported!'), E_USER_WARNING);
					continue;
				}
				if (in_array($group, $properties)){
					trigger_error(_('Group name not unique!'), E_USER_WARNING);
					continue;
				}
				
				// TODO: Symbol values are not supported yet!
				if (preg_match('/\^/', $line)){
					trigger_error(_('Symbol values are not supported yet!'), E_USER_WARNING);
				}
				$complete= preg_replace('@^\*OpenGroup:\s+(.*)$@i', '\1', $line);
				$complete= trim($complete, '"');
				if (preg_match('@/@', $complete)){
					$group= trim(preg_replace('@^\*OpenGroup:\s+"?([^/]+)/.*$@i', '\1', $line));
					$name = preg_replace('@^\*OpenGroup:\s+"?[^/]+/([^/]+).*$@i', '\1', $line);
				} else {
					$group= $complete;
					$name = $complete;
				}
				$properties[$group]= array('_name' => $name);
				continue;
			}
			if (preg_match("/^\*CloseGroup:\s+\"?$group\"?/i", $line)){
				$group= "";
				continue;
			}

			/* Trigger for options */
			if (preg_match('/^\*OpenUI\s+/i', $line)){

				/* Sanity check */
				if ($option != ""){
					trigger_error(_('Nested options are not supported!'), E_USER_WARNING);
					continue;
				}
				
				// TODO: Symbol values are not supported yet!
				if (preg_match('/\^/', $line)){
					trigger_error(_('Symbol values are not supported yet!'), E_USER_WARNING);
				}
				$complete= preg_replace('@^\*OpenUI\s+(.*)$@i', '\1', $line);
				$complete= trim($complete, '"');
				if (preg_match('@/@', $complete)){
					$option= trim(preg_replace('@^\*OpenUI\s+([^/]+)/.*$@i', '\1', $line));
					$name = trim(preg_replace('@^\*OpenUI\s+[^/]+/([^/]+).*$@i', '\1', $line));
				} else {
					$option= trim($complete);
					$name  = trim($complete);
				}

				/* Extract option type */
				$type= trim(preg_replace('/^[^:]+:\s+/', '', $line));
				$name= preg_replace('/:.*$/', '', $name);
				$option= preg_replace('/:.*$/', '', $option);
				
				// TODO: PickMany is not supported yet!
				if (preg_match('/PickMany/i', $type)){
					trigger_error(_('PickMany is not supported yet!'), E_USER_WARNING);
				}
				
				$properties[$group][$option]= array('_name' => $name, '_type' => $type);
				continue;
			}
			/* Show interest for option parsing */
			if ($option != ""){

				$eoption= preg_replace('@\*@', '', $option);

				/* Close section? */
				if (preg_match("@^\*CloseUI:\s+\*$eoption@i", $line)){
					$option= "";
					continue;
				}

				/* Default value? */
				if (preg_match("@^\*Default$eoption:@", $line)){
					$c= preg_replace("@^\*Default$eoption:\s+@", "", $line);
					$properties[$group][$option]['_default']= trim(trim($c, '"'));
					continue;
				}

				/* Possible value? */
				if (preg_match("@^\*$eoption\s+@", $line)){
				#*PageSize Letter/US Letter: "<>setpagedevice"
					$c= preg_replace("@^\*$eoption\s+([^/]+).*$@", "$1", $line);
					$d= preg_replace("@^\*$eoption\s+[^/]+/([^:]+).*$@", "$1", $line);
					$properties[$group][$option][trim($c)]= trim($d);
					continue;
				}
			}

		}
		fclose ($fh);

		return ($properties);
	}
	
	function loadDescription($ppdFile)
	{
		$model= "";
		$manufacturer= "";
		
		$fh= fopen ($ppdFile, 'r');
		while (!feof($fh)) {

			/* Read line */
			$line= fgets($fh, 256);
			if (strlen($line) >= 256){
				trigger_error(_('Parsing PPD file %s failed - line too long. Trailing characters have been ignored'), E_USER_WARNING);
			}
			
			/* Extract interesting informations */
			if (preg_match('/^\*Manufacturer:/i', $line)){
				$manufacturer= preg_replace('/^\*Manufacturer:\s+"?([^"]+)"?.*$/i', '\1', $line);
			}
			if (preg_match('/^\*ModelName:/i', $line)){
				$model= preg_replace('/^\*ModelName:\s+"?([^"]+)"?.*$/i', '\1', $line);
			}

			/* Got everything we need? Skip rest for speed reasons... */
			if ($model != '' && $manufacturer != ''){
				break;
			}
		}
		fclose ($fh);

		/* Write out a notice that the PPD file seems to be broken if we can't
		   extract any usefull informations */
		if ($model == '' || $manufacturer == ''){
			trigger_error(sprintf(_('Parsing PPD file %s failed - no information found.'), $ppdFile), E_USER_WARNING);
		}

		return ($manufacturer.' - '.$model);
	}


	function getPrinterList($reload= false)
	{
		/* Load list of PPD files */
		if (count($this->cachedList) == 0 || $reload){
			$list= $this->findPPD($this->path);

			/* Load descriptive informations to build final printer list */
			$this->cachedList= array();
			foreach ($list as $ppdFile){
				$this->cachedList[$ppdFile]= $this->loadDescription($ppdFile);
			}

		}

		return ($this->cachedList);
	}
	
}

?>
