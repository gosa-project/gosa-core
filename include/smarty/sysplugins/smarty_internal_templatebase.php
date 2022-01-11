<?php
/**
 * Smarty Internal Plugin Smarty Template  Base
<<<<<<< HEAD
 *
 * This file contains the basic shared methodes for template handling
=======
 * This file contains the basic shared methods for template handling
>>>>>>> gosa-core_v2.8
 *
 * @package Smarty
 * @subpackage Template
 * @author Uwe Tews
 */

/**
<<<<<<< HEAD
 * Class with shared template methodes
 *
 * @package Smarty
 * @subpackage Template
 */
abstract class Smarty_Internal_TemplateBase extends Smarty_Internal_Data {

    /**
     * fetches a rendered Smarty template
     *
     * @param string $template          the resource handle of the template file or template object
     * @param mixed  $cache_id          cache id to be used with this template
     * @param mixed  $compile_id        compile id to be used with this template
     * @param object $parent            next higher level of Smarty variables
     * @param bool   $display           true: display, false: fetch
     * @param bool   $merge_tpl_vars    if true parent template variables merged in to local scope
     * @param bool   $no_output_filter  if true do not run output filter
     * @return string rendered template output
     */
    public function fetch($template = null, $cache_id = null, $compile_id = null, $parent = null, $display = false, $merge_tpl_vars = true, $no_output_filter = false)
    {
        if ($template === null && $this instanceof $this->template_class) {
            $template = $this;
        }
        if (!empty($cache_id) && is_object($cache_id)) {
            $parent = $cache_id;
            $cache_id = null;
        }
        if ($parent === null && ($this instanceof Smarty || is_string($template))) {
            $parent = $this;
        }
        // create template object if necessary
        $_template = ($template instanceof $this->template_class)
        ? $template
        : $this->smarty->createTemplate($template, $cache_id, $compile_id, $parent, false);
        // if called by Smarty object make sure we use current caching status
        if ($this instanceof Smarty) {
            $_template->caching = $this->caching;
        }
        // merge all variable scopes into template
        if ($merge_tpl_vars) {
            // save local variables
            $save_tpl_vars = $_template->tpl_vars;
            $save_config_vars = $_template->config_vars;
            $ptr_array = array($_template);
            $ptr = $_template;
            while (isset($ptr->parent)) {
                $ptr_array[] = $ptr = $ptr->parent;
            }
            $ptr_array = array_reverse($ptr_array);
            $parent_ptr = reset($ptr_array);
            $tpl_vars = $parent_ptr->tpl_vars;
            $config_vars = $parent_ptr->config_vars;
            while ($parent_ptr = next($ptr_array)) {
                if (!empty($parent_ptr->tpl_vars)) {
                    $tpl_vars = array_merge($tpl_vars, $parent_ptr->tpl_vars);
                }
                if (!empty($parent_ptr->config_vars)) {
                    $config_vars = array_merge($config_vars, $parent_ptr->config_vars);
                }
            }
            if (!empty(Smarty::$global_tpl_vars)) {
                $tpl_vars = array_merge(Smarty::$global_tpl_vars, $tpl_vars);
            }
            $_template->tpl_vars = $tpl_vars;
            $_template->config_vars = $config_vars;
        }
        // dummy local smarty variable
        if (!isset($_template->tpl_vars['smarty'])) {
            $_template->tpl_vars['smarty'] = new Smarty_Variable;
        }
        if (isset($this->smarty->error_reporting)) {
            $_smarty_old_error_level = error_reporting($this->smarty->error_reporting);
        }
        // check URL debugging control
        if (!$this->smarty->debugging && $this->smarty->debugging_ctrl == 'URL') {
            if (isset($_SERVER['QUERY_STRING'])) {
                $_query_string = $_SERVER['QUERY_STRING'];
            } else {
                $_query_string = '';
            }
            if (false !== strpos($_query_string, $this->smarty->smarty_debug_id)) {
                if (false !== strpos($_query_string, $this->smarty->smarty_debug_id . '=on')) {
                    // enable debugging for this browser session
                    setcookie('SMARTY_DEBUG', true);
                    $this->smarty->debugging = true;
                } elseif (false !== strpos($_query_string, $this->smarty->smarty_debug_id . '=off')) {
                    // disable debugging for this browser session
                    setcookie('SMARTY_DEBUG', false);
                    $this->smarty->debugging = false;
                } else {
                    // enable debugging for this page
                    $this->smarty->debugging = true;
                }
            } else {
                if (isset($_COOKIE['SMARTY_DEBUG'])) {
                    $this->smarty->debugging = true;
                }
            }
        }
        // must reset merge template date
        $_template->smarty->merged_templates_func = array();
        // get rendered template
        // disable caching for evaluated code
        if ($_template->source->recompiled) {
            $_template->caching = false;
        }
        // checks if template exists
        if (!$_template->source->exists) {
            if ($_template->parent instanceof Smarty_Internal_Template) {
                $parent_resource = " in '{$_template->parent->template_resource}'";
            } else {
                $parent_resource = '';
            }
            throw new SmartyException("Unable to load template {$_template->source->type} '{$_template->source->name}'{$parent_resource}");
        }
        // read from cache or render
        if (!($_template->caching == Smarty::CACHING_LIFETIME_CURRENT || $_template->caching == Smarty::CACHING_LIFETIME_SAVED) || !$_template->cached->valid) {
            // render template (not loaded and not in cache)
            if (!$_template->source->uncompiled) {
                $_smarty_tpl = $_template;
                if ($_template->source->recompiled) {
                    if ($this->smarty->debugging) {
                        Smarty_Internal_Debug::start_compile($_template);
                    }
                    $code = $_template->compiler->compileTemplate($_template);
                    if ($this->smarty->debugging) {
                        Smarty_Internal_Debug::end_compile($_template);
                    }
                    if ($this->smarty->debugging) {
                        Smarty_Internal_Debug::start_render($_template);
                    }
                    try {
                        ob_start();
                        eval("?>" . $code);
                        unset($code);
                    } catch (Exception $e) {
                        ob_get_clean();
                        throw $e;
                    }
                } else {
                    if (!$_template->compiled->exists || ($_template->smarty->force_compile && !$_template->compiled->isCompiled)) {
                        $_template->compileTemplateSource();
                    }
                    if ($this->smarty->debugging) {
                        Smarty_Internal_Debug::start_render($_template);
                    }
                    if (!$_template->compiled->loaded) {
                        include($_template->compiled->filepath);
                        if ($_template->mustCompile) {
                            // recompile and load again
                            $_template->compileTemplateSource();
                            include($_template->compiled->filepath);
                        }
                        $_template->compiled->loaded = true;
                    } else {
                        $_template->decodeProperties($_template->compiled->_properties, false);
                    }
                    try {
                        ob_start();
                        if (empty($_template->properties['unifunc']) || !is_callable($_template->properties['unifunc'])) {
                            throw new SmartyException("Invalid compiled template for '{$_template->template_resource}'");
                        }
                        array_unshift($_template->_capture_stack,array());
                        //
                        // render compiled template
                        //
                        $_template->properties['unifunc']($_template);
                        // any unclosed {capture} tags ?
                        if (isset($_template->_capture_stack[0][0])) {
                            $_template->capture_error();
                        }
                        array_shift($_template->_capture_stack);
                    } catch (Exception $e) {
                        ob_get_clean();
                        throw $e;
                    }
                }
            } else {
                if ($_template->source->uncompiled) {
                    if ($this->smarty->debugging) {
                        Smarty_Internal_Debug::start_render($_template);
                    }
                    try {
                        ob_start();
                        $_template->source->renderUncompiled($_template);
                    } catch (Exception $e) {
                        ob_get_clean();
                        throw $e;
                    }
                } else {
                    throw new SmartyException("Resource '$_template->source->type' must have 'renderUncompiled' method");
                }
            }
            $_output = ob_get_clean();
            if (!$_template->source->recompiled && empty($_template->properties['file_dependency'][$_template->source->uid])) {
                $_template->properties['file_dependency'][$_template->source->uid] = array($_template->source->filepath, $_template->source->timestamp, $_template->source->type);
            }
            if ($_template->parent instanceof Smarty_Internal_Template) {
                $_template->parent->properties['file_dependency'] = array_merge($_template->parent->properties['file_dependency'], $_template->properties['file_dependency']);
                foreach ($_template->required_plugins as $code => $tmp1) {
                    foreach ($tmp1 as $name => $tmp) {
                        foreach ($tmp as $type => $data) {
                            $_template->parent->required_plugins[$code][$name][$type] = $data;
                        }
                    }
                }
            }
            if ($this->smarty->debugging) {
                Smarty_Internal_Debug::end_render($_template);
            }
            // write to cache when nessecary
            if (!$_template->source->recompiled && ($_template->caching == Smarty::CACHING_LIFETIME_SAVED || $_template->caching == Smarty::CACHING_LIFETIME_CURRENT)) {
                if ($this->smarty->debugging) {
                    Smarty_Internal_Debug::start_cache($_template);
                }
                $_template->properties['has_nocache_code'] = false;
                // get text between non-cached items
                $cache_split = preg_split("!/\*%%SmartyNocache:{$_template->properties['nocache_hash']}%%\*\/(.+?)/\*/%%SmartyNocache:{$_template->properties['nocache_hash']}%%\*/!s", $_output);
                // get non-cached items
                preg_match_all("!/\*%%SmartyNocache:{$_template->properties['nocache_hash']}%%\*\/(.+?)/\*/%%SmartyNocache:{$_template->properties['nocache_hash']}%%\*/!s", $_output, $cache_parts);
                $output = '';
                // loop over items, stitch back together
                foreach ($cache_split as $curr_idx => $curr_split) {
                    // escape PHP tags in template content
                    $output .= preg_replace('/(<%|%>|<\?php|<\?|\?>)/', '<?php echo \'$1\'; ?>', $curr_split);
                    if (isset($cache_parts[0][$curr_idx])) {
                        $_template->properties['has_nocache_code'] = true;
                        // remove nocache tags from cache output
                        $output .= preg_replace("!/\*/?%%SmartyNocache:{$_template->properties['nocache_hash']}%%\*/!", '', $cache_parts[0][$curr_idx]);
                    }
                }
                if (!$no_output_filter && !$_template->has_nocache_code && (isset($this->smarty->autoload_filters['output']) || isset($this->smarty->registered_filters['output']))) {
                    $output = Smarty_Internal_Filter_Handler::runFilter('output', $output, $_template);
                }
                // rendering (must be done before writing cache file because of {function} nocache handling)
                $_smarty_tpl = $_template;
                try {
                    ob_start();
                    eval("?>" . $output);
                    $_output = ob_get_clean();
                } catch (Exception $e) {
                    ob_get_clean();
                    throw $e;
                }
                // write cache file content
                $_template->writeCachedContent($output);
                if ($this->smarty->debugging) {
                    Smarty_Internal_Debug::end_cache($_template);
                }
            } else {
                // var_dump('renderTemplate', $_template->has_nocache_code, $_template->template_resource, $_template->properties['nocache_hash'], $_template->parent->properties['nocache_hash'], $_output);
                if (!empty($_template->properties['nocache_hash']) && !empty($_template->parent->properties['nocache_hash'])) {
                    // replace nocache_hash
                    $_output = str_replace("{$_template->properties['nocache_hash']}", $_template->parent->properties['nocache_hash'], $_output);
                    $_template->parent->has_nocache_code = $_template->parent->has_nocache_code || $_template->has_nocache_code;
                }
            }
        } else {
            if ($this->smarty->debugging) {
                Smarty_Internal_Debug::start_cache($_template);
            }
            try {
                ob_start();
                array_unshift($_template->_capture_stack,array());
                //
                // render cached template
                //
                $_template->properties['unifunc']($_template);
                // any unclosed {capture} tags ?
                if (isset($_template->_capture_stack[0][0])) {
                    $_template->capture_error();
                }
                array_shift($_template->_capture_stack);
                $_output = ob_get_clean();
            } catch (Exception $e) {
                ob_get_clean();
                throw $e;
            }
            if ($this->smarty->debugging) {
                Smarty_Internal_Debug::end_cache($_template);
            }
        }
        if ((!$this->caching || $_template->has_nocache_code || $_template->source->recompiled) && !$no_output_filter && (isset($this->smarty->autoload_filters['output']) || isset($this->smarty->registered_filters['output']))) {
            $_output = Smarty_Internal_Filter_Handler::runFilter('output', $_output, $_template);
        }
        if (isset($this->error_reporting)) {
            error_reporting($_smarty_old_error_level);
        }
        // display or fetch
        if ($display) {
            if ($this->caching && $this->cache_modified_check) {
                $_isCached = $_template->isCached() && !$_template->has_nocache_code;
                $_last_modified_date = @substr($_SERVER['HTTP_IF_MODIFIED_SINCE'], 0, strpos($_SERVER['HTTP_IF_MODIFIED_SINCE'], 'GMT') + 3);
                if ($_isCached && $_template->cached->timestamp <= strtotime($_last_modified_date)) {
                    switch (PHP_SAPI) {
                        case 'cgi':         // php-cgi < 5.3
                        case 'cgi-fcgi':    // php-cgi >= 5.3
                        case 'fpm-fcgi':    // php-fpm >= 5.3.3
                        header('Status: 304 Not Modified');
                        break;

                        case 'cli':
                        if (/* ^phpunit */!empty($_SERVER['SMARTY_PHPUNIT_DISABLE_HEADERS'])/* phpunit$ */) {
                            $_SERVER['SMARTY_PHPUNIT_HEADERS'][] = '304 Not Modified';
                        }
                        break;

                        default:
                        header($_SERVER['SERVER_PROTOCOL'].' 304 Not Modified');
                        break;
                    }
                } else {
                    switch (PHP_SAPI) {
                        case 'cli':
                        if (/* ^phpunit */!empty($_SERVER['SMARTY_PHPUNIT_DISABLE_HEADERS'])/* phpunit$ */) {
                            $_SERVER['SMARTY_PHPUNIT_HEADERS'][] = 'Last-Modified: ' . gmdate('D, d M Y H:i:s', $_template->cached->timestamp) . ' GMT';
                        }
                        break;

                        default:
                        header('Last-Modified: ' . gmdate('D, d M Y H:i:s', $_template->cached->timestamp) . ' GMT');
                        break;
                    }
                    echo $_output;
                }
            } else {
                echo $_output;
            }
            // debug output
            if ($this->smarty->debugging) {
                Smarty_Internal_Debug::display_debug($this);
            }
            if ($merge_tpl_vars) {
                // restore local variables
                $_template->tpl_vars = $save_tpl_vars;
                $_template->config_vars =  $save_config_vars;
            }
            return;
        } else {
            if ($merge_tpl_vars) {
                // restore local variables
                $_template->tpl_vars = $save_tpl_vars;
                $_template->config_vars =  $save_config_vars;
            }
            // return fetched content
            return $_output;
        }
    }

