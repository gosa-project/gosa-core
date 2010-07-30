<?php

class stats 
{
    



    static function checkDatabase()
    {
        $TABLE_NAME = 'stats';
    
        // Check for modules
        // php5-sqlite


        // Try to create database, if it exists just open it.
        $res = sqlite_open('/var/spool/gosa/stats', 0666, $error);
        if(!$res){
            return($res);
        }

        // Delete Table 
        $query = "DROP TABLE '{$TABLE_NAME}'";
#        $ret = sqlite_query($query, $res);

        // List Tables an check if there is already everything we need.
        $query = "SELECT name FROM sqlite_master WHERE type='table' and name='{$TABLE_NAME}'";
        $ret = sqlite_query($query, $res);
        if(!count(sqlite_fetch_all($ret))){

            // Check for table existance    
            $query = "
                CREATE TABLE {$TABLE_NAME} (
                        ID          INTEGER PRIMARY KEY,
                        ACTID       INTEGER,
                        TYPE        TEXT,
                        PLUGIN      TEXT,
                        ACTION      TEXT,
                        UUID        TEXT,
                        TIMESTAMP   INTEGER,
                        MTIMESTAMP  INTEGER,
                        AMOUNT      INTEGER,
                        DURATION    INTEGER
                        )";
            $ret = sqlite_query($query, $res);
        }

        return($res);
    }



    static function log($type, $plugin, $action, $amount = 1, $duration = 0)
    {
        global $config;
        global $clicks;
        $type       = sqlite_escape_string($type);
        $plugin     = sqlite_escape_string($plugin);
        $action     = sqlite_escape_string($action);
        $timestamp  = time();
        $mtimestamp = microtime(TRUE);
        $uuid       = $config->getGOsaUUID();
        $amount     = sqlite_escape_string($amount);
        $duration   = sqlite_escape_string($duration);
        $clicks     = sqlite_escape_string($clicks);

        $res = stats::checkDatabase();
        $TABLE_NAME = 'stats';
        $query = "
            INSERT INTO {$TABLE_NAME}
                (ACTID, TYPE, PLUGIN, ACTION, UUID, MTIMESTAMP, TIMESTAMP, AMOUNT, DURATION) 
            VALUES 
                ('{$clicks}','{$type}','{$plugin}','{$action}','{$uuid}','{$mtimestamp}','{$timestamp}','{$amount}','{$duration}')";
        sqlite_query($query, $res);
    }

    static function show()
    {
        $res = stats::checkDatabase();
        $TABLE_NAME = 'stats';
        $query = "SELECT * FROM {$TABLE_NAME} ORDER BY MTIMESTAMP";
        $query = "SELECT PLUGIN, ACTION, MAX(DURATION) as 'DURATION' FROM {$TABLE_NAME} WHERE ACTION='modify' GROUP BY PLUGIN,ACTION ";
        $query = "SELECT * FROM {$TABLE_NAME} ORDER BY MTIMESTAMP";
        $res = sqlite_query($query, $res);
        echo "<pre>";
        foreach(sqlite_fetch_all($res) as $entry){
            foreach($entry as $key => $str){
                if(is_numeric($key)) continue;
                
                if($key == "DURATION"){
                    $str = sprintf("%0.4f", $str);
                    echo str_pad($str,20,' ', STR_PAD_LEFT)."|"; 
                }else{
                    echo str_pad($str,20,' ')."|"; 
                }
            }
            echo "\n";
        }
        echo "</pre>";
        echo sqlite_error_string($res);
    }
}


?>
