<?php
/**
 * Smarty Internal Plugin Compile Assign
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {assign} tag
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Assign Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Assign extends Smarty_Internal_CompileBase {
=======
class Smarty_Internal_Compile_Assign extends Smarty_Internal_CompileBase
{
    /**
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
        'local'    => Smarty::SCOPE_LOCAL, 'parent' => Smarty::SCOPE_PARENT,
        'root'     => Smarty::SCOPE_ROOT, 'global' => Smarty::SCOPE_GLOBAL,
        'tpl_root' => Smarty::SCOPE_TPL_ROOT, 'smarty' => Smarty::SCOPE_SMARTY
    );
>>>>>>> gosa-core_v2.8

    /**
     * Compiles code for the {assign} tag
     *
     * @param array  $args      array with attributes from parser
<<<<<<< HEAD
     * @param object $compiler  compiler object
     * @param array  $parameter array with compilation parameter
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
        // the following must be assigned at runtime because it will be overwritten in Smarty_Internal_Compile_Append
        $this->required_attributes = array('var', 'value');
        $this->shorttag_order = array('var', 'value');
        $this->optional_attributes = array('scope');
<<<<<<< HEAD
        $_nocache = 'null';
        $_scope = Smarty::SCOPE_LOCAL;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        // nocache ?
        if ($compiler->tag_nocache || $compiler->nocache) {
            $_nocache = 'true';
            // create nocache var to make it know for further compiling
            if (isset($compiler->template->tpl_vars[trim($_attr['var'], "'")])) {
                $compiler->template->tpl_vars[trim($_attr['var'], "'")]->nocache = true;
            } else {
                $compiler->template->tpl_vars[trim($_attr['var'], "'")] = new Smarty_variable(null, true);
            }
        }
        // scope setup
        if (isset($_attr['scope'])) {
            $_attr['scope'] = trim($_attr['scope'], "'\"");
            if ($_attr['scope'] == 'parent') {
                $_scope = Smarty::SCOPE_PARENT;
            } elseif ($_attr['scope'] == 'root') {
                $_scope = Smarty::SCOPE_ROOT;
            } elseif ($_attr['scope'] == 'global') {
                $_scope = Smarty::SCOPE_GLOBAL;
            } else {
                $compiler->trigger_template_error('illegal value for "scope" attribute', $compiler->lex->taglineno);
            }
        }
        // compiled output
        if (isset($parameter['smarty_internal_index'])) {
            $output = "<?php \$_smarty_tpl->createLocalArrayVariable($_attr[var], $_nocache, $_scope);\n\$_smarty_tpl->tpl_vars[$_attr[var]]->value$parameter[smarty_internal_index] = $_attr[value];";
        } else {
            // implement Smarty2's behaviour of variables assigned by reference
            if ($compiler->template->smarty instanceof SmartyBC) {
                $output = "<?php if (isset(\$_smarty_tpl->tpl_vars[$_attr[var]])) {\$_smarty_tpl->tpl_vars[$_attr[var]] = clone \$_smarty_tpl->tpl_vars[$_attr[var]];";
                $output .= "\n\$_smarty_tpl->tpl_vars[$_attr[var]]->value = $_attr[value]; \$_smarty_tpl->tpl_vars[$_attr[var]]->nocache = $_nocache; \$_smarty_tpl->tpl_vars[$_attr[var]]->scope = $_scope;";
                $output .= "\n} else \$_smarty_tpl->tpl_vars[$_attr[var]] = new Smarty_variable($_attr[value], $_nocache, $_scope);";
            } else {
                $output = "<?php \$_smarty_tpl->tpl_vars[$_attr[var]] = new Smarty_variable($_attr[value], $_nocache, $_scope);";
            }
        }
        if ($_scope == Smarty::SCOPE_PARENT) {
            $output .= "\nif (\$_smarty_tpl->parent != null) \$_smarty_tpl->parent->tpl_vars[$_attr[var]] = clone \$_smarty_tpl->tpl_vars[$_attr[var]];";
        } elseif ($_scope == Smarty::SCOPE_ROOT || $_scope == Smarty::SCOPE_GLOBAL) {
            $output .= "\n\$_ptr = \$_smarty_tpl->parent; while (\$_ptr != null) {\$_ptr->tpl_vars[$_attr[var]] = clone \$_smarty_tpl->tpl_vars[$_attr[var]]; \$_ptr = \$_ptr->parent; }";
        }
        if ( $_scope == Smarty::SCOPE_GLOBAL) {
            $output .= "\nSmarty::\$global_tpl_vars[$_attr[var]] = clone \$_smarty_tpl->tpl_vars[$_attr[var]];";
        }
        $output .= '?>';
        return $output;
    }

}

?>
=======
        $this->mapCache = array();
        $_nocache = false;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        // nocache ?
        if ($_var = $compiler->getId($_attr[ 'var' ])) {
            $_var = "'{$_var}'";
        } else {
            $_var = $_attr[ 'var' ];
        }
        if ($compiler->tag_nocache || $compiler->nocache) {
            $_nocache = true;
            // create nocache var to make it know for further compiling
            $compiler->setNocacheInVariable($_attr[ 'var' ]);
        }
        // scope setup
        if ($_attr[ 'noscope' ]) {
            $_scope = -1;
        } else {
            $_scope = $compiler->convertScope($_attr, $this->valid_scopes);
        }
        // optional parameter
        $_params = '';
        if ($_nocache || $_scope) {
            $_params .= ' ,' . var_export($_nocache, true);
        }
        if ($_scope) {
            $_params .= ' ,' . $_scope;
        }
        if (isset($parameter[ 'smarty_internal_index' ])) {
            $output =
                "<?php \$_tmp_array = isset(\$_smarty_tpl->tpl_vars[{$_var}]) ? \$_smarty_tpl->tpl_vars[{$_var}]->value : array();\n";
            $output .= "if (!(is_array(\$_tmp_array) || \$_tmp_array instanceof ArrayAccess)) {\n";
            $output .= "settype(\$_tmp_array, 'array');\n";
            $output .= "}\n";
            $output .= "\$_tmp_array{$parameter['smarty_internal_index']} = {$_attr['value']};\n";
            $output .= "\$_smarty_tpl->_assignInScope({$_var}, \$_tmp_array{$_params});?>";
        } else {
            $output = "<?php \$_smarty_tpl->_assignInScope({$_var}, {$_attr['value']}{$_params});?>";
        }
        return $output;
    }
}
>>>>>>> gosa-core_v2.8
