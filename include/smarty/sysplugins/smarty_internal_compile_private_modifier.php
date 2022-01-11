<?php
<<<<<<< HEAD

/**
 * Smarty Internal Plugin Compile Modifier
 *
=======
/**
 * Smarty Internal Plugin Compile Modifier
>>>>>>> gosa-core_v2.8
 * Compiles code for modifier execution
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Modifier Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Private_Modifier extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Private_Modifier extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Compiles code for modifier execution
     *
     * @param array  $args      array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter) {
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler  compiler object
     * @param array                                 $parameter array with compilation parameter
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
>>>>>>> gosa-core_v2.8
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $output = $parameter['value'];
        // loop over list of modifiers
        foreach ($parameter['modifierlist'] as $single_modifier) {
<<<<<<< HEAD
=======
            /* @var string $modifier */
>>>>>>> gosa-core_v2.8
            $modifier = $single_modifier[0];
            $single_modifier[0] = $output;
            $params = implode(',', $single_modifier);
            // check if we know already the type of modifier
            if (isset($compiler->known_modifier_type[$modifier])) {
                $modifier_types = array($compiler->known_modifier_type[$modifier]);
            } else {
                $modifier_types = array(1, 2, 3, 4, 5, 6);
            }
            foreach ($modifier_types as $type) {
                switch ($type) {
                    case 1:
                        // registered modifier
                        if (isset($compiler->smarty->registered_plugins[Smarty::PLUGIN_MODIFIER][$modifier])) {
<<<<<<< HEAD
                            $function = $compiler->smarty->registered_plugins[Smarty::PLUGIN_MODIFIER][$modifier][0];
                            if (!is_array($function)) {
                                $output = "{$function}({$params})";
                            } else {
                                if (is_object($function[0])) {
                                    $output = '$_smarty_tpl->smarty->registered_plugins[Smarty::PLUGIN_MODIFIER][\'' . $modifier . '\'][0][0]->' . $function[1] . '(' . $params . ')';
                                } else {
                                    $output = $function[0] . '::' . $function[1] . '(' . $params . ')';
                                }
                            }
                            $compiler->known_modifier_type[$modifier] = $type;
                            break 2;
                        }
=======
                            if (is_callable($compiler->smarty->registered_plugins[ Smarty::PLUGIN_MODIFIER ][ $modifier ][ 0 ])) {
                                $output =
                                    sprintf(
                                        'call_user_func_array($_smarty_tpl->registered_plugins[ \'%s\' ][ %s ][ 0 ], array( %s ))',
                                        Smarty::PLUGIN_MODIFIER,
                                        var_export($modifier, true),
                                        $params
                                    );
                                $compiler->known_modifier_type[ $modifier ] = $type;
                                break 2;
                            }
                        }
>>>>>>> gosa-core_v2.8
                        break;
                    case 2:
                        // registered modifier compiler
                        if (isset($compiler->smarty->registered_plugins[Smarty::PLUGIN_MODIFIERCOMPILER][$modifier][0])) {
<<<<<<< HEAD
                            $output = call_user_func($compiler->smarty->registered_plugins[Smarty::PLUGIN_MODIFIERCOMPILER][$modifier][0], $single_modifier, $compiler->smarty);
=======
                            $output =
                                call_user_func(
                                    $compiler->smarty->registered_plugins[ Smarty::PLUGIN_MODIFIERCOMPILER ][ $modifier ][ 0 ],
                                    $single_modifier,
                                    $compiler->smarty
                                );
>>>>>>> gosa-core_v2.8
                            $compiler->known_modifier_type[$modifier] = $type;
                            break 2;
                        }
                        break;
                    case 3:
                        // modifiercompiler plugin
                        if ($compiler->smarty->loadPlugin('smarty_modifiercompiler_' . $modifier)) {
                            // check if modifier allowed
<<<<<<< HEAD
                            if (!is_object($compiler->smarty->security_policy) || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)) {
=======
                            if (!is_object($compiler->smarty->security_policy)
                                || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)
                            ) {
>>>>>>> gosa-core_v2.8
                                $plugin = 'smarty_modifiercompiler_' . $modifier;
                                $output = $plugin($single_modifier, $compiler);
                            }
                            $compiler->known_modifier_type[$modifier] = $type;
                            break 2;
                        }
                        break;
                    case 4:
                        // modifier plugin
                        if ($function = $compiler->getPlugin($modifier, Smarty::PLUGIN_MODIFIER)) {
                            // check if modifier allowed
<<<<<<< HEAD
                            if (!is_object($compiler->smarty->security_policy) || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)) {
=======
                            if (!is_object($compiler->smarty->security_policy)
                                || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)
                            ) {
>>>>>>> gosa-core_v2.8
                                $output = "{$function}({$params})";
                            }
                            $compiler->known_modifier_type[$modifier] = $type;
                            break 2;
                        }
                        break;
                    case 5:
                        // PHP function
                        if (is_callable($modifier)) {
                            // check if modifier allowed
<<<<<<< HEAD
                            if (!is_object($compiler->smarty->security_policy) || $compiler->smarty->security_policy->isTrustedPhpModifier($modifier, $compiler)) {
=======
                            if (!is_object($compiler->smarty->security_policy)
                                || $compiler->smarty->security_policy->isTrustedPhpModifier($modifier, $compiler)
                            ) {
>>>>>>> gosa-core_v2.8
                                $output = "{$modifier}({$params})";
                            }
                            $compiler->known_modifier_type[$modifier] = $type;
                            break 2;
                        }
                        break;
                    case 6:
                        // default plugin handler
<<<<<<< HEAD
                        if (isset($compiler->default_handler_plugins[Smarty::PLUGIN_MODIFIER][$modifier]) || (is_callable($compiler->smarty->default_plugin_handler_func) && $compiler->getPluginFromDefaultHandler($modifier, Smarty::PLUGIN_MODIFIER))) {
                            $function = $compiler->default_handler_plugins[Smarty::PLUGIN_MODIFIER][$modifier][0];
                            // check if modifier allowed
                            if (!is_object($compiler->smarty->security_policy) || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)) {
=======
                        if (isset($compiler->default_handler_plugins[ Smarty::PLUGIN_MODIFIER ][ $modifier ])
                            || (is_callable($compiler->smarty->default_plugin_handler_func)
                                && $compiler->getPluginFromDefaultHandler($modifier, Smarty::PLUGIN_MODIFIER))
                        ) {
                            $function = $compiler->default_handler_plugins[ Smarty::PLUGIN_MODIFIER ][ $modifier ][ 0 ];
                            // check if modifier allowed
                            if (!is_object($compiler->smarty->security_policy)
                                || $compiler->smarty->security_policy->isTrustedModifier($modifier, $compiler)
                            ) {
>>>>>>> gosa-core_v2.8
                                if (!is_array($function)) {
                                    $output = "{$function}({$params})";
                                } else {
                                    if (is_object($function[0])) {
<<<<<<< HEAD
                                        $output = '$_smarty_tpl->smarty->registered_plugins[Smarty::PLUGIN_MODIFIER][\'' . $modifier . '\'][0][0]->' . $function[1] . '(' . $params . ')';
=======
                                        $output = $function[ 0 ] . '->' . $function[ 1 ] . '(' . $params . ')';
>>>>>>> gosa-core_v2.8
                                    } else {
                                        $output = $function[0] . '::' . $function[1] . '(' . $params . ')';
                                    }
                                }
                            }
<<<<<<< HEAD
                            if (isset($compiler->template->required_plugins['nocache'][$modifier][Smarty::PLUGIN_MODIFIER]['file']) || isset($compiler->template->required_plugins['compiled'][$modifier][Smarty::PLUGIN_MODIFIER]['file'])) {
=======
                            if (isset($compiler->required_plugins[ 'nocache' ][ $modifier ][ Smarty::PLUGIN_MODIFIER ][ 'file' ])
                                ||
                                isset($compiler->required_plugins[ 'compiled' ][ $modifier ][ Smarty::PLUGIN_MODIFIER ][ 'file' ])
                            ) {
>>>>>>> gosa-core_v2.8
                                // was a plugin
                                $compiler->known_modifier_type[$modifier] = 4;
                            } else {
                                $compiler->known_modifier_type[$modifier] = $type;
                            }
                            break 2;
                        }
                }
            }
            if (!isset($compiler->known_modifier_type[$modifier])) {
<<<<<<< HEAD
                $compiler->trigger_template_error("unknown modifier \"" . $modifier . "\"", $compiler->lex->taglineno);
=======
                $compiler->trigger_template_error("unknown modifier '{$modifier}'", null, true);
>>>>>>> gosa-core_v2.8
            }
        }
        return $output;
    }
<<<<<<< HEAD

}

?>
=======
}
>>>>>>> gosa-core_v2.8
