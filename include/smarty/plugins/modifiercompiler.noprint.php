<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty noprint modifier plugin
 *
 * Type:     modifier<br>
 * Name:     noprint<br>
 * Purpose:  return an empty string
 *
 * @author   Uwe Tews
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_noprint($params, $compiler)
{
    return "''";
}

?>
=======
/**
 * Smarty noprint modifier plugin
 * Type:     modifier
 * Name:     noprint
 * Purpose:  return an empty string
 *
 * @author Uwe Tews
 * @return string with compiled code
 */
function smarty_modifiercompiler_noprint()
{
    return "''";
}
>>>>>>> gosa-core_v2.8
