<?php
/**
 * Smarty plugin
<<<<<<< HEAD
 * @package Smarty
 * @subpackage PluginsModifier
 */

/**
 * Smarty replace modifier plugin
 * 
 * Type:     modifier<br>
 * Name:     replace<br>
=======
 *
 * @package    Smarty
 * @subpackage PluginsModifier
 */
/**
 * Smarty replace modifier plugin
 * Type:     modifier
 * Name:     replace
>>>>>>> gosa-core_v2.8
 * Purpose:  simple search/replace
 * 
 * @link http://smarty.php.net/manual/en/language.modifier.replace.php replace (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com> 
 * @author Uwe Tews 
<<<<<<< HEAD
 * @param string $string  input string
 * @param string $search  text to search for
 * @param string $replace replacement text
=======
 *
 * @param string $string  input string
 * @param string $search  text to search for
 * @param string $replace replacement text
 *
>>>>>>> gosa-core_v2.8
 * @return string 
 */
function smarty_modifier_replace($string, $search, $replace)
{
<<<<<<< HEAD
    if (Smarty::$_MBSTRING) {
        require_once(SMARTY_PLUGINS_DIR . 'shared.mb_str_replace.php');
        return smarty_mb_str_replace($search, $replace, $string);
    }
    
    return str_replace($search, $replace, $string);
} 

?>
=======
    static $is_loaded = false;
    if (Smarty::$_MBSTRING) {
        if (!$is_loaded) {
            if (!is_callable('smarty_mb_str_replace')) {
                include_once SMARTY_PLUGINS_DIR . 'shared.mb_str_replace.php';
            }
            $is_loaded = true;
        }
        return smarty_mb_str_replace($search, $replace, $string);
    }
    return str_replace($search, $replace, $string);
}
>>>>>>> gosa-core_v2.8
