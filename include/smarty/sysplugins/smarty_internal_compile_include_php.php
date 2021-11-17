<?php
/**
 * Smarty Internal Plugin Compile Include PHP
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {include_php} tag
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Insert Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Include_Php extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Include_Php extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array('file');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('file');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('once', 'assign');

    /**
     * Compiles code for the {include_php} tag
     *
     * @param array  $args     array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler)
=======
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
>>>>>>> gosa-core_v2.8
    {
        if (!($compiler->smarty instanceof SmartyBC)) {
            throw new SmartyException("{include_php} is deprecated, use SmartyBC class to enable");
        }
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
<<<<<<< HEAD

        $_output = '<?php ';

        $_smarty_tpl = $compiler->template;
        $_filepath = false;
        eval('$_file = ' . $_attr['file'] . ';');
        if (!isset($compiler->smarty->security_policy) && file_exists($_file)) {
            $_filepath = $_file;
=======
        /**
         *
         *
         * @var Smarty_Internal_Template $_smarty_tpl
         * used in evaluated code
         */
        $_smarty_tpl = $compiler->template;
        $_filepath = false;
        $_file = null;
        eval('$_file = @' . $_attr[ 'file' ] . ';');
        if (!isset($compiler->smarty->security_policy) && file_exists($_file)) {
            $_filepath = $compiler->smarty->_realpath($_file, true);
>>>>>>> gosa-core_v2.8
        } else {
            if (isset($compiler->smarty->security_policy)) {
                $_dir = $compiler->smarty->security_policy->trusted_dir;
            } else {
                $_dir = $compiler->smarty->trusted_dir;
            }
            if (!empty($_dir)) {
                foreach((array)$_dir as $_script_dir) {
<<<<<<< HEAD
                    $_script_dir = rtrim($_script_dir, '/\\') . DS;
                    if (file_exists($_script_dir . $_file)) {
                        $_filepath = $_script_dir .  $_file;
=======
                    $_path = $compiler->smarty->_realpath($_script_dir . DIRECTORY_SEPARATOR . $_file, true);
                    if (file_exists($_path)) {
                        $_filepath = $_path;
>>>>>>> gosa-core_v2.8
                        break;
                    }
                }
            }
        }
<<<<<<< HEAD
        if ($_filepath == false) {
            $compiler->trigger_template_error("{include_php} file '{$_file}' is not readable", $compiler->lex->taglineno);
        }

        if (isset($compiler->smarty->security_policy)) {
            $compiler->smarty->security_policy->isTrustedPHPDir($_filepath);
        }

=======
        if ($_filepath === false) {
            $compiler->trigger_template_error("{include_php} file '{$_file}' is not readable", null, true);
        }
        if (isset($compiler->smarty->security_policy)) {
            $compiler->smarty->security_policy->isTrustedPHPDir($_filepath);
        }
>>>>>>> gosa-core_v2.8
        if (isset($_attr['assign'])) {
            // output will be stored in a smarty variable instead of being displayed
            $_assign = $_attr['assign'];
        }
        $_once = '_once';
        if (isset($_attr['once'])) {
<<<<<<< HEAD
            if ($_attr['once'] == 'false') {
                $_once = '';
            }
        }

        if (isset($_assign)) {
            return "<?php ob_start(); include{$_once} ('{$_filepath}'); \$_smarty_tpl->assign({$_assign},ob_get_contents()); ob_end_clean();?>";
=======
            if ($_attr[ 'once' ] === 'false') {
                $_once = '';
            }
        }
        if (isset($_assign)) {
            return "<?php ob_start();\ninclude{$_once} ('{$_filepath}');\n\$_smarty_tpl->assign({$_assign},ob_get_clean());\n?>";
>>>>>>> gosa-core_v2.8
        } else {
            return "<?php include{$_once} ('{$_filepath}');?>\n";
        }
    }
<<<<<<< HEAD

}

?>
=======
}
>>>>>>> gosa-core_v2.8
