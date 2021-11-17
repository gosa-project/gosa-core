<?php
/**
 * Smarty Internal Plugin Compile Foreach
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {foreach} {foreachelse} {/foreach} tags
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Foreach Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Foreach extends Smarty_Internal_CompileBase {
=======
class Smarty_Internal_Compile_Foreach extends Smarty_Internal_Compile_Private_ForeachSection
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array('from', 'item');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
<<<<<<< HEAD
    public $optional_attributes = array('name', 'key');
=======
    public $optional_attributes = array('name', 'key', 'properties');

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('from','item','key','name');

    /**
<<<<<<< HEAD
     * Compiles code for the {foreach} tag
     *
     * @param array  $args      array with attributes from parser
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        $tpl = $compiler->template;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        $from = $_attr['from'];
        $item = $_attr['item'];
        if (!strncmp("\$_smarty_tpl->tpl_vars[$item]", $from, strlen($item) + 24)) {
            $compiler->trigger_template_error("item variable {$item} may not be the same variable as at 'from'", $compiler->lex->taglineno);
        }

        if (isset($_attr['key'])) {
            $key = $_attr['key'];
        } else {
            $key = null;
        }

        $this->openTag($compiler, 'foreach', array('foreach', $compiler->nocache, $item, $key));
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;

        if (isset($_attr['name'])) {
            $name = $_attr['name'];
            $has_name = true;
            $SmartyVarName = '$smarty.foreach.' . trim($name, '\'"') . '.';
        } else {
            $name = null;
            $has_name = false;
        }
        $ItemVarName = '$' . trim($item, '\'"') . '@';
        // evaluates which Smarty variables and properties have to be computed
        if ($has_name) {
            $usesSmartyFirst = strpos($tpl->source->content, $SmartyVarName . 'first') !== false;
            $usesSmartyLast = strpos($tpl->source->content, $SmartyVarName . 'last') !== false;
            $usesSmartyIndex = strpos($tpl->source->content, $SmartyVarName . 'index') !== false;
            $usesSmartyIteration = strpos($tpl->source->content, $SmartyVarName . 'iteration') !== false;
            $usesSmartyShow = strpos($tpl->source->content, $SmartyVarName . 'show') !== false;
            $usesSmartyTotal = strpos($tpl->source->content, $SmartyVarName . 'total') !== false;
        } else {
            $usesSmartyFirst = false;
            $usesSmartyLast = false;
            $usesSmartyTotal = false;
            $usesSmartyShow = false;
        }

        $usesPropFirst = $usesSmartyFirst || strpos($tpl->source->content, $ItemVarName . 'first') !== false;
        $usesPropLast = $usesSmartyLast || strpos($tpl->source->content, $ItemVarName . 'last') !== false;
        $usesPropIndex = $usesPropFirst || strpos($tpl->source->content, $ItemVarName . 'index') !== false;
        $usesPropIteration = $usesPropLast || strpos($tpl->source->content, $ItemVarName . 'iteration') !== false;
        $usesPropShow = strpos($tpl->source->content, $ItemVarName . 'show') !== false;
        $usesPropTotal = $usesSmartyTotal || $usesSmartyShow || $usesPropShow || $usesPropLast || strpos($tpl->source->content, $ItemVarName . 'total') !== false;
        // generate output code
        $output = "<?php ";
        $output .= " \$_smarty_tpl->tpl_vars[$item] = new Smarty_Variable; \$_smarty_tpl->tpl_vars[$item]->_loop = false;\n";
        if ($key != null) {
            $output .= " \$_smarty_tpl->tpl_vars[$key] = new Smarty_Variable;\n";
        }
        $output .= " \$_from = $from; if (!is_array(\$_from) && !is_object(\$_from)) { settype(\$_from, 'array');}\n";
        if ($usesPropTotal) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->total= \$_smarty_tpl->_count(\$_from);\n";
        }
        if ($usesPropIteration) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->iteration=0;\n";
        }
        if ($usesPropIndex) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->index=-1;\n";
        }
        if ($usesPropShow) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->show = (\$_smarty_tpl->tpl_vars[$item]->total > 0);\n";
        }
        if ($has_name) {
            if ($usesSmartyTotal) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['total'] = \$_smarty_tpl->tpl_vars[$item]->total;\n";
            }
            if ($usesSmartyIteration) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['iteration']=0;\n";
            }
            if ($usesSmartyIndex) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['index']=-1;\n";
            }
            if ($usesSmartyShow) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['show']=(\$_smarty_tpl->tpl_vars[$item]->total > 0);\n";
            }
        }
        $output .= "foreach (\$_from as \$_smarty_tpl->tpl_vars[$item]->key => \$_smarty_tpl->tpl_vars[$item]->value){\n\$_smarty_tpl->tpl_vars[$item]->_loop = true;\n";
        if ($key != null) {
            $output .= " \$_smarty_tpl->tpl_vars[$key]->value = \$_smarty_tpl->tpl_vars[$item]->key;\n";
        }
        if ($usesPropIteration) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->iteration++;\n";
        }
        if ($usesPropIndex) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->index++;\n";
        }
        if ($usesPropFirst) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->first = \$_smarty_tpl->tpl_vars[$item]->index === 0;\n";
        }
        if ($usesPropLast) {
            $output .= " \$_smarty_tpl->tpl_vars[$item]->last = \$_smarty_tpl->tpl_vars[$item]->iteration === \$_smarty_tpl->tpl_vars[$item]->total;\n";
        }
        if ($has_name) {
            if ($usesSmartyFirst) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['first'] = \$_smarty_tpl->tpl_vars[$item]->first;\n";
            }
            if ($usesSmartyIteration) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['iteration']++;\n";
            }
            if ($usesSmartyIndex) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['index']++;\n";
            }
            if ($usesSmartyLast) {
                $output .= " \$_smarty_tpl->tpl_vars['smarty']->value['foreach'][$name]['last'] = \$_smarty_tpl->tpl_vars[$item]->last;\n";
            }
        }
        $output .= "?>";

        return $output;
    }
=======
     * counter
     *
     * @var int
     */
    public $counter = 0;

    /**
     * Name of this tag
     *
     * @var string
     */
    public $tagName = 'foreach';

    /**
     * Valid properties of $smarty.foreach.name.xxx variable
     *
     * @var array
     */
    public $nameProperties = array('first', 'last', 'index', 'iteration', 'show', 'total');

    /**
     * Valid properties of $item@xxx variable
     *
     * @var array
     */
    public $itemProperties = array('first', 'last', 'index', 'iteration', 'show', 'total', 'key');

    /**
     * Flag if tag had name attribute
     *
     * @var bool
     */
    public $isNamed = false;

    /**
     * Compiles code for the {foreach} tag
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $compiler->loopNesting++;
        // init
        $this->isNamed = false;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $from = $_attr[ 'from' ];
        $item = $compiler->getId($_attr[ 'item' ]);
        if ($item === false) {
            $item = $compiler->getVariableName($_attr[ 'item' ]);
        }
        $key = $name = null;
        $attributes = array('item' => $item);
        if (isset($_attr[ 'key' ])) {
            $key = $compiler->getId($_attr[ 'key' ]);
            if ($key === false) {
                $key = $compiler->getVariableName($_attr[ 'key' ]);
            }
            $attributes[ 'key' ] = $key;
        }
        if (isset($_attr[ 'name' ])) {
            $this->isNamed = true;
            $name = $attributes[ 'name' ] = $compiler->getId($_attr[ 'name' ]);
        }
        foreach ($attributes as $a => $v) {
            if ($v === false) {
                $compiler->trigger_template_error("'{$a}' attribute/variable has illegal value", null, true);
            }
        }
        $fromName = $compiler->getVariableName($_attr[ 'from' ]);
        if ($fromName) {
            foreach (array('item', 'key') as $a) {
                if (isset($attributes[ $a ]) && $attributes[ $a ] === $fromName) {
                    $compiler->trigger_template_error(
                        "'{$a}' and 'from' may not have same variable name '{$fromName}'",
                        null,
                        true
                    );
                }
            }
        }
        $itemVar = "\$_smarty_tpl->tpl_vars['{$item}']";
        $local = '$__foreach_' . $attributes[ 'item' ] . '_' . $this->counter++ . '_';
        // search for used tag attributes
        $itemAttr = array();
        $namedAttr = array();
        $this->scanForProperties($attributes, $compiler);
        if (!empty($this->matchResults[ 'item' ])) {
            $itemAttr = $this->matchResults[ 'item' ];
        }
        if (!empty($this->matchResults[ 'named' ])) {
            $namedAttr = $this->matchResults[ 'named' ];
        }
        if (isset($_attr[ 'properties' ]) && preg_match_all('/[\'](.*?)[\']/', $_attr[ 'properties' ], $match)) {
            foreach ($match[ 1 ] as $prop) {
                if (in_array($prop, $this->itemProperties)) {
                    $itemAttr[ $prop ] = true;
                } else {
                    $compiler->trigger_template_error("Invalid property '{$prop}'", null, true);
                }
            }
            if ($this->isNamed) {
                foreach ($match[ 1 ] as $prop) {
                    if (in_array($prop, $this->nameProperties)) {
                        $nameAttr[ $prop ] = true;
                    } else {
                        $compiler->trigger_template_error("Invalid property '{$prop}'", null, true);
                    }
                }
            }
        }
        if (isset($itemAttr[ 'first' ])) {
            $itemAttr[ 'index' ] = true;
        }
        if (isset($namedAttr[ 'first' ])) {
            $namedAttr[ 'index' ] = true;
        }
        if (isset($namedAttr[ 'last' ])) {
            $namedAttr[ 'iteration' ] = true;
            $namedAttr[ 'total' ] = true;
        }
        if (isset($itemAttr[ 'last' ])) {
            $itemAttr[ 'iteration' ] = true;
            $itemAttr[ 'total' ] = true;
        }
        if (isset($namedAttr[ 'show' ])) {
            $namedAttr[ 'total' ] = true;
        }
        if (isset($itemAttr[ 'show' ])) {
            $itemAttr[ 'total' ] = true;
        }
        $keyTerm = '';
        if (isset($attributes[ 'key' ])) {
            $keyTerm = "\$_smarty_tpl->tpl_vars['{$key}']->value => ";
        }
        if (isset($itemAttr[ 'key' ])) {
            $keyTerm = "{$itemVar}->key => ";
        }
        if ($this->isNamed) {
            $foreachVar = "\$_smarty_tpl->tpl_vars['__smarty_foreach_{$attributes['name']}']";
        }
        $needTotal = isset($itemAttr[ 'total' ]);
        // Register tag
        $this->openTag(
            $compiler,
            'foreach',
            array('foreach', $compiler->nocache, $local, $itemVar, empty($itemAttr) ? 1 : 2)
        );
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        // generate output code
        $output = "<?php\n";
        $output .= "\$_from = \$_smarty_tpl->smarty->ext->_foreach->init(\$_smarty_tpl, $from, " .
                   var_export($item, true);
        if ($name || $needTotal || $key) {
            $output .= ', ' . var_export($needTotal, true);
        }
        if ($name || $key) {
            $output .= ', ' . var_export($key, true);
        }
        if ($name) {
            $output .= ', ' . var_export($name, true) . ', ' . var_export($namedAttr, true);
        }
        $output .= ");\n";
        if (isset($itemAttr[ 'show' ])) {
            $output .= "{$itemVar}->show = ({$itemVar}->total > 0);\n";
        }
        if (isset($itemAttr[ 'iteration' ])) {
            $output .= "{$itemVar}->iteration = 0;\n";
        }
        if (isset($itemAttr[ 'index' ])) {
            $output .= "{$itemVar}->index = -1;\n";
        }
	    $output .= "{$itemVar}->do_else = true;\n";
        $output .= "if (\$_from !== null) foreach (\$_from as {$keyTerm}{$itemVar}->value) {\n";
	    $output .= "{$itemVar}->do_else = false;\n";
        if (isset($attributes[ 'key' ]) && isset($itemAttr[ 'key' ])) {
            $output .= "\$_smarty_tpl->tpl_vars['{$key}']->value = {$itemVar}->key;\n";
        }
        if (isset($itemAttr[ 'iteration' ])) {
            $output .= "{$itemVar}->iteration++;\n";
        }
        if (isset($itemAttr[ 'index' ])) {
            $output .= "{$itemVar}->index++;\n";
        }
        if (isset($itemAttr[ 'first' ])) {
            $output .= "{$itemVar}->first = !{$itemVar}->index;\n";
        }
        if (isset($itemAttr[ 'last' ])) {
            $output .= "{$itemVar}->last = {$itemVar}->iteration === {$itemVar}->total;\n";
        }
        if (isset($foreachVar)) {
            if (isset($namedAttr[ 'iteration' ])) {
                $output .= "{$foreachVar}->value['iteration']++;\n";
            }
            if (isset($namedAttr[ 'index' ])) {
                $output .= "{$foreachVar}->value['index']++;\n";
            }
            if (isset($namedAttr[ 'first' ])) {
                $output .= "{$foreachVar}->value['first'] = !{$foreachVar}->value['index'];\n";
            }
            if (isset($namedAttr[ 'last' ])) {
                $output .= "{$foreachVar}->value['last'] = {$foreachVar}->value['iteration'] === {$foreachVar}->value['total'];\n";
            }
        }
        if (!empty($itemAttr)) {
            $output .= "{$local}saved = {$itemVar};\n";
        }
        $output .= '?>';
        return $output;
    }

    /**
     * Compiles code for to restore saved template variables
     *
     * @param int $levels number of levels to restore
     *
     * @return string compiled code
     */
    public function compileRestore($levels)
    {
        return "\$_smarty_tpl->smarty->ext->_foreach->restore(\$_smarty_tpl, {$levels});";
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Foreachelse Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Foreachelse extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Foreachelse extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {foreachelse} tag
     *
     * @param array  $args array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler compiler object
     * @param array  $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        list($openTag, $nocache, $item, $key) = $this->closeTag($compiler, array('foreach'));
        $this->openTag($compiler, 'foreachelse', array('foreachelse', $nocache, $item, $key));

        return "<?php }\nif (!\$_smarty_tpl->tpl_vars[$item]->_loop) {\n?>";
    }