    /**
     * displays a Smarty template
     *
     * @param string $template   the resource handle of the template file or template object
     * @param mixed  $cache_id   cache id to be used with this template
     * @param mixed  $compile_id compile id to be used with this template
     * @param object $parent     next higher level of Smarty variables
     */
    public function display($template = null, $cache_id = null, $compile_id = null, $parent = null)
    {
        // display template
        $this->fetch($template, $cache_id, $compile_id, $parent, true);
    }

    /**
     * test if cache is valid
     *
     * @param string|object $template   the resource handle of the template file or template object
     * @param mixed         $cache_id   cache id to be used with this template
     * @param mixed         $compile_id compile id to be used with this template
     * @param object        $parent     next higher level of Smarty variables
     * @return boolean cache status
     */
    public function isCached($template = null, $cache_id = null, $compile_id = null, $parent = null)
    {
        if ($template === null && $this instanceof $this->template_class) {
            return $this->cached->valid;
        }
        if (!($template instanceof $this->template_class)) {
            if ($parent === null) {
                $parent = $this;
            }
            $template = $this->smarty->createTemplate($template, $cache_id, $compile_id, $parent, false);
        }
        // return cache status of template
        return $template->cached->valid;
    }

    /**
     * creates a data object
     *
     * @param object $parent next higher level of Smarty variables
     * @returns Smarty_Data data object
     */
    public function createData($parent = null)
    {
        return new Smarty_Data($parent, $this);
    }

