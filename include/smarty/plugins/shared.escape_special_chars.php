<?php
/**
 * Smarty shared plugin
 *
 * @package Smarty
 * @subpackage PluginsShared
 */
<<<<<<< HEAD

if (version_compare(PHP_VERSION, '5.2.3', '>=')) {
    /**
     * escape_special_chars common function
     *
     * Function: smarty_function_escape_special_chars<br>
=======
/**
 * escape_special_chars common function
 * Function: smarty_function_escape_special_chars
>>>>>>> gosa-core_v2.8
     * Purpose:  used by other smarty functions to escape
     *           special chars except for already escaped ones
     *
     * @author   Monte Ohrt <monte at ohrt dot com>
<<<<<<< HEAD
     * @param string $string text that should by escaped
=======
 *
 * @param string $string text that should by escaped
 *
>>>>>>> gosa-core_v2.8
     * @return string
     */
    function smarty_function_escape_special_chars($string)
    {
        if (!is_array($string)) {
<<<<<<< HEAD
            $string = htmlspecialchars($string, ENT_COMPAT, Smarty::$_CHARSET, false);
        }
        return $string;
    }  
} else {         
    /**
     * escape_special_chars common function
     *
     * Function: smarty_function_escape_special_chars<br>
     * Purpose:  used by other smarty functions to escape
     *           special chars except for already escaped ones
     *
     * @author   Monte Ohrt <monte at ohrt dot com>
     * @param string $string text that should by escaped
     * @return string
     */
    function smarty_function_escape_special_chars($string)
    {
        if (!is_array($string)) {
=======
        if (version_compare(PHP_VERSION, '5.2.3', '>=')) {
            $string = htmlspecialchars($string, ENT_COMPAT, Smarty::$_CHARSET, false);
        } else {
>>>>>>> gosa-core_v2.8
            $string = preg_replace('!&(#?\w+);!', '%%%SMARTY_START%%%\\1%%%SMARTY_END%%%', $string);
            $string = htmlspecialchars($string);
            $string = str_replace(array('%%%SMARTY_START%%%', '%%%SMARTY_END%%%'), array('&', ';'), $string); 
        }
<<<<<<< HEAD
        return $string;
    }                                                                                                             
} 

?>
=======
    }
    return $string;
}
>>>>>>> gosa-core_v2.8
