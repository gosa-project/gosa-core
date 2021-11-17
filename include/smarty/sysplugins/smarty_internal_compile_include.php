<?php
/**
* Smarty Internal Plugin Compile Include
<<<<<<< HEAD
*
=======
>>>>>>> gosa-core_v2.8
* Compiles the {include} tag
*
* @package Smarty
* @subpackage Compiler
* @author Uwe Tews
*/

/**
* Smarty Internal Plugin Compile Include Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Include extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Include extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
    * caching mode to create nocache code but no cache file
    */
    const CACHING_NOCACHE_CODE = 9999;
<<<<<<< HEAD
=======

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
    public $option_flags = array('nocache', 'inline', 'caching');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
    * Attribute definition: Overwrites base class.
    *
    * @var array
    * @see Smarty_Internal_CompileBase
    */
    public $optional_attributes = array('_any');

    /**
<<<<<<< HEAD
    * Compiles code for the {include} tag
    *
     * @param array $args array with attributes from parser
     * @param object $compiler compiler object
     * @param array $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compile($args, $compiler, $parameter)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        // save posible attributes
        $include_file = $_attr['file'];

        if (isset($_attr['assign'])) {
            // output will be stored in a smarty variable instead of beind displayed
            $_assign = $_attr['assign'];
        }

        $_parent_scope = Smarty::SCOPE_LOCAL;
        if (isset($_attr['scope'])) {
            $_attr['scope'] = trim($_attr['scope'], "'\"");
            if ($_attr['scope'] == 'parent') {
                $_parent_scope = Smarty::SCOPE_PARENT;
            } elseif ($_attr['scope'] == 'root') {
                $_parent_scope = Smarty::SCOPE_ROOT;
            } elseif ($_attr['scope'] == 'global') {
                $_parent_scope = Smarty::SCOPE_GLOBAL;
            }
        }
        $_caching = 'null';
        if ($compiler->nocache || $compiler->tag_nocache) {
            $_caching = Smarty::CACHING_OFF;
        }
        // default for included templates
        if ($compiler->template->caching && !$compiler->nocache && !$compiler->tag_nocache) {
            $_caching = self::CACHING_NOCACHE_CODE;
        }
        /*
        * if the {include} tag provides individual parameter for caching
        * it will not be included into the common cache file and treated like
        * a nocache section
        */
        if (isset($_attr['cache_lifetime'])) {
            $_cache_lifetime = $_attr['cache_lifetime'];
            $compiler->tag_nocache = true;
            $_caching = Smarty::CACHING_LIFETIME_CURRENT;
        } else {
            $_cache_lifetime = 'null';
        }
        if (isset($_attr['cache_id'])) {
            $_cache_id = $_attr['cache_id'];
            $compiler->tag_nocache = true;
            $_caching = Smarty::CACHING_LIFETIME_CURRENT;
=======
     * Valid scope names
     *
     * @var array
     */
    public $valid_scopes = array(
        'parent' => Smarty::SCOPE_PARENT, 'root' => Smarty::SCOPE_ROOT,
        'global' => Smarty::SCOPE_GLOBAL, 'tpl_root' => Smarty::SCOPE_TPL_ROOT,
        'smarty' => Smarty::SCOPE_SMARTY
    );

    /**
     * Compiles code for the {include} tag
     *
     * @param array                                  $args     array with attributes from parser
     * @param Smarty_Internal_SmartyTemplateCompiler $compiler compiler object
     *
     * @return string
     * @throws \Exception
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_SmartyTemplateCompiler $compiler)
    {
        $uid = $t_hash = null;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        $fullResourceName = $source_resource = $_attr[ 'file' ];
        $variable_template = false;
        $cache_tpl = false;
        // parse resource_name
        if (preg_match('/^([\'"])(([A-Za-z0-9_\-]{2,})[:])?(([^$()]+)|(.+))\1$/', $source_resource, $match)) {
            $type = !empty($match[ 3 ]) ? $match[ 3 ] : $compiler->template->smarty->default_resource_type;
            $name = !empty($match[ 5 ]) ? $match[ 5 ] : $match[ 6 ];
            $handler = Smarty_Resource::load($compiler->smarty, $type);
            if ($handler->recompiled || $handler->uncompiled) {
                $variable_template = true;
            }
            if (!$variable_template) {
                if ($type !== 'string') {
                    $fullResourceName = "{$type}:{$name}";
                    $compiled = $compiler->parent_compiler->template->compiled;
                    if (isset($compiled->includes[ $fullResourceName ])) {
                        $compiled->includes[ $fullResourceName ]++;
                        $cache_tpl = true;
                    } else {
                        if ("{$compiler->template->source->type}:{$compiler->template->source->name}" ==
                            $fullResourceName
                        ) {
                            // recursive call of current template
                            $compiled->includes[ $fullResourceName ] = 2;
                            $cache_tpl = true;
                        } else {
                            $compiled->includes[ $fullResourceName ] = 1;
                        }
                    }
                    $fullResourceName = $match[ 1 ] . $fullResourceName . $match[ 1 ];
                }
            }
            if (empty($match[ 5 ])) {
                $variable_template = true;
            }
        } else {
            $variable_template = true;
        }
        // scope setup
        $_scope = $compiler->convertScope($_attr, $this->valid_scopes);
        // set flag to cache subtemplate object when called within loop or template name is variable.
        if ($cache_tpl || $variable_template || $compiler->loopNesting > 0) {
            $_cache_tpl = 'true';
        } else {
            $_cache_tpl = 'false';
        }
        // assume caching is off
        $_caching = Smarty::CACHING_OFF;
        $call_nocache = $compiler->tag_nocache || $compiler->nocache;
        // caching was on and {include} is not in nocache mode
        if ($compiler->template->caching && !$compiler->nocache && !$compiler->tag_nocache) {
            $_caching = self::CACHING_NOCACHE_CODE;
        }
        // flag if included template code should be merged into caller
        $merge_compiled_includes = ($compiler->smarty->merge_compiled_includes || $_attr[ 'inline' ] === true) &&
                                   !$compiler->template->source->handler->recompiled;
        if ($merge_compiled_includes) {
            // variable template name ?
            if ($variable_template) {
                $merge_compiled_includes = false;
            }
            // variable compile_id?
            if (isset($_attr[ 'compile_id' ]) && $compiler->isVariable($_attr[ 'compile_id' ])) {
                $merge_compiled_includes = false;
            }
        }
        /*
        * if the {include} tag provides individual parameter for caching or compile_id
        * the subtemplate must not be included into the common cache file and is treated like
        * a call in nocache mode.
        *
        */
        if ($_attr[ 'nocache' ] !== true && $_attr[ 'caching' ]) {
            $_caching = $_new_caching = (int)$_attr[ 'caching' ];
            $call_nocache = true;
        } else {
            $_new_caching = Smarty::CACHING_LIFETIME_CURRENT;
        }
        if (isset($_attr[ 'cache_lifetime' ])) {
            $_cache_lifetime = $_attr[ 'cache_lifetime' ];
            $call_nocache = true;
            $_caching = $_new_caching;
        } else {
            $_cache_lifetime = '$_smarty_tpl->cache_lifetime';
        }
        if (isset($_attr[ 'cache_id' ])) {
            $_cache_id = $_attr[ 'cache_id' ];
            $call_nocache = true;
            $_caching = $_new_caching;
>>>>>>> gosa-core_v2.8
        } else {
            $_cache_id = '$_smarty_tpl->cache_id';
        }
        if (isset($_attr['compile_id'])) {
            $_compile_id = $_attr['compile_id'];
        } else {
            $_compile_id = '$_smarty_tpl->compile_id';
        }
<<<<<<< HEAD
        if ($_attr['caching'] === true) {
            $_caching = Smarty::CACHING_LIFETIME_CURRENT;
        }
        if ($_attr['nocache'] === true) {
            $compiler->tag_nocache = true;
            $_caching = Smarty::CACHING_OFF;
        }

        $has_compiled_template = false;
        if (($compiler->smarty->merge_compiled_includes || $_attr['inline'] === true) && !$compiler->template->source->recompiled
            && !($compiler->template->caching && ($compiler->tag_nocache || $compiler->nocache)) && $_caching != Smarty::CACHING_LIFETIME_CURRENT) {
            // check if compiled code can be merged (contains no variable part)
            if (!$compiler->has_variable_string && (substr_count($include_file, '"') == 2 or substr_count($include_file, "'") == 2)
               and substr_count($include_file, '(') == 0 and substr_count($include_file, '$_smarty_tpl->') == 0) {
                $tpl_name = null;
                eval("\$tpl_name = $include_file;");
                if (!isset($compiler->smarty->merged_templates_func[$tpl_name]) || $compiler->inheritance) {
                    $tpl = new $compiler->smarty->template_class ($tpl_name, $compiler->smarty, $compiler->template, $compiler->template->cache_id, $compiler->template->compile_id);
                    // save unique function name
                    $compiler->smarty->merged_templates_func[$tpl_name]['func'] = $tpl->properties['unifunc'] = 'content_'. str_replace('.', '_', uniqid('', true));
                    // use current nocache hash for inlined code
                    $compiler->smarty->merged_templates_func[$tpl_name]['nocache_hash'] = $tpl->properties['nocache_hash'] = $compiler->template->properties['nocache_hash'];
                    if ($compiler->template->caching) {
                        // needs code for cached page but no cache file
                        $tpl->caching = self::CACHING_NOCACHE_CODE;
                    }
                    // make sure whole chain gest compiled
                    $tpl->mustCompile = true;
                    if (!($tpl->source->uncompiled) && $tpl->source->exists) {
                        // get compiled code
                        $compiled_code = $tpl->compiler->compileTemplate($tpl);
                        // release compiler object to free memory
                        unset($tpl->compiler);
                        // merge compiled code for {function} tags
                        $compiler->template->properties['function'] = array_merge($compiler->template->properties['function'], $tpl->properties['function']);
                        // merge filedependency
                        $tpl->properties['file_dependency'][$tpl->source->uid] = array($tpl->source->filepath, $tpl->source->timestamp,$tpl->source->type);
                        $compiler->template->properties['file_dependency'] = array_merge($compiler->template->properties['file_dependency'], $tpl->properties['file_dependency']);
                        // remove header code
                        $compiled_code = preg_replace("/(<\?php \/\*%%SmartyHeaderCode:{$tpl->properties['nocache_hash']}%%\*\/(.+?)\/\*\/%%SmartyHeaderCode%%\*\/\?>\n)/s", '', $compiled_code);
                        if ($tpl->has_nocache_code) {
                            // replace nocache_hash
                            $compiled_code = str_replace("{$tpl->properties['nocache_hash']}", $compiler->template->properties['nocache_hash'], $compiled_code);
                            $compiler->template->has_nocache_code = true;
                        }
                        $compiler->merged_templates[$tpl->properties['unifunc']] = $compiled_code;
                        $has_compiled_template = true;
                    }
                } else {
                    $has_compiled_template = true;
                }
            }
=======
        // if subtemplate will be called in nocache mode do not merge
        if ($compiler->template->caching && $call_nocache) {
            $merge_compiled_includes = false;
        }
        // assign attribute
        if (isset($_attr[ 'assign' ])) {
            // output will be stored in a smarty variable instead of being displayed
            if ($_assign = $compiler->getId($_attr[ 'assign' ])) {
                $_assign = "'{$_assign}'";
                if ($compiler->tag_nocache || $compiler->nocache || $call_nocache) {
                    // create nocache var to make it know for further compiling
                    $compiler->setNocacheInVariable($_attr[ 'assign' ]);
                }
            } else {
                $_assign = $_attr[ 'assign' ];
            }
        }
        $has_compiled_template = false;
        if ($merge_compiled_includes) {
            $c_id = isset($_attr[ 'compile_id' ]) ? $_attr[ 'compile_id' ] : $compiler->template->compile_id;
            // we must observe different compile_id and caching
            $t_hash = sha1($c_id . ($_caching ? '--caching' : '--nocaching'));
            $compiler->smarty->allow_ambiguous_resources = true;
            /* @var Smarty_Internal_Template $tpl */
            $tpl = new $compiler->smarty->template_class(
                trim($fullResourceName, '"\''),
                $compiler->smarty,
                $compiler->template,
                $compiler->template->cache_id,
                $c_id,
                $_caching
            );
            $uid = $tpl->source->type . $tpl->source->uid;
            if (!isset($compiler->parent_compiler->mergedSubTemplatesData[ $uid ][ $t_hash ])) {
                $has_compiled_template = $this->compileInlineTemplate($compiler, $tpl, $t_hash);
            } else {
                $has_compiled_template = true;
            }
            unset($tpl);
>>>>>>> gosa-core_v2.8
        }
        // delete {include} standard attributes
        unset($_attr['file'], $_attr['assign'], $_attr['cache_id'], $_attr['compile_id'], $_attr['cache_lifetime'], $_attr['nocache'], $_attr['caching'], $_attr['scope'], $_attr['inline']);
        // remaining attributes must be assigned as smarty variable
<<<<<<< HEAD
        if (!empty($_attr)) {
            if ($_parent_scope == Smarty::SCOPE_LOCAL) {
=======
        $_vars = 'array()';
        if (!empty($_attr)) {
            $_pairs = array();
>>>>>>> gosa-core_v2.8
                // create variables
                foreach ($_attr as $key => $value) {
                    $_pairs[] = "'$key'=>$value";
                }
                $_vars = 'array('.join(',',$_pairs).')';
<<<<<<< HEAD
                $_has_vars = true;
            } else {
                $compiler->trigger_template_error('variable passing not allowed in parent/global scope', $compiler->lex->taglineno);
            }
        } else {
            $_vars = 'array()';
            $_has_vars = false;
        }
        if ($has_compiled_template) {
            $_hash = $compiler->smarty->merged_templates_func[$tpl_name]['nocache_hash'];
            $_output = "<?php /*  Call merged included template \"" . $tpl_name . "\" */\n";
            $_output .= "\$_tpl_stack[] = \$_smarty_tpl;\n";
            $_output .= " \$_smarty_tpl = \$_smarty_tpl->setupInlineSubTemplate($include_file, $_cache_id, $_compile_id, $_caching, $_cache_lifetime, $_vars, $_parent_scope, '$_hash');\n";
            if (isset($_assign)) {
                $_output .= 'ob_start(); ';
            }
            $_output .= $compiler->smarty->merged_templates_func[$tpl_name]['func']. "(\$_smarty_tpl);\n";
            $_output .= "\$_smarty_tpl = array_pop(\$_tpl_stack); ";
            if (isset($_assign)) {
                $_output .= " \$_smarty_tpl->tpl_vars[$_assign] = new Smarty_variable(ob_get_clean());";
            }
            $_output .= "/*  End of included template \"" . $tpl_name . "\" */?>";
            return $_output;
        }

        // was there an assign attribute
        if (isset($_assign)) {
            $_output = "<?php \$_smarty_tpl->tpl_vars[$_assign] = new Smarty_variable(\$_smarty_tpl->getSubTemplate ($include_file, $_cache_id, $_compile_id, $_caching, $_cache_lifetime, $_vars, $_parent_scope));?>\n";;
        } else {
            $_output = "<?php echo \$_smarty_tpl->getSubTemplate ($include_file, $_cache_id, $_compile_id, $_caching, $_cache_lifetime, $_vars, $_parent_scope);?>\n";
        }
        return $_output;
    }

}

?>
=======
        }
        $update_compile_id = $compiler->template->caching && !$compiler->tag_nocache && !$compiler->nocache &&
                             $_compile_id !== '$_smarty_tpl->compile_id';
        if ($has_compiled_template && !$call_nocache) {
            $_output = "<?php\n";
            if ($update_compile_id) {
                $_output .= $compiler->makeNocacheCode("\$_compile_id_save[] = \$_smarty_tpl->compile_id;\n\$_smarty_tpl->compile_id = {$_compile_id};\n");
            }
            if (!empty($_attr) && $_caching === 9999 && $compiler->template->caching) {
                $_vars_nc = "foreach ($_vars as \$ik => \$iv) {\n";
                $_vars_nc .= "\$_smarty_tpl->tpl_vars[\$ik] =  new Smarty_Variable(\$iv);\n";
                $_vars_nc .= "}\n";
                $_output .= substr($compiler->processNocacheCode('<?php ' . $_vars_nc . "?>\n", true), 6, -3);
            }
            if (isset($_assign)) {
                $_output .= "ob_start();\n";
            }
            $_output .= "\$_smarty_tpl->_subTemplateRender({$fullResourceName}, {$_cache_id}, {$_compile_id}, {$_caching}, {$_cache_lifetime}, {$_vars}, {$_scope}, {$_cache_tpl}, '{$compiler->parent_compiler->mergedSubTemplatesData[$uid][$t_hash]['uid']}', '{$compiler->parent_compiler->mergedSubTemplatesData[$uid][$t_hash]['func']}');\n";
            if (isset($_assign)) {
                $_output .= "\$_smarty_tpl->assign({$_assign}, ob_get_clean());\n";
            }
            if ($update_compile_id) {
                $_output .= $compiler->makeNocacheCode("\$_smarty_tpl->compile_id = array_pop(\$_compile_id_save);\n");
            }
            $_output .= "?>";
            return $_output;
        }
        if ($call_nocache) {
            $compiler->tag_nocache = true;
        }
        $_output = "<?php ";
        if ($update_compile_id) {
            $_output .= "\$_compile_id_save[] = \$_smarty_tpl->compile_id;\n\$_smarty_tpl->compile_id = {$_compile_id};\n";
        }
        // was there an assign attribute
        if (isset($_assign)) {
            $_output .= "ob_start();\n";
        }
        $_output .= "\$_smarty_tpl->_subTemplateRender({$fullResourceName}, $_cache_id, $_compile_id, $_caching, $_cache_lifetime, $_vars, $_scope, {$_cache_tpl});\n";
        if (isset($_assign)) {
            $_output .= "\$_smarty_tpl->assign({$_assign}, ob_get_clean());\n";
        }
        if ($update_compile_id) {
            $_output .= "\$_smarty_tpl->compile_id = array_pop(\$_compile_id_save);\n";
        }
        $_output .= "?>";
        return $_output;
    }

    /**
     * Compile inline sub template
     *
     * @param \Smarty_Internal_SmartyTemplateCompiler $compiler
     * @param \Smarty_Internal_Template               $tpl
     * @param string                                  $t_hash
     *
     * @return bool
     * @throws \Exception
     * @throws \SmartyException
     */
    public function compileInlineTemplate(
        Smarty_Internal_SmartyTemplateCompiler $compiler,
        Smarty_Internal_Template $tpl,
        $t_hash
    ) {
        $uid = $tpl->source->type . $tpl->source->uid;
        if (!($tpl->source->handler->uncompiled) && $tpl->source->exists) {
            $compiler->parent_compiler->mergedSubTemplatesData[ $uid ][ $t_hash ][ 'uid' ] = $tpl->source->uid;
            if (isset($compiler->template->inheritance)) {
                $tpl->inheritance = clone $compiler->template->inheritance;
            }
            $tpl->compiled = new Smarty_Template_Compiled();
            $tpl->compiled->nocache_hash = $compiler->parent_compiler->template->compiled->nocache_hash;
            $tpl->loadCompiler();
            // save unique function name
            $compiler->parent_compiler->mergedSubTemplatesData[ $uid ][ $t_hash ][ 'func' ] =
            $tpl->compiled->unifunc = 'content_' . str_replace(array('.', ','), '_', uniqid('', true));
            // make sure whole chain gets compiled
            $tpl->mustCompile = true;
            $compiler->parent_compiler->mergedSubTemplatesData[ $uid ][ $t_hash ][ 'nocache_hash' ] =
                $tpl->compiled->nocache_hash;
            if ($tpl->source->type === 'file') {
                $sourceInfo = $tpl->source->filepath;
            } else {
                $basename = $tpl->source->handler->getBasename($tpl->source);
                $sourceInfo = $tpl->source->type . ':' .
                              ($basename ? $basename : $tpl->source->name);
            }
            // get compiled code
            $compiled_code = "<?php\n\n";
            $compiled_code .= "/* Start inline template \"{$sourceInfo}\" =============================*/\n";
            $compiled_code .= "function {$tpl->compiled->unifunc} (Smarty_Internal_Template \$_smarty_tpl) {\n";
            $compiled_code .= "?>\n" . $tpl->compiler->compileTemplateSource($tpl, null, $compiler->parent_compiler);
            $compiled_code .= "<?php\n";
            $compiled_code .= "}\n?>\n";
            $compiled_code .= $tpl->compiler->postFilter($tpl->compiler->blockOrFunctionCode);
            $compiled_code .= "<?php\n\n";
            $compiled_code .= "/* End inline template \"{$sourceInfo}\" =============================*/\n";
            $compiled_code .= '?>';
            unset($tpl->compiler);
            if ($tpl->compiled->has_nocache_code) {
                // replace nocache_hash
                $compiled_code =
                    str_replace(
                        "{$tpl->compiled->nocache_hash}",
                        $compiler->template->compiled->nocache_hash,
                        $compiled_code
                    );
                $compiler->template->compiled->has_nocache_code = true;
            }
            $compiler->parent_compiler->mergedSubTemplatesCode[ $tpl->compiled->unifunc ] = $compiled_code;
            return true;
        } else {
            return false;
        }
    }
}
>>>>>>> gosa-core_v2.8
