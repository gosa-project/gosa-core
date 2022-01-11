<?php
/**
 * Smarty Internal Plugin Compile Nocache
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {nocache} {/nocache} tags.
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
<<<<<<< HEAD
 * Smarty Internal Plugin Compile Nocache Classv
=======
 * Smarty Internal Plugin Compile Nocache Class
>>>>>>> gosa-core_v2.8
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Nocache extends Smarty_Internal_CompileBase {

    /**
     * Compiles code for the {nocache} tag
     *
     * This tag does not generate compiled output. It only sets a compiler flag.
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
     * @return bool
     */
    public function compile($args, $compiler)
    {
        $_attr = $this->getAttributes($compiler, $args);
        if ($_attr['nocache'] === true) {
            $compiler->trigger_template_error('nocache option not allowed', $compiler->lex->taglineno);
        }
=======
class Smarty_Internal_Compile_Nocache extends Smarty_Internal_CompileBase
{
    /**
     * Array of names of valid option flags
     *
     * @var array
     */
    public $option_flags = array();

    /**
     * Compiles code for the {nocache} tag
     * This tag does not generate compiled output. It only sets a compiler flag.
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return bool
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $_attr = $this->getAttributes($compiler, $args);
        $this->openTag($compiler, 'nocache', array($compiler->nocache));
>>>>>>> gosa-core_v2.8
        // enter nocache mode
        $compiler->nocache = true;
        // this tag does not return compiled code
        $compiler->has_code = false;
        return true;
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Nocacheclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Nocacheclose extends Smarty_Internal_CompileBase {

    /**
     * Compiles code for the {/nocache} tag
     *
     * This tag does not generate compiled output. It only sets a compiler flag.
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
     * @return bool
     */
    public function compile($args, $compiler)
    {
        $_attr = $this->getAttributes($compiler, $args);
        // leave nocache mode
        $compiler->nocache = false;
=======
class Smarty_Internal_Compile_Nocacheclose extends Smarty_Internal_CompileBase
{
    /**
     * Compiles code for the {/nocache} tag
     * This tag does not generate compiled output. It only sets a compiler flag.
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return bool
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $_attr = $this->getAttributes($compiler, $args);
        // leave nocache mode
        list($compiler->nocache) = $this->closeTag($compiler, array('nocache'));
>>>>>>> gosa-core_v2.8
        // this tag does not return compiled code
        $compiler->has_code = false;
        return true;
    }
<<<<<<< HEAD

}

?>
=======
}
>>>>>>> gosa-core_v2.8