=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        list($openTag, $nocache, $local, $itemVar, $restore) = $this->closeTag($compiler, array('foreach'));
        $this->openTag($compiler, 'foreachelse', array('foreachelse', $nocache, $local, $itemVar, 0));
        $output = "<?php\n";
        if ($restore === 2) {
            $output .= "{$itemVar} = {$local}saved;\n";
        }
        $output .= "}\nif ({$itemVar}->do_else) {\n?>";
        return $output;
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Foreachclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Foreachclose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Foreachclose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {/foreach} tag
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
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $compiler->loopNesting--;
>>>>>>> gosa-core_v2.8
        // must endblock be nocache?
        if ($compiler->nocache) {
            $compiler->tag_nocache = true;
        }
<<<<<<< HEAD

        list($openTag, $compiler->nocache, $item, $key) = $this->closeTag($compiler, array('foreach', 'foreachelse'));

        return "<?php } ?>";
    }

}

?>
=======
        list(
            $openTag, $compiler->nocache, $local, $itemVar, $restore
            ) = $this->closeTag($compiler, array('foreach', 'foreachelse'));
        $output = "<?php\n";
        if ($restore === 2) {
            $output .= "{$itemVar} = {$local}saved;\n";
        }
        $output .= "}\n";
        /* @var Smarty_Internal_Compile_Foreach $foreachCompiler */
        $foreachCompiler = $compiler->getTagCompiler('foreach');
        $output .= $foreachCompiler->compileRestore(1);
        $output .= "?>";
        return $output;
    }
}
>>>>>>> gosa-core_v2.8
