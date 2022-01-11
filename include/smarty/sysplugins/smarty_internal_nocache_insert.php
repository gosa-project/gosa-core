<?php
/**
 * Smarty Internal Plugin Nocache Insert
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {insert} tag into the cache file
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Insert Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Nocache_Insert {

=======
class Smarty_Internal_Nocache_Insert
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {insert} tag into cache file
     *
     * @param string                   $_function insert function name
     * @param array                    $_attr     array with parameter
     * @param Smarty_Internal_Template $_template template object
     * @param string                   $_script   script name to load or 'null'
     * @param string                   $_assign   optional variable name
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string compiled code
     */
    public static function compile($_function, $_attr, $_template, $_script, $_assign = null)
    {
        $_output = '<?php ';
<<<<<<< HEAD
        if ($_script != 'null') {
=======
        if ($_script !== 'null') {
>>>>>>> gosa-core_v2.8
            // script which must be included
            // code for script file loading
            $_output .= "require_once '{$_script}';";
        }
        // call insert
        if (isset($_assign)) {
<<<<<<< HEAD
            $_output .= "\$_smarty_tpl->assign('{$_assign}' , {$_function} (" . var_export($_attr, true) . ",\$_smarty_tpl), true);?>";
        } else {
            $_output .= "echo {$_function}(" . var_export($_attr, true) . ",\$_smarty_tpl);?>";
        }
        $_tpl = $_template;
        while ($_tpl->parent instanceof Smarty_Internal_Template) {
            $_tpl = $_tpl->parent;
        }
        return "/*%%SmartyNocache:{$_tpl->properties['nocache_hash']}%%*/" . $_output . "/*/%%SmartyNocache:{$_tpl->properties['nocache_hash']}%%*/";
    }

}

?>
=======
            $_output .= "\$_smarty_tpl->assign('{$_assign}' , {$_function} (" . var_export($_attr, true) .
                        ',\$_smarty_tpl), true);?>';
        } else {
            $_output .= "echo {$_function}(" . var_export($_attr, true) . ',$_smarty_tpl);?>';
        }
        $_tpl = $_template;
        while ($_tpl->_isSubTpl()) {
            $_tpl = $_tpl->parent;
        }
        return "/*%%SmartyNocache:{$_tpl->compiled->nocache_hash}%%*/{$_output}/*/%%SmartyNocache:{$_tpl->compiled->nocache_hash}%%*/";
    }
}
>>>>>>> gosa-core_v2.8
