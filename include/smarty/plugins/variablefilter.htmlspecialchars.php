<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsFilter
 */
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
/**
 * Smarty htmlspecialchars variablefilter plugin
 *
 * @param string                   $source input string
<<<<<<< HEAD
 * @param Smarty_Internal_Template $smarty Smarty object
 * @return string filtered output
 */
function smarty_variablefilter_htmlspecialchars($source, $smarty)
{
    return htmlspecialchars($source, ENT_QUOTES, Smarty::$_CHARSET);
}

?>
=======
 * @param \Smarty_Internal_Template $template
 *
 * @return string filtered output
 */
function smarty_variablefilter_htmlspecialchars($source, Smarty_Internal_Template $template)
{
    return htmlspecialchars($source, ENT_QUOTES, Smarty::$_CHARSET);
}
>>>>>>> gosa-core_v2.8
