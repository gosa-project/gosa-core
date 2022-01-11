<?php
/**
 * Smarty Internal Plugin Compile Special Smarty Variable
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the special $smarty variables
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile special Smarty Variable Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Private_Special_Variable extends Smarty_Internal_CompileBase {

    /**
     * Compiles code for the speical $smarty variables
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        $_index = preg_split("/\]\[/",substr($parameter, 1, strlen($parameter)-2));
        $compiled_ref = ' ';
        $variable = trim($_index[0], "'");
        switch ($variable) {
            case 'foreach':
                return "\$_smarty_tpl->getVariable('smarty')->value$parameter";
            case 'section':
                return "\$_smarty_tpl->getVariable('smarty')->value$parameter";
            case 'capture':
                return "Smarty::\$_smarty_vars$parameter";
            case 'now':
                return 'time()';
            case 'cookies':
                if (isset($compiler->smarty->security_policy) && !$compiler->smarty->security_policy->allow_super_globals) {
=======
class Smarty_Internal_Compile_Private_Special_Variable extends Smarty_Internal_CompileBase
{
    /**
     * Compiles code for the special $smarty variables
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     * @param                                       $parameter
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler, $parameter)
    {
        $_index = preg_split("/\]\[/", substr($parameter, 1, strlen($parameter) - 2));
        $variable = strtolower($compiler->getId($_index[ 0 ]));
        if ($variable === false) {
            $compiler->trigger_template_error("special \$Smarty variable name index can not be variable", null, true);
        }
        if (!isset($compiler->smarty->security_policy)
            || $compiler->smarty->security_policy->isTrustedSpecialSmartyVar($variable, $compiler)
        ) {
            switch ($variable) {
                case 'foreach':
                case 'section':
                    if (!isset(Smarty_Internal_TemplateCompilerBase::$_tag_objects[ $variable ])) {
                        $class = 'Smarty_Internal_Compile_' . ucfirst($variable);
                        Smarty_Internal_TemplateCompilerBase::$_tag_objects[ $variable ] = new $class;
                    }
                    return Smarty_Internal_TemplateCompilerBase::$_tag_objects[ $variable ]->compileSpecialVariable(
                        array(),
                        $compiler,
                        $_index
                    );
                case 'capture':
                    if (class_exists('Smarty_Internal_Compile_Capture')) {
                        return Smarty_Internal_Compile_Capture::compileSpecialVariable(array(), $compiler, $_index);
                    }
                    return '';
                case 'now':
                    return 'time()';
                case 'cookies':
                    if (isset($compiler->smarty->security_policy)
                        && !$compiler->smarty->security_policy->allow_super_globals
                    ) {
>>>>>>> gosa-core_v2.8
                    $compiler->trigger_template_error("(secure mode) super globals not permitted");
                    break;
                }
                $compiled_ref = '$_COOKIE';
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'get':
            case 'post':
            case 'env':
            case 'server':
            case 'session':
            case 'request':
<<<<<<< HEAD
                if (isset($compiler->smarty->security_policy) && !$compiler->smarty->security_policy->allow_super_globals) {
=======
                    if (isset($compiler->smarty->security_policy)
                        && !$compiler->smarty->security_policy->allow_super_globals
                    ) {
>>>>>>> gosa-core_v2.8
                    $compiler->trigger_template_error("(secure mode) super globals not permitted");
                    break;
                }
                $compiled_ref = '$_'.strtoupper($variable);
                break;
<<<<<<< HEAD

            case 'template':
                return 'basename($_smarty_tpl->source->filepath)';

            case 'template_object':
                return '$_smarty_tpl';

            case 'current_dir':
                return 'dirname($_smarty_tpl->source->filepath)';

            case 'version':
                $_version = Smarty::SMARTY_VERSION;
                return "'$_version'";

            case 'const':
                if (isset($compiler->smarty->security_policy) && !$compiler->smarty->security_policy->allow_constants) {
                    $compiler->trigger_template_error("(secure mode) constants not permitted");
                    break;
                }
                return "@constant({$_index[1]})";

            case 'config':
                if (isset($_index[2])) {
                    return "(is_array(\$tmp = \$_smarty_tpl->getConfigVariable($_index[1])) ? \$tmp[$_index[2]] : null)";
                } else {
                    return "\$_smarty_tpl->getConfigVariable($_index[1])";
                }
            case 'ldelim':
                $_ldelim = $compiler->smarty->left_delimiter;
                return "'$_ldelim'";

            case 'rdelim':
                $_rdelim = $compiler->smarty->right_delimiter;
                return "'$_rdelim'";

            default:
                $compiler->trigger_template_error('$smarty.' . trim($_index[0], "'") . ' is invalid');
=======
                case 'template':
                    return 'basename($_smarty_tpl->source->filepath)';
                case 'template_object':
                    if (isset($compiler->smarty->security_policy)) {
                        $compiler->trigger_template_error("(secure mode) template_object not permitted");
                        break;
                    }
                    return '$_smarty_tpl';
                case 'current_dir':
                    return 'dirname($_smarty_tpl->source->filepath)';
                case 'version':
                    return "Smarty::SMARTY_VERSION";
                case 'const':
                    if (isset($compiler->smarty->security_policy)
                        && !$compiler->smarty->security_policy->allow_constants
                    ) {
                        $compiler->trigger_template_error("(secure mode) constants not permitted");
                        break;
                    }
                    if (strpos($_index[ 1 ], '$') === false && strpos($_index[ 1 ], '\'') === false) {
                        return "(defined('{$_index[1]}') ? constant('{$_index[1]}') : null)";
                    } else {
                        return "(defined({$_index[1]}) ? constant({$_index[1]}) : null)";
                    }
                // no break
                case 'config':
                    if (isset($_index[ 2 ])) {
                        return "(is_array(\$tmp = \$_smarty_tpl->smarty->ext->configload->_getConfigVariable(\$_smarty_tpl, $_index[1])) ? \$tmp[$_index[2]] : null)";
                    } else {
                        return "\$_smarty_tpl->smarty->ext->configload->_getConfigVariable(\$_smarty_tpl, $_index[1])";
                    }
                // no break
                case 'ldelim':
                    return "\$_smarty_tpl->smarty->left_delimiter";
                case 'rdelim':
                    return "\$_smarty_tpl->smarty->right_delimiter";
                default:
                    $compiler->trigger_template_error('$smarty.' . trim($_index[ 0 ], "'") . ' is not defined');
>>>>>>> gosa-core_v2.8
                break;
        }
        if (isset($_index[1])) {
            array_shift($_index);
            foreach ($_index as $_ind) {
                $compiled_ref = $compiled_ref . "[$_ind]";
            }
        }
        return $compiled_ref;
    }
<<<<<<< HEAD

}

?>
=======
    }
}
>>>>>>> gosa-core_v2.8
