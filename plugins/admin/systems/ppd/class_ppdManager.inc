<?php

class ppdManager
{
    var $path= "";
    var $cachedList= array();
    var $timestamps = array();

    var $useGzip  = true;

    function __construct($path)
    {
        global $config;
        if($config->boolValueIsTrue("core","ppdGzip")){
            $this->useGzip  = false;
        }

        if(is_dir($path)){
            $this->path= $path;
        }elseif(is_link($path) && (is_dir(readlink($path)))) {
            $this->path= $path;
        } else {
            msg_dialog::display(_("PPD manager error"), sprintf(_("The specified path '%s' does not exist."),$path), ERROR_DIALOG);
            return (false);
        }
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
            $check_path= $path.'/'.$file;
            if(is_dir($check_path) || (is_link($check_path) && (is_dir(readlink($check_path))))){
                $list= array_merge($list, $this->findPPD($check_path));
                continue;
            }

            /* Check for PPD extension */
            if (preg_match('/\.ppd(.gz)?$/i', $file)){
                $list[]= $path.'/'.$file;
            }
        }

        closedir($dh);
        chdir ($currentDir);
        return ($list);
    }


    function updateAttribute($file, $section, $attribute, $value)
    {
        $fsection= false;
        $fattribute= false;
        $section= preg_replace('/^\*/', '', $section);
        $attribute= preg_replace('/^\*/', '', $attribute);

        if($this->useGzip){
            $rp= @gzopen($file, "r");
            $wp= @gzopen("$file.tmp", "w");
        }else{
            $rp= @gzopen($file, "r");
            $wp= @fopen("$file.tmp", "w");
        }

        if($this->useGzip){	
            while (!gzeof($rp)){
                $lines[]= gzgets($rp, 1024);
            }
        }else{
            while (!feof($rp)){
                $lines[]= fgets($rp, 1024);
            }
        }

        $ret = "";
        $done =false;
        foreach($lines as $nr => $line){

            if (preg_match("/\*OpenGroup:*\s+\**$section\/*/", $line)){
                $fsection= true;
                $ret .=$line; 
                continue;
            }

            /* Change model name .. */
            if ((preg_match("/^\*".$attribute.":*\s+/",$line)) && ($attribute == "ModelName")){
                $line= "*$attribute: \"$value\"\n";
                $done =true;
            }

            if (($fsection) && ($section != "NO_SECTION")){
                if (preg_match("/^\*CloseGroup:*\s+\**$section\/*/", $line)){
                    $fsection= false;
                    $ret .=$line; 
                    continue;
                }


                if (preg_match("/^\*OpenUI:*\s+\**$attribute\/*/", $line)){
                    $fattribute= true;
                    $ret .= $line; 
                    continue;
                }

                if ($fattribute){
                    if (preg_match("/^\*CloseUI:*\s+\**$attribute\/*/", $line)){
                        $fattribute= false;
                        $ret .= $line; 
                        continue;
                    }

                    if (preg_match("/^\*Default$attribute:*\s+/", $line)){
                        $line= "*Default$attribute: $value\n";
                        $done =true;
                    }
                }
            }else{
                if (preg_match("/^\*OpenUI:*\s+\**$attribute\/*/", $line)){
                    $fattribute= true;
                    $ret .= $line; 
                    continue;
                }

                if ($fattribute){
                    if (preg_match("/^\*CloseUI:*\s+\**$attribute\/*/", $line)){
                        $fattribute= false;
                        $ret .= $line; 
                        continue;
                    }

                    if (preg_match("/^\*Default$attribute:*\s+/", $line)){
                        $line= "*Default$attribute: $value\n";
                        $done =true;
                    }
                }
            }
            $ret .=$line; 
        }

        if($this->useGzip){
            gzwrite($wp,$ret);
            gzclose($wp);
            gzclose($rp);
        }else{
            fwrite($wp,$ret);
            fclose($wp);
            gzclose($rp);
        }

        copy("$file.tmp", "$file");
        unlink("$file.tmp");
    }


    function saveProperties($ppdFile, $properties)
    {
        if(!is_readable($ppdFile)){
            msg_dialog::display(_("PPD manager error"), sprintf(_("Specified PPD file '%s' cannot be opened for reading."),$ppdFile), ERROR_DIALOG);
        }elseif(!is_writeable(preg_replace("#(^.*/).*$#","\\1",$ppdFile.".tmp"))){
            msg_dialog::display(_("PPD manager error"), sprintf(_("The temporary file '%s' cannot be opened for writing."),$ppdFile.".tmp"), ERROR_DIALOG);
        }else{
            if(is_array($properties)){
                foreach ($properties as $name => $section){
                    foreach ($section as $attribute => $value){
                        if (is_array($value)){
                            $this->updateAttribute($ppdFile, $name, $attribute, $value['_default']);
                        }
                    }
                }
            }
        }
    }

    function loadProperties($ppdFile)
    {
        $group= "";
        $option= "";
        $properties= array();

        // Check for empty files
        if(!filesize($ppdFile)) {
            trigger_error(_('Parsing PPD file failed - file is empty!'));
            return;
        }

        $fh= gzopen ($ppdFile, 'r');
        while (!gzeof($fh) && $fh){

            /* Read line */
            $line= gzgets($fh, 256);
            if (strlen($line) >= 256){
                trigger_error(_('Parsing PPD file %s failed - line too long. Trailing characters have been ignored!'), E_USER_WARNING);
            }

            /* Trigger for option groups */
            if (preg_match('/^\*OpenGroup:/i', $line)){

                /* Sanity checks */
                if ($group != ""){
                    trigger_error(_('Nested groups are not supported!'), E_USER_WARNING);
                    continue;
                }
                if (in_array_strict($group, $properties)){
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
                if(empty($group)){
                    $properties["NO_SECTION"][$option]= array('_name' => $name, '_type' => $type);
                }else{
                    $properties[$group][$option]= array('_name' => $name, '_type' => $type);
                }
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
                    if(empty($group)){
                        $properties["NO_SECTION"][$option]['_default']= trim(trim($c, '"'));
                    }else{
                        $properties[$group][$option]['_default']= trim(trim($c, '"'));
                    }
                    continue;
                }

                /* Possible value? */
                if (preg_match("@^\*$eoption\s+@", $line)){

                    // Detect what comes first, the  '/' or the ':'. 
                    // We may have entries like this: 
                    //  *Resolution 300dpi: "<</HWResolution [300 300] >> setpagedevice"
                    // and
                    //  *Resolution 300dpi/300: "<</HWResolution [300 300] >> setpagedevice"
                    $name = $value = preg_replace("@^\*$eoption\s+([^:]+).*$@", "$1", $line);
                    if(preg_match("/\//", $name)){
                        list($name, $value) = preg_split("/\//", $name);
                    }
                    if(empty($group)){
                        $properties["NO_SECTION"][$option][$name]= $value;
                    }else{
                        $properties[$group][$option][$name]= $value;
                    }
                    continue;
                }
            }
        }

        gzclose ($fh);
        return ($properties);
    }

    function loadDescription($ppdFile)
    {
        // Check for empty files
        if(!filesize($ppdFile)) {
            trigger_error(_('Parsing PPD file failed - file is empty!'));
            return;
        }

        $ppdDesc = array();

        /* Only parse complete PPD file again, if it was changed */
        $modified = filemtime($ppdFile);
        if(isset($this->cachedList[$ppdFile]) && isset($this->timestamps[$ppdFile]) && $modified == $this->timestamps[$ppdFile]){
            return($this->cachedList[$ppdFile]);
        }

        /* Remember modified timestamp, to speed up next request */
        $this->timestamps[$ppdFile] = filemtime($ppdFile);
        $fh= gzopen ($ppdFile, 'r');

        while (!gzeof($fh) && $fh){

            $line= gzgets($fh, 256);
            if (strlen($line) >= 256){
                trigger_error(_('Parsing PPD file %s failed - line too long. Trailing characters have been ignored'), E_USER_WARNING);
            }

            /* Extract interesting informations */
            if (preg_match('/^\*Manufacturer:/i', $line)){
                $ppdDesc['manufacturer'] = trim(preg_replace('/^\*Manufacturer:\s+"?([^"]+)"?.*$/i', '\1',    $line));
            }
            if (preg_match('/^\*ModelName:/i', $line)){
                $ppdDesc['model'] = trim(preg_replace('/^\*ModelName:\s+"?([^"]+)"?.*$/i', '\1', $line));
            }
            if (preg_match('/^\*ShortNickName:/i', $line)) { 
                $ppdDesc['nickname'] = trim(preg_replace('/^\*ShortNickName:\s+"?([^"]+)"?.*$/i', '\1', $line)); 
            } 

            /* Got everything we need? Skip rest for speed reasons... */
            if (isset($ppdDesc['manufacturer'])  && isset($ppdDesc['model']) && isset($ppdDesc['nickname'])){
                break;
            }
        }

        gzclose ($fh);

        /* If model contains manufacturer strip it */
        $ppdDesc['model'] = str_replace($ppdDesc['manufacturer']." ", '', $ppdDesc['model']);

        /* Write out a notice that the PPD file seems to be broken if we can't
           extract any usefull informations */
        if ($ppdDesc['manufacturer'] == '' || $ppdDesc['model'] == ''){
            trigger_error(sprintf(_('Parsing PPD file %s failed - no information found.'), $ppdFile), E_USER_WARNING);
        }

        $ppdDesc['name'] = $ppdDesc['manufacturer'] . " - " . $ppdDesc['model'];
        return $ppdDesc;
    }


    function getPrinterList($reload= false)
    {
        /* Load list of PPD files */
        if (count($this->cachedList) == 0 || $reload){
            $list= $this->findPPD($this->path);

            /* Load descriptive informations to build final printer list */
            $new = array();
            foreach ($list as $ppdFile){
                $new[$ppdFile] = $this->loadDescription($ppdFile); 
            }
            $this->cachedList= $new ;

        }

        return ($this->cachedList);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
