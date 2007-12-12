<?php
/*
 * This code is part of GOsa (https://gosa.gonicus.de)
 * Copyright (C) 2005 Jan Wenzel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
/* This file builds a list of used php_functions in all files matching $pattern
 * (walks recursively through current directory but skips dirs matching $skip_dirs).
 *
 * 1. builds an array of filenames
 * 2. get contents from files
 * 2. fetches user defined functions using content
 * 3. fetches functions calls using content
 * 4. checks function calls for matching known functions
 */

/** 
 * Print status messages if set to true (useful for command-line use).
 * default: false */
$verbose= false;

/** 
 * Print debug messages if set to true.
 * default: false 
 */
$debug= false;

/**
 * Used by command-line client.
 */
function cmd_test_functions($path) {
  global $verbose;
  $verbose= true;
  test_functions($path);
}

/**
 * Scans files in $path and checks for calls of functions.
 * @param $path Path to search for files in (default: "./")
 * @param $pattern RegEx-Pattern for matching PHP-Files (default: "/.+\.(php[34]?$)|(inc$)/")
 * @param $skip_dirs RegEx-Pattern for directories to ignore (default: ".svn")
 * @return Array of used function-names
 */
function test_defined_functions($path="",$pattern="",$skip_dirs="") {
  global $debug,$verbose;
  $verbose= true;

  // Check for empty parameters. If empty, set default.
  if(strlen($path)==0) {
    $path= "./";
  }
  if(strlen($pattern)==0) {
    $pattern= "/.+\.(php[34]?$)|(inc$)/";
  }
  if(strlen($skip_dirs)==0) {
    $skip_dirs= ".svn";
  }
  
  // Create file-list
  $array= read_php_files($path,$pattern,$skip_dirs);

  // Needed arrays
  $functions= array();
  
  // We need to include our own function here, because PHP seems to lock the current executed function
  $user_functions= array('test_defined_functions');
  
  // Scan files for used defined functions
  foreach($array as $file) {
    // Build array of user defined functions
    $user_functions= array_merge($user_functions, extract_user_functions($file));
  }
  
  // Scan files for called functions
  foreach($array as $file) {
    // Build array of called functions
    array_push($functions, extract_php_functions($file));
  }
  
  // Make Array 'flat'
  $functions= array_values_recursive($functions);

  // remove duplicates
  $functions= array_unique($functions);

  // remove 'non-real' functions
  $reduced_functions= array_filter($functions,"filter_meta_functions");

  // remove user-defined functions
  $reduced_functions= array_diff($reduced_functions, $user_functions);

  // Now the array keys are messed up. Build an array with 'normal' keys (1,2,3,...).
  $functions= array_values($reduced_functions);
  
  return $functions;
}

/**
 * Scans files in $path and checks for calls of functions that are undefined.
 * Edit $debug and $verbose to control output. Default is 'false' for both to disallow
 * console functionality.
 * @param $path Path to search for files in (default: "./")
 * @param $pattern RegEx-Pattern for matching PHP-Files (default: "/.+\.(php[34]?$)|(inc$)/")
 * @param $skip_dirs RegEx-Pattern for directories to ignore (default: ".svn")
 * @return Array of invalid function-names (Scalars)
 */
function test_functions($path="",$pattern="",$skip_dirs="") {
  global $debug,$verbose;

  // Check for empty parameters
  if(strlen($path)==0) {
    $path= "./";
  }
  
  if(strlen($pattern)==0) {
    $pattern= "/.+\.(php[34]?$)|(inc$)/";
  }

  if(strlen($skip_dirs)==0) {
    $skip_dirs= ".svn";
  }
  
  if($debug || $verbose) {
    print("Scanning Directory...");
  }

  // Create file-list
  $array= read_php_files($path,$pattern,$skip_dirs);
  if($debug || $verbose) {
    print("done\n");
  }

  $functions= array();
  $user_functions= array();
  $invalid_functions= array();
  
  if($debug || $verbose) {
    print("Importing user functions...");
  }
  
  // Scan files for used defined functions
  foreach($array as $file) {
    // Build array of user defined functions
    $user_functions= array_merge($user_functions, extract_user_functions($file));
  }
  
  if($debug || $verbose) {
    print("done\n");
  }
  
  if($debug || $verbose) {
    print("Checking function calls...");
  }
  
  // Scan files for called functions
  foreach($array as $file) {
    // Build array of called functions
    $functions= extract_php_functions($file);
    foreach($functions as $key => $value) {
      foreach($value as $fn_key => $function_name) {
        if(!check_function($function_name) &&
           // array_search returns key of entry - this can be 0
           !is_int(array_search($function_name,$user_functions))
          ) {
            array_push($invalid_functions, $function_name);
        }
      }
    }
  }
  
  if($debug || $verbose) {
    print("done\n");
  }
  
  // remove duplicates
  $invalid_functions= array_unique($invalid_functions);
  
  if($debug || $verbose) {
    foreach($invalid_functions as $function_name) {
      print("\nFound invalid function ".$function_name);
    }
    print("\n");
  }

  return $invalid_functions;
}

