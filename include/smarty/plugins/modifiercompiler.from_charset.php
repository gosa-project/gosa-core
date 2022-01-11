<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty from_charset modifier plugin
 *
 * Type:     modifier<br>
 * Name:     from_charset<br>
 * Purpose:  convert character encoding from $charset to internal encoding
 *
 * @author Rodney Rehm
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_from_charset($params, $compiler)
=======
/**
 * Smarty from_charset modifier plugin
 * Type:     modifier
 * Name:     from_charset
 * Purpose:  convert character encoding from $charset to internal encoding
 *
 * @author Rodney Rehm
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_from_charset($params)
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

    return 'mb_convert_encoding(' . $params[0] . ', "' . addslashes(Smarty::$_CHARSET) . '", ' . $params[1] . ')';
}

?>
=======
    if (!isset($params[ 1 ])) {
        $params[ 1 ] = '"ISO-8859-1"';
    }
    return 'mb_convert_encoding(' . $params[ 0 ] . ', "' . addslashes(Smarty::$_CHARSET) . '", ' . $params[ 1 ] . ')';
}
>>>>>>> gosa-core_v2.8
