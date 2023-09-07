<?php

/**
 * Smarty Internal Plugin Smarty Template Compiler Base
 *
 * This file contains the basic classes and methodes for compiling Smarty templates with lexer/parser
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Main abstract compiler class
 *
 * @package Smarty
 * @subpackage Compiler
 */
abstract class Smarty_Internal_TemplateCompilerBase {

    /**
     * hash for nocache sections
     *
     * @var mixed
     */
    private $nocache_hash = null;

    /**
     * suppress generation of nocache code
     *
     * @var bool
     */
    public $suppressNocacheProcessing = false;

    /**
     * suppress generation of merged template code
     *
     * @var bool
     */
    public $suppressMergedTemplates = false;

    /**
     * compile tag objects
     *
     * @var array
     */
    public static $_tag_objects = array();

    /**
     * tag stack
     *
     * @var array
     */
    public $_tag_stack = array();

    /**
     * current template
     *
     * @var Smarty_Internal_Template
     */
    public $template = null;

    /**
     * merged templates
     *
     * @var array
     */
    public $merged_templates = array();

    /**
     * flag when compiling {block}
     *
     * @var bool
     */
    public $inheritance = false;

    /**
     * plugins loaded by default plugin handler
     *
     * @var array
     */
    public $default_handler_plugins = array();

    /**
     * saved preprocessed modifier list
     *
     * @var mixed
     */
    public $default_modifier_list = null;

    /**
     * force compilation of complete template as nocache
     * @var boolean
     */
    public $forceNocache = false;

    /**
     * suppress Smarty header code in compiled template
     * @var bool
     */
    public $suppressHeader = false;

    /**
     * Template functions
     *
     * @var array
     */
    public $tpl_function = array();

    /**
     * called sub functions from template function
     *
     * @var array
     */
    public $called_functions = array();

    /**
     * compiled template or block function code
     *
     * @var string
     */
    public $blockOrFunctionCode = '';

    /**
     * flags for used modifier plugins
     *
     * @var array
     */
    public $modifier_plugins = array();

    /**
     * type of already compiled modifier
     *
     * @var array
     */
    public $known_modifier_type = array();

    /**
     * parent compiler object for merged subtemplates and template functions
     *
     * @var Smarty_Internal_TemplateCompilerBase
     */
    public $parent_compiler = null;

    /**
     * Flag true when compiling nocache section
     *
     * @var bool
     */
    public $suppressTemplatePropertyHeader = false;

    /**
     * suppress pre and post filter
     * @var bool
     */
    public $suppressFilter = false;

    /**
     * flag if compiled template file shall we written
     * @var bool
     */
    public $write_compiled_code = true;

    /**
     * flag if currently a template function is compiled
     * @var bool
     */
    public $compiles_template_function = false;

    /**
     * called subfuntions from template function
     * @var array
     */
    public $called_functions = array();

    /**
     * flags for used modifier plugins
     * @var array
     */
    public $modifier_plugins = array();

    /**
     * type of already compiled modifier
     * @var array
     */
    public $known_modifier_type = array();

    /**
     * Methode to compile a Smarty template
     *
     * @param  mixed $_content template source
     * @return bool true if compiling succeeded, false if it failed
     */
    abstract protected function doCompile($_content);

    /**
     * Initialize compiler
     */
    public function __construct() {
        $this->nocache_hash = str_replace('.', '-', uniqid(rand(), true));
    }

