<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsShared
 */
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
/**
 * evaluate compiler parameter
 *
 * @param array   $params  parameter array as given to the compiler function
 * @param integer $index   array index of the parameter to convert
 * @param mixed   $default value to be returned if the parameter is not present
<<<<<<< HEAD
=======
 *
>>>>>>> gosa-core_v2.8
 * @return mixed evaluated value of parameter or $default
 * @throws SmartyException if parameter is not a literal (but an expression, variable, …)
 * @author Rodney Rehm
 */
function smarty_literal_compiler_param($params, $index, $default=null)
{
    // not set, go default
    if (!isset($params[$index])) {
        return $default;
    }
    // test if param is a literal
<<<<<<< HEAD
    if (!preg_match('/^([\'"]?)[a-zA-Z0-9]+(\\1)$/', $params[$index])) {
        throw new SmartyException('$param[' . $index . '] is not a literal and is thus not evaluatable at compile time');
    }

=======
    if (!preg_match('/^([\'"]?)[a-zA-Z0-9-]+(\\1)$/', $params[ $index ])) {
        throw new SmartyException(
            '$param[' . $index .
            '] is not a literal and is thus not evaluatable at compile time'
        );
    }
>>>>>>> gosa-core_v2.8
    $t = null;
    eval("\$t = " . $params[$index] . ";");
    return $t;
}
