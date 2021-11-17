<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty default modifier plugin
 *
 * Type:     modifier<br>
 * Name:     default<br>
=======
/**
 * Smarty default modifier plugin
 * Type:     modifier
 * Name:     default
>>>>>>> gosa-core_v2.8
 * Purpose:  designate default value for empty variables
 *
 * @link http://www.smarty.net/manual/en/language.modifier.default.php default (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_default ($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_default($params)
>>>>>>> gosa-core_v2.8
{
    $output = $params[0];
    if (!isset($params[1])) {
        $params[1] = "''";
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    array_shift($params);
    foreach ($params as $param) {
        $output = '(($tmp = @' . $output . ')===null||$tmp===\'\' ? ' . $param . ' : $tmp)';
    }
    return $output;
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
