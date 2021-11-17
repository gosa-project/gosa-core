<?php
/**
* Smarty Internal Plugin Compile If
<<<<<<< HEAD
*
=======
>>>>>>> gosa-core_v2.8
* Compiles the {if} {else} {elseif} {/if} tags
*
* @package Smarty
* @subpackage Compiler
* @author Uwe Tews
*/

/**
* Smarty Internal Plugin Compile If Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_If extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_If extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * Compiles code for the {if} tag
    *
    * @param array  $args       array with attributes from parser
<<<<<<< HEAD
    * @param object $compiler   compiler object
    * @param array  $parameter  array with compilation parameter
    * @return string compiled code
    */
    public function compile($args, $compiler, $parameter)
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
>>>>>>> gosa-core_v2.8
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $this->openTag($compiler, 'if', array(1, $compiler->nocache));
        // must whole block be nocache ?
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
<<<<<<< HEAD

        if (!array_key_exists("if condition",$parameter)) {
            $compiler->trigger_template_error("missing if condition", $compiler->lex->taglineno);
        }

        if (is_array($parameter['if condition'])) {
            if ($compiler->nocache) {
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
                $_output = "<?php if (!isset(\$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']['var']."]) || !is_array(\$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']['var']."]->value)) \$_smarty_tpl->createLocalArrayVariable(".$parameter['if condition']['var']['var']."$_nocache);\n";
                $_output .= "if (\$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']['var']."]->value".$parameter['if condition']['var']['smarty_internal_index']." = ".$parameter['if condition']['value']."){?>";
            } else {
                $_output = "<?php if (!isset(\$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']."])) \$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']."] = new Smarty_Variable(null{$_nocache});";
                $_output .= "if (\$_smarty_tpl->tpl_vars[".$parameter['if condition']['var']."]->value = ".$parameter['if condition']['value']."){?>";
            }
=======
        if (!isset($parameter[ 'if condition' ])) {
            $compiler->trigger_template_error('missing if condition', null, true);
        }
        if (is_array($parameter[ 'if condition' ])) {
            if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                $var = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
            } else {
                $var = $parameter[ 'if condition' ][ 'var' ];
            }
            if ($compiler->nocache) {
                // create nocache var to make it know for further compiling
                $compiler->setNocacheInVariable($var);
            }
            $prefixVar = $compiler->getNewPrefixVariable();
            $_output = "<?php {$prefixVar} = {$parameter[ 'if condition' ][ 'value' ]};?>\n";
            $assignAttr = array();
            $assignAttr[][ 'value' ] = $prefixVar;
            $assignCompiler = new Smarty_Internal_Compile_Assign();
            if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
                $_output .= $assignCompiler->compile(
                    $assignAttr,
                    $compiler,
                    array('smarty_internal_index' => $parameter[ 'if condition' ][ 'var' ][ 'smarty_internal_index' ])
                );
            } else {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ];
                $_output .= $assignCompiler->compile($assignAttr, $compiler, array());
            }
            $_output .= "<?php if ({$prefixVar}) {?>";
>>>>>>> gosa-core_v2.8
            return $_output;
        } else {
            return "<?php if ({$parameter['if condition']}){?>";
        }
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
}

/**
* Smarty Internal Plugin Compile Else Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Else extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Else extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * Compiles code for the {else} tag
    *
    * @param array  $args       array with attributes from parser
<<<<<<< HEAD
    * @param object $compiler   compiler object
    * @param array  $parameter  array with compilation parameter
    * @return string compiled code
    */
    public function compile($args, $compiler, $parameter)
    {
        list($nesting, $compiler->tag_nocache) = $this->closeTag($compiler, array('if', 'elseif'));
        $this->openTag($compiler, 'else', array($nesting, $compiler->tag_nocache));

        return "<?php }else{ ?>";
    }

=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        list($nesting, $compiler->tag_nocache) = $this->closeTag($compiler, array('if', 'elseif'));
        $this->openTag($compiler, 'else', array($nesting, $compiler->tag_nocache));
        return '<?php } else { ?>';
    }
>>>>>>> gosa-core_v2.8
}

/**
* Smarty Internal Plugin Compile ElseIf Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Elseif extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Elseif extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * Compiles code for the {elseif} tag
    *
    * @param array  $args       array with attributes from parser
<<<<<<< HEAD
    * @param object $compiler   compiler object
    * @param array  $parameter  array with compilation parameter
    * @return string compiled code
    */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        list($nesting, $compiler->tag_nocache) = $this->closeTag($compiler, array('if', 'elseif'));

        if (!array_key_exists("if condition",$parameter)) {
            $compiler->trigger_template_error("missing elseif condition", $compiler->lex->taglineno);
        }

        if (is_array($parameter['if condition'])) {
            $condition_by_assign = true;
            if ($compiler->nocache) {
                $_nocache = ',true';
                // create nocache var to make it know for further compiling
                if (is_array($parameter['if condition']['var'])) {
                    $compiler->template->tpl_vars[trim($parameter['if condition']['var']['var'], "'")] = new Smarty_variable(null, true);
                } else {
                    $compiler->template->tpl_vars[trim($parameter['if condition']['var'], "'")] = new Smarty_variable(null, true);
                }
            } else {
                $_nocache = '';
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        list($nesting, $compiler->tag_nocache) = $this->closeTag($compiler, array('if', 'elseif'));
        if (!isset($parameter[ 'if condition' ])) {
            $compiler->trigger_template_error('missing elseif condition', null, true);
        }
        $assignCode = '';
        $var = '';
        if (is_array($parameter[ 'if condition' ])) {
            $condition_by_assign = true;
            if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                $var = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
            } else {
                $var = $parameter[ 'if condition' ][ 'var' ];
            }
            if ($compiler->nocache) {
                // create nocache var to make it know for further compiling
                $compiler->setNocacheInVariable($var);
            }
            $prefixVar = $compiler->getNewPrefixVariable();
            $assignCode = "<?php {$prefixVar} = {$parameter[ 'if condition' ][ 'value' ]};?>\n";
            $assignCompiler = new Smarty_Internal_Compile_Assign();
            $assignAttr = array();
            $assignAttr[][ 'value' ] = $prefixVar;
            if (is_array($parameter[ 'if condition' ][ 'var' ])) {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ][ 'var' ];
                $assignCode .= $assignCompiler->compile(
                    $assignAttr,
                    $compiler,
                    array('smarty_internal_index' => $parameter[ 'if condition' ][ 'var' ][ 'smarty_internal_index' ])
                );
            } else {
                $assignAttr[][ 'var' ] = $parameter[ 'if condition' ][ 'var' ];
                $assignCode .= $assignCompiler->compile($assignAttr, $compiler, array());
>>>>>>> gosa-core_v2.8
            }
        } else {
            $condition_by_assign = false;
        }
