<?php

class stats 
{

    static protected $lastCpuLoad = "";
    static protected $lastCpuLoadTimestamp = 0;

    static protected $tableName = "stats";

    static protected $lastHandle = NULL;
    static protected $statsEnabled = FALSE;


    /*! \brief     This method tries to connect the GOsa-stats database and 
     *              then returns a database handle on success else NULL.
     *
     *             (The GOsa-stats database has to be enabled : statsDatabaseEnabled/statsDatabaseDirectory)
     *
     *             This database will then contain information about the use of GOsa,
     *              no customer data will be stored.
     *
     *  @return     handle      Returns a sqlite database handle.
     */
    static function getDatabaseHandle($filename = '')
    {
        // We cannot log while the path to store logs in is empty.
        global $config;

        // Check if Logging is enabled
        if(!is_object($config) || ! $config instanceOf config){
            return(NULL);
        }

        $path = $config->get_cfg_value('core', 'statsDatabaseDirectory');
        if(empty($path)){
            return(NULL);
        }

        // Check if path exists, if not try to create it
        if(!is_dir($path) ){
            $res = @mkdir($path);
            if(!$res){
                return(NULL);
            }
        }

        // Append a date suffix to the database file, to prevent huge and unusable database files.
        if($filename == ''){
            $filename = date('Y-m-d');
        }
        $tableFile = $path.'/'.$filename;

        // Try to return last valid handle.
        if(file_exists($tableFile) && 
                isset(stats::$lastHandle[$filename]) && 
                stats::$lastHandle[$filename] != NULL &&
                is_resource(stats::$lastHandle[$filename])){
            return(stats::$lastHandle[$filename]);
        }

        // Get statsFile property 
        stats::$statsEnabled = $config->boolValueIsTrue('core', 'statsDatabaseEnabled');
        if(!stats::$statsEnabled){
            return(NULL);
        }

        // Check for SQLite extension
        if(!stats::checkSQLiteExtension()){
            return(NULL);
        }

        // Check if we are able to read/write the given database file.
        if(!is_writeable($path) && !is_writeable(dirname($tableFile))){
            return(NULL);
        }

        // Try to create database, if it exists just open it.
        $handle = sqlite_open($tableFile, 0666, $error);
        if($handle){
            stats::createDatabaseOnDemand($handle);
        }
        stats::$lastHandle[$filename] = $handle;
        return($handle);
    }


    /*! \brief      Returns a list of all created stat files
     *  @return     Array   A list of all currently stored stat files.
     */
    static function getLocalStatFiles()
    {
        $res = array();

        // Check if we're creating logs right now.
        if(stats::getDatabaseHandle()){
            global $config;

            // Walk through all files found in the storage path
            $path = $config->get_cfg_value('core', 'statsDatabaseDirectory');
            $dir = opendir($path);
            while($file = readdir($dir)){
                if(is_file($path.'/'.$file) && !preg_match('/.old$/', $file)) {
                    $res[] = $file;
                }
            }
        } 
        return($res);   
    }


    /*! \brief      Check whether the qlite extension is available or not.
     *  @return     boolean     TRUE on success else FALSE
     */  
    static function checkSQLiteExtension()
    {
        return(function_exists('sqlite_popen'));
    }


    /*! \brief      Drops the current stats table and thus enforces a recreation.
     *  @param      handle      The database handle to use.
     */  
    static function dropTable($handle)
    {
        $TABLE_NAME = stats::$tableName;
        $query = "DROP TABLE '{$TABLE_NAME}'";
        $ret = sqlite_query($query, $handle);
        stats::$lastHandle = NULL;
        stats::getDatabaseHandle();
    }


    /*! \brief      Returns the currently used amount of memory form the PHP process.
     *  @return     int     The amount of bytes used for the PHP process.
     */  
    static function get_memory_usage()
    {
        return(memory_get_usage());
    }


    /*! \brief      Returns the current CPU load. 
     *              The result will be cached and one updated every 5 seconds.
     *  @return     float       The current 'cpu_load'.
     */  
    static function get_cpu_load()
    {
        $cur = time();
        if(empty(stats::$lastCpuLoad) || (($cur - stats::$lastCpuLoadTimestamp) >= 5 )){
            list($one, $five, $ten) =preg_split("/ /",shell_exec('cat /proc/loadavg'));
            stats::$lastCpuLoad = $one;
            stats::$lastCpuLoadTimestamp = $cur;
        }
        return(stats::$lastCpuLoad);
    }


