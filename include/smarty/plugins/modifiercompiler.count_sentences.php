<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty count_sentences modifier plugin
 *
 * Type:     modifier<br>
=======
/**
 * Smarty count_sentences modifier plugin
 * Type:     modifier
>>>>>>> gosa-core_v2.8
 * Name:     count_sentences
 * Purpose:  count the number of sentences in a text
 *
 * @link http://www.smarty.net/manual/en/language.modifier.count.paragraphs.php
 *          count_sentences (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_count_sentences($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_count_sentences($params)
>>>>>>> gosa-core_v2.8
{
    // find periods, question marks, exclamation marks with a word before but not after.
    return 'preg_match_all("#\w[\.\?\!](\W|$)#S' . Smarty::$_UTF8_MODIFIER . '", ' . $params[0] . ', $tmp)';
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
