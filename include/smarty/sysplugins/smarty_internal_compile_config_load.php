<?php
/**
 * Smarty Internal Plugin Compile Config Load
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {config load} tag
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Config Load Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Config_Load extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Config_Load extends Smarty_Internal_CompileBase
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
    public $shorttag_order = array('file','section');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('section', 'scope');

    /**
<<<<<<< HEAD
     * Compiles code for the {config_load} tag
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
     * @return string compiled code
     */
    public function compile($args, $compiler)
    {
        static $_is_legal_scope = array('local' => true,'parent' => true,'root' => true,'global' => true);
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        if ($_attr['nocache'] === true) {
            $compiler->trigger_template_error('nocache option not allowed', $compiler->lex->taglineno);
        }


        // save posible attributes
=======
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $option_flags = array('nocache', 'noscope');

    /**
     * Valid scope names
     *
     * @var array
     */
    public $valid_scopes = array(
        'local'  => Smarty::SCOPE_LOCAL, 'parent' => Smarty::SCOPE_PARENT,
        'root'   => Smarty::SCOPE_ROOT, 'tpl_root' => Smarty::SCOPE_TPL_ROOT,
        'smarty' => Smarty::SCOPE_SMARTY, 'global' => Smarty::SCOPE_SMARTY
    );

    /**
     * Compiles code for the {config_load} tag
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if ($_attr[ 'nocache' ] === true) {
            $compiler->trigger_template_error('nocache option not allowed', null, true);
        }
        // save possible attributes
>>>>>>> gosa-core_v2.8
        $conf_file = $_attr['file'];
        if (isset($_attr['section'])) {
            $section = $_attr['section'];
        } else {
            $section = 'null';
        }
<<<<<<< HEAD
        $scope = 'local';
        // scope setup
        if (isset($_attr['scope'])) {
            $_attr['scope'] = trim($_attr['scope'], "'\"");
            if (isset($_is_legal_scope[$_attr['scope']])) {
                $scope = $_attr['scope'];
           } else {
                $compiler->trigger_template_error('illegal value for "scope" attribute', $compiler->lex->taglineno);
           }
        }
        // create config object
        $_output = "<?php  \$_config = new Smarty_Internal_Config($conf_file, \$_smarty_tpl->smarty, \$_smarty_tpl);";
        $_output .= "\$_config->loadConfigVars($section, '$scope'); ?>";
        return $_output;
    }

}

?>
=======
        // scope setup
        if ($_attr[ 'noscope' ]) {
            $_scope = -1;
        } else {
            $_scope = $compiler->convertScope($_attr, $this->valid_scopes);
        }
        // create config object
        $_output =
            "<?php\n\$_smarty_tpl->smarty->ext->configLoad->_loadConfigFile(\$_smarty_tpl, {$conf_file}, {$section}, {$_scope});\n?>\n";
        return $_output;
    }
}
>>>>>>> gosa-core_v2.8
