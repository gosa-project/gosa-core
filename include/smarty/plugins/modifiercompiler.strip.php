<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty strip modifier plugin
 *
 * Type:     modifier<br>
 * Name:     strip<br>
 * Purpose:  Replace all repeated spaces, newlines, tabs
 *              with a single space or supplied replacement string.<br>
 * Example:  {$var|strip} {$var|strip:"&nbsp;"}<br>
=======
/**
 * Smarty strip modifier plugin
 * Type:     modifier
 * Name:     strip
 * Purpose:  Replace all repeated spaces, newlines, tabs
 *              with a single space or supplied replacement string.
 * Example:  {$var|strip} {$var|strip:"&nbsp;"}
>>>>>>> gosa-core_v2.8
 * Date:     September 25th, 2002
 *
 * @link http://www.smarty.net/manual/en/language.modifier.strip.php strip (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */

function smarty_modifiercompiler_strip($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_strip($params)
>>>>>>> gosa-core_v2.8
{
    if (!isset($params[1])) {
        $params[1] = "' '";
    }
    return "preg_replace('!\s+!" . Smarty::$_UTF8_MODIFIER . "', {$params[1]},{$params[0]})";
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
