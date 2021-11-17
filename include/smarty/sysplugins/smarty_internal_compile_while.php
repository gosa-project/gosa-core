<?php
/**
 * Smarty Internal Plugin Compile While
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {while} tag
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile While Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_While extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_While extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {while} tag
     *
     * @param array  $args      array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $this->openTag($compiler, 'while', $compiler->nocache);

        if (!array_key_exists("if condition",$parameter)) {
            $compiler->trigger_template_error("missing while condition", $compiler->lex->taglineno);
        }

=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
        $compiler->loopNesting++;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $this->openTag($compiler, 'while', $compiler->nocache);
        if (!array_key_exists('if condition', $parameter)) {
            $compiler->trigger_template_error('missing while condition', null, true);
        }
>>>>>>> gosa-core_v2.8
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        if (is_array($parameter['if condition'])) {
            if ($compiler->nocache) {
<<<<<<< HEAD
                $_nocache = ',true';
                // create nocache var to make it know for further compiling
                if (is_array($parameter['if condition']['var'])) {
                    $compiler->template->tpl_vars[trim($parameter['if condition']['var']['var'], "'")] = new Smarty_variable(null, true);
                } else {
                    $compiler->template->tpl_vars[trim($parameter['if condition']['var'], "'")] = new Smarty_variable(null, true);
                }
            } else {
                $_nocache = '';
            }
            if (is_array($parameter['if condition']['var'])) {
                $_output = "<?php if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]) || !is_array(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value)) \$_smarty_tpl->createLocalArrayVariable(" . $parameter['if condition']['var']['var'] . "$_nocache);\n";
                $_output .= "while (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value" . $parameter['if condition']['var']['smarty_internal_index'] . " = " . $parameter['if condition']['value'] . "){?>";
            } else {
                $_output = "<?php if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "])) \$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "] = new Smarty_Variable(null{$_nocache});";
                $_output .= "while (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "]->value = " . $parameter['if condition']['value'] . "){?>";
            }
            return $_output;
        } else {
            return "<?php while ({$parameter['if condition']}){?>";
        }
    }

=======
                // create nocache var to make it know for further compiling
                if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                    $var = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
                } else {
                    $var = $parameter[ 'if condition' ][ 'var' ];
                }
                $compiler->setNocacheInVariable($var);
            }
            $prefixVar = $compiler->getNewPrefixVariable();
            $assignCompiler = new Smarty_Internal_Compile_Assign();
            $assignAttr = array();
            $assignAttr[][ 'value' ] = $prefixVar;
            if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
                $_output = "<?php while ({$prefixVar} = {$parameter[ 'if condition' ][ 'value' ]}) {?>";
                $_output .= $assignCompiler->compile(
                    $assignAttr,
                    $compiler,
                    array('smarty_internal_index' => $parameter[ 'if condition' ][ 'var' ][ 'smarty_internal_index' ])
                );
            } else {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ];
                $_output = "<?php while ({$prefixVar} = {$parameter[ 'if condition' ][ 'value' ]}) {?>";
                $_output .= $assignCompiler->compile($assignAttr, $compiler, array());
            }
            return $_output;
        } else {
            return "<?php\n while ({$parameter['if condition']}) {?>";
        }
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Whileclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Whileclose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Whileclose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {/while} tag
     *
     * @param array  $args     array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler)
    {
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $compiler->loopNesting--;
>>>>>>> gosa-core_v2.8
        // must endblock be nocache?
        if ($compiler->nocache) {
            $compiler->tag_nocache = true;
        }
        $compiler->nocache = $this->closeTag($compiler, array('while'));
<<<<<<< HEAD
        return "<?php }?>";
    }

}

?>
=======
        return "<?php }?>\n";
    }
}
>>>>>>> gosa-core_v2.8