    /**
     * Method to compile a Smarty template
     *
     * @param  Smarty_Internal_Template $template template object to compile
     * @return bool true if compiling succeeded, false if it failed
     */
    public function compileTemplate(Smarty_Internal_Template $template) {
        if (empty($template->properties['nocache_hash'])) {
            $template->properties['nocache_hash'] = $this->nocache_hash;
        } else {
            $this->nocache_hash = $template->properties['nocache_hash'];
        }
        // flag for nochache sections
        $this->nocache = false;
        $this->tag_nocache = false;
        // save template object in compiler class
        $this->template = $template;
        // reset has noche code flag
        $this->template->has_nocache_code = false;
        $this->smarty->_current_file = $saved_filepath = $this->template->source->filepath;
        // template header code
        $template_header = '';
        if (!$this->suppressHeader) {
            $template_header .= "<?php /* Smarty version " . Smarty::SMARTY_VERSION . ", created on " . strftime("%Y-%m-%d %H:%M:%S") . "\n";
            $template_header .= "         compiled from \"" . $this->template->source->filepath . "\" */ ?>\n";
        }

        do {
            // flag for aborting current and start recompile
            $this->abort_and_recompile = false;
            // get template source
            $_content = $template->source->content;
            // run prefilter if required
            if ((isset($this->smarty->autoload_filters['pre']) || isset($this->smarty->registered_filters['pre'])) && !$this->suppressFilter) {
                $_content = Smarty_Internal_Filter_Handler::runFilter('pre', $_content, $template);
            }
            // on empty template just return header
            if ($_content == '') {
                if ($this->suppressTemplatePropertyHeader) {
                    $code = '';
                } else {
                    $code = $template_header . $template->createTemplateCodeFrame();
                }
                return $code;
            }
            // call compiler
            $_compiled_code = $this->doCompile($_content);
        } while ($this->abort_and_recompile);
        $this->template->source->filepath = $saved_filepath;
        // free memory
        unset($this->parser->root_buffer, $this->parser->current_buffer, $this->parser, $this->lex, $this->template);
        self::$_tag_objects = array();
        // return compiled code to template object
        $merged_code = '';
        if (!$this->suppressMergedTemplates && !empty($this->merged_templates)) {
            foreach ($this->merged_templates as $code) {
                $merged_code .= $code;
            }
        }
        // run postfilter if required on compiled template code
        if ((isset($this->smarty->autoload_filters['post']) || isset($this->smarty->registered_filters['post'])) && !$this->suppressFilter) {
            $_compiled_code = Smarty_Internal_Filter_Handler::runFilter('post', $_compiled_code, $template);
        }
        if ($this->suppressTemplatePropertyHeader) {
            $code = $_compiled_code . $merged_code;
        } else {
            $code = $template_header . $template->createTemplateCodeFrame($_compiled_code) . $merged_code;
        }
        // unset content because template inheritance could have replace source with parent code
        unset ($template->source->content);
        return $code;
    }