    /*! \brief      This method checks if the 'stats' table is already present,
     *               if it is not then it will be created.
     *  @param      handle      The sqlite database handle
     */  
    static function createDatabaseOnDemand($handle)
    {
        $TABLE_NAME = stats::$tableName;

        // List Tables an check if there is already everything we need,
        //  if not create it.
        $query = "SELECT name FROM sqlite_master WHERE type='table' and name='{$TABLE_NAME}'";
        $ret = sqlite_query($query, $handle);
        if(!count(sqlite_fetch_all($ret))){
            $query = "
                CREATE TABLE {$TABLE_NAME} (
                        ID              INTEGER PRIMARY KEY,
                        ACTID           INTEGER,
                        TYPE            TEXT,
                        PLUGIN          TEXT,
                        CATEGORY        TEXT,
                        ACTION          TEXT,
                        UUID            TEXT,
                        TIMESTAMP       INTEGER,
                        MTIMESTAMP      REAL,
                        DURATION        REAL,
                        RENDER_TIME     REAL,
                        AMOUNT          INTEGER,
                        MEMORY_USAGE    INTEGER,
                        CPU_LOAD        FLOAT,
                        INFO            BLOB
                        )";
            $ret = sqlite_query($query, $handle);
        }
    }


    /*! \brief      Creates a new 'stats' table entry.
     *              -> Logs a GOsa action/activity in the sqlite stats table.
     *  @param      string  type        The action type, e.g. ldap/plugin/management
     *  @param      string  plugin      The plugin name, e.g. userManagement/user/posixAccount
     *  @param      string  category    The plugin category e.g. users/servers/groups
     *  @param      string  action      The action done e.g. edit/view/open/move
     *  @param      int     amount      The amount, e.g. for multiple edit
     *  @param      float   duration    The elapsed time.
     *  @param      string  info        Some infos form the action, e.g. the used hashing mehtod for pwd changes.
     */  
    static function log($type, $plugin, $category, $action, $amount = 1, $duration = 0, $info ='')
    {
        global $config;
        global $clicks;
        global $overallRenderTimer;

        // Get database handle, if it is invalid (NULL) return without creating stats
        $res = stats::getDatabaseHandle();
        if(!$res) return;

        // Ensure that 'clicks' and 'overallRenderTimer' are present and set correctly, 
        //  if not simply create them with dummy values... 
        //   -- 'clicks' is a counter wich is set in main.php -> Number of page reloads 
        //   -- 'overallRenderTimer' is set in main.php -> timestamp of rendering start.
        if(!isset($clicks) || empty($clicks)) $clicks = 0;
        if(!isset($overallRenderTimer)){
            $renderTime = 0;
        }else{
            $renderTime = microtime(TRUE) - $overallRenderTimer;

            // Now set the overallRenderTimer to the current timestamp - else 
            //  we will not be able to sum up the render time in a single SQL statement.
            $overallRenderTimer = microtime(TRUE);
        }

        // Prepare values to be useable within a database
        $uuid = $config->getGOsaUUID();
        $type           = sqlite_escape_string($type);
        $plugin         = sqlite_escape_string($plugin);
        $action         = sqlite_escape_string($action);
        $timestamp      = time();
        $mtimestamp     = number_format(microtime(TRUE), 4,'.','');
        $amount         = sqlite_escape_string($amount);
        $duration       = sqlite_escape_string(number_format($duration, 4,'.',''));
        $renderTime     = sqlite_escape_string(number_format($renderTime, 4,'.',''));
        $info           = sqlite_escape_string($info);
        $clicks         = sqlite_escape_string($clicks);
        $memory_usage   = sqlite_escape_string(stats::get_memory_usage());
        $cpu_load       = sqlite_escape_string(number_format(stats::get_cpu_load(),4,'.',''));

        // Clean up category, which usally comes from acl_category and may still contain 
        //  some special chars like /
        $tmp = array();
        foreach($category as $cat){
            $tmp[] = trim($cat, '\/,; ');
        }
        $category = sqlite_escape_string(implode($tmp, ', '));

        // Create insert statement.
        $TABLE_NAME = stats::$tableName;
        $query = "
            INSERT INTO {$TABLE_NAME}
        (ACTID, TYPE, PLUGIN, CATEGORY, ACTION, UUID, MTIMESTAMP, TIMESTAMP, 
         AMOUNT, DURATION, RENDER_TIME, MEMORY_USAGE, CPU_LOAD, INFO) 
            VALUES 
            ('{$clicks}','{$type}','{$plugin}','{$category}','{$action}','{$uuid}',
             '{$mtimestamp}','{$timestamp}','{$amount}','{$duration}','{$renderTime}',
             '{$memory_usage}','{$cpu_load}','{$info}')";
        sqlite_query($query, $res);
    }


    /*! \brief      Closes all sqlite handles opened by this class
     */ 
    static function closeHandles()
    {
        foreach(stats::lastHandle as $handle){
            if($handle && is_resource($handle)){
                sqlite_close($handle);
            }
        }
    }