/**
 * Reads $path for files matching $pattern but not in $skip_dirs.
 * @param $path Path to search for files in (default: "./")
 * @param $pattern RegEx-Pattern for matching PHP-Files (default: "/.+\.(php[34]?$)|(inc$)/")
 * @param $skip_dirs RegEx-Pattern for directories to ignore (default: ".svn")
 * @return array of content from PHP-Files scanned
 */
function read_php_files($path,$pattern,$skip_dirs) {
  global $debug,$verbose;
  $result= array();
  
  if(is_file($path)) {
    $file= $path;
    
    $php_content="";
    
    // Open Filehandle to process
    $fh= popen("`which php` -w $file", 'r');
      while(!feof($fh)) {
        // Read block-wise
        $php_content.=fread($fh,1024);
      }
    
    // Close Filehandle
    pclose($fh);

    array_push($result, $php_content);
  } else {
   $files= list_directory($path,$pattern,$skip_dirs);

   // Walk through files
   foreach ($files as $key => $file) {
     $php_content= "";
     
     // Open Filehandle to process
     $fh= popen("`which php` -w $file", 'r');
       while(!feof($fh)) {
         // Read block-wise
         $php_content.=fread($fh,1024);
       }
     
     // Close Filehandle
     pclose($fh);

     array_push($result, $php_content);
   }
  }

  return $result;
}

/** Only used as callback function when doing array_filter().
 * @return true if $function is not meta-function or keyword, false otherwise
 */
function filter_meta_functions ($function) {
  $result= true;
  if(is_meta_function($function)||is_keyword($function)) {
    $result= false;
  }
  return $result;
}

/**
 * Returns true if $function is a known php-function, false otherwise.
 * @return true if $function is a known php-function, false otherwise.
 */
function check_function($function) {
  $result= false;
  if(is_keyword($function)) {
    $result= true;
  } else if (is_meta_function($function)) {
    $result= true;
  } else if (function_exists($function)) {
    $result= true;
  }
  return $result;
}

/** Returns true if $function is meta-function, false otherwise.
 * @return true if $function is meta-function, false otherwise.
 */
function is_meta_function($function) {
  $meta_functions= array(
      "print","array","isset","exit","unset",
      "die","list","eval","empty"
    );
  return(in_array($function,$meta_functions));
}

/** Returns true if $function is keyword, false otherwise.
 * @return true if $function is keyword, false otherwise.
 */
function is_keyword($function) {
  $keywords= array(
     "if","else","elseif","while","do","case",
    "for","foreach","break","continue",
    "switch","declare","return","require",
    "include","require_once","include_once",
    "try","catch"
  );

  return(in_array($function,$keywords));
}

/**
 * Returns array of called functions.
 * @param $string PHP-Code
 * @return array of called functions.
 */
function extract_php_functions($string) {
  // Function names have to be A-z or _ for the first letter
  // plus 0-9, +, - for the following letters
  // Length is 2 minimum
  $rx_name="[A-Za-z_][A-Za-z0-9_+-]+";

  // Function calls can be after ',', '.', '(' or spaces
  // Note: Method calls are not detected - this is wanted
  $rx_function="[.,(\s]\s*($rx_name)\s*\(";

  // Get code inside <?php ... (question-mark)>
  $string= trim(array_pop(get_php_code($string)));
  $string= remove_strings($string);
  $result= array();
  preg_match_all("/$rx_function/",$string,$result);
  array_shift($result);


  // We need to check if "function" is actual a class created by new operator,
  // but negative lookbehind isn't possible with php yet.
  // So we must scan the array again and remove the found keys later (not while we're walking through).
  $classes= array();
  foreach($result[0] as $key => $function) {
    $match= array();
    if(preg_match("/new\s+($function)/",$string,$match)) {
      array_shift($match);
      array_push($classes,$match);
    }
  }

  // We need to manually unset the keys that we found above
  foreach($classes as $key => $value) {
    for($i=0;$i<count($value);$i++) {
      if(isset($result[0])) {
        unset($result[0][array_search($value[$i],$result[0])]);
      }
    }
  }

  return $result;
}

