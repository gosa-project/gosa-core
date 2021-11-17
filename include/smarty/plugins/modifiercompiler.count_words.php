<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty count_words modifier plugin
 *
 * Type:     modifier<br>
 * Name:     count_words<br>
=======
/**
 * Smarty count_words modifier plugin
 * Type:     modifier
 * Name:     count_words
>>>>>>> gosa-core_v2.8
 * Purpose:  count the number of words in a text
 *
 * @link http://www.smarty.net/manual/en/language.modifier.count.words.php count_words (Smarty online manual)
 * @author Uwe Tews
<<<<<<< HEAD
 * @param array $params parameters
 * @return string with compiled code
*/
function smarty_modifiercompiler_count_words($params, $compiler)
=======
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_count_words($params)
>>>>>>> gosa-core_v2.8
{
    if (Smarty::$_MBSTRING) {
        // return 'preg_match_all(\'#[\w\pL]+#' . Smarty::$_UTF8_MODIFIER . '\', ' . $params[0] . ', $tmp)';
        // expression taken from http://de.php.net/manual/en/function.str-word-count.php#85592
<<<<<<< HEAD
        return 'preg_match_all(\'/\p{L}[\p{L}\p{Mn}\p{Pd}\\\'\x{2019}]*/' . Smarty::$_UTF8_MODIFIER . '\', ' . $params[0] . ', $tmp)';
=======
        return 'preg_match_all(\'/\p{L}[\p{L}\p{Mn}\p{Pd}\\\'\x{2019}]*/' . Smarty::$_UTF8_MODIFIER . '\', ' .
               $params[ 0 ] . ', $tmp)';
>>>>>>> gosa-core_v2.8
    }
    // no MBString fallback
    return 'str_word_count(' . $params[0] . ')';
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
