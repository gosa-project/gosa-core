<?php
/**
 * Smarty Internal Plugin Compile Function
 *
 * Compiles the {function} {/function} tags
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Function Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
class Smarty_Internal_Compile_Function extends Smarty_Internal_CompileBase {

    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array('name');
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('name');
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('_any');

    /**
     * Compiles code for the {function} tag
     *
     * @param array $args array with attributes from parser
     * @param object $compiler compiler object
     * @param array $parameter array with compilation parameter
     * @return boolean true
     */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);

        if ($_attr['nocache'] === true) {
            $compiler->trigger_template_error('nocache option not allowed', $compiler->lex->taglineno);
        }
        unset($_attr['nocache']);
        $save = array($_attr, $compiler->parser->current_buffer,
            $compiler->template->has_nocache_code, $compiler->template->required_plugins);
        $this->openTag($compiler, 'function', $save);
        // Init temporary context
        $compiler->parser->current_buffer = new Smarty_Internal_ParseTree_Template();
        $compiler->template->compiled->has_nocache_code = false;
        $compiler->saveRequiredPlugins(true);
        return true;
    }

}

/**
 * Smarty Internal Plugin Compile Functionclose Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
class Smarty_Internal_Compile_Functionclose extends Smarty_Internal_CompileBase {

    /**
     * Compiles code for the {/function} tag
     *
     * @param array $args array with attributes from parser
     * @param object $compiler compiler object
     * @param array $parameter array with compilation parameter
     * @return boolean true
     */
    public function compile($args, $compiler, $parameter)
    {
        $_attr = $this->getAttributes($compiler, $args);
        $saved_data = $this->closeTag($compiler, array('function'));
        $_name = trim($saved_data[0]['name'], "'\"");
        // build plugin include code
        $plugins_string = '';
        if (!empty($compiler->template->required_plugins['compiled'])) {
            $plugins_string = '<?php ';
            foreach($compiler->template->required_plugins['compiled'] as $tmp) {
                foreach($tmp as $data) {
                    $plugins_string .= "if (!is_callable('{$data['function']}')) include '{$data['file']}';\n";
                }
            }
            $plugins_string .= '?>';
        }
        if (!empty($compiler->template->required_plugins['nocache'])) {
            $plugins_string .= "<?php echo '/*%%SmartyNocache:{$compiler->template->properties['nocache_hash']}%%*/<?php ";
            foreach($compiler->template->required_plugins['nocache'] as $tmp) {
                foreach($tmp as $data) {
                    $plugins_string .= "if (!is_callable(\'{$data['function']}\')) include \'{$data['file']}\';\n";
                }
            }
            $plugins_string .= "?>/*/%%SmartyNocache:{$compiler->template->properties['nocache_hash']}%%*/';?>\n";
        }
         // remove last line break from function definition
         $last = count($compiler->parser->current_buffer->subtrees) - 1;
         if ($compiler->parser->current_buffer->subtrees[$last] instanceof _smarty_linebreak) {
             unset($compiler->parser->current_buffer->subtrees[$last]);
         }
        // if caching save template function for possible nocache call
        if ($compiler->template->caching) {
            $compiler->template->properties['function'][$_name]['compiled'] .= $plugins_string
             . $compiler->parser->current_buffer->to_smarty_php();
            $compiler->template->properties['function'][$_name]['nocache_hash'] = $compiler->template->properties['nocache_hash'];
            $compiler->template->properties['function'][$_name]['has_nocache_code'] = $compiler->template->has_nocache_code;
            $compiler->template->properties['function'][$_name]['called_functions'] = $compiler->called_functions;
            $compiler->called_functions = array();
            $compiler->smarty->template_functions[$_name] = $compiler->template->properties['function'][$_name];
            $compiler->has_code = false;
            $output = true;
        } else {
            $output = $plugins_string . $compiler->parser->current_buffer->to_smarty_php() . "<?php \$_smarty_tpl->tpl_vars = \$saved_tpl_vars;
foreach (Smarty::\$global_tpl_vars as \$key => \$value) if(!isset(\$_smarty_tpl->tpl_vars[\$key])) \$_smarty_tpl->tpl_vars[\$key] = \$value;}}?>\n";
        }
        $_functionCode = $compiler->parser->current_buffer;
        // setup buffer for template function code
        $compiler->parser->current_buffer = new Smarty_Internal_ParseTree_Template();
        $_funcName = "smarty_template_function_{$_name}_{$compiler->template->compiled->nocache_hash}";
        $_funcNameCaching = $_funcName . '_nocache';
        if ($compiler->template->compiled->has_nocache_code) {
            $compiler->parent_compiler->tpl_function[ $_name ][ 'call_name_caching' ] = $_funcNameCaching;
            $output = "<?php\n";
            $output .= "/* {$_funcNameCaching} */\n";
            $output .= "if (!function_exists('{$_funcNameCaching}')) {\n";
            $output .= "function {$_funcNameCaching} (Smarty_Internal_Template \$_smarty_tpl,\$params) {\n";
            $output .= "ob_start();\n";
            $output .= $compiler->compileRequiredPlugins();
            $output .= "\$_smarty_tpl->compiled->has_nocache_code = true;\n";
            $output .= $_paramsCode;
            $output .= "foreach (\$params as \$key => \$value) {\n\$_smarty_tpl->tpl_vars[\$key] = new Smarty_Variable(\$value, \$_smarty_tpl->isRenderingCache);\n}\n";
            $output .= "\$params = var_export(\$params, true);\n";
            $output .= "echo \"/*%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%*/<?php ";
            $output .= "\\\$_smarty_tpl->smarty->ext->_tplFunction->saveTemplateVariables(\\\$_smarty_tpl, '{$_name}');\nforeach (\$params as \\\$key => \\\$value) {\n\\\$_smarty_tpl->tpl_vars[\\\$key] = new Smarty_Variable(\\\$value, \\\$_smarty_tpl->isRenderingCache);\n}\n?>";
            $output .= "/*/%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%*/\";?>";
            $compiler->parser->current_buffer->append_subtree(
                $compiler->parser,
                new Smarty_Internal_ParseTree_Tag(
                    $compiler->parser,
                    $output
                )
            );
            $compiler->parser->current_buffer->append_subtree($compiler->parser, $_functionCode);
            $output = "<?php echo \"/*%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%*/<?php ";
            $output .= "\\\$_smarty_tpl->smarty->ext->_tplFunction->restoreTemplateVariables(\\\$_smarty_tpl, '{$_name}');?>\n";
            $output .= "/*/%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%*/\";\n?>";
            $output .= "<?php echo str_replace('{$compiler->template->compiled->nocache_hash}', \$_smarty_tpl->compiled->nocache_hash ?? '', ob_get_clean());\n";
            $output .= "}\n}\n";
            $output .= "/*/ {$_funcName}_nocache */\n\n";
            $output .= "?>\n";
            $compiler->parser->current_buffer->append_subtree(
                $compiler->parser,
                new Smarty_Internal_ParseTree_Tag(
                    $compiler->parser,
                    $output
                )
            );
            $_functionCode = new Smarty_Internal_ParseTree_Tag(
                $compiler->parser,
                preg_replace_callback(
                    "/((<\?php )?echo '\/\*%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%\*\/([\S\s]*?)\/\*\/%%SmartyNocache:{$compiler->template->compiled->nocache_hash}%%\*\/';(\?>\n)?)/",
                    array($this, 'removeNocache'),
                    $_functionCode->to_smarty_php($compiler->parser)
                )
            );
        }
        $compiler->parent_compiler->tpl_function[ $_name ][ 'call_name' ] = $_funcName;
        $output = "<?php\n";
        $output .= "/* {$_funcName} */\n";
        $output .= "if (!function_exists('{$_funcName}')) {\n";
        $output .= "function {$_funcName}(Smarty_Internal_Template \$_smarty_tpl,\$params) {\n";
        $output .= $_paramsCode;
        $output .= "foreach (\$params as \$key => \$value) {\n\$_smarty_tpl->tpl_vars[\$key] = new Smarty_Variable(\$value, \$_smarty_tpl->isRenderingCache);\n}\n";
        $output .= $compiler->compileCheckPlugins(array_merge($compiler->required_plugins[ 'compiled' ],
            $compiler->required_plugins[ 'nocache' ]));
        $output .= "?>\n";
        $compiler->parser->current_buffer->append_subtree(
            $compiler->parser,
            new Smarty_Internal_ParseTree_Tag(
                $compiler->parser,
                $output
            )
        );
        $compiler->parser->current_buffer->append_subtree($compiler->parser, $_functionCode);
        $output = "<?php\n}}\n";
        $output .= "/*/ {$_funcName} */\n\n";
        $output .= "?>\n";
        $compiler->parser->current_buffer->append_subtree(
            $compiler->parser,
            new Smarty_Internal_ParseTree_Tag(
                $compiler->parser,
                $output
            )
        );
        $compiler->parent_compiler->blockOrFunctionCode .= $compiler->parser->current_buffer->to_smarty_php($compiler->parser);
        // restore old buffer
        $compiler->parser->current_buffer = $saved_data[ 1 ];
        // restore old status
        $compiler->restoreRequiredPlugins();
        $compiler->template->compiled->has_nocache_code = $saved_data[ 2 ];
        $compiler->template->caching = $saved_data[ 3 ];
        return true;
    }

}

?>