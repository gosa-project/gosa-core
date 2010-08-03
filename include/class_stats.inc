<?php

class stats 
{
    
    static protected $lastCpuLoad = "";
    static protected $lastCpuLoadTimestamp = 0;

    static protected $tableName = "stats";
    static protected $tableFile = "/var/spool/gosa/stats";

    static protected $lastHandle = NULL;
    static protected $statsEnabled = FALSE;


    /*! \brief     This method tries to connect the GOsa-stats database and 
     *              then returns a database handle on success else NULL.
     *
     *             (The GOsa-stats database has to be enabled : statsDatabaseEnabled/statsDatabaseFile)
     *
     *             This database will then contain information about the use of GOsa,
     *              no customer data will be stored.
     *
     *  @return     handle      Returns a sqlite database handle.
     */
    static function getDatabaseHandle()
    {
        // Try to return last valid handle.
        if(stats::$lastHandle != NULL && is_resource(stats::$lastHandle)){
            return(stats::$lastHandle);
        }

        // Check if Logging is enabled
        global $config;
        if(!is_object($config) || ! $config instanceOf config){
            return(NULL);
        }
    
        // Get statsFile property 
        stats::$tableFile    = $config->get_cfg_value('core', 'statsDatabaseFile');
        stats::$statsEnabled = $config->boolValueIsTrue('core', 'statsDatabaseEnabled');
        if(!stats::$statsEnabled){
            return;
        }

        // Check for SQLite extension
        if(!stats::checkSQLiteExtension()){
            return(NULL);
        }

        // Check if we are able to read/write the given database file.
        if(!is_writeable(stats::$tableFile) && !is_writeable(dirname(stats::$tableFile))){
            return(NULL);
        }

        // Try to create database, if it exists just open it.
        $handle = sqlite_popen(stats::$tableFile, 0666, $error);
        if($handle){
            stats::createDatabaseOnDemand($handle);
        }
        stats::$lastHandle = $handle;
        return($handle);
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

        $duration   = stats::prepareFloatForWriting($duration);
        $renderTime = stats::prepareFloatForWriting($renderTime);

        // Prepare values to be useable within a database
        $uuid = $config->getGOsaUUID();
        $type           = sqlite_escape_string($type);
        $plugin         = sqlite_escape_string($plugin);
        $action         = sqlite_escape_string($action);
        $timestamp      = time();
        $mtimestamp     = microtime(TRUE);
        $amount         = sqlite_escape_string($amount);
        $duration       = sqlite_escape_string($duration);
        $renderTime     = sqlite_escape_string($renderTime);
        $info           = sqlite_escape_string($info);
        $clicks         = sqlite_escape_string($clicks);
        $memory_usage   = sqlite_escape_string(stats::get_memory_usage());
        $cpu_load       = sqlite_escape_string(sprintf("%0.6f",stats::get_cpu_load()));

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
  
 
    /*! \brief      This method returns all entries of the GOsa-stats table.
     *              You can limit the result by setting the from/to parameter (timestamp).
     *  @param      int     from    The timestamp to start the result from. 
     *  @param      int     to      The timestamp to end the request.
     *  @return     array           An array containing the requested entries.
     */  
    static function dumpTables($from = NULL, $to = NULL)
    {
        // Get database connection
        $TABLE_NAME = stats::$tableName;
        $handle = stats::getDatabaseHandle();
        if(!$handle) return;        

        // Build up filter to limit dumped entries to the given range.
        $tim = "";
        if($from != NULL){
            $from = sqlite_escape_string($from);
            $tim.= "AND TIMESTAMP >= '{$from}' ";
        }
        if($to != NULL){
            $to = sqlite_escape_string($to);
            $tim.= "AND TIMESTAMP <= '{$to}' ";
        }
        $tim = preg_replace("/^AND /"," WHERE ",$tim);

        // Create Filter and start query
        $filter = "SELECT * FROM {$TABLE_NAME}{$tim} ORDER BY ID";
        $ret = sqlite_array_query($filter, $handle, SQLITE_ASSOC);
        return($ret);
    }

 
    /*! \brief      This is just a dummy output/debug method 
     *              It directly prints some stats and table infos on the screen.
     */  
    static function show()
    {
        $res = stats::getDatabaseHandle();
#       stats::dropTable($res);
        if(!$res) return;        

        $TABLE_NAME = stats::$tableName;
        $query = "SELECT * FROM {$TABLE_NAME} ORDER BY MTIMESTAMP";
        $query = "SELECT PLUGIN, ACTION, MAX(DURATION) as 'DURATION' FROM {$TABLE_NAME} WHERE ACTION='modify' GROUP BY PLUGIN,ACTION ";
        $query = "SELECT * FROM {$TABLE_NAME} ORDER BY ID DESC LIMIT 30";
        $query = "SELECT * FROM {$TABLE_NAME} WHERE plugin != 'LDAP' ORDER BY ID DESC LIMIT 30";
        $ret = sqlite_query($query, $res);
        echo "<pre>";

        $colSize = 16;

        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            if(!$title){
                foreach($entry as $key => $str) {
                    if(is_numeric($key)) continue;
                    echo str_pad($key,$colSize,' ')."|";
                }
                echo "\n";
                foreach($entry as $key => $str) {
                    if(is_numeric($key)) continue;
                    echo str_pad('',$colSize,'-')."-";
                }
                echo "\n";
                $title = TRUE;
            }
 
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                if($key == "DURATION" || $key == "MTIMESTAMP" || $key == "CPU_LOAD"){
                    $str = sprintf("%0.4f", preg_replace("/,/",".",$str));
                    echo str_pad($str,$colSize,' ', STR_PAD_LEFT)."|"; 
                }else{
                    echo str_pad($str,$colSize,' ')."|"; 
                }
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Time spent per plugin-category \n";
        echo "------ \n";

        $query = "
            SELECT SUM(DURATION) AS DUR, CATEGORY 
            FROM {$TABLE_NAME}
            WHERE plugin != 'LDAP' 
            GROUP BY CATEGORY 
            ORDER BY DUR DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }


        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Time spent per plugin \n";
        echo "------ \n";

        $query = "
            SELECT SUM(DURATION) AS DUR, PLUGIN, UUID 
            FROM {$TABLE_NAME}
            WHERE plugin != 'LDAP' 
            GROUP BY PLUGIN, UUID 
            ORDER BY DUR DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

 # * Anzahl von Objekttypen
 # * Anzahl der Löschungen pro Objekttyp
 # * Anzahl der Erzeugungen pro Objekttyp
 # * Anzahl der Bearbeitungen pro Objekttyp
 # * Anzahl der Verschiebungen pro Objekttyp
 # * Anzahl der Mehrfachbearbeitungen pro Objekttyp
 # * Antwortzeiten pro aktion
 # * Anzahl der Passwortänderungen
 # * Anzahl der unterschiedlichen Anmeldungen


        echo "\n------ \n";
        echo "Actions done per plugin \n";
        echo "------ \n";

        $query = "
            SELECT COUNT(ACTION) as CNT, ACTION, PLUGIN 
            FROM {$TABLE_NAME}
            WHERE   TYPE = 'plugin'
             AND    PLUGIN != 'LDAP'
            GROUP BY ACTION,PLUGIN 
            ORDER BY CNT DESC LIMIT 30";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "'create' actions done per plugin  (5 most)\n";
        echo "------ \n";

        $query = "
            SELECT COUNT(ACTION) as CNT, ACTION, PLUGIN 
            FROM {$TABLE_NAME}
            WHERE   TYPE = 'plugin'
             AND    PLUGIN != 'LDAP'
             AND    ACTION = 'create'
            GROUP BY ACTION,PLUGIN 
            ORDER BY CNT DESC LIMIT 5";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "'move' actions done per plugin  (5 most)\n";
        echo "------ \n";

        $query = "
            SELECT COUNT(ACTION) as CNT, ACTION, PLUGIN 
            FROM {$TABLE_NAME}
            WHERE   TYPE = 'plugin'
             AND    PLUGIN != 'LDAP'
             AND    ACTION = 'move'
            GROUP BY ACTION,PLUGIN 
            ORDER BY CNT DESC LIMIT 5";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "'view' actions done per plugin  (5 most)\n";
        echo "------ \n";

        $query = "
            SELECT COUNT(ACTION) as CNT, ACTION, PLUGIN 
            FROM {$TABLE_NAME}
            WHERE   TYPE = 'plugin'
             AND    PLUGIN != 'LDAP'
             AND    ACTION = 'view'
            GROUP BY ACTION,PLUGIN 
            ORDER BY CNT DESC LIMIT 5";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "'open' actions done per plugin  (5 most)\n";
        echo "------ \n";

        $query = "
            SELECT COUNT(ACTION) as CNT, ACTION, PLUGIN, UUID 
            FROM {$TABLE_NAME}
            WHERE   TYPE = 'plugin'
             AND    PLUGIN != 'LDAP'
             AND    ACTION = 'open'
            GROUP BY ACTION,PLUGIN, UUID
            ORDER BY CNT DESC LIMIT 5";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Time per session\n";
        echo "------ \n";

        $query = "
            SELECT SUM(DURATION) as DUR, UUID
            FROM {$TABLE_NAME}
            GROUP BY UUID
            ORDER BY DUR DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Most used password hashes\n";
        echo "------ \n";

        $query = "
            SELECT DISTINCT(INFO), COUNT(INFO) as CNT
            FROM {$TABLE_NAME}
            WHERE ACTION = 'change_password'
            GROUP BY INFO
            ORDER BY INFO DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Actions at all\n";
        echo "------ \n";

        $query = "
            SELECT DISTINCT(ACTION), COUNT(ACTION)  AS CNT
            FROM {$TABLE_NAME}
            GROUP BY ACTION
            ORDER BY CNT DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Time spent per action\n";
        echo "------ \n";

        $query = "
            SELECT DISTINCT(ACTION), SUM(DURATION) AS DUR
            FROM {$TABLE_NAME}
            GROUP BY ACTION
            ORDER BY DUR DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "\n------ \n";
        echo "Average time per action\n";
        echo "------ \n";

        $query = "
            SELECT DISTINCT(ACTION), AVG(DURATION) AS DUR
            FROM {$TABLE_NAME}
            GROUP BY ACTION
            ORDER BY DUR DESC LIMIT 10";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);


        echo "\n------ \n";
        echo "Rendertime per plugin\n";
        echo "------ \n";

        $query = "
            SELECT PLUGIN, RENDER_TIME AS RM
            FROM {$TABLE_NAME}
            GROUP BY PLUGIN
            ORDER BY RM DESC
            LIMIT 10
            ";
        $ret = sqlite_query($query, $res);

        $colSize = 16;
        $title = FALSE;
        foreach(sqlite_fetch_all($ret) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                echo str_pad($str,$colSize,' ')."|"; 
            }
            echo "\n";
        }
        echo sqlite_error_string($ret);

        echo "</pre>";
    }


    /*! \brief     Somehow sqlite can not work with float values when it comes to AVG() SUM().
     *             We use this methods to convert float values to int and vice versa.
     *             The database will then contain 'int' intead of 'float'.
     *                  prepareFloatForReading -> Used for reading 'float' values.
     *                  prepareFloatForWriting -> Used for writing 'float' values.
     *  @param     float    The 'float' value to convert.
     *  @return    int      The converted float value. 
     */
    static function prepareFloatForWriting($float)
    {
        return(floor($float * 1000));
    }



    /*! \brief     Somehow sqlite can not work with float values when it comes to AVG() SUM().
     *             We use this methods to convert float values to int and vice versa.
     *             The database will then contain 'int' intead of 'float'.
     *                  prepareFloatForWriting -> Used for writing 'float' values.
     *  @param     float    The 'int' value read from the table.
     *  @return    int      The converted int value. 
     */
    static function prepareFloatForReading($int)
    {
        return($int / 1000);
    }
}

?>
