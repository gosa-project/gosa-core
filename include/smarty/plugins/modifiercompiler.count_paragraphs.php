<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty count_paragraphs modifier plugin
 *
 * Type:     modifier<br>
 * Name:     count_paragraphs<br>
=======
/**
 * Smarty count_paragraphs modifier plugin
 * Type:     modifier
 * Name:     count_paragraphs
>>>>>>> gosa-core_v2.8
 * Purpose:  count the number of paragraphs in a text
 *
 * @link http://www.smarty.net/manual/en/language.modifier.count.paragraphs.php
 *          count_paragraphs (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_count_paragraphs($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_count_paragraphs($params)
>>>>>>> gosa-core_v2.8
{
    // count \r or \n characters
    return '(preg_match_all(\'#[\r\n]+#\', ' . $params[0] . ', $tmp)+1)';
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
