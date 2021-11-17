<?php
/**
 * Smarty Internal Plugin Compile Section
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {section} {sectionelse} {/section} tags
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Section Class
 * 
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Section extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Section extends Smarty_Internal_Compile_Private_ForeachSection
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array('name', 'loop');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('name', 'loop');
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
    public $optional_attributes = array('start', 'step', 'max', 'show');
=======
    public $optional_attributes = array('start', 'step', 'max', 'show', 'properties');

    /**
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
    public $tagName = 'section';

    /**
     * Valid properties of $smarty.section.name.xxx variable
     *
     * @var array
     */
    public $nameProperties = array(
        'first', 'last', 'index', 'iteration', 'show', 'total', 'rownum', 'index_prev',
        'index_next', 'loop'
    );

    /**
     * {section} tag has no item properties
     *
     * @var array
     */
    public $itemProperties = null;

    /**
     * {section} tag has always name attribute
     *
     * @var bool
     */
    public $isNamed = true;
>>>>>>> gosa-core_v2.8

    /**
     * Compiles code for the {section} tag
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

        $this->openTag($compiler, 'section', array('section', $compiler->nocache));
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;

        $output = "<?php ";

        $section_name = $_attr['name'];

        $output .= "if (isset(\$_smarty_tpl->tpl_vars['smarty']->value['section'][$section_name])) unset(\$_smarty_tpl->tpl_vars['smarty']->value['section'][$section_name]);\n";
        $section_props = "\$_smarty_tpl->tpl_vars['smarty']->value['section'][$section_name]";

        foreach ($_attr as $attr_name => $attr_value) {
            switch ($attr_name) {
                case 'loop':
                    $output .= "{$section_props}['loop'] = is_array(\$_loop=$attr_value) ? count(\$_loop) : max(0, (int)\$_loop); unset(\$_loop);\n";
                    break;

                case 'show':
                    if (is_bool($attr_value))
                        $show_attr_value = $attr_value ? 'true' : 'false';
                    else
                        $show_attr_value = "(bool)$attr_value";
                    $output .= "{$section_props}['show'] = $show_attr_value;\n";
                    break;

                case 'name':
                    $output .= "{$section_props}['$attr_name'] = $attr_value;\n";
                    break;

                case 'max':
                case 'start':
                    $output .= "{$section_props}['$attr_name'] = (int)$attr_value;\n";
                    break;

                case 'step':
                    $output .= "{$section_props}['$attr_name'] = ((int)$attr_value) == 0 ? 1 : (int)$attr_value;\n";
                    break;
            }
        }

        if (!isset($_attr['show']))
            $output .= "{$section_props}['show'] = true;\n";

        if (!isset($_attr['loop']))
            $output .= "{$section_props}['loop'] = 1;\n";

        if (!isset($_attr['max']))
            $output .= "{$section_props}['max'] = {$section_props}['loop'];\n";
        else
            $output .= "if ({$section_props}['max'] < 0)\n" . "    {$section_props}['max'] = {$section_props}['loop'];\n";

        if (!isset($_attr['step']))
            $output .= "{$section_props}['step'] = 1;\n";

        if (!isset($_attr['start']))
            $output .= "{$section_props}['start'] = {$section_props}['step'] > 0 ? 0 : {$section_props}['loop']-1;\n";
        else {
            $output .= "if ({$section_props}['start'] < 0)\n" . "    {$section_props}['start'] = max({$section_props}['step'] > 0 ? 0 : -1, {$section_props}['loop'] + {$section_props}['start']);\n" . "else\n" . "    {$section_props}['start'] = min({$section_props}['start'], {$section_props}['step'] > 0 ? {$section_props}['loop'] : {$section_props}['loop']-1);\n";
        }

        $output .= "if ({$section_props}['show']) {\n";
        if (!isset($_attr['start']) && !isset($_attr['step']) && !isset($_attr['max'])) {
            $output .= "    {$section_props}['total'] = {$section_props}['loop'];\n";
        } else {
            $output .= "    {$section_props}['total'] = min(ceil(({$section_props}['step'] > 0 ? {$section_props}['loop'] - {$section_props}['start'] : {$section_props}['start']+1)/abs({$section_props}['step'])), {$section_props}['max']);\n";
        }
        $output .= "    if ({$section_props}['total'] == 0)\n" . "        {$section_props}['show'] = false;\n" . "} else\n" . "    {$section_props}['total'] = 0;\n";

        $output .= "if ({$section_props}['show']):\n";
        $output .= "
            for ({$section_props}['index'] = {$section_props}['start'], {$section_props}['iteration'] = 1;
                 {$section_props}['iteration'] <= {$section_props}['total'];
                 {$section_props}['index'] += {$section_props}['step'], {$section_props}['iteration']++):\n";
        $output .= "{$section_props}['rownum'] = {$section_props}['iteration'];\n";
        $output .= "{$section_props}['index_prev'] = {$section_props}['index'] - {$section_props}['step'];\n";
        $output .= "{$section_props}['index_next'] = {$section_props}['index'] + {$section_props}['step'];\n";
        $output .= "{$section_props}['first']      = ({$section_props}['iteration'] == 1);\n";
        $output .= "{$section_props}['last']       = ({$section_props}['iteration'] == {$section_props}['total']);\n";

        $output .= "?>";
        return $output;
    }

=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $compiler->loopNesting++;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $attributes = array('name' => $compiler->getId($_attr[ 'name' ]));
        unset($_attr[ 'name' ]);
        foreach ($attributes as $a => $v) {
            if ($v === false) {
                $compiler->trigger_template_error("'{$a}' attribute/variable has illegal value", null, true);
            }
        }
        $local = "\$__section_{$attributes['name']}_" . $this->counter++ . '_';
        $sectionVar = "\$_smarty_tpl->tpl_vars['__smarty_section_{$attributes['name']}']";
        $this->openTag($compiler, 'section', array('section', $compiler->nocache, $local, $sectionVar));
        // maybe nocache because of nocache variables
        $compiler->nocache = $compiler->nocache | $compiler->tag_nocache;
        $initLocal = array();
        $initNamedProperty = array();
        $initFor = array();
        $incFor = array();
        $cmpFor = array();
        $propValue = array(
            'index'     => "{$sectionVar}->value['index']", 'show' => 'true', 'step' => 1,
            'iteration' => "{$local}iteration",
        );
        $propType = array('index' => 2, 'iteration' => 2, 'show' => 0, 'step' => 0,);
        // search for used tag attributes
        $this->scanForProperties($attributes, $compiler);
        if (!empty($this->matchResults[ 'named' ])) {
            $namedAttr = $this->matchResults[ 'named' ];
        }
        if (isset($_attr[ 'properties' ]) && preg_match_all("/['](.*?)[']/", $_attr[ 'properties' ], $match)) {
            foreach ($match[ 1 ] as $prop) {
                if (in_array($prop, $this->nameProperties)) {
                    $namedAttr[ $prop ] = true;
                } else {
                    $compiler->trigger_template_error("Invalid property '{$prop}'", null, true);
                }
            }
        }
        $namedAttr[ 'index' ] = true;
        $output = "<?php\n";
        foreach ($_attr as $attr_name => $attr_value) {
            switch ($attr_name) {
                case 'loop':
                    if (is_numeric($attr_value)) {
                        $v = (int)$attr_value;
                        $t = 0;
                    } else {
                        $v = "(is_array(@\$_loop=$attr_value) ? count(\$_loop) : max(0, (int) \$_loop))";
                        $t = 1;
                    }
                    if ($t === 1) {
                        $initLocal[ 'loop' ] = $v;
                        $v = "{$local}loop";
                    }
                    break;
                case 'show':
                    if (is_bool($attr_value)) {
                        $v = $attr_value ? 'true' : 'false';
                        $t = 0;
                    } else {
                        $v = "(bool) $attr_value";
                        $t = 3;
                    }
                    break;
                case 'step':
                    if (is_numeric($attr_value)) {
                        $v = (int)$attr_value;
                        $v = ($v === 0) ? 1 : $v;
                        $t = 0;
                        break;
                    }
                    $initLocal[ 'step' ] = "((int)@$attr_value) === 0 ? 1 : (int)@$attr_value";
                    $v = "{$local}step";
                    $t = 2;
                    break;
                case 'max':
                case 'start':
                    if (is_numeric($attr_value)) {
                        $v = (int)$attr_value;
                        $t = 0;
                        break;
                    }
                    $v = "(int)@$attr_value";
                    $t = 3;
                    break;
            }
            if ($t === 3 && $compiler->getId($attr_value)) {
                $t = 1;
            }
            $propValue[ $attr_name ] = $v;
            $propType[ $attr_name ] = $t;
        }
        if (isset($namedAttr[ 'step' ])) {
            $initNamedProperty[ 'step' ] = $propValue[ 'step' ];
        }
        if (isset($namedAttr[ 'iteration' ])) {
            $propValue[ 'iteration' ] = "{$sectionVar}->value['iteration']";
        }
        $incFor[ 'iteration' ] = "{$propValue['iteration']}++";
        $initFor[ 'iteration' ] = "{$propValue['iteration']} = 1";
        if ($propType[ 'step' ] === 0) {
            if ($propValue[ 'step' ] === 1) {
                $incFor[ 'index' ] = "{$sectionVar}->value['index']++";
            } elseif ($propValue[ 'step' ] > 1) {
                $incFor[ 'index' ] = "{$sectionVar}->value['index'] += {$propValue['step']}";
            } else {
                $incFor[ 'index' ] = "{$sectionVar}->value['index'] -= " . -$propValue[ 'step' ];
            }
        } else {
            $incFor[ 'index' ] = "{$sectionVar}->value['index'] += {$propValue['step']}";
        }
        if (!isset($propValue[ 'max' ])) {
            $propValue[ 'max' ] = $propValue[ 'loop' ];
            $propType[ 'max' ] = $propType[ 'loop' ];
        } elseif ($propType[ 'max' ] !== 0) {
            $propValue[ 'max' ] = "{$propValue['max']} < 0 ? {$propValue['loop']} : {$propValue['max']}";
            $propType[ 'max' ] = 1;
        } else {
            if ($propValue[ 'max' ] < 0) {
                $propValue[ 'max' ] = $propValue[ 'loop' ];
                $propType[ 'max' ] = $propType[ 'loop' ];
            }
        }
        if (!isset($propValue[ 'start' ])) {
            $start_code =
                array(1 => "{$propValue['step']} > 0 ? ", 2 => '0', 3 => ' : ', 4 => $propValue[ 'loop' ], 5 => ' - 1');
            if ($propType[ 'loop' ] === 0) {
                $start_code[ 5 ] = '';
                $start_code[ 4 ] = $propValue[ 'loop' ] - 1;
            }
            if ($propType[ 'step' ] === 0) {
                if ($propValue[ 'step' ] > 0) {
                    $start_code = array(1 => '0');
                    $propType[ 'start' ] = 0;
                } else {
                    $start_code[ 1 ] = $start_code[ 2 ] = $start_code[ 3 ] = '';
                    $propType[ 'start' ] = $propType[ 'loop' ];
                }
            } else {
                $propType[ 'start' ] = 1;
            }
            $propValue[ 'start' ] = join('', $start_code);
        } else {
            $start_code =
                array(
                    1  => "{$propValue['start']} < 0 ? ", 2 => 'max(', 3 => "{$propValue['step']} > 0 ? ", 4 => '0',
                    5  => ' : ', 6 => '-1', 7 => ', ', 8 => "{$propValue['start']} + {$propValue['loop']}", 10 => ')',
                    11 => ' : ', 12 => 'min(', 13 => $propValue[ 'start' ], 14 => ', ',
                    15 => "{$propValue['step']} > 0 ? ", 16 => $propValue[ 'loop' ], 17 => ' : ',
                    18 => $propType[ 'loop' ] === 0 ? $propValue[ 'loop' ] - 1 : "{$propValue['loop']} - 1",
                    19 => ')'
                );
            if ($propType[ 'step' ] === 0) {
                $start_code[ 3 ] = $start_code[ 5 ] = $start_code[ 15 ] = $start_code[ 17 ] = '';
                if ($propValue[ 'step' ] > 0) {
                    $start_code[ 6 ] = $start_code[ 18 ] = '';
                } else {
                    $start_code[ 4 ] = $start_code[ 16 ] = '';
                }
            }
            if ($propType[ 'start' ] === 0) {
                if ($propType[ 'loop' ] === 0) {
                    $start_code[ 8 ] = $propValue[ 'start' ] + $propValue[ 'loop' ];
                }
                $propType[ 'start' ] = $propType[ 'step' ] + $propType[ 'loop' ];
                $start_code[ 1 ] = '';
                if ($propValue[ 'start' ] < 0) {
                    for ($i = 11; $i <= 19; $i++) {
                        $start_code[ $i ] = '';
                    }
                    if ($propType[ 'start' ] === 0) {
                        $start_code = array(
                            max(
                                $propValue[ 'step' ] > 0 ? 0 : -1,
                                $propValue[ 'start' ] + $propValue[ 'loop' ]
                            )
                        );
                    }
                } else {
                    for ($i = 1; $i <= 11; $i++) {
                        $start_code[ $i ] = '';
                    }
                    if ($propType[ 'start' ] === 0) {
                        $start_code =
                            array(
                                min(
                                    $propValue[ 'step' ] > 0 ? $propValue[ 'loop' ] : $propValue[ 'loop' ] - 1,
                                    $propValue[ 'start' ]
                                )
                            );
                    }
                }
            }
            $propValue[ 'start' ] = join('', $start_code);
        }
        if ($propType[ 'start' ] !== 0) {
            $initLocal[ 'start' ] = $propValue[ 'start' ];
            $propValue[ 'start' ] = "{$local}start";
        }
        $initFor[ 'index' ] = "{$sectionVar}->value['index'] = {$propValue['start']}";
        if (!isset($_attr[ 'start' ]) && !isset($_attr[ 'step' ]) && !isset($_attr[ 'max' ])) {
            $propValue[ 'total' ] = $propValue[ 'loop' ];
            $propType[ 'total' ] = $propType[ 'loop' ];
        } else {
            $propType[ 'total' ] =
                $propType[ 'start' ] + $propType[ 'loop' ] + $propType[ 'step' ] + $propType[ 'max' ];
            if ($propType[ 'total' ] === 0) {
                $propValue[ 'total' ] =
                    min(
                        ceil(
                            ($propValue[ 'step' ] > 0 ? $propValue[ 'loop' ] - $propValue[ 'start' ] :
                                (int)$propValue[ 'start' ] + 1) / abs($propValue[ 'step' ])
                        ),
                        $propValue[ 'max' ]
                    );
            } else {
                $total_code = array(
                    1  => 'min(', 2 => 'ceil(', 3 => '(', 4 => "{$propValue['step']} > 0 ? ",
                    5  => $propValue[ 'loop' ], 6 => ' - ', 7 => $propValue[ 'start' ], 8 => ' : ',
                    9  => $propValue[ 'start' ], 10 => '+ 1', 11 => ')', 12 => '/ ', 13 => 'abs(',
                    14 => $propValue[ 'step' ], 15 => ')', 16 => ')', 17 => ", {$propValue['max']})",
                );
                if (!isset($propValue[ 'max' ])) {
                    $total_code[ 1 ] = $total_code[ 17 ] = '';
                }
                if ($propType[ 'loop' ] + $propType[ 'start' ] === 0) {
                    $total_code[ 5 ] = $propValue[ 'loop' ] - $propValue[ 'start' ];
                    $total_code[ 6 ] = $total_code[ 7 ] = '';
                }
                if ($propType[ 'start' ] === 0) {
                    $total_code[ 9 ] = (int)$propValue[ 'start' ] + 1;
                    $total_code[ 10 ] = '';
                }
                if ($propType[ 'step' ] === 0) {
                    $total_code[ 13 ] = $total_code[ 15 ] = '';
                    if ($propValue[ 'step' ] === 1 || $propValue[ 'step' ] === -1) {
                        $total_code[ 2 ] = $total_code[ 12 ] = $total_code[ 14 ] = $total_code[ 16 ] = '';
                    } elseif ($propValue[ 'step' ] < 0) {
                        $total_code[ 14 ] = -$propValue[ 'step' ];
                    }
                    $total_code[ 4 ] = '';
                    if ($propValue[ 'step' ] > 0) {
                        $total_code[ 8 ] = $total_code[ 9 ] = $total_code[ 10 ] = '';
                    } else {
                        $total_code[ 5 ] = $total_code[ 6 ] = $total_code[ 7 ] = $total_code[ 8 ] = '';
                    }
                }
                $propValue[ 'total' ] = join('', $total_code);
            }
        }
        if (isset($namedAttr[ 'loop' ])) {
            $initNamedProperty[ 'loop' ] = "'loop' => {$propValue['loop']}";
        }
        if (isset($namedAttr[ 'total' ])) {
            $initNamedProperty[ 'total' ] = "'total' => {$propValue['total']}";
            if ($propType[ 'total' ] > 0) {
                $propValue[ 'total' ] = "{$sectionVar}->value['total']";
            }
        } elseif ($propType[ 'total' ] > 0) {
            $initLocal[ 'total' ] = $propValue[ 'total' ];
            $propValue[ 'total' ] = "{$local}total";
        }
        $cmpFor[ 'iteration' ] = "{$propValue['iteration']} <= {$propValue['total']}";
        foreach ($initLocal as $key => $code) {
            $output .= "{$local}{$key} = {$code};\n";
        }
        $_vars = 'array(' . join(', ', $initNamedProperty) . ')';
        $output .= "{$sectionVar} = new Smarty_Variable({$_vars});\n";
        $cond_code = "{$propValue['total']} !== 0";
        if ($propType[ 'total' ] === 0) {
            if ($propValue[ 'total' ] === 0) {
                $cond_code = 'false';
            } else {
                $cond_code = 'true';
            }
        }
        if ($propType[ 'show' ] > 0) {
            $output .= "{$local}show = {$propValue['show']} ? {$cond_code} : false;\n";
            $output .= "if ({$local}show) {\n";
        } elseif ($propValue[ 'show' ] === 'true') {
            $output .= "if ({$cond_code}) {\n";
        } else {
            $output .= "if (false) {\n";
        }
        $jinit = join(', ', $initFor);
        $jcmp = join(', ', $cmpFor);
        $jinc = join(', ', $incFor);
        $output .= "for ({$jinit}; {$jcmp}; {$jinc}){\n";
        if (isset($namedAttr[ 'rownum' ])) {
            $output .= "{$sectionVar}->value['rownum'] = {$propValue['iteration']};\n";
        }
        if (isset($namedAttr[ 'index_prev' ])) {
            $output .= "{$sectionVar}->value['index_prev'] = {$propValue['index']} - {$propValue['step']};\n";
        }
        if (isset($namedAttr[ 'index_next' ])) {
            $output .= "{$sectionVar}->value['index_next'] = {$propValue['index']} + {$propValue['step']};\n";
        }
        if (isset($namedAttr[ 'first' ])) {
            $output .= "{$sectionVar}->value['first'] = ({$propValue['iteration']} === 1);\n";
        }
        if (isset($namedAttr[ 'last' ])) {
            $output .= "{$sectionVar}->value['last'] = ({$propValue['iteration']} === {$propValue['total']});\n";
        }
        $output .= '?>';
        return $output;
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Sectionelse Class
 * 
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Sectionelse extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Sectionelse extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {sectionelse} tag
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

        list($openTag, $nocache) = $this->closeTag($compiler, array('section'));
        $this->openTag($compiler, 'sectionelse', array('sectionelse', $nocache));

        return "<?php endfor; else: ?>";
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
        list($openTag, $nocache, $local, $sectionVar) = $this->closeTag($compiler, array('section'));
        $this->openTag($compiler, 'sectionelse', array('sectionelse', $nocache, $local, $sectionVar));
        return "<?php }} else {\n ?>";
    }
>>>>>>> gosa-core_v2.8
}

/**
 * Smarty Internal Plugin Compile Sectionclose Class
 * 
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Sectionclose extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Sectionclose extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for the {/section} tag
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
<<<<<<< HEAD

        list($openTag, $compiler->nocache) = $this->closeTag($compiler, array('section', 'sectionelse'));

        if ($openTag == 'sectionelse') {
            return "<?php endif; ?>";
        } else {
            return "<?php endfor; endif; ?>";
        }
    }

}

?>
=======
        list($openTag, $compiler->nocache, $local, $sectionVar) =
            $this->closeTag($compiler, array('section', 'sectionelse'));
        $output = "<?php\n";
        if ($openTag === 'sectionelse') {
            $output .= "}\n";
        } else {
            $output .= "}\n}\n";
        }
        $output .= '?>';
        return $output;
    }
}
>>>>>>> gosa-core_v2.8
