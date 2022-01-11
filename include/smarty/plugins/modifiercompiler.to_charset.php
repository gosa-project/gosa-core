<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty to_charset modifier plugin
 *
 * Type:     modifier<br>
 * Name:     to_charset<br>
 * Purpose:  convert character encoding from internal encoding to $charset
 *
 * @author Rodney Rehm
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_to_charset($params, $compiler)
=======
/**
 * Smarty to_charset modifier plugin
 * Type:     modifier
 * Name:     to_charset
 * Purpose:  convert character encoding from internal encoding to $charset
 *
 * @author Rodney Rehm
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_to_charset($params)
>>>>>>> gosa-core_v2.8
{
    if (!Smarty::$_MBSTRING) {
        // FIXME: (rodneyrehm) shouldn't this throw an error?
        return $params[0];
    }
<<<<<<< HEAD

    if (!isset($params[1])) {
        $params[1] = '"ISO-8859-1"';
    }

    return 'mb_convert_encoding(' . $params[0] . ', ' . $params[1] . ', "' . addslashes(Smarty::$_CHARSET) . '")';
}

?>
=======
    if (!isset($params[ 1 ])) {
        $params[ 1 ] = '"ISO-8859-1"';
    }
    return 'mb_convert_encoding(' . $params[ 0 ] . ', ' . $params[ 1 ] . ', "' . addslashes(Smarty::$_CHARSET) . '")';
}
>>>>>>> gosa-core_v2.8
