<?php
/**
 * Smarty plugin
 * 
 * @package Smarty
 * @subpackage Debug
 */
<<<<<<< HEAD

/**
 * Smarty debug_print_var modifier plugin
 * 
 * Type:     modifier<br>
 * Name:     debug_print_var<br>
 * Purpose:  formats variable contents for display in the console
 *
 * @author Monte Ohrt <monte at ohrt dot com> 
 * @param array|object $var     variable to be formatted
 * @param integer      $depth   maximum recursion depth if $var is an array
 * @param integer      $length  maximum string length if $var is a string
 * @return string 
 */
function smarty_modifier_debug_print_var ($var, $depth = 0, $length = 40)
{
    $_replace = array("\n" => '<i>\n</i>',
        "\r" => '<i>\r</i>',
        "\t" => '<i>\t</i>'
        );

    switch (gettype($var)) {
        case 'array' :
            $results = '<b>Array (' . count($var) . ')</b>';
            foreach ($var as $curr_key => $curr_val) {
                $results .= '<br>' . str_repeat('&nbsp;', $depth * 2)
                 . '<b>' . strtr($curr_key, $_replace) . '</b> =&gt; '
                 . smarty_modifier_debug_print_var($curr_val, ++$depth, $length);
                $depth--;
            } 
            break;
            
        case 'object' :
            $object_vars = get_object_vars($var);
            $results = '<b>' . get_class($var) . ' Object (' . count($object_vars) . ')</b>';
            foreach ($object_vars as $curr_key => $curr_val) {
                $results .= '<br>' . str_repeat('&nbsp;', $depth * 2)
                 . '<b> -&gt;' . strtr($curr_key, $_replace) . '</b> = '
                 . smarty_modifier_debug_print_var($curr_val, ++$depth, $length);
                $depth--;
            } 
            break;
            
=======
/**
 * Smarty debug_print_var modifier plugin
 * Type:     modifier
 * Name:     debug_print_var
 * Purpose:  formats variable contents for display in the console
 *
 * @author Monte Ohrt <monte at ohrt dot com>
 *
 * @param array|object $var     variable to be formatted
 * @param int          $max     maximum recursion depth if $var is an array or object
 * @param int          $length  maximum string length if $var is a string
 * @param int          $depth   actual recursion depth
 * @param array        $objects processed objects in actual depth to prevent recursive object processing
 *
 * @return string
 */
function smarty_modifier_debug_print_var($var, $max = 10, $length = 40, $depth = 0, $objects = array())
{
    $_replace = array("\n" => '\n', "\r" => '\r', "\t" => '\t');
    switch (gettype($var)) {
        case 'array':
            $results = '<b>Array (' . count($var) . ')</b>';
            if ($depth === $max) {
                break;
            }
            foreach ($var as $curr_key => $curr_val) {
                $results .= '<br>' . str_repeat('&nbsp;', $depth * 2) . '<b>' . strtr($curr_key, $_replace) .
                            '</b> =&gt; ' .
                            smarty_modifier_debug_print_var($curr_val, $max, $length, ++$depth, $objects);
                $depth--;
            }
            break;
        case 'object':
            $object_vars = get_object_vars($var);
            $results = '<b>' . get_class($var) . ' Object (' . count($object_vars) . ')</b>';
            if (in_array($var, $objects)) {
                $results .= ' called recursive';
                break;
            }
            if ($depth === $max) {
                break;
            }
            $objects[] = $var;
            foreach ($object_vars as $curr_key => $curr_val) {
                $results .= '<br>' . str_repeat('&nbsp;', $depth * 2) . '<b> -&gt;' . strtr($curr_key, $_replace) .
                            '</b> = ' . smarty_modifier_debug_print_var($curr_val, $max, $length, ++$depth, $objects);
                $depth--;
            }
            break;
>>>>>>> gosa-core_v2.8
        case 'boolean' :
        case 'NULL' :
        case 'resource' :
            if (true === $var) {
                $results = 'true';
            } elseif (false === $var) {
                $results = 'false';
            } elseif (null === $var) {
                $results = 'null';
            } else {
                $results = htmlspecialchars((string) $var);
            } 
            $results = '<i>' . $results . '</i>';
            break;
<<<<<<< HEAD
            
=======
>>>>>>> gosa-core_v2.8
        case 'integer' :
        case 'float' :
            $results = htmlspecialchars((string) $var);
            break;
<<<<<<< HEAD
            
=======
>>>>>>> gosa-core_v2.8
        case 'string' :
            $results = strtr($var, $_replace);
            if (Smarty::$_MBSTRING) {
                if (mb_strlen($var, Smarty::$_CHARSET) > $length) {
                    $results = mb_substr($var, 0, $length - 3, Smarty::$_CHARSET) . '...';
                }
            } else {
                if (isset($var[$length])) {
                    $results = substr($var, 0, $length - 3) . '...';
                }
            }
<<<<<<< HEAD

            $results = htmlspecialchars('"' . $results . '"');
            break;
            
=======
            $results = htmlspecialchars('"' . $results . '"', ENT_QUOTES, Smarty::$_CHARSET);
            break;
>>>>>>> gosa-core_v2.8
        case 'unknown type' :
        default :
            $results = strtr((string) $var, $_replace);
            if (Smarty::$_MBSTRING) {
                if (mb_strlen($results, Smarty::$_CHARSET) > $length) {
                    $results = mb_substr($results, 0, $length - 3, Smarty::$_CHARSET) . '...';
                }
            } else {
                if (strlen($results) > $length) {
                    $results = substr($results, 0, $length - 3) . '...';
                }
            }
<<<<<<< HEAD
             
            $results = htmlspecialchars($results);
    } 

    return $results;
} 

?>
=======
            $results = htmlspecialchars($results, ENT_QUOTES, Smarty::$_CHARSET);
    }
    return $results;
}
>>>>>>> gosa-core_v2.8