    /**
     * Compile Tag
     *
     * This is a call back from the lexer/parser
     * It executes the required compile plugin for the Smarty tag
     *
     * @param string $tag       tag name
     * @param array  $args      array with tag attributes
     * @param array  $parameter array with compilation parameter
     * @return string compiled code
     */
    public function compileTag($tag, $args, $parameter = array()) {
        // $args contains the attributes parsed and compiled by the lexer/parser
        // assume that tag does compile into code, but creates no HTML output
        $this->has_code = true;
        $this->has_output = false;
        // log tag/attributes
        if (isset($this->smarty->get_used_tags) && $this->smarty->get_used_tags) {
            $this->template->used_tags[] = array($tag, $args);
        }
        // check nocache option flag
        if (in_array("'nocache'", $args) || in_array(array('nocache' => 'true'), $args)
                || in_array(array('nocache' => '"true"'), $args) || in_array(array('nocache' => "'true'"), $args)) {
            $this->tag_nocache = true;
        }
        // compile the smarty tag (required compile classes to compile the tag are autoloaded)
        if (($_output = $this->callTagCompiler($tag, $args, $parameter)) === false) {
            if (isset($this->smarty->template_functions[$tag])) {
                // template defined by {template} tag
                $args['_attr']['name'] = "'" . $tag . "'";
                $_output = $this->callTagCompiler('call', $args, $parameter);
            }
            $this->parent_compiler = $parent_compiler ? $parent_compiler : $this;
            $nocache = isset($nocache) ? $nocache : false;
            if (empty($template->compiled->nocache_hash)) {
                $template->compiled->nocache_hash = $this->nocache_hash;
            } else {
                $this->nocache_hash = $template->compiled->nocache_hash;
            }
            $this->caching = $template->caching;
            // flag for nocache sections
            $this->nocache = $nocache;
            $this->tag_nocache = false;
            // reset has nocache code flag
            $this->template->compiled->has_nocache_code = false;
            $this->has_variable_string = false;
            $this->prefix_code = array();
            // add file dependency
            if ($this->smarty->merge_compiled_includes || $this->template->source->handler->checkTimestamps()) {
                $this->parent_compiler->template->compiled->file_dependency[ $this->template->source->uid ] =
                    array(
                        $this->template->source->filepath,
                        $this->template->source->getTimeStamp(),
                        $this->template->source->type,
                    );
            }
            $this->smarty->_current_file = $this->template->source->filepath;
            // get template source
            if (!empty($this->template->source->components)) {
                // we have array of inheritance templates by extends: resource
                // generate corresponding source code sequence
                $_content =
                    Smarty_Internal_Compile_Extends::extendsSourceArrayCode($this->template);
            } else {
                // get template source
                $_content = $this->template->source->getContent();
            }
            $_compiled_code = $this->postFilter($this->doCompile($this->preFilter($_content), true));
            if (!empty($this->required_plugins[ 'compiled' ]) || !empty($this->required_plugins[ 'nocache' ])) {
                $_compiled_code = '<?php ' . $this->compileRequiredPlugins() . "?>\n" . $_compiled_code;
            }
        } catch (Exception $e) {
            if ($this->smarty->debugging) {
                $this->smarty->_debug->end_compile($this->template);
            }
            $this->_tag_stack = array();
            // free memory
            $this->parent_compiler = null;
            $this->template = null;
            $this->parser = null;
            throw $e;
        }
        if ($this->smarty->debugging) {
            $this->smarty->_debug->end_compile($this->template);
        }
        $this->parent_compiler = null;
        $this->parser = null;
        return $_compiled_code;
    }

    /**
     * Optionally process compiled code by post filter
     *
     * @param string $code compiled code
     *
     * @return string
     * @throws \SmartyException
     */
    public function postFilter($code)
    {
        // run post filter if on code
        if (!empty($code)
            && (isset($this->smarty->autoload_filters[ 'post' ]) || isset($this->smarty->registered_filters[ 'post' ]))
        ) {
            return $this->smarty->ext->_filterHandler->runFilter('post', $code, $this->template);
        } else {
            return $code;
        }
    }

    /**
     * Run optional prefilter
     *
     * @param string $_content template source
     *
     * @return string
     * @throws \SmartyException
     */
    public function preFilter($_content)
    {
        // run pre filter if required
        if ($_content !== ''
            && ((isset($this->smarty->autoload_filters[ 'pre' ]) || isset($this->smarty->registered_filters[ 'pre' ])))
        ) {
            return $this->smarty->ext->_filterHandler->runFilter('pre', $_content, $this->template);
        } else {
            return $_content;
        }
    }

    /**
     * Compile Tag
     * This is a call back from the lexer/parser
     *
     * Save current prefix code
     * Compile tag
     * Merge tag prefix code with saved one
     * (required nested tags in attributes)
     *
     * @param string $tag       tag name
     * @param array  $args      array with tag attributes
     * @param array  $parameter array with compilation parameter
     *
     * @throws SmartyCompilerException
     * @throws SmartyException
     * @return string compiled code
     */
    public function compileTag($tag, $args, $parameter = array())
    {
        $this->prefixCodeStack[] = $this->prefix_code;
        $this->prefix_code = array();
        $result = $this->compileTag2($tag, $args, $parameter);
        $this->prefix_code = array_merge($this->prefix_code, array_pop($this->prefixCodeStack));
        return $result;
    }