<<<<<<< HEAD

        if (empty($compiler->prefix_code)) {
            if ($condition_by_assign) {
                $this->openTag($compiler, 'elseif', array($nesting + 1, $compiler->tag_nocache));
                if (is_array($parameter['if condition']['var'])) {
                    $_output = "<?php }else{ if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]) || !is_array(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value)) \$_smarty_tpl->createLocalArrayVariable(" . $parameter['if condition']['var']['var'] . "$_nocache);\n";
                    $_output .= "if (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value" . $parameter['if condition']['var']['smarty_internal_index'] . " = " . $parameter['if condition']['value'] . "){?>";
                } else {
                    $_output = "<?php  }else{ if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "])) \$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "] = new Smarty_Variable(null{$_nocache});";
                    $_output .= "if (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "]->value = " . $parameter['if condition']['value'] . "){?>";
                }
                return $_output;
=======
        $prefixCode = $compiler->getPrefixCode();
        if (empty($prefixCode)) {
            if ($condition_by_assign) {
                $this->openTag($compiler, 'elseif', array($nesting + 1, $compiler->tag_nocache));
                $_output = $compiler->appendCode("<?php } else {\n?>", $assignCode);
                return $compiler->appendCode($_output, "<?php if ({$prefixVar}) {?>");
>>>>>>> gosa-core_v2.8
            } else {
                $this->openTag($compiler, 'elseif', array($nesting, $compiler->tag_nocache));
                return "<?php }elseif({$parameter['if condition']}){?>";
            }
        } else {
<<<<<<< HEAD
            $tmp = '';
            foreach ($compiler->prefix_code as $code)
            $tmp .= $code;
            $compiler->prefix_code = array();
            $this->openTag($compiler, 'elseif', array($nesting + 1, $compiler->tag_nocache));
            if ($condition_by_assign) {
                if (is_array($parameter['if condition']['var'])) {
                    $_output = "<?php }else{?>{$tmp}<?php  if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]) || !is_array(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value)) \$_smarty_tpl->createLocalArrayVariable(" . $parameter['if condition']['var']['var'] . "$_nocache);\n";
                    $_output .= "if (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var']['var'] . "]->value" . $parameter['if condition']['var']['smarty_internal_index'] . " = " . $parameter['if condition']['value'] . "){?>";
                } else {
                    $_output = "<?php }else{?>{$tmp}<?php if (!isset(\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "])) \$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "] = new Smarty_Variable(null{$_nocache});";
                    $_output .= "if (\$_smarty_tpl->tpl_vars[" . $parameter['if condition']['var'] . "]->value = " . $parameter['if condition']['value'] . "){?>";
                }
                return $_output;
            } else {
                return "<?php }else{?>{$tmp}<?php if ({$parameter['if condition']}){?>";
            }
        }
    }

=======
            $_output = $compiler->appendCode("<?php } else {\n?>", $prefixCode);
            $this->openTag($compiler, 'elseif', array($nesting + 1, $compiler->tag_nocache));
            if ($condition_by_assign) {
                $_output = $compiler->appendCode($_output, $assignCode);
                return $compiler->appendCode($_output, "<?php if ({$prefixVar}) {?>");
            } else {
                return $compiler->appendCode($_output, "<?php if ({$parameter['if condition']}) {?>");
            }
        }
    }
>>>>>>> gosa-core_v2.8
}

/**
* Smarty Internal Plugin Compile Ifclose Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Ifclose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Ifclose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * Compiles code for the {/if} tag
    *
    * @param array  $args       array with attributes from parser
<<<<<<< HEAD
    * @param object $compiler   compiler object
    * @param array  $parameter  array with compilation parameter
    * @return string compiled code
    */
    public function compile($args, $compiler, $parameter)
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
>>>>>>> gosa-core_v2.8
    {
        // must endblock be nocache?
        if ($compiler->nocache) {
            $compiler->tag_nocache = true;
        }
        list($nesting, $compiler->nocache) = $this->closeTag($compiler, array('if', 'else', 'elseif'));
        $tmp = '';
        for ($i = 0; $i < $nesting; $i++) {
            $tmp .= '}';
        }
        return "<?php {$tmp}?>";
    }
<<<<<<< HEAD

}

?>
=======
}
>>>>>>> gosa-core_v2.8
