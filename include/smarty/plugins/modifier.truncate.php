<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifier
 */
<<<<<<< HEAD
 
/**
 * Smarty truncate modifier plugin
 * 
 * Type:     modifier<br>
 * Name:     truncate<br>
=======
/**
 * Smarty truncate modifier plugin
 * Type:     modifier
 * Name:     truncate
>>>>>>> gosa-core_v2.8
 * Purpose:  Truncate a string to a certain length if necessary,
 *               optionally splitting in the middle of a word, and
 *               appending the $etc string or inserting $etc into the middle.
 * 
 * @link http://smarty.php.net/manual/en/language.modifier.truncate.php truncate (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com> 
<<<<<<< HEAD
=======
 *
>>>>>>> gosa-core_v2.8
 * @param string  $string      input string
 * @param integer $length      length of truncated text
 * @param string  $etc         end string
 * @param boolean $break_words truncate at word boundary
 * @param boolean $middle      truncate in the middle of text
<<<<<<< HEAD
 * @return string truncated string
 */
function smarty_modifier_truncate($string, $length = 80, $etc = '...', $break_words = false, $middle = false) {
    if ($length == 0)
        return '';

=======
 *
 * @return string truncated string
 */
function smarty_modifier_truncate($string, $length = 80, $etc = '...', $break_words = false, $middle = false)
{
    if ($length === 0) {
        return '';
    }
>>>>>>> gosa-core_v2.8
    if (Smarty::$_MBSTRING) {
        if (mb_strlen($string, Smarty::$_CHARSET) > $length) {
            $length -= min($length, mb_strlen($etc, Smarty::$_CHARSET));
            if (!$break_words && !$middle) {
<<<<<<< HEAD
                $string = preg_replace('/\s+?(\S+)?$/' . Smarty::$_UTF8_MODIFIER, '', mb_substr($string, 0, $length + 1, Smarty::$_CHARSET));
=======
                $string = preg_replace(
                    '/\s+?(\S+)?$/' . Smarty::$_UTF8_MODIFIER,
                    '',
                    mb_substr($string, 0, $length + 1, Smarty::$_CHARSET)
                );
>>>>>>> gosa-core_v2.8
            } 
            if (!$middle) {
                return mb_substr($string, 0, $length, Smarty::$_CHARSET) . $etc;
            }
<<<<<<< HEAD
            return mb_substr($string, 0, $length / 2, Smarty::$_CHARSET) . $etc . mb_substr($string, - $length / 2, $length, Smarty::$_CHARSET);
        }
        return $string;
    }
    
=======
            return mb_substr($string, 0, $length / 2, Smarty::$_CHARSET) . $etc .
                   mb_substr($string, -$length / 2, $length, Smarty::$_CHARSET);
        }
        return $string;
    }
>>>>>>> gosa-core_v2.8
    // no MBString fallback
    if (isset($string[$length])) {
        $length -= min($length, strlen($etc));
        if (!$break_words && !$middle) {
            $string = preg_replace('/\s+?(\S+)?$/', '', substr($string, 0, $length + 1));
        } 
        if (!$middle) {
            return substr($string, 0, $length) . $etc;
        }
        return substr($string, 0, $length / 2) . $etc . substr($string, - $length / 2);
    }
    return $string;
} 
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
