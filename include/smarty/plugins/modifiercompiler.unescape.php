<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsModifierCompiler
 */
<<<<<<< HEAD

/**
 * Smarty unescape modifier plugin
 *
 * Type:     modifier<br>
 * Name:     unescape<br>
 * Purpose:  unescape html entities
 *
 * @author Rodney Rehm
 * @param array $params parameters
 * @return string with compiled code
 */
function smarty_modifiercompiler_unescape($params, $compiler)
=======
/**
 * Smarty unescape modifier plugin
 * Type:     modifier
 * Name:     unescape
 * Purpose:  unescape html entities
 *
 * @author Rodney Rehm
 *
 * @param array $params parameters
 *
 * @return string with compiled code
 */
function smarty_modifiercompiler_unescape($params)
>>>>>>> gosa-core_v2.8
{
    if (!isset($params[1])) {
        $params[1] = 'html';
    }
    if (!isset($params[2])) {
        $params[2] = '\'' . addslashes(Smarty::$_CHARSET) . '\'';
    } else {
<<<<<<< HEAD
        $params[2] = "'" . $params[2] . "'";
    }

=======
        $params[ 2 ] = "'{$params[ 2 ]}'";
    }
>>>>>>> gosa-core_v2.8
    switch (trim($params[1], '"\'')) {
        case 'entity':
        case 'htmlall':
            if (Smarty::$_MBSTRING) {
                return 'mb_convert_encoding(' . $params[0] . ', ' . $params[2] . ', \'HTML-ENTITIES\')';
            }
<<<<<<< HEAD

            return 'html_entity_decode(' . $params[0] . ', ENT_NOQUOTES, ' . $params[2] . ')';

        case 'html':
            return 'htmlspecialchars_decode(' . $params[0] . ', ENT_QUOTES)';

        case 'url':
            return 'rawurldecode(' . $params[0] . ')';

=======
            return 'html_entity_decode(' . $params[ 0 ] . ', ENT_NOQUOTES, ' . $params[ 2 ] . ')';
        case 'html':
            return 'htmlspecialchars_decode(' . $params[ 0 ] . ', ENT_QUOTES)';
        case 'url':
            return 'rawurldecode(' . $params[ 0 ] . ')';
>>>>>>> gosa-core_v2.8
        default:
            return $params[0];
    }
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