    /**
     * compile variable
     *
     * @param string $variable
     *
     * @return string
     */
    public function compileVariable($variable)
    {
        if (!strpos($variable, '(')) {
            // not a variable variable
            $var = trim($variable, '\'');
            $this->tag_nocache = $this->tag_nocache |
                                 $this->template->ext->getTemplateVars->_getVariable(
                                     $this->template,
                                     $var,
                                     null,
                                     true,
                                     false
                                 )->nocache;
            // todo $this->template->compiled->properties['variables'][$var] = $this->tag_nocache | $this->nocache;
        }
        return '$_smarty_tpl->tpl_vars[' . $variable . ']->value';
    }

    /**
     * compile config variable
     *
     * @param string $variable
     *
     * @return string
     */
    public function compileConfigVariable($variable)
    {
        // return '$_smarty_tpl->config_vars[' . $variable . ']';
        return '$_smarty_tpl->smarty->ext->configLoad->_getConfigVariable($_smarty_tpl, ' . $variable . ')';
    }

    /**
     * compile PHP function call
     *
     * @param string $name
     * @param array  $parameter
     *
     * @return string
     * @throws \SmartyCompilerException
     */
    public function compilePHPFunctionCall($name, $parameter)
    {
        if (!$this->smarty->security_policy || $this->smarty->security_policy->isTrustedPhpFunction($name, $this)) {
            if (strcasecmp($name, 'isset') === 0 || strcasecmp($name, 'empty') === 0
                || strcasecmp($name, 'array') === 0 || is_callable($name)
            ) {
                $func_name = strtolower($name);

                if ($func_name === 'isset') {
                    if (count($parameter) === 0) {
                        $this->trigger_template_error('Illegal number of parameter in "isset()"');
                    }

                    $pa = array();
                    foreach ($parameter as $p) {
                        $pa[] = $this->syntaxMatchesVariable($p) ? 'isset(' . $p . ')' : '(' . $p . ' !== null )';
                    }
                    return '(' . implode(' && ', $pa) . ')';

                } elseif (in_array(
                    $func_name,
                    array(
                        'empty',
                        'reset',
                        'current',
                        'end',
                        'prev',
                        'next'
                    )
                )
                ) {
                    if (count($parameter) !== 1) {
                        $this->trigger_template_error("Illegal number of parameter in '{$func_name()}'");
                    }
                    if ($func_name === 'empty') {
                        return $func_name . '(' .
                               str_replace("')->value", "',null,true,false)->value", $parameter[ 0 ]) . ')';
                    } else {
                        return $func_name . '(' . $parameter[ 0 ] . ')';
                    }
                } else {
                    return $name . '(' . implode(',', $parameter) . ')';
                }
            } else {
                $this->trigger_template_error("unknown function '{$name}'");
            }
        }
    }

    /**
     * Determines whether the passed string represents a valid (PHP) variable.
     * This is important, because `isset()` only works on variables and `empty()` can only be passed
     * a variable prior to php5.5
     * @param $string
     * @return bool
     */
    private function syntaxMatchesVariable($string) {
        static $regex_pattern = '/^\$[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*((->)[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*|\[.*]*\])*$/';
        return 1 === preg_match($regex_pattern, trim($string));
    }

