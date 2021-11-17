<?php
/**
 * Smarty plugin
<<<<<<< HEAD
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */

/**
 * Smarty lower modifier plugin
 *
 * Type:     modifier<br>
 * Name:     lower<br>
=======
 *
 * @package    Smarty
 * @subpackage PluginsModifierCompiler
 */
/**
 * Smarty lower modifier plugin
 * Type:     modifier
 * Name:     lower
>>>>>>> gosa-core_v2.8
 * Purpose:  convert string to lowercase
 *
 * @link http://www.smarty.net/manual/en/language.modifier.lower.php lower (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com>
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */

function smarty_modifiercompiler_lower($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_lower($params)
>>>>>>> gosa-core_v2.8
{
    if (Smarty::$_MBSTRING) {
        return 'mb_strtolower(' . $params[0] . ', \'' . addslashes(Smarty::$_CHARSET) . '\')' ;
    }
    // no MBString fallback
    return 'strtolower(' . $params[0] . ')';
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