/**
 * Extracts function-definitions from php-code
 * @param $string php-code to extract function definitions from
 * @return array of functions defined
 */
function extract_user_functions($string) {
  $rx_name="[A-Za-z_][A-Za-z0-9_+-]*";
  $rx_user_function="function\s+($rx_name)\s*\(";

  // Get code inside <?php ... (question-mark)>
  $string= trim(array_pop(get_php_code($string)));
  $string= remove_strings($string);
  $result= array();
  preg_match_all("/$rx_user_function/s",$string,$result);
  array_shift($result);
  
  return $result= array_pop($result);
}

/**
 * Returns php-code without <?php ... markers.
 * @return php-code without <?php ... markers.
 */
function get_php_code($string) {
  $array= array();
  preg_match_all('/\<(\?php|\%)(.*)(\?|\%)\>/sx', $string, $array);

  // Pop the first two entries as we don't want them
  $match= array_shift($array);
  $match= array_shift($array);

  // Do the same for the last entry
  $match= array_pop($array);

  // The array $result only contains regex group (.*) now
  return array_pop($array);
}

/**
 * Removes all double and single quotes strings from sourcecode.
 * Returns 'print ();' for 'print ("hello world!\n");'
 * AND:
 * Returns '$message= sprintf(_()$foo,$bar); for
 *  $message= sprintf(_("Command '%s', specified as POSTREMOVE for plugin '%s' doesn't seem to exist.")$foo,$bar);
 * (Note the "doesn't")
 * @param $string code with strings
 * @return code with strings removed
 */
function remove_strings($string) {
  $result= "";
  
  $inside_string=false;
  $inside_string_uni=false;
  $inside_string_double=false;
  
  // Walk through $string
  for($i=0;$i<strlen($string);$i++) {
    if($string[$i]=="'" && ($i>0 && ($string[$i-1] != "\\"))) {
      if(!$inside_string_uni) {
        // We're now inside
        if(!$inside_string_double) {
          $inside_string_uni= true;
          $inside_string= true;
        }
      } else {
        // We're now outside
        $inside_string_uni= false;
        if(!$inside_string_double) {
          $inside_string= false;
        }
      }
    } else if($string[$i]=='"' && ($i>0 && ($string[$i-1] != "\\"))) {
      if(!$inside_string_double) {
        // We're now inside
        if(!$inside_string_uni) {
          $inside_string_double= true;
          $inside_string= true;
        }
      } else {
        // We're now outside
        $inside_string_double= false;
        if(!$inside_string_uni) {
          $inside_string= false;
        }
      }
    } else {
      // Push char to $result if not inside string
      $result.= (!$inside_string)?$string[$i]:"";
    }
  }

  // Return string
  return $result;
}

/** 
 * Scans directory $dir for files (filenames) matching regular expression $pattern
 * @param $dir Initial Directory to start scan
 * @param $pattern Regex-Pattern to match on files to scan
 * @param $skip_dirs Regex-Patten to match on directories to skip
 * @return file list
 */
function list_directory($dir, $pattern, $skip_dirs) {
  $file_list= '';
  $stack[]= $dir;
  while ($stack) {
    $current_dir= array_pop($stack);
    if ($dh= opendir($current_dir)) {
      while (($file= readdir($dh)) !== false) {
         if ($file !== '.' && $file !== '..' && preg_match("/$skip_dirs/",$file)==0) {
          $current_file= "{$current_dir}/{$file}";
          if (is_file($current_file) && preg_match($pattern, $current_file)) {
            $file_list[]= "{$current_dir}/{$file}";
          } elseif (is_dir($current_file)) {
            $stack[]= $current_file;
          }
        }
      }
    }
  }
  return $file_list;
}

/**
 * 'Flats' a multi-dimensional array. Keys are newly (incrementally) created by PHP.
 */
function array_values_recursive($array) {
  $arrayValues = array();
  
  foreach ($array as $value) {
    if (is_scalar($value) OR is_resource($value)) {
      $arrayValues[] = $value;
    } elseif (is_array($value)) {
      $arrayValues = array_merge($arrayValues, array_values_recursive($value));
    }
  }

  return $arrayValues;
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>

