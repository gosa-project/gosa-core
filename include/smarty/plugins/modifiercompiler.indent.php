<?php
/**
 * Smarty plugin
<<<<<<< HEAD
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */

/**
 * Smarty indent modifier plugin
 *
 * Type:     modifier<br>
 * Name:     indent<br>
=======
 *
 * @package    Smarty
 * @subpackage PluginsModifierCompiler
 */
/**
 * Smarty indent modifier plugin
 * Type:     modifier
 * Name:     indent
>>>>>>> gosa-core_v2.8
 * Purpose:  indent lines of text
 *
 * @link http://www.smarty.net/manual/en/language.modifier.indent.php indent (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */

function smarty_modifiercompiler_indent($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_indent($params)
>>>>>>> gosa-core_v2.8
{
    if (!isset($params[1])) {
        $params[1] = 4;
    }
    if (!isset($params[2])) {
        $params[2] = "' '";
    }
    return 'preg_replace(\'!^!m\',str_repeat(' . $params[2] . ',' . $params[1] . '),' . $params[0] . ')';
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
