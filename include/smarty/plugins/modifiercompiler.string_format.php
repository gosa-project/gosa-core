<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty string_format modifier plugin
 *
 * Type:     modifier<br>
 * Name:     string_format<br>
=======
/**
 * Smarty string_format modifier plugin
 * Type:     modifier
 * Name:     string_format
>>>>>>> gosa-core_v2.8
 * Purpose:  format strings via sprintf
 *
 * @link http://www.smarty.net/manual/en/language.modifier.string.format.php string_format (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_string_format($params, $compiler)
{
    return 'sprintf(' . $params[1] . ',' . $params[0] . ')';
}

?>
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_string_format($params)
{
    return 'sprintf(' . $params[ 1 ] . ',' . $params[ 0 ] . ')';
}
>>>>>>> gosa-core_v2.8
