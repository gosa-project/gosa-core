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


	function loadDescription($ppdFile)
	{
		$model= "";
		$manufacturer= "";
		
		$fh= fopen ($ppdFile, 'r');
		while (!feof($fh)) {

			/* Read line */
			$line= fgets($fh, 4096);
			if (strlen($line) >= 4095){
				trigger_error(_('Parsing PPD file %s faild - line too long. Trailing characters have been ignored'), E_USER_WARNING);
			}
			
			/* Extract interesting informations */
			if (preg_match('/^\*Manufacturer:/i', $line)){
				$manufacturer= preg_replace('/^\*Manufacturer:\s+"([^"]+)".*$/i', '\1', $line);
			}
			if (preg_match('/^\*ModelName:/i', $line)){
				$model= preg_replace('/^\*ModelName:\s+"([^"]+)".*$/i', '\1', $line);
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