    /**
     * Registers plugin to be used in templates
     *
     * @param string   $type       plugin type
     * @param string   $tag        name of template tag
     * @param callback $callback   PHP callback to register
     * @param boolean  $cacheable  if true (default) this fuction is cachable
     * @param array    $cache_attr caching attributes if any
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException when the plugin tag is invalid
     */
    public function registerPlugin($type, $tag, $callback, $cacheable = true, $cache_attr = null)
    {
        if (isset($this->smarty->registered_plugins[$type][$tag])) {
            throw new SmartyException("Plugin tag \"{$tag}\" already registered");
        } elseif (!is_callable($callback)) {
            throw new SmartyException("Plugin \"{$tag}\" not callable");
        } else {
            $this->smarty->registered_plugins[$type][$tag] = array($callback, (bool) $cacheable, (array) $cache_attr);
        }

        return $this;
    }

    /**
     * Unregister Plugin
     *
     * @param string $type of plugin
     * @param string $tag name of plugin
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unregisterPlugin($type, $tag)
    {
        if (isset($this->smarty->registered_plugins[$type][$tag])) {
            unset($this->smarty->registered_plugins[$type][$tag]);
        }

        return $this;
    }

    /**
     * Registers a resource to fetch a template
     *
     * @param string $type name of resource type
     * @param Smarty_Resource|array $callback or instance of Smarty_Resource, or array of callbacks to handle resource (deprecated)
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function registerResource($type, $callback)
    {
        $this->smarty->registered_resources[$type] = $callback instanceof Smarty_Resource ? $callback : array($callback, false);
        return $this;
    }

    /**
     * Unregisters a resource
     *
     * @param string $type name of resource type
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unregisterResource($type)
    {
        if (isset($this->smarty->registered_resources[$type])) {
            unset($this->smarty->registered_resources[$type]);
        }

        return $this;
    }

    /**
     * Registers a cache resource to cache a template's output
     *
     * @param string               $type     name of cache resource type
     * @param Smarty_CacheResource $callback instance of Smarty_CacheResource to handle output caching
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function registerCacheResource($type, Smarty_CacheResource $callback)
    {
        $this->smarty->registered_cache_resources[$type] = $callback;
        return $this;
    }

    /**
     * Unregisters a cache resource
     *
     * @param string $type name of cache resource type
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unregisterCacheResource($type)
    {
        if (isset($this->smarty->registered_cache_resources[$type])) {
            unset($this->smarty->registered_cache_resources[$type]);
        }

        return $this;
    }

    /**
     * Registers object to be used in templates
     *
     * @param string  $object        name of template object
     * @param object  $object_impl   the referenced PHP object to register
     * @param array   $allowed       list of allowed methods (empty = all)
     * @param boolean $smarty_args   smarty argument format, else traditional
     * @param array   $block_methods list of block-methods
     * @param array $block_functs list of methods that are block format
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException if any of the methods in $allowed or $block_methods are invalid
     */
    public function registerObject($object_name, $object_impl, $allowed = array(), $smarty_args = true, $block_methods = array())
    {
        // test if allowed methodes callable
        if (!empty($allowed)) {
            foreach ((array) $allowed as $method) {
                if (!is_callable(array($object_impl, $method))) {
                    throw new SmartyException("Undefined method '$method' in registered object");
                }
            }
        }
        // test if block methodes callable
        if (!empty($block_methods)) {
            foreach ((array) $block_methods as $method) {
                if (!is_callable(array($object_impl, $method))) {
                    throw new SmartyException("Undefined method '$method' in registered object");
                }
            }
        }
        // register the object
        $this->smarty->registered_objects[$object_name] =
        array($object_impl, (array) $allowed, (boolean) $smarty_args, (array) $block_methods);
        return $this;
    }