    /**
     * This method is called from parser to process a text content section if strip is enabled
     * - remove text from inheritance child templates as they may generate output
     *
     * @param string $text
     *
     * @return string
     */
    public function processText($text)
    {

        if (strpos($text, '<') === false) {
            return preg_replace($this->stripRegEx, '', $text);
        }

        $store = array();
        $_store = 0;

        // capture html elements not to be messed with
        $_offset = 0;
        if (preg_match_all(
            '#(<script[^>]*>.*?</script[^>]*>)|(<textarea[^>]*>.*?</textarea[^>]*>)|(<pre[^>]*>.*?</pre[^>]*>)#is',
            $text,
            $matches,
            PREG_OFFSET_CAPTURE | PREG_SET_ORDER
        )
        ) {
            foreach ($matches as $match) {
                $store[] = $match[ 0 ][ 0 ];
                $_length = strlen($match[ 0 ][ 0 ]);
                $replace = '@!@SMARTY:' . $_store . ':SMARTY@!@';
                $text = substr_replace($text, $replace, $match[ 0 ][ 1 ] - $_offset, $_length);
                $_offset += $_length - strlen($replace);
                $_store++;
            }
        }
        $expressions = array(// replace multiple spaces between tags by a single space
                             '#(:SMARTY@!@|>)[\040\011]+(?=@!@SMARTY:|<)#s'                            => '\1 \2',
                             // remove newline between tags
                             '#(:SMARTY@!@|>)[\040\011]*[\n]\s*(?=@!@SMARTY:|<)#s'                     => '\1\2',
                             // remove multiple spaces between attributes (but not in attribute values!)
                             '#(([a-z0-9]\s*=\s*("[^"]*?")|(\'[^\']*?\'))|<[a-z0-9_]+)\s+([a-z/>])#is' => '\1 \5',
                             '#>[\040\011]+$#Ss'                                                       => '> ',
                             '#>[\040\011]*[\n]\s*$#Ss'                                                => '>',
                             $this->stripRegEx                                                         => '',
        );
        $text = preg_replace(array_keys($expressions), array_values($expressions), $text);
        $_offset = 0;
        if (preg_match_all(
            '#@!@SMARTY:([0-9]+):SMARTY@!@#is',
            $text,
            $matches,
            PREG_OFFSET_CAPTURE | PREG_SET_ORDER
        )
        ) {
            foreach ($matches as $match) {
                $_length = strlen($match[ 0 ][ 0 ]);
                $replace = $store[ $match[ 1 ][ 0 ] ];
                $text = substr_replace($text, $replace, $match[ 0 ][ 1 ] + $_offset, $_length);
                $_offset += strlen($replace) - $_length;
                $_store++;
            }
        }
        return $text;
    }

    /**
     * lazy loads internal compile plugin for tag and calls the compile method
     * compile objects cached for reuse.
     * class name format:  Smarty_Internal_Compile_TagName
     * plugin filename format: Smarty_Internal_TagName.php
     *
     * @param string $tag    tag name
     * @param array  $args   list of tag attributes
     * @param mixed  $param1 optional parameter
     * @param mixed  $param2 optional parameter
     * @param mixed  $param3 optional parameter
     *
     * @return bool|string compiled code or false
     * @throws \SmartyCompilerException
     */
    public function callTagCompiler($tag, $args, $param1 = null, $param2 = null, $param3 = null)
    {
        /* @var Smarty_Internal_CompileBase $tagCompiler */
        $tagCompiler = $this->getTagCompiler($tag);
        // compile this tag
        return $tagCompiler === false ? false : $tagCompiler->compile($args, $this, $param1, $param2, $param3);
    }

    /**
     * lazy loads internal compile plugin for tag and calls the compile methode
     *
     * compile objects cached for reuse.
     * class name format:  Smarty_Internal_Compile_TagName
     * plugin filename format: Smarty_Internal_Tagname.php
     *
     * @param string $tag   tag name
     * @param array $args   list of tag attributes
     * @param mixed $param1 optional parameter
     * @param mixed $param2 optional parameter
     * @param mixed $param3 optional parameter
     * @return string compiled code
     */
    public function callTagCompiler($tag, $args, $param1 = null, $param2 = null, $param3 = null) {
        // re-use object if already exists
        if (isset(self::$_tag_objects[$tag])) {
            // compile this tag
            return self::$_tag_objects[$tag]->compile($args, $this, $param1, $param2, $param3);
        }
        // lazy load internal compiler plugin
        $class_name = 'Smarty_Internal_Compile_' . $tag;
        if ($this->smarty->loadPlugin($class_name)) {
            // check if tag allowed by security
            if (!isset($this->smarty->security_policy) || $this->smarty->security_policy->isTrustedTag($tag, $this)) {
                // use plugin if found
                self::$_tag_objects[$tag] = new $class_name;
                // compile this tag
                return self::$_tag_objects[$tag]->compile($args, $this, $param1, $param2, $param3);
            }
        }
        // no internal compile plugin for this tag
        return false;
    }

