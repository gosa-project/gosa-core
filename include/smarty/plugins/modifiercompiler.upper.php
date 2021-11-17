<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty upper modifier plugin
 * 
 * Type:     modifier<br>
 * Name:     lower<br>
=======
/**
 * Smarty upper modifier plugin
 * Type:     modifier
 * Name:     lower
>>>>>>> gosa-core_v2.8
 * Purpose:  convert string to uppercase
 * 
 * @link http://smarty.php.net/manual/en/language.modifier.upper.php lower (Smarty online manual)
 * @author Uwe Tews 
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_upper($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_upper($params)
>>>>>>> gosa-core_v2.8
{
    if (Smarty::$_MBSTRING) {
        return 'mb_strtoupper(' . $params[0] . ', \'' . addslashes(Smarty::$_CHARSET) . '\')' ;
    }
    // no MBString fallback
    return 'strtoupper(' . $params[0] . ')';
} 
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
