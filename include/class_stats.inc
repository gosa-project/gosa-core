<?php

class stats 
{
    
    static protected $lastCpuLoad = "";
    static protected $lastCpuLoadTimestamp = 0;

    static protected $tableName = "stats";
    static protected $tableFile = "/var/spool/gosa/stats";

    static protected $lastHandle = NULL;
    static protected $statsEnabled = FALSE;

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

    /*! \brief      |
     *  @param      |
     *  @return     |
     */  
    static function checkSQLiteExtension()
    {
        return(function_exists('sqlite_popen'));
    }


    /*! \brief      |
     *  @param      |
     *  @return     |
     */  
    static function dropTable($handle)
    {
        $TABLE_NAME = stats::$tableName;
        $query = "DROP TABLE '{$TABLE_NAME}'";
        $ret = sqlite_query($query, $handle);
    }


    /*! \brief      |
     *  @param      |
     *  @return     |
     */  
    static function get_memory_usage()
    {
        return(memory_get_usage());
    }


    /*! \brief      |
     *  @param      |
     *  @return     |
     */  
    static function get_cpu_load()
    {
        $cur = time();
        if(empty(stats::$lastCpuLoad) || (($cur - stats::$lastCpuLoadTimestamp) >=2 )){
            list($one, $five, $ten) =preg_split("/ /",shell_exec('cat /proc/loadavg'));
            stats::$lastCpuLoad = $one;
            stats::$lastCpuLoadTimestamp = $cur;
        }
        return(stats::$lastCpuLoad);
    }


    /*! \brief      |
     *  @param      |
     *  @return     |
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


    /*! \brief      |
     *  @param      |
     *  @return     |
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
        if(!isset($overallRenderTimer) || empty($overallRenderTimer)){
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
        $mtimestamp     = microtime(TRUE);
        $amount         = sqlite_escape_string($amount);
        $duration       = sqlite_escape_string($duration);
        $renderTime     = sqlite_escape_string($renderTime);
        $info           = sqlite_escape_string($info);
        $clicks         = sqlite_escape_string($clicks);
        $memory_usage   = sqlite_escape_string(stats::get_memory_usage());
        $cpu_load       = sqlite_escape_string(stats::get_cpu_load());

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
   
 
    /*! \brief      |
     *  @param      |
     *  @return     |
     */  
    static function show()
    {
        $res = stats::getDatabaseHandle();
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

        echo "</pre>";
    }
}


?>