    /**
     * Check for plugins and return function name
     *
     * @param string $pugin_name  name of plugin or function
     * @param string $plugin_type type of plugin
     * @return string call name of function
     */
    public function getPlugin($plugin_name, $plugin_type) {
        $function = null;
        if ($this->template->caching && ($this->nocache || $this->tag_nocache)) {
            if (isset($this->template->required_plugins['nocache'][$plugin_name][$plugin_type])) {
                $function = $this->template->required_plugins['nocache'][$plugin_name][$plugin_type]['function'];
            } else if (isset($this->template->required_plugins['compiled'][$plugin_name][$plugin_type])) {
                $this->template->required_plugins['nocache'][$plugin_name][$plugin_type] = $this->template->required_plugins['compiled'][$plugin_name][$plugin_type];
                $function = $this->template->required_plugins['nocache'][$plugin_name][$plugin_type]['function'];
            }
        } else {
            if (isset($this->template->required_plugins['compiled'][$plugin_name][$plugin_type])) {
                $function = $this->template->required_plugins['compiled'][$plugin_name][$plugin_type]['function'];
            } else if (isset($this->template->required_plugins['nocache'][$plugin_name][$plugin_type])) {
                $this->template->required_plugins['compiled'][$plugin_name][$plugin_type] = $this->template->required_plugins['nocache'][$plugin_name][$plugin_type];
                $function = $this->template->required_plugins['compiled'][$plugin_name][$plugin_type]['function'];
            }
        }
        if (isset($function)) {
            if ($plugin_type == 'modifier') {
                $this->modifier_plugins[$plugin_name] = true;
            }
            return $function;
        }
        // loop through plugin dirs and find the plugin
        $function = 'smarty_' . $plugin_type . '_' . $plugin_name;
        $file = $this->smarty->loadPlugin($function, false);

        if (is_string($file)) {
            if ($this->template->caching && ($this->nocache || $this->tag_nocache)) {
                $this->template->required_plugins['nocache'][$plugin_name][$plugin_type]['file'] = $file;
                $this->template->required_plugins['nocache'][$plugin_name][$plugin_type]['function'] = $function;
            } else {
                $this->template->required_plugins['compiled'][$plugin_name][$plugin_type]['file'] = $file;
                $this->template->required_plugins['compiled'][$plugin_name][$plugin_type]['function'] = $function;
            }
            if ($plugin_type == 'modifier') {
                $this->modifier_plugins[$plugin_name] = true;
            }
            return $function;
        }
        if (is_callable($function)) {
            // plugin function is defined in the script
            return $function;
        }
        return false;
    }

    /**
     * Check for plugins by default plugin handler
     *
     * @param string $tag         name of tag
     * @param string $plugin_type type of plugin
     * @return boolean true if found
     */
    public function getPluginFromDefaultHandler($tag, $plugin_type) {
        $callback = null;
        $script = null;
        $cacheable = true;
        $result = call_user_func_array(
                $this->smarty->default_plugin_handler_func, array($tag, $plugin_type, $this->template, &$callback, &$script, &$cacheable)
        );
        if ($result) {
            $this->tag_nocache = $this->tag_nocache || !$cacheable;
            if ($script !== null) {
                if (is_file($script)) {
                    if ($this->template->caching && ($this->nocache || $this->tag_nocache)) {
                        $this->template->required_plugins['nocache'][$tag][$plugin_type]['file'] = $script;
                        $this->template->required_plugins['nocache'][$tag][$plugin_type]['function'] = $callback;
                    } else {
                        $this->template->required_plugins['compiled'][$tag][$plugin_type]['file'] = $script;
                        $this->template->required_plugins['compiled'][$tag][$plugin_type]['function'] = $callback;
                    }
                    include_once $script;
                } else {
                    $this->trigger_template_error("Default plugin handler: Returned script file \"{$script}\" for \"{$tag}\" not found");
                }
            }
            if (!is_string($callback) && !(is_array($callback) && is_string($callback[0]) && is_string($callback[1]))) {
                $this->trigger_template_error("Default plugin handler: Returned callback for \"{$tag}\" must be a static function name or array of class and function name");
            }
            if (is_callable($callback)) {
                $this->default_handler_plugins[$plugin_type][$tag] = array($callback, true, array());
                return true;
            } else {
                $this->trigger_template_error("Default plugin handler: Returned callback for \"{$tag}\" not callable");
            }
        }
        return false;
    }