    /**
     * return a reference to a registered object
     *
     * @param string $name object name
     * @return object
     * @throws SmartyException if no such object is found
     */
    public function getRegisteredObject($name)
    {
        if (!isset($this->smarty->registered_objects[$name])) {
            throw new SmartyException("'$name' is not a registered object");
        }
        if (!is_object($this->smarty->registered_objects[$name][0])) {
            throw new SmartyException("registered '$name' is not an object");
        }
        return $this->smarty->registered_objects[$name][0];
    }

    /**
     * unregister an object
     *
     * @param string $name object name
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unregisterObject($name)
    {
        if (isset($this->smarty->registered_objects[$name])) {
            unset($this->smarty->registered_objects[$name]);
        }

        return $this;
    }

    /**
     * Registers static classes to be used in templates
     *
     * @param string $class name of template class
     * @param string $class_impl the referenced PHP class to register
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException if $class_impl does not refer to an existing class
     */
    public function registerClass($class_name, $class_impl)
    {
        // test if exists
        if (!class_exists($class_impl)) {
            throw new SmartyException("Undefined class '$class_impl' in register template class");
        }
        // register the class
        $this->smarty->registered_classes[$class_name] = $class_impl;
        return $this;
    }

    /**
     * Registers a default plugin handler
     *
     * @param callable $callback class/method name
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException if $callback is not callable
     */
    public function registerDefaultPluginHandler($callback)
    {
        if (is_callable($callback)) {
            $this->smarty->default_plugin_handler_func = $callback;
        } else {
            throw new SmartyException("Default plugin handler '$callback' not callable");
        }

        return $this;
    }

