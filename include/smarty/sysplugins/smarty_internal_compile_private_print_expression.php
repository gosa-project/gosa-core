<?php
/**
* Smarty Internal Plugin Compile Print Expression
<<<<<<< HEAD
*
=======
>>>>>>> gosa-core_v2.8
* Compiles any tag which will output an expression or variable
*
* @package Smarty
* @subpackage Compiler
* @author Uwe Tews
*/

/**
* Smarty Internal Plugin Compile Print Expression Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Private_Print_Expression extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Private_Print_Expression extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * Attribute definition: Overwrites base class.
    *
    * @var array
    * @see Smarty_Internal_CompileBase
    */
    public $optional_attributes = array('assign');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
    * Attribute definition: Overwrites base class.
    *
    * @var array
    * @see Smarty_Internal_CompileBase
    */
    public $option_flags = array('nocache', 'nofilter');

    /**
<<<<<<< HEAD
    * Compiles code for gererting output from any expression
    *
    * @param array  $args      array with attributes from parser
    * @param object $compiler  compiler object
    * @param array  $parameter array with compilation parameter
    * @return string compiled code
    */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        // nocache option
        if ($_attr['nocache'] === true) {
            $compiler->tag_nocache = true;
        }
        // filter handling
        if ($_attr['nofilter'] === true) {
            $_filter = 'false';
        } else {
            $_filter = 'true';
        }
        if (isset($_attr['assign'])) {
            // assign output to variable
            $output = "<?php \$_smarty_tpl->assign({$_attr['assign']},{$parameter['value']});?>";
        } else {
            // display value
            $output = $parameter['value'];
            // tag modifier
            if (!empty($parameter['modifierlist'])) {
                $output = $compiler->compileTag('private_modifier', array(), array('modifierlist' => $parameter['modifierlist'], 'value' => $output));
            }
=======
     * Compiles code for generating output from any expression
     *
     * @param array                                 $args      array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $output = $parameter[ 'value' ];
        // tag modifier
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
        if (isset($_attr[ 'assign' ])) {
            // assign output to variable
            return "<?php \$_smarty_tpl->assign({$_attr['assign']},{$output});?>";
        } else {
            // display value
>>>>>>> gosa-core_v2.8
            if (!$_attr['nofilter']) {
                // default modifier
                if (!empty($compiler->smarty->default_modifiers)) {
                    if (empty($compiler->default_modifier_list)) {
                        $modifierlist = array();
                        foreach ($compiler->smarty->default_modifiers as $key => $single_default_modifier) {
<<<<<<< HEAD
                            preg_match_all('/(\'[^\'\\\\]*(?:\\\\.[^\'\\\\]*)*\'|"[^"\\\\]*(?:\\\\.[^"\\\\]*)*"|:|[^:]+)/', $single_default_modifier, $mod_array);
                            for ($i = 0, $count = count($mod_array[0]);$i < $count;$i++) {
                                if ($mod_array[0][$i] != ':') {
=======
                            preg_match_all(
                                '/(\'[^\'\\\\]*(?:\\\\.[^\'\\\\]*)*\'|"[^"\\\\]*(?:\\\\.[^"\\\\]*)*"|:|[^:]+)/',
                                $single_default_modifier,
                                $mod_array
                            );
                            for ($i = 0, $count = count($mod_array[ 0 ]); $i < $count; $i++) {
                                if ($mod_array[ 0 ][ $i ] !== ':') {
>>>>>>> gosa-core_v2.8
                                    $modifierlist[$key][] = $mod_array[0][$i];
                                }
                            }
                        }
                        $compiler->default_modifier_list  = $modifierlist;
                    }
<<<<<<< HEAD
                    $output = $compiler->compileTag('private_modifier', array(), array('modifierlist' => $compiler->default_modifier_list, 'value' => $output));
=======
                    $output = $compiler->compileTag(
                        'private_modifier',
                        array(),
                        array(
                            'modifierlist' => $compiler->default_modifier_list,
                            'value'        => $output
                        )
                    );
>>>>>>> gosa-core_v2.8
                }
                // autoescape html
                if ($compiler->template->smarty->escape_html) {
                    $output = "htmlspecialchars({$output}, ENT_QUOTES, '" . addslashes(Smarty::$_CHARSET) . "')";
                }
<<<<<<< HEAD
                // loop over registerd filters
                if (!empty($compiler->template->smarty->registered_filters[Smarty::FILTER_VARIABLE])) {
                    foreach ($compiler->template->smarty->registered_filters[Smarty::FILTER_VARIABLE] as $key => $function) {
                        if (!is_array($function)) {
                            $output = "{$function}({$output},\$_smarty_tpl)";
                        } else if (is_object($function[0])) {
                            $output = "\$_smarty_tpl->smarty->registered_filters[Smarty::FILTER_VARIABLE][{$key}][0]->{$function[1]}({$output},\$_smarty_tpl)";
=======
                // loop over registered filters
                if (!empty($compiler->template->smarty->registered_filters[ Smarty::FILTER_VARIABLE ])) {
                    foreach ($compiler->template->smarty->registered_filters[ Smarty::FILTER_VARIABLE ] as $key =>
                        $function) {
                        if (!is_array($function)) {
                            $output = "{$function}({$output},\$_smarty_tpl)";
                        } elseif (is_object($function[ 0 ])) {
                            $output =
                                "\$_smarty_tpl->smarty->registered_filters[Smarty::FILTER_VARIABLE]['{$key}'][0]->{$function[1]}({$output},\$_smarty_tpl)";
>>>>>>> gosa-core_v2.8
                        } else {
                            $output = "{$function[0]}::{$function[1]}({$output},\$_smarty_tpl)";
                        }
                    }
                }
                // auto loaded filters
                if (isset($compiler->smarty->autoload_filters[Smarty::FILTER_VARIABLE])) {
                    foreach ((array)$compiler->template->smarty->autoload_filters[Smarty::FILTER_VARIABLE] as $name) {
<<<<<<< HEAD
                        $result = $this->compile_output_filter($compiler, $name, $output);
=======
                        $result = $this->compile_variable_filter($compiler, $name, $output);
>>>>>>> gosa-core_v2.8
                        if ($result !== false) {
                            $output = $result;
                        } else {
                            // not found, throw exception
<<<<<<< HEAD
                            throw new SmartyException("Unable to load filter '{$name}'");
                        }
                    }
                }
                if (isset($compiler->template->variable_filters)) {
                    foreach ($compiler->template->variable_filters as $filter) {
                        if (count($filter) == 1 && ($result = $this->compile_output_filter($compiler, $filter[0], $output)) !== false) {
                            $output = $result;
                        } else {
                            $output = $compiler->compileTag('private_modifier', array(), array('modifierlist' => array($filter), 'value' => $output));
                        }
                    }
                }
            }

            $compiler->has_output = true;
            $output = "<?php echo {$output};?>";
=======
                            throw new SmartyException("Unable to load variable filter '{$name}'");
                        }
                    }
                }
                foreach ($compiler->variable_filters as $filter) {
                    if (count($filter) === 1
                        && ($result = $this->compile_variable_filter($compiler, $filter[ 0 ], $output)) !== false
                    ) {
                        $output = $result;
                    } else {
                        $output = $compiler->compileTag(
                            'private_modifier',
                            array(),
                            array('modifierlist' => array($filter), 'value' => $output)
                        );
                    }
                }
            }
            $output = "<?php echo {$output};?>\n";
>>>>>>> gosa-core_v2.8
        }
        return $output;
    }

    /**
<<<<<<< HEAD
    * @param object $compiler compiler object
    * @param string $name     name of variable filter
    * @param type   $output   embedded output
    * @return string
    */
    private function compile_output_filter($compiler, $name, $output)
    {
        $plugin_name = "smarty_variablefilter_{$name}";
        $path = $compiler->smarty->loadPlugin($plugin_name, false);
        if ($path) {
            if ($compiler->template->caching) {
                $compiler->template->required_plugins['nocache'][$name][Smarty::FILTER_VARIABLE]['file'] = $path;
                $compiler->template->required_plugins['nocache'][$name][Smarty::FILTER_VARIABLE]['function'] = $plugin_name;
            } else {
                $compiler->template->required_plugins['compiled'][$name][Smarty::FILTER_VARIABLE]['file'] = $path;
                $compiler->template->required_plugins['compiled'][$name][Smarty::FILTER_VARIABLE]['function'] = $plugin_name;
            }
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     * @param string                                $name     name of variable filter
     * @param string                                $output   embedded output
     *
     * @return string
     * @throws \SmartyException
     */
    private function compile_variable_filter(Smarty_Internal_TemplateCompilerBase $compiler, $name, $output)
    {
        $function = $compiler->getPlugin($name, 'variablefilter');
        if ($function) {
            return "{$function}({$output},\$_smarty_tpl)";
>>>>>>> gosa-core_v2.8
        } else {
            // not found
            return false;
        }
<<<<<<< HEAD
        return "{$plugin_name}({$output},\$_smarty_tpl)";
    }

}

?>
=======
    }
}
>>>>>>> gosa-core_v2.8
