<?php
    /**
    * Smarty Internal Plugin CacheResource File
    *
    * @package Smarty
    * @subpackage Cacher
    * @author Uwe Tews
    * @author Rodney Rehm
    */

    /**
    * This class does contain all necessary methods for the HTML cache on file system
<<<<<<< HEAD
    *
=======
>>>>>>> gosa-core_v2.8
    * Implements the file system as resource for the HTML cache Version ussing nocache inserts.
    *
    * @package Smarty
    * @subpackage Cacher
    */
<<<<<<< HEAD
    class Smarty_Internal_CacheResource_File extends Smarty_CacheResource {

=======
class Smarty_Internal_CacheResource_File extends Smarty_CacheResource
{
>>>>>>> gosa-core_v2.8
        /**
        * populate Cached Object with meta data from Resource
        *
        * @param Smarty_Template_Cached   $cached    cached object
        * @param Smarty_Internal_Template $_template template object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
        * @return void
        */
        public function populate(Smarty_Template_Cached $cached, Smarty_Internal_Template $_template)
        {
<<<<<<< HEAD
            $_source_file_path = str_replace(':', '.', $_template->source->filepath);
            $_cache_id = isset($_template->cache_id) ? preg_replace('![^\w\|]+!', '_', $_template->cache_id) : null;
            $_compile_id = isset($_template->compile_id) ? preg_replace('![^\w\|]+!', '_', $_template->compile_id) : null;
            $_filepath = $_template->source->uid;
            // if use_sub_dirs, break file into directories
            if ($_template->smarty->use_sub_dirs) {
                $_filepath = substr($_filepath, 0, 2) . DS
                . substr($_filepath, 2, 2) . DS
                . substr($_filepath, 4, 2) . DS
                . $_filepath;
            }
            $_compile_dir_sep = $_template->smarty->use_sub_dirs ? DS : '^';
            if (isset($_cache_id)) {
                $_cache_id = str_replace('|', $_compile_dir_sep, $_cache_id) . $_compile_dir_sep;
            } else {
                $_cache_id = '';
            }
            if (isset($_compile_id)) {
                $_compile_id = $_compile_id . $_compile_dir_sep;
            } else {
                $_compile_id = '';
            }
            $_cache_dir = $_template->smarty->getCacheDir();
            if ($_template->smarty->cache_locking) {
                // create locking file name
                // relative file name?
                if (!preg_match('/^([\/\\\\]|[a-zA-Z]:[\/\\\\])/', $_cache_dir)) {
                    $_lock_dir = rtrim(getcwd(), '/\\') . DS . $_cache_dir;
                } else {
                    $_lock_dir = $_cache_dir;
                }
                $cached->lock_id = $_lock_dir.sha1($_cache_id.$_compile_id.$_template->source->uid).'.lock';
            }
            $cached->filepath = $_cache_dir . $_cache_id . $_compile_id . $_filepath . '.' . basename($_source_file_path) . '.php';
            $cached->timestamp = @filemtime($cached->filepath);
            $cached->exists = !!$cached->timestamp;
=======
        $source = &$_template->source;
        $smarty = &$_template->smarty;
        $_compile_dir_sep = $smarty->use_sub_dirs ? DIRECTORY_SEPARATOR : '^';
        $_filepath = sha1($source->uid . $smarty->_joined_template_dir);
        $cached->filepath = $smarty->getCacheDir();
        if (isset($_template->cache_id)) {
            $cached->filepath .= preg_replace(
                                     array(
                                         '![^\w|]+!',
                                         '![|]+!'
                                     ),
                                     array(
                                         '_',
                                         $_compile_dir_sep
                                     ),
                                     $_template->cache_id
                                 ) . $_compile_dir_sep;
        }
        if (isset($_template->compile_id)) {
            $cached->filepath .= preg_replace('![^\w]+!', '_', $_template->compile_id) . $_compile_dir_sep;
        }
        // if use_sub_dirs, break file into directories
        if ($smarty->use_sub_dirs) {
            $cached->filepath .= $_filepath[ 0 ] . $_filepath[ 1 ] . DIRECTORY_SEPARATOR . $_filepath[ 2 ] .
                                 $_filepath[ 3 ] .
                                 DIRECTORY_SEPARATOR .
                                 $_filepath[ 4 ] . $_filepath[ 5 ] . DIRECTORY_SEPARATOR;
        }
        $cached->filepath .= $_filepath;
        $basename = $source->handler->getBasename($source);
        if (!empty($basename)) {
            $cached->filepath .= '.' . $basename;
        }
        if ($smarty->cache_locking) {
            $cached->lock_id = $cached->filepath . '.lock';
        }
        $cached->filepath .= '.php';
        $cached->timestamp = $cached->exists = is_file($cached->filepath);
        if ($cached->exists) {
            $cached->timestamp = filemtime($cached->filepath);
        }
>>>>>>> gosa-core_v2.8
        }

        /**
        * populate Cached Object with timestamp and exists from Resource
        *
        * @param Smarty_Template_Cached $cached cached object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
        * @return void
        */
        public function populateTimestamp(Smarty_Template_Cached $cached)
        {
<<<<<<< HEAD
            $cached->timestamp = @filemtime($cached->filepath);
            $cached->exists = !!$cached->timestamp;
=======
        $cached->timestamp = $cached->exists = is_file($cached->filepath);
        if ($cached->exists) {
            $cached->timestamp = filemtime($cached->filepath);
        }
>>>>>>> gosa-core_v2.8
        }

        /**
        * Read the cached template and process its header
        *
<<<<<<< HEAD
        * @param Smarty_Internal_Template $_template template object
        * @param Smarty_Template_Cached $cached cached object
        * @return booelan true or false if the cached content does not exist
        */
        public function process(Smarty_Internal_Template $_template, Smarty_Template_Cached $cached=null)
        {
            $_smarty_tpl = $_template;
            return @include $_template->cached->filepath;
=======
     * @param \Smarty_Internal_Template $_smarty_tpl do not change variable name, is used by compiled template
     * @param Smarty_Template_Cached    $cached      cached object
     * @param bool                      $update      flag if called because cache update
     *
     * @return boolean true or false if the cached content does not exist
     */
    public function process(
        Smarty_Internal_Template $_smarty_tpl,
        Smarty_Template_Cached $cached = null,
        $update = false
    ) {
        $_smarty_tpl->cached->valid = false;
        if ($update && defined('HHVM_VERSION')) {
            eval('?>' . file_get_contents($_smarty_tpl->cached->filepath));
            return true;
        } else {
            return @include $_smarty_tpl->cached->filepath;
        }
>>>>>>> gosa-core_v2.8
        }

        /**
        * Write the rendered template output to cache
        *
        * @param Smarty_Internal_Template $_template template object
        * @param string                   $content   content to cache
<<<<<<< HEAD
        * @return boolean success
        */
        public function writeCachedContent(Smarty_Internal_Template $_template, $content)
        {
            if (Smarty_Internal_Write_File::writeFile($_template->cached->filepath, $content, $_template->smarty) === true) {
                $_template->cached->timestamp = @filemtime($_template->cached->filepath);
                $_template->cached->exists = !!$_template->cached->timestamp;
                if ($_template->cached->exists) {
=======
     *
     * @return bool success
     * @throws \SmartyException
     */
    public function writeCachedContent(Smarty_Internal_Template $_template, $content)
    {
        if ($_template->smarty->ext->_writeFile->writeFile(
                $_template->cached->filepath,
                $content,
                $_template->smarty
            ) === true
        ) {
            if (function_exists('opcache_invalidate')
                && (!function_exists('ini_get') || strlen(ini_get('opcache.restrict_api'))) < 1
            ) {
                opcache_invalidate($_template->cached->filepath, true);
            } elseif (function_exists('apc_compile_file')) {
                apc_compile_file($_template->cached->filepath);
            }
            $cached = $_template->cached;
            $cached->timestamp = $cached->exists = is_file($cached->filepath);
            if ($cached->exists) {
                $cached->timestamp = filemtime($cached->filepath);
>>>>>>> gosa-core_v2.8
                    return true;
                }
            }
            return false;
        }

        /**
<<<<<<< HEAD
        * Empty cache
        *
        * @param Smarty_Internal_Template $_template template object
        * @param integer                  $exp_time  expiration time (number of seconds, not timestamp)
=======
     * Read cached template from cache
     *
     * @param Smarty_Internal_Template $_template template object
     *
     * @return string  content
     */
    public function readCachedContent(Smarty_Internal_Template $_template)
    {
        if (is_file($_template->cached->filepath)) {
            return file_get_contents($_template->cached->filepath);
        }
        return false;
    }

    /**
     * Empty cache
     *
     * @param Smarty  $smarty
     * @param integer $exp_time expiration time (number of seconds, not timestamp)
     *
>>>>>>> gosa-core_v2.8
        * @return integer number of cache files deleted
        */
        public function clearAll(Smarty $smarty, $exp_time = null)
        {
<<<<<<< HEAD
            return $this->clear($smarty, null, null, null, $exp_time);
=======
        return $smarty->ext->_cacheResourceFile->clear($smarty, null, null, null, $exp_time);
>>>>>>> gosa-core_v2.8
        }

        /**
        * Empty cache for a specific template
        *
<<<<<<< HEAD
        * @param Smarty  $_template     template object
=======
     * @param Smarty  $smarty
>>>>>>> gosa-core_v2.8
        * @param string  $resource_name template name
        * @param string  $cache_id      cache id
        * @param string  $compile_id    compile id
        * @param integer $exp_time      expiration time (number of seconds, not timestamp)
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
        * @return integer number of cache files deleted
        */
        public function clear(Smarty $smarty, $resource_name, $cache_id, $compile_id, $exp_time)
        {
<<<<<<< HEAD
            $_cache_id = isset($cache_id) ? preg_replace('![^\w\|]+!', '_', $cache_id) : null;
            $_compile_id = isset($compile_id) ? preg_replace('![^\w\|]+!', '_', $compile_id) : null;
            $_dir_sep = $smarty->use_sub_dirs ? '/' : '^';
            $_compile_id_offset = $smarty->use_sub_dirs ? 3 : 0;
            $_dir = $smarty->getCacheDir();
            $_dir_length = strlen($_dir);
            if (isset($_cache_id)) {
                $_cache_id_parts = explode('|', $_cache_id);
                $_cache_id_parts_count = count($_cache_id_parts);
                if ($smarty->use_sub_dirs) {
                    foreach ($_cache_id_parts as $id_part) {
                        $_dir .= $id_part . DS;
                    }
                }
            }
            if (isset($resource_name)) {
                $_save_stat = $smarty->caching;
                $smarty->caching = true;
                $tpl = new $smarty->template_class($resource_name, $smarty);
                $smarty->caching = $_save_stat;

                // remove from template cache
                $tpl->source; // have the template registered before unset()
                if ($smarty->allow_ambiguous_resources) {
                    $_templateId = $tpl->source->unique_resource . $tpl->cache_id . $tpl->compile_id;
                } else {
                    $_templateId = $smarty->joined_template_dir . '#' . $resource_name . $tpl->cache_id . $tpl->compile_id;
                }
                if (isset($_templateId[150])) {
                    $_templateId = sha1($_templateId);
                }
                unset($smarty->template_objects[$_templateId]);

                if ($tpl->source->exists) {
                    $_resourcename_parts = basename(str_replace('^', '/', $tpl->cached->filepath));
                } else {
                    return 0;
                }
            }
            $_count = 0;
            $_time = time();
            if (file_exists($_dir)) {
                $_cacheDirs = new RecursiveDirectoryIterator($_dir);
                $_cache = new RecursiveIteratorIterator($_cacheDirs, RecursiveIteratorIterator::CHILD_FIRST);
                foreach ($_cache as $_file) {
                    if (substr(basename($_file->getPathname()),0,1) == '.' || strpos($_file, '.svn') !== false) continue;
                    // directory ?
                    if ($_file->isDir()) {
                        if (!$_cache->isDot()) {
                            // delete folder if empty
                            @rmdir($_file->getPathname());
                        }
                    } else {
                        $_parts = explode($_dir_sep, str_replace('\\', '/', substr((string)$_file, $_dir_length)));
                        $_parts_count = count($_parts);
                        // check name
                        if (isset($resource_name)) {
                            if ($_parts[$_parts_count-1] != $_resourcename_parts) {
                                continue;
                            }
                        }
                        // check compile id
                        if (isset($_compile_id) && (!isset($_parts[$_parts_count-2 - $_compile_id_offset]) || $_parts[$_parts_count-2 - $_compile_id_offset] != $_compile_id)) {
                            continue;
                        }
                        // check cache id
                        if (isset($_cache_id)) {
                            // count of cache id parts
                            $_parts_count = (isset($_compile_id)) ? $_parts_count - 2 - $_compile_id_offset : $_parts_count - 1 - $_compile_id_offset;
                            if ($_parts_count < $_cache_id_parts_count) {
                                continue;
                            }
                            for ($i = 0; $i < $_cache_id_parts_count; $i++) {
                                if ($_parts[$i] != $_cache_id_parts[$i]) continue 2;
                            }
                        }
                        // expired ? 
                        if  (isset($exp_time)) {
                            if ($exp_time < 0) {
                                preg_match('#\'cache_lifetime\' =>\s*(\d*)#', file_get_contents($_file), $match); 
                                if ($_time < (@filemtime($_file) + $match[1])) { 
                                    continue; 
                                }
                            } else {
                                if ($_time - @filemtime($_file) < $exp_time) {
                                    continue;
                                } 
                            }
                        }
                        $_count += @unlink((string) $_file) ? 1 : 0;
                    }
                }
            }
            return $_count;
=======
        return $smarty->ext->_cacheResourceFile->clear($smarty, $resource_name, $cache_id, $compile_id, $exp_time);
>>>>>>> gosa-core_v2.8
        }

        /**
        * Check is cache is locked for this template
        *
        * @param Smarty $smarty Smarty object
        * @param Smarty_Template_Cached $cached cached object
<<<<<<< HEAD
        * @return booelan true or false if cache is locked
=======
     *
     * @return boolean true or false if cache is locked
>>>>>>> gosa-core_v2.8
        */
        public function hasLock(Smarty $smarty, Smarty_Template_Cached $cached)
        {
            if (version_compare(PHP_VERSION, '5.3.0', '>=')) {
                clearstatcache(true, $cached->lock_id);
            } else {
                clearstatcache();
            }
<<<<<<< HEAD
            $t = @filemtime($cached->lock_id);
            return $t && (time() - $t < $smarty->locking_timeout);
=======
        if (is_file($cached->lock_id)) {
            $t = filemtime($cached->lock_id);
            return $t && (time() - $t < $smarty->locking_timeout);
        } else {
            return false;
        }
>>>>>>> gosa-core_v2.8
        }

        /**
        * Lock cache for this template
        *
        * @param Smarty $smarty Smarty object
        * @param Smarty_Template_Cached $cached cached object
<<<<<<< HEAD
=======
     *
     * @return bool|void
>>>>>>> gosa-core_v2.8
        */
        public function acquireLock(Smarty $smarty, Smarty_Template_Cached $cached)
        {
            $cached->is_locked = true;
            touch($cached->lock_id);
        }

        /**
        * Unlock cache for this template
        *
        * @param Smarty $smarty Smarty object
        * @param Smarty_Template_Cached $cached cached object
<<<<<<< HEAD
=======
     *
     * @return bool|void
>>>>>>> gosa-core_v2.8
        */
        public function releaseLock(Smarty $smarty, Smarty_Template_Cached $cached)
        {
            $cached->is_locked = false;
            @unlink($cached->lock_id);
        }
    }
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