    /**
     * Registers a default template handler
     *
     * @param callable $callback class/method name
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException if $callback is not callable
     */
    public function registerDefaultTemplateHandler($callback)
    {
        if (is_callable($callback)) {
            $this->smarty->default_template_handler_func = $callback;
        } else {
            throw new SmartyException("Default template handler '$callback' not callable");
        }

        return $this;
    }

    /**
     * Registers a default template handler
     *
     * @param callable $callback class/method name
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     * @throws SmartyException if $callback is not callable
     */
    public function registerDefaultConfigHandler($callback)
    {
        if (is_callable($callback)) {
            $this->smarty->default_config_handler_func = $callback;
        } else {
            throw new SmartyException("Default config handler '$callback' not callable");
        }

        return $this;
=======
 * Class with shared smarty/template methods
 *
 * @package    Smarty
 * @subpackage Template
 *
 * @property int $_objType
 *
 * The following methods will be dynamically loaded by the extension handler when they are called.
 * They are located in a corresponding Smarty_Internal_Method_xxxx class
 *
 * @method Smarty_Internal_TemplateBase addAutoloadFilters(mixed $filters, string $type = null)
 * @method Smarty_Internal_TemplateBase addDefaultModifiers(mixed $modifiers)
 * @method Smarty_Internal_TemplateBase addLiterals(mixed $literals)
 * @method Smarty_Internal_TemplateBase createData(Smarty_Internal_Data $parent = null, string $name = null)
 * @method array getAutoloadFilters(string $type = null)
 * @method string getDebugTemplate()
 * @method array getDefaultModifier()
 * @method array getLiterals()
 * @method array getTags(mixed $template = null)
 * @method object getRegisteredObject(string $object_name)
 * @method Smarty_Internal_TemplateBase registerCacheResource(string $name, Smarty_CacheResource $resource_handler)
 * @method Smarty_Internal_TemplateBase registerClass(string $class_name, string $class_impl)
 * @method Smarty_Internal_TemplateBase registerDefaultConfigHandler(callback $callback)
 * @method Smarty_Internal_TemplateBase registerDefaultPluginHandler(callback $callback)
 * @method Smarty_Internal_TemplateBase registerDefaultTemplateHandler(callback $callback)
 * @method Smarty_Internal_TemplateBase registerResource(string $name, mixed $resource_handler)
 * @method Smarty_Internal_TemplateBase setAutoloadFilters(mixed $filters, string $type = null)
 * @method Smarty_Internal_TemplateBase setDebugTemplate(string $tpl_name)
 * @method Smarty_Internal_TemplateBase setDefaultModifiers(mixed $modifiers)
 * @method Smarty_Internal_TemplateBase setLiterals(mixed $literals)
 * @method Smarty_Internal_TemplateBase unloadFilter(string $type, string $name)
 * @method Smarty_Internal_TemplateBase unregisterCacheResource(string $name)
 * @method Smarty_Internal_TemplateBase unregisterObject(string $object_name)
 * @method Smarty_Internal_TemplateBase unregisterPlugin(string $type, string $name)
 * @method Smarty_Internal_TemplateBase unregisterFilter(string $type, mixed $callback)
 * @method Smarty_Internal_TemplateBase unregisterResource(string $name)
 */
abstract class Smarty_Internal_TemplateBase extends Smarty_Internal_Data
{
    /**
     * Set this if you want different sets of cache files for the same
     * templates.
     *
     * @var string
     */
    public $cache_id = null;

    /**
     * Set this if you want different sets of compiled files for the same
     * templates.
     *
     * @var string
     */
    public $compile_id = null;

    /**
     * caching enabled
     *
     * @var int
     */
    public $caching = Smarty::CACHING_OFF;

    /**
     * check template for modifications?
     *
     * @var int
     */
    public $compile_check = Smarty::COMPILECHECK_ON;

    /**
     * cache lifetime in seconds
     *
     * @var integer
     */
    public $cache_lifetime = 3600;

    /**
     * Array of source information for known template functions
     *
     * @var array
     */
    public $tplFunctions = array();

    /**
     * universal cache
     *
     * @var array()
     */
    public $_cache = array();

    /**
     * fetches a rendered Smarty template
     *
     * @param string $template   the resource handle of the template file or template object
     * @param mixed  $cache_id   cache id to be used with this template
     * @param mixed  $compile_id compile id to be used with this template
     * @param object $parent     next higher level of Smarty variables
     *
     * @throws Exception
     * @throws SmartyException
     * @return string rendered template output
     */
    public function fetch($template = null, $cache_id = null, $compile_id = null, $parent = null)
    {
        $result = $this->_execute($template, $cache_id, $compile_id, $parent, 0);
        return $result === null ? ob_get_clean() : $result;
    }

    /**
     * displays a Smarty template
     *
     * @param string $template   the resource handle of the template file or template object
     * @param mixed  $cache_id   cache id to be used with this template
     * @param mixed  $compile_id compile id to be used with this template
     * @param object $parent     next higher level of Smarty variables
     *
     * @throws \Exception
     * @throws \SmartyException
     */
    public function display($template = null, $cache_id = null, $compile_id = null, $parent = null)
    {
        // display template
        $this->_execute($template, $cache_id, $compile_id, $parent, 1);
    }

    /**
     * test if cache is valid
     *
     * @api  Smarty::isCached()
     * @link http://www.smarty.net/docs/en/api.is.cached.tpl
     *
     * @param null|string|\Smarty_Internal_Template $template   the resource handle of the template file or template
     *                                                          object
     * @param mixed                                 $cache_id   cache id to be used with this template
     * @param mixed                                 $compile_id compile id to be used with this template
     * @param object                                $parent     next higher level of Smarty variables
     *
     * @return bool cache status
     * @throws \Exception
     * @throws \SmartyException
     */
    public function isCached($template = null, $cache_id = null, $compile_id = null, $parent = null)
    {
        return $this->_execute($template, $cache_id, $compile_id, $parent, 2);
    }

    /**
     * fetches a rendered Smarty template
     *
     * @param string $template   the resource handle of the template file or template object
     * @param mixed  $cache_id   cache id to be used with this template
     * @param mixed  $compile_id compile id to be used with this template
     * @param object $parent     next higher level of Smarty variables
     * @param string $function   function type 0 = fetch,  1 = display, 2 = isCache
     *
     * @return mixed
     * @throws \Exception
     * @throws \SmartyException
     */
    private function _execute($template, $cache_id, $compile_id, $parent, $function)
    {
        $smarty = $this->_getSmartyObj();
        $saveVars = true;
        if ($template === null) {
            if (!$this->_isTplObj()) {
                throw new SmartyException($function . '():Missing \'$template\' parameter');
            } else {
                $template = $this;
            }
        } elseif (is_object($template)) {
            /* @var Smarty_Internal_Template $template */
            if (!isset($template->_objType) || !$template->_isTplObj()) {
                throw new SmartyException($function . '():Template object expected');
            }
        } else {
            // get template object
            $saveVars = false;
            $template = $smarty->createTemplate($template, $cache_id, $compile_id, $parent ? $parent : $this, false);
            if ($this->_objType === 1) {
                // set caching in template object
                $template->caching = $this->caching;
            }
        }
        // make sure we have integer values
        $template->caching = (int)$template->caching;
        // fetch template content
        $level = ob_get_level();
        try {
            $_smarty_old_error_level =
                isset($smarty->error_reporting) ? error_reporting($smarty->error_reporting) : null;
            if ($this->_objType === 2) {
                /* @var Smarty_Internal_Template $this */
                $template->tplFunctions = $this->tplFunctions;
                $template->inheritance = $this->inheritance;
            }
            /* @var Smarty_Internal_Template $parent */
            if (isset($parent->_objType) && ($parent->_objType === 2) && !empty($parent->tplFunctions)) {
                $template->tplFunctions = array_merge($parent->tplFunctions, $template->tplFunctions);
            }
            if ($function === 2) {
                if ($template->caching) {
                    // return cache status of template
                    if (!isset($template->cached)) {
                        $template->loadCached();
                    }
                    $result = $template->cached->isCached($template);
                    Smarty_Internal_Template::$isCacheTplObj[ $template->_getTemplateId() ] = $template;
                } else {
                    return false;
                }
            } else {
                if ($saveVars) {
                    $savedTplVars = $template->tpl_vars;
                    $savedConfigVars = $template->config_vars;
                }
                ob_start();
                $template->_mergeVars();
                if (!empty(Smarty::$global_tpl_vars)) {
                    $template->tpl_vars = array_merge(Smarty::$global_tpl_vars, $template->tpl_vars);
                }
                $result = $template->render(false, $function);
                $template->_cleanUp();
                if ($saveVars) {
                    $template->tpl_vars = $savedTplVars;
                    $template->config_vars = $savedConfigVars;
                } else {
                    if (!$function && !isset(Smarty_Internal_Template::$tplObjCache[ $template->templateId ])) {
                        $template->parent = null;
                        $template->tpl_vars = $template->config_vars = array();
                        Smarty_Internal_Template::$tplObjCache[ $template->templateId ] = $template;
                    }
                }
            }
            if (isset($_smarty_old_error_level)) {
                error_reporting($_smarty_old_error_level);
            }
            return $result;
        } catch (Exception $e) {
            while (ob_get_level() > $level) {
                ob_end_clean();
            }
            if (isset($_smarty_old_error_level)) {
                error_reporting($_smarty_old_error_level);
            }
            throw $e;
        }
    }

    /**
     * Registers plugin to be used in templates
     *
     * @api  Smarty::registerPlugin()
     * @link http://www.smarty.net/docs/en/api.register.plugin.tpl
     *
     * @param string   $type       plugin type
     * @param string   $name       name of template tag
     * @param callable $callback   PHP callback to register
     * @param bool     $cacheable  if true (default) this function is cache able
     * @param mixed    $cache_attr caching attributes if any
     *
     * @return \Smarty|\Smarty_Internal_Template
     * @throws \SmartyException
     */
    public function registerPlugin($type, $name, $callback, $cacheable = true, $cache_attr = null)
    {
        return $this->ext->registerPlugin->registerPlugin($this, $type, $name, $callback, $cacheable, $cache_attr);
    }

    /**
     * load a filter of specified type and name
     *
     * @api  Smarty::loadFilter()
     * @link http://www.smarty.net/docs/en/api.load.filter.tpl
     *
     * @param string $type filter type
     * @param string $name filter name
     *
     * @return bool
     * @throws \SmartyException
     */
    public function loadFilter($type, $name)
    {
        return $this->ext->loadFilter->loadFilter($this, $type, $name);
>>>>>>> gosa-core_v2.8
    }

    /**
     * Registers a filter function
     *
<<<<<<< HEAD
     * @param string $type filter type
     * @param callback $callback
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function registerFilter($type, $callback)
    {
        $this->smarty->registered_filters[$type][$this->_get_filter_name($callback)] = $callback;
        return $this;
    }

    /**
     * Unregisters a filter function
     *
     * @param string $type filter type
     * @param callback $callback
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unregisterFilter($type, $callback)
    {
        $name = $this->_get_filter_name($callback);
        if (isset($this->smarty->registered_filters[$type][$name])) {
            unset($this->smarty->registered_filters[$type][$name]);
        }

        return $this;
    }

    /**
     * Return internal filter name
     *
     * @param callback $function_name
     * @return string internal filter name
     */
    public function _get_filter_name($function_name)
    {
        if (is_array($function_name)) {
            $_class_name = (is_object($function_name[0]) ?
            get_class($function_name[0]) : $function_name[0]);
            return $_class_name . '_' . $function_name[1];
        } else {
            return $function_name;
        }
    }

    /**
     * load a filter of specified type and name
     *
     * @param string $type filter type
     * @param string $name filter name
     * @throws SmartyException if filter could not be loaded
     */
    public function loadFilter($type, $name)
    {
        $_plugin = "smarty_{$type}filter_{$name}";
        $_filter_name = $_plugin;
        if ($this->smarty->loadPlugin($_plugin)) {
            if (class_exists($_plugin, false)) {
                $_plugin = array($_plugin, 'execute');
            }
            if (is_callable($_plugin)) {
                $this->smarty->registered_filters[$type][$_filter_name] = $_plugin;
                return true;
            }
        }
        throw new SmartyException("{$type}filter \"{$name}\" not callable");
    }

    /**
     * unload a filter of specified type and name
     *
     * @param string $type filter type
     * @param string $name filter name
     * @return Smarty_Internal_Templatebase current Smarty_Internal_Templatebase (or Smarty or Smarty_Internal_Template) instance for chaining
     */
    public function unloadFilter($type, $name)
    {
        $_filter_name = "smarty_{$type}filter_{$name}";
        if (isset($this->smarty->registered_filters[$type][$_filter_name])) {
            unset ($this->smarty->registered_filters[$type][$_filter_name]);
        }

        return $this;
    }

    /**
     * preg_replace callback to convert camelcase getter/setter to underscore property names
     *
     * @param string $match match string
     * @return string replacemant
     */
    private function replaceCamelcase($match) {
        return "_" . strtolower($match[1]);
    }

    /**
     * Handle unknown class methods
     *
     * @param string $name unknown method-name
     * @param array  $args argument array
     */
    public function __call($name, $args)
    {
        static $_prefixes = array('set' => true, 'get' => true);
        static $_resolved_property_name = array();
        static $_resolved_property_source = array();

        // method of Smarty object?
        if (method_exists($this->smarty, $name)) {
            return call_user_func_array(array($this->smarty, $name), $args);
        }
        // see if this is a set/get for a property
        $first3 = strtolower(substr($name, 0, 3));
        if (isset($_prefixes[$first3]) && isset($name[3]) && $name[3] !== '_') {
            if (isset($_resolved_property_name[$name])) {
                $property_name = $_resolved_property_name[$name];
            } else {
                // try to keep case correct for future PHP 6.0 case-sensitive class methods
                // lcfirst() not available < PHP 5.3.0, so improvise
                $property_name = strtolower(substr($name, 3, 1)) . substr($name, 4);
                // convert camel case to underscored name
                $property_name = preg_replace_callback('/([A-Z])/', array($this,'replaceCamelcase'), $property_name);
                $_resolved_property_name[$name] = $property_name;
            }
            if (isset($_resolved_property_source[$property_name])) {
                $_is_this = $_resolved_property_source[$property_name];
            } else {
                $_is_this = null;
                if (property_exists($this, $property_name)) {
                    $_is_this = true;
                } else if (property_exists($this->smarty, $property_name)) {
                    $_is_this = false;
                }
                $_resolved_property_source[$property_name] = $_is_this;
            }
            if ($_is_this) {
                if ($first3 == 'get')
                return $this->$property_name;
                else
                return $this->$property_name = $args[0];
            } else if ($_is_this === false) {
                if ($first3 == 'get')
                return $this->smarty->$property_name;
                else
                return $this->smarty->$property_name = $args[0];
            } else {
                throw new SmartyException("property '$property_name' does not exist.");
                return false;
            }
        }
        if ($name == 'Smarty') {
            throw new SmartyException("PHP5 requires you to call __construct() instead of Smarty()");
        }
        // must be unknown
        throw new SmartyException("Call of unknown method '$name'.");
    }

}

?>
=======
     * @api  Smarty::registerFilter()
     * @link http://www.smarty.net/docs/en/api.register.filter.tpl
     *
     * @param string      $type filter type
     * @param callable    $callback
     * @param string|null $name optional filter name
     *
     * @return \Smarty|\Smarty_Internal_Template
     * @throws \SmartyException
     */
    public function registerFilter($type, $callback, $name = null)
    {
        return $this->ext->registerFilter->registerFilter($this, $type, $callback, $name);
    }

    /**
     * Registers object to be used in templates
     *
     * @api  Smarty::registerObject()
     * @link http://www.smarty.net/docs/en/api.register.object.tpl
     *
     * @param string $object_name
     * @param object $object                     the referenced PHP object to register
     * @param array  $allowed_methods_properties list of allowed methods (empty = all)
     * @param bool   $format                     smarty argument format, else traditional
     * @param array  $block_methods              list of block-methods
     *
     * @return \Smarty|\Smarty_Internal_Template
     * @throws \SmartyException
     */
    public function registerObject(
        $object_name,
        $object,
        $allowed_methods_properties = array(),
        $format = true,
        $block_methods = array()
    ) {
        return $this->ext->registerObject->registerObject(
            $this,
            $object_name,
            $object,
            $allowed_methods_properties,
            $format,
            $block_methods
        );
    }

    /**
     * @param int $compile_check
     */
    public function setCompileCheck($compile_check)
    {
        $this->compile_check = (int)$compile_check;
    }

    /**
     * @param int $caching
     */
    public function setCaching($caching)
    {
        $this->caching = (int)$caching;
    }

    /**
     * @param int $cache_lifetime
     */
    public function setCacheLifetime($cache_lifetime)
    {
        $this->cache_lifetime = $cache_lifetime;
    }

    /**
     * @param string $compile_id
     */
    public function setCompileId($compile_id)
    {
        $this->compile_id = $compile_id;
    }

    /**
     * @param string $cache_id
     */
    public function setCacheId($cache_id)
    {
        $this->cache_id = $cache_id;
    }
}
>>>>>>> gosa-core_v2.8
