<?php
/**
 * Smarty Internal Plugin Compile Capture
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {capture} tag
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Capture Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Capture extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Capture extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('name');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('name', 'assign', 'append');

    /**
<<<<<<< HEAD
     * Compiles code for the {capture} tag
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        $buffer = isset($_attr['name']) ? $_attr['name'] : "'default'";
        $assign = isset($_attr['assign']) ? $_attr['assign'] : 'null';
        $append = isset($_attr['append']) ? $_attr['append'] : 'null';

        $compiler->_capture_stack[0][] = array($buffer, $assign, $append, $compiler->nocache);
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        $_output = "<?php \$_smarty_tpl->_capture_stack[0][] = array($buffer, $assign, $append); ob_start(); ?>";

        return $_output;
    }

=======
     * Compiles code for the {$smarty.capture.xxx}
     *
     * @param array                                 $args      array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string compiled code
     */
    public static function compileSpecialVariable(
        $args,
        Smarty_Internal_TemplateCompilerBase $compiler,
        $parameter = null
    ) {
        return '$_smarty_tpl->smarty->ext->_capture->getBuffer($_smarty_tpl' .
               (isset($parameter[ 1 ]) ? ", {$parameter[ 1 ]})" : ')');
    }

    /**
     * Compiles code for the {capture} tag
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     * @param null                                  $parameter
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter = null)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args, $parameter, 'capture');
        $buffer = isset($_attr[ 'name' ]) ? $_attr[ 'name' ] : "'default'";
        $assign = isset($_attr[ 'assign' ]) ? $_attr[ 'assign' ] : 'null';
        $append = isset($_attr[ 'append' ]) ? $_attr[ 'append' ] : 'null';
        $compiler->_cache[ 'capture_stack' ][] = array($compiler->nocache);
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        $_output = "<?php \$_smarty_tpl->smarty->ext->_capture->open(\$_smarty_tpl, $buffer, $assign, $append);?>";
        return $_output;
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Captureclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_CaptureClose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_CaptureClose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {/capture} tag
     *
     * @param array  $args     array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     * @param null                                  $parameter
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args, $parameter, '/capture');
>>>>>>> gosa-core_v2.8
        // must endblock be nocache?
        if ($compiler->nocache) {
            $compiler->tag_nocache = true;
        }
<<<<<<< HEAD

        list($buffer, $assign, $append, $compiler->nocache) = array_pop($compiler->_capture_stack[0]);

        $_output = "<?php list(\$_capture_buffer, \$_capture_assign, \$_capture_append) = array_pop(\$_smarty_tpl->_capture_stack[0]);\n";
        $_output .= "if (!empty(\$_capture_buffer)) {\n";
        $_output .= " if (isset(\$_capture_assign)) \$_smarty_tpl->assign(\$_capture_assign, ob_get_contents());\n";
        $_output .= " if (isset( \$_capture_append)) \$_smarty_tpl->append( \$_capture_append, ob_get_contents());\n";
        $_output .= " Smarty::\$_smarty_vars['capture'][\$_capture_buffer]=ob_get_clean();\n";
        $_output .= "} else \$_smarty_tpl->capture_error();?>";
        return $_output;
    }

}

?>
=======
        list($compiler->nocache) = array_pop($compiler->_cache[ 'capture_stack' ]);
        return "<?php \$_smarty_tpl->smarty->ext->_capture->close(\$_smarty_tpl);?>";
    }
}
>>>>>>> gosa-core_v2.8