    /**
     * Inject inline code for nocache template sections
     *
     * This method gets the content of each template element from the parser.
     * If the content is compiled code and it should be not cached the code is injected
     * into the rendered output.
     *
     * @param string  $content content of template element
     * @param boolean $is_code true if content is compiled code
     * @return string content
     */
    public function processNocacheCode($content, $is_code) {
        // If the template is not evaluated and we have a nocache section and or a nocache tag
        if ($is_code && !empty($content)) {
            // generate replacement code
            if ((!($this->template->source->recompiled) || $this->forceNocache) && $this->template->caching && !$this->suppressNocacheProcessing &&
                    ($this->nocache || $this->tag_nocache || $this->forceNocache == 2)) {
                $this->template->has_nocache_code = true;
                $_output = addcslashes($content,'\'\\');
                $_output = str_replace("^#^", "'", $_output);
                $_output = "<?php echo '/*%%SmartyNocache:{$this->nocache_hash}%%*/" . $_output . "/*/%%SmartyNocache:{$this->nocache_hash}%%*/';?>\n";
                // make sure we include modifier plugins for nocache code
                foreach ($this->modifier_plugins as $plugin_name => $dummy) {
                    if (isset($this->template->required_plugins['compiled'][$plugin_name]['modifier'])) {
                        $this->template->required_plugins['nocache'][$plugin_name]['modifier'] = $this->template->required_plugins['compiled'][$plugin_name]['modifier'];
                    }
                }
            } else {
                $_output = $content;
            }
        } else {
            $_output = $content;
        }
        $this->modifier_plugins = array();
        $this->suppressNocacheProcessing = false;
        $this->tag_nocache = false;
        return $_output;
    }

    /**
     * display compiler error messages without dying
     *
     * If parameter $args is empty it is a parser detected syntax error.
     * In this case the parser is called to obtain information about expected tokens.
     *
     * If parameter $args contains a string this is used as error message
     *
     * @param string $args individual error message or null
     * @param string $line line-number
     * @throws SmartyCompilerException when an unexpected token is found
     */
    public function trigger_template_error($args = null, $line = null) {
        // get template source line which has error
        if (!isset($line)) {
            $line = $this->lex->line;
        }
        $match = preg_split("/\n/", $this->lex->data);
        $error_text = 'Syntax Error in template "' . $this->template->source->filepath . '"  on line ' . $line . ' "' . trim(preg_replace('![\t\r\n]+!', ' ', $match[$line - 1])) . '" ';
        if (isset($args)) {
            // individual error message
            $error_text .= $args;
        } else {
            // expected token from parser
            $error_text .= ' - Unexpected "' . $this->lex->value . '"';
            if (count($this->parser->yy_get_expected_tokens($this->parser->yymajor)) <= 4) {
                foreach ($this->parser->yy_get_expected_tokens($this->parser->yymajor) as $token) {
                    $exp_token = $this->parser->yyTokenName[$token];
                    if (isset($this->lex->smarty_token_names[$exp_token])) {
                        // token type from lexer
                        $expect[] = '"' . $this->lex->smarty_token_names[$exp_token] . '"';
                    } else {
                        // otherwise internal token name
                        $expect[] = $this->parser->yyTokenName[$token];
                    }
                }
                $error_text .= ', expected one of: ' . implode(' , ', $expect);
            }
        }
        if ($this->smarty->_parserdebug) {
            $this->parser->errorRunDown();
            echo ob_get_clean();
            flush();
        }
        $e = new SmartyCompilerException($error_text);
        $e->setLine($line);
        $e->source = trim(preg_replace('![\t\r\n]+!', ' ', $match[ $line - 1 ]));
        $e->desc = $args;
        $e->template = $this->template->source->filepath;
        throw $e;
    }

}

?>