    /*! \brief      This method returns all entries of the GOsa-stats table.
     *              You can limit the result by setting the from/to parameter (timestamp).
     *  @param      int     from    The timestamp to start the result from. 
     *  @param      int     to      The timestamp to end the request.
     *  @return     array           An array containing the requested entries.
     */  
    static function generateStatisticDump($filename)
    {
        // Get database connection
        $TABLE_NAME = stats::$tableName;
        $handle = stats::getDatabaseHandle($filename);
        if(!$handle) return;        

        $query = 
            "  SELECT ".
            "      TYPE, PLUGIN, CATEGORY, ACTION, ".
            "      UUID, DATE(TIMESTAMP, 'unixepoch') as date, ".
            "      AVG(DURATION), AVG(RENDER_TIME), SUM(AMOUNT), ".
            "      AVG(MEMORY_USAGE), AVG(CPU_LOAD), INFO ".
            "  FROM ".
            "      stats ".
            "  GROUP BY ".
            "      TYPE, PLUGIN, CATEGORY, ACTION, UUID, date, INFO ".
            "  ORDER BY ".
            "      ID ";

        // Create Filter and start query
        $ret = sqlite_array_query($query, $handle, SQLITE_ASSOC);
        return($ret);
    }


    static function removeStatsFile($filename)
    {
        // Get statsFile property 
        global $config;
        $path = $config->get_cfg_value('core', 'statsDatabaseDirectory');
        stats::$statsEnabled = $config->boolValueIsTrue('core', 'statsDatabaseEnabled');
        if(!stats::$statsEnabled){
            return(NULL);
        }

        // We cannot log while the path to store logs in is empty.
        if(empty($path)){
            return(NULL);
        }

        // Check if file exists and then remove it
        if(isset(stats::$lastHandle[$filename]) && is_resource(stats::$lastHandle[$filename])){
            sqlite_close(stats::$lastHandle[$filename]);
        }
        if(file_exists($path.'/'.$filename)){
            #unlink($path.'/'.$filename);
            rename($path.'/'.$filename,  $path.'/'.$filename.".old");
        }
    }

    
    static function getLdapObjectCount($config, $statisticConformResult = FALSE)    
    {
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);
        
        // A list of objectClasses to search for, indexed by their 
        //  object-category
        $ocsToSearchFor = array(
                "department" => array("gosaDepartment"),
                "devices" => array("gotoDevice"),
                "fai" => array("FAIobject"),
                "gofaxlist" => array("goFaxRBlock","goFaxSBlock"),
                "gofonconference" => array("goFonConference"),
                "phone" => array("goFonHardware"),
                "gofonmacro" => array("goFonMacro"),
                "users" => array("gosaAccount"),
                "acls" => array("gosaAcl","gosaRole"),
                "application" => array("gosaApplication"),
                "ogroups" => array("gosaGroupOfNames"),
                "roles" => array("organizationalRole"),
                "server" => array("goServer"),
                "printer" => array("gotoPrinter"),
                "terminal" => array("gotoTerminal"),
                "workstation" => array("gotoWorkstation"),
                "winworkstation" => array("sambaSamAccount"),
                "incoming" => array("goHard"),
                "component" => array("ieee802Device"),
                "mimetypes" => array("gotoMimeType"),
                "groups" => array("posixGroup"),
                "sudo" => array("sudoRole"));

        // Build up a filter which contains all objectClass combined by OR.
        // We will later sum up the results using PHP.
        $filter = "";
        $categoryCounter = array();
        foreach($ocsToSearchFor as $category => $ocs){
            foreach($ocs as $oc){
                $filter.= "(objectClass={$oc})";
            }
            $categoryCounter[$category] = 0;
        }
        $filter = "(|{$filter})";

        // Initiate the ldap query
        $res = $ldap->search($filter, array('objectClass'));
        if($ldap->success()) {

            // Count number of results per category
            while($entry = $ldap->fetch()){
                foreach($ocsToSearchFor as $category => $ocs){
                    if(count(array_intersect($ocs, $entry['objectClass']))){
                        $categoryCounter[$category] ++;
                        break; 
                    }
                }
            }
        }
        arsort($categoryCounter);

        // Do we have to return the result as SQL INSERT statement?
        if($statisticConformResult){ 
            $uuid           = $config->getGOsaUUID();
            $type           = 'objectCount';
            $plugin         = '';
            $action         = '';
            $date           = date('Y-m-d');
            $memory_usage   = sqlite_escape_string(stats::get_memory_usage());
            $cpu_load       = sqlite_escape_string(number_format(stats::get_cpu_load(),4,'.',''));
            $sql = array();
            foreach($categoryCounter as $category => $amount){
                $sql[] = array($type, $plugin, $category,$action,$uuid,$date, 0,0,$amount,$memory_usage,$cpu_load,'');
            }
            return($sql);
        }else{
            return($categoryCounter); 
        }
    }
}

?>
