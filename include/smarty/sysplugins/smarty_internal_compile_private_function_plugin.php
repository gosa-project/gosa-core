<?php
/**
 * Smarty Internal Plugin Compile Function Plugin
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles code for the execution of function plugin
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Function Plugin Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Private_Function_Plugin extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Private_Function_Plugin extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array();
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('_any');

    /**
     * Compiles code for the execution of function plugin
     *
     * @param array $args array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler compiler object
     * @param array $parameter array with compilation parameter
     * @param string $tag name of function plugin
     * @param string $function PHP function name
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter, $tag, $function)
    {
        // This tag does create output
        $compiler->has_output = true;

        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if ($_attr['nocache'] === true) {
            $compiler->tag_nocache = true;
        }
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     * @param string                                $tag       name of function plugin
     * @param string                                $function  PHP function name
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter, $tag, $function)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
>>>>>>> gosa-core_v2.8
        unset($_attr['nocache']);
        // convert attributes into parameter array string
        $_paramsArray = array();
        foreach ($_attr as $_key => $_value) {
            if (is_int($_key)) {
                $_paramsArray[] = "$_key=>$_value";
            } else {
                $_paramsArray[] = "'$_key'=>$_value";
            }
        }
<<<<<<< HEAD
        $_params = 'array(' . implode(",", $_paramsArray) . ')';
        // compile code
        $output = "<?php echo {$function}({$_params},\$_smarty_tpl);?>\n";
        return $output;
    }

}

?>
=======
        $_params = 'array(' . implode(',', $_paramsArray) . ')';
        // compile code
        $output = "{$function}({$_params},\$_smarty_tpl)";
        if (!empty($parameter[ 'modifierlist' ])) {
            $output = $compiler->compileTag(
                'private_modifier',
                array(),
                array(
                    'modifierlist' => $parameter[ 'modifierlist' ],
                    'value'        => $output
                )
            );
        }
        $output = "<?php echo {$output};?>\n";
        return $output;
    }
}
>>>>>>> gosa-core_v2.8
