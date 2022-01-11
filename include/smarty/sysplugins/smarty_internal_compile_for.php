<?php
/**
 * Smarty Internal Plugin Compile For
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {for} {forelse} {/for} tags
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile For Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_For extends Smarty_Internal_CompileBase {

    /**
     * Compiles code for the {for} tag
     *
     * Smarty 3 does implement two different sytaxes:
     *
     * - {for $var in $array}
     * For looping over arrays or iterators
     *
     * - {for $x=0; $x<$y; $x++}
     * For general loops
     *
     * The parser is gereration different sets of attribute by which this compiler can
     * determin which syntax is used.
=======
class Smarty_Internal_Compile_For extends Smarty_Internal_CompileBase
{
    /**
     * Compiles code for the {for} tag
     * Smarty 3 does implement two different syntax's:
     * - {for $var in $array}
     * For looping over arrays or iterators
     * - {for $x=0; $x<$y; $x++}
     * For general loops
     * The parser is generating different sets of attribute by which this compiler can
     * determine which syntax is used.
>>>>>>> gosa-core_v2.8
     *
     * @param array  $args      array with attributes from parser
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
<<<<<<< HEAD
        if ($parameter == 0) {
=======
        $compiler->loopNesting++;
        if ($parameter === 0) {
>>>>>>> gosa-core_v2.8
            $this->required_attributes = array('start', 'to');
            $this->optional_attributes = array('max', 'step');
        } else {
            $this->required_attributes = array('start', 'ifexp', 'var', 'step');
            $this->optional_attributes = array();
        }
<<<<<<< HEAD
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        $output = "<?php ";
        if ($parameter == 1) {
            foreach ($_attr['start'] as $_statement) {
                $output .= " \$_smarty_tpl->tpl_vars[$_statement[var]] = new Smarty_Variable;";
                $output .= " \$_smarty_tpl->tpl_vars[$_statement[var]]->value = $_statement[value];\n";
            }
            $output .= "  if ($_attr[ifexp]){ for (\$_foo=true;$_attr[ifexp]; \$_smarty_tpl->tpl_vars[$_attr[var]]->value$_attr[step]){\n";
        } else {
            $_statement = $_attr['start'];
            $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]] = new Smarty_Variable;";
            if (isset($_attr['step'])) {
                $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->step = $_attr[step];";
            } else {
                $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->step = 1;";
            }
            if (isset($_attr['max'])) {
                $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->total = (int)min(ceil((\$_smarty_tpl->tpl_vars[$_statement[var]]->step > 0 ? $_attr[to]+1 - ($_statement[value]) : $_statement[value]-($_attr[to])+1)/abs(\$_smarty_tpl->tpl_vars[$_statement[var]]->step)),$_attr[max]);\n";
            } else {
                $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->total = (int)ceil((\$_smarty_tpl->tpl_vars[$_statement[var]]->step > 0 ? $_attr[to]+1 - ($_statement[value]) : $_statement[value]-($_attr[to])+1)/abs(\$_smarty_tpl->tpl_vars[$_statement[var]]->step));\n";
            }
            $output .= "if (\$_smarty_tpl->tpl_vars[$_statement[var]]->total > 0){\n";
            $output .= "for (\$_smarty_tpl->tpl_vars[$_statement[var]]->value = $_statement[value], \$_smarty_tpl->tpl_vars[$_statement[var]]->iteration = 1;\$_smarty_tpl->tpl_vars[$_statement[var]]->iteration <= \$_smarty_tpl->tpl_vars[$_statement[var]]->total;\$_smarty_tpl->tpl_vars[$_statement[var]]->value += \$_smarty_tpl->tpl_vars[$_statement[var]]->step, \$_smarty_tpl->tpl_vars[$_statement[var]]->iteration++){\n";
            $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->first = \$_smarty_tpl->tpl_vars[$_statement[var]]->iteration == 1;";
            $output .= "\$_smarty_tpl->tpl_vars[$_statement[var]]->last = \$_smarty_tpl->tpl_vars[$_statement[var]]->iteration == \$_smarty_tpl->tpl_vars[$_statement[var]]->total;";
        }
        $output .= "?>";

=======
        $this->mapCache = array();
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $output = "<?php\n";
        if ($parameter === 1) {
            foreach ($_attr[ 'start' ] as $_statement) {
                if (is_array($_statement[ 'var' ])) {
                    $var = $_statement[ 'var' ][ 'var' ];
                    $index = $_statement[ 'var' ][ 'smarty_internal_index' ];
                } else {
                    $var = $_statement[ 'var' ];
                    $index = '';
                }
                $output .= "\$_smarty_tpl->tpl_vars[$var] = new Smarty_Variable(null, \$_smarty_tpl->isRenderingCache);\n";
                $output .= "\$_smarty_tpl->tpl_vars[$var]->value{$index} = {$_statement['value']};\n";
            }
            if (is_array($_attr[ 'var' ])) {
                $var = $_attr[ 'var' ][ 'var' ];
                $index = $_attr[ 'var' ][ 'smarty_internal_index' ];
            } else {
                $var = $_attr[ 'var' ];
                $index = '';
            }
            $output .= "if ($_attr[ifexp]) {\nfor (\$_foo=true;$_attr[ifexp]; \$_smarty_tpl->tpl_vars[$var]->value{$index}$_attr[step]) {\n";
        } else {
            $_statement = $_attr[ 'start' ];
            if (is_array($_statement[ 'var' ])) {
                $var = $_statement[ 'var' ][ 'var' ];
                $index = $_statement[ 'var' ][ 'smarty_internal_index' ];
            } else {
                $var = $_statement[ 'var' ];
                $index = '';
            }
            $output .= "\$_smarty_tpl->tpl_vars[$var] = new Smarty_Variable(null, \$_smarty_tpl->isRenderingCache);";
            if (isset($_attr[ 'step' ])) {
                $output .= "\$_smarty_tpl->tpl_vars[$var]->step = $_attr[step];";
            } else {
                $output .= "\$_smarty_tpl->tpl_vars[$var]->step = 1;";
            }
            if (isset($_attr[ 'max' ])) {
                $output .= "\$_smarty_tpl->tpl_vars[$var]->total = (int) min(ceil((\$_smarty_tpl->tpl_vars[$var]->step > 0 ? $_attr[to]+1 - ($_statement[value]) : $_statement[value]-($_attr[to])+1)/abs(\$_smarty_tpl->tpl_vars[$var]->step)),$_attr[max]);\n";
            } else {
                $output .= "\$_smarty_tpl->tpl_vars[$var]->total = (int) ceil((\$_smarty_tpl->tpl_vars[$var]->step > 0 ? $_attr[to]+1 - ($_statement[value]) : $_statement[value]-($_attr[to])+1)/abs(\$_smarty_tpl->tpl_vars[$var]->step));\n";
            }
            $output .= "if (\$_smarty_tpl->tpl_vars[$var]->total > 0) {\n";
            $output .= "for (\$_smarty_tpl->tpl_vars[$var]->value{$index} = $_statement[value], \$_smarty_tpl->tpl_vars[$var]->iteration = 1;\$_smarty_tpl->tpl_vars[$var]->iteration <= \$_smarty_tpl->tpl_vars[$var]->total;\$_smarty_tpl->tpl_vars[$var]->value{$index} += \$_smarty_tpl->tpl_vars[$var]->step, \$_smarty_tpl->tpl_vars[$var]->iteration++) {\n";
            $output .= "\$_smarty_tpl->tpl_vars[$var]->first = \$_smarty_tpl->tpl_vars[$var]->iteration === 1;";
            $output .= "\$_smarty_tpl->tpl_vars[$var]->last = \$_smarty_tpl->tpl_vars[$var]->iteration === \$_smarty_tpl->tpl_vars[$var]->total;";
        }
        $output .= '?>';
>>>>>>> gosa-core_v2.8
        $this->openTag($compiler, 'for', array('for', $compiler->nocache));
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        // return compiled code
        return $output;
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Forelse Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Forelse extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Forelse extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {forelse} tag
     *
     * @param array  $args      array with attributes from parser
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr  = $this->getAttributes($compiler, $args);
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
        list($openTag, $nocache) = $this->closeTag($compiler, array('for'));
        $this->openTag($compiler, 'forelse', array('forelse', $nocache));
        return "<?php }} else { ?>";
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Forclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Forclose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Forclose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {/for} tag
     *
     * @param array  $args      array with attributes from parser
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
<<<<<<< HEAD
=======
        $compiler->loopNesting--;
>>>>>>> gosa-core_v2.8
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        // must endblock be nocache?
        if ($compiler->nocache) {
            $compiler->tag_nocache = true;
        }
<<<<<<< HEAD

        list($openTag, $compiler->nocache) = $this->closeTag($compiler, array('for', 'forelse'));

        if ($openTag == 'forelse') {
            return "<?php }  ?>";
        } else {
            return "<?php }} ?>";
        }
    }

}

?>
=======
        list($openTag, $compiler->nocache) = $this->closeTag($compiler, array('for', 'forelse'));
        $output = "<?php }\n";
        if ($openTag !== 'forelse') {
            $output .= "}\n";
        }
        $output .= "?>";
        return $output;
    }
}
>>>>>>> gosa-core_v2.8
