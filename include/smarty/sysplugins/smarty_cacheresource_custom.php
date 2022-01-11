<?php
/**
 * Smarty Internal Plugin
 *
 * @package Smarty
 * @subpackage Cacher
 */

/**
 * Cache Handler API
 *
 * @package Smarty
 * @subpackage Cacher
 * @author Rodney Rehm
 */
<<<<<<< HEAD
abstract class Smarty_CacheResource_Custom extends Smarty_CacheResource {

=======
abstract class Smarty_CacheResource_Custom extends Smarty_CacheResource
{
>>>>>>> gosa-core_v2.8
    /**
     * fetch cached content and its modification time from data source
     *
     * @param string $id         unique cache content identifier
     * @param string $name       template name
     * @param string $cache_id   cache id
     * @param string $compile_id compile id
     * @param string $content    cached content
     * @param integer $mtime cache modification timestamp (epoch)
<<<<<<< HEAD
     * @return void
     */
    protected abstract function fetch($id, $name, $cache_id, $compile_id, &$content, &$mtime);

    /**
     * Fetch cached content's modification timestamp from data source
     *
=======
     *
     * @return void
     */
    abstract protected function fetch($id, $name, $cache_id, $compile_id, &$content, &$mtime);

    /**
     * Fetch cached content's modification timestamp from data source
>>>>>>> gosa-core_v2.8
     * {@internal implementing this method is optional.
     *  Only implement it if modification times can be accessed faster than loading the complete cached content.}}
     *
     * @param string $id         unique cache content identifier
     * @param string $name       template name
     * @param string $cache_id   cache id
     * @param string $compile_id compile id
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return integer|boolean timestamp (epoch) the template was modified, or false if not found
     */
    protected function fetchTimestamp($id, $name, $cache_id, $compile_id)
    {
<<<<<<< HEAD
        return null;
=======
        return false;
>>>>>>> gosa-core_v2.8
    }

    /**
     * Save content to cache
     *
     * @param string       $id         unique cache content identifier
     * @param string       $name       template name
     * @param string       $cache_id   cache id
     * @param string       $compile_id compile id
     * @param integer|null $exp_time   seconds till expiration or null
     * @param string $content content to cache
<<<<<<< HEAD
     * @return boolean success
     */
    protected abstract function save($id, $name, $cache_id, $compile_id, $exp_time, $content);
=======
     *
     * @return boolean      success
     */
    abstract protected function save($id, $name, $cache_id, $compile_id, $exp_time, $content);
>>>>>>> gosa-core_v2.8

    /**
     * Delete content from cache
     *
<<<<<<< HEAD
     * @param string       $name       template name
     * @param string       $cache_id   cache id
     * @param string       $compile_id compile id
     * @param integer|null $exp_time   seconds till expiration time in seconds or null
     * @return integer number of deleted caches
     */
    protected abstract function delete($name, $cache_id, $compile_id, $exp_time);
=======
     * @param string|null  $name       template name
     * @param string|null  $cache_id   cache id
     * @param string|null  $compile_id compile id
     * @param integer|null $exp_time   seconds till expiration time in seconds or null
     *
     * @return integer      number of deleted caches
     */
    abstract protected function delete($name, $cache_id, $compile_id, $exp_time);
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
        $_cache_id = isset($cached->cache_id) ? preg_replace('![^\w\|]+!', '_', $cached->cache_id) : null;
<<<<<<< HEAD
        $_compile_id = isset($cached->compile_id) ? preg_replace('![^\w\|]+!', '_', $cached->compile_id) : null;

        $cached->filepath = sha1($cached->source->filepath . $_cache_id . $_compile_id);
=======
        $_compile_id = isset($cached->compile_id) ? preg_replace('![^\w]+!', '_', $cached->compile_id) : null;
        $path = $cached->source->uid . $_cache_id . $_compile_id;
        $cached->filepath = sha1($path);
        if ($_template->smarty->cache_locking) {
            $cached->lock_id = sha1('lock.' . $path);
        }
>>>>>>> gosa-core_v2.8
        $this->populateTimestamp($cached);
    }

    /**
     * populate Cached Object with timestamp and exists from Resource
     *
<<<<<<< HEAD
     * @param Smarty_Template_Cached $source cached object
=======
     * @param Smarty_Template_Cached $cached
     *
>>>>>>> gosa-core_v2.8
     * @return void
     */
    public function populateTimestamp(Smarty_Template_Cached $cached)
    {
<<<<<<< HEAD
        $mtime = $this->fetchTimestamp($cached->filepath, $cached->source->name, $cached->cache_id, $cached->compile_id);
=======
        $mtime =
            $this->fetchTimestamp($cached->filepath, $cached->source->name, $cached->cache_id, $cached->compile_id);
>>>>>>> gosa-core_v2.8
        if ($mtime !== null) {
            $cached->timestamp = $mtime;
            $cached->exists = !!$cached->timestamp;
            return;
        }
        $timestamp = null;
<<<<<<< HEAD
        $this->fetch($cached->filepath, $cached->source->name, $cached->cache_id, $cached->compile_id, $cached->content, $timestamp);
=======
        $this->fetch(
            $cached->filepath,
            $cached->source->name,
            $cached->cache_id,
            $cached->compile_id,
            $cached->content,
            $timestamp
        );
>>>>>>> gosa-core_v2.8
        $cached->timestamp = isset($timestamp) ? $timestamp : false;
        $cached->exists = !!$cached->timestamp;
    }

    /**
     * Read the cached template and process the header
     *
<<<<<<< HEAD
     * @param Smarty_Internal_Template $_template template object
     * @param Smarty_Template_Cached $cached cached object
     * @return booelan true or false if the cached content does not exist
     */
    public function process(Smarty_Internal_Template $_template, Smarty_Template_Cached $cached=null)
    {
        if (!$cached) {
            $cached = $_template->cached;
=======
     * @param \Smarty_Internal_Template $_smarty_tpl do not change variable name, is used by compiled template
     * @param Smarty_Template_Cached    $cached      cached object
     * @param boolean                   $update      flag if called because cache update
     *
     * @return boolean                 true or false if the cached content does not exist
     */
    public function process(
        Smarty_Internal_Template $_smarty_tpl,
        Smarty_Template_Cached $cached = null,
        $update = false
    ) {
        if (!$cached) {
            $cached = $_smarty_tpl->cached;
>>>>>>> gosa-core_v2.8
        }
        $content = $cached->content ? $cached->content : null;
        $timestamp = $cached->timestamp ? $cached->timestamp : null;
        if ($content === null || !$timestamp) {
            $this->fetch(
<<<<<<< HEAD
                $_template->cached->filepath,
                $_template->source->name,
                $_template->cache_id,
                $_template->compile_id,
=======
                $_smarty_tpl->cached->filepath,
                $_smarty_tpl->source->name,
                $_smarty_tpl->cache_id,
                $_smarty_tpl->compile_id,
>>>>>>> gosa-core_v2.8
                $content,
                $timestamp
            );
        }
        if (isset($content)) {
<<<<<<< HEAD
            $_smarty_tpl = $_template;
            eval("?>" . $content);
=======
            eval('?>' . $content);
            $cached->content = null;
>>>>>>> gosa-core_v2.8
            return true;
        }
        return false;
    }

    /**
     * Write the rendered template output to cache
     *
     * @param Smarty_Internal_Template $_template template object
     * @param string                   $content   content to cache
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return boolean success
     */
    public function writeCachedContent(Smarty_Internal_Template $_template, $content)
    {
        return $this->save(
            $_template->cached->filepath,
            $_template->source->name,
            $_template->cache_id,
            $_template->compile_id,
<<<<<<< HEAD
            $_template->properties['cache_lifetime'],
=======
            $_template->cache_lifetime,
>>>>>>> gosa-core_v2.8
            $content
        );
    }

    /**
<<<<<<< HEAD
=======
     * Read cached template from cache
     *
     * @param Smarty_Internal_Template $_template template object
     *
     * @return string|boolean  content
     */
    public function readCachedContent(Smarty_Internal_Template $_template)
    {
        $content = $_template->cached->content ? $_template->cached->content : null;
        $timestamp = null;
        if ($content === null) {
            $timestamp = null;
            $this->fetch(
                $_template->cached->filepath,
                $_template->source->name,
                $_template->cache_id,
                $_template->compile_id,
                $content,
                $timestamp
            );
        }
        if (isset($content)) {
            return $content;
        }
        return false;
    }

    /**
>>>>>>> gosa-core_v2.8
     * Empty cache
     *
     * @param Smarty  $smarty   Smarty object
     * @param integer $exp_time expiration time (number of seconds, not timestamp)
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return integer number of cache files deleted
     */
    public function clearAll(Smarty $smarty, $exp_time=null)
    {
<<<<<<< HEAD
        $this->cache = array();
=======
>>>>>>> gosa-core_v2.8
        return $this->delete(null, null, null, $exp_time);
    }

    /**
     * Empty cache for a specific template
     *
     * @param Smarty  $smarty        Smarty object
     * @param string  $resource_name template name
     * @param string  $cache_id      cache id
     * @param string  $compile_id    compile id
     * @param integer $exp_time      expiration time (number of seconds, not timestamp)
<<<<<<< HEAD
     * @return integer number of cache files deleted
     */
    public function clear(Smarty $smarty, $resource_name, $cache_id, $compile_id, $exp_time)
    {
        $this->cache = array();
        return $this->delete($resource_name, $cache_id, $compile_id, $exp_time);
=======
     *
     * @return int number of cache files deleted
     * @throws \SmartyException
     */
    public function clear(Smarty $smarty, $resource_name, $cache_id, $compile_id, $exp_time)
    {
        $cache_name = null;
        if (isset($resource_name)) {
            $source = Smarty_Template_Source::load(null, $smarty, $resource_name);
            if ($source->exists) {
                $cache_name = $source->name;
            } else {
                return 0;
            }
        }
        return $this->delete($cache_name, $cache_id, $compile_id, $exp_time);
>>>>>>> gosa-core_v2.8
    }

    /**
     * Check is cache is locked for this template
     *
     * @param Smarty $smarty Smarty object
     * @param Smarty_Template_Cached $cached cached object
<<<<<<< HEAD
     * @return booelan true or false if cache is locked
     */
    public function hasLock(Smarty $smarty, Smarty_Template_Cached $cached)
    {
        $id = $cached->filepath;
        $name = $cached->source->name . '.lock';

        $mtime = $this->fetchTimestamp($id, $name, null, null);
        if ($mtime === null) {
            $this->fetch($id, $name, null, null, $content, $mtime);
        }

        return $mtime && time() - $mtime < $smarty->locking_timeout;
=======
     *
     * @return boolean               true or false if cache is locked
     */
    public function hasLock(Smarty $smarty, Smarty_Template_Cached $cached)
    {
        $id = $cached->lock_id;
        $name = $cached->source->name . '.lock';
        $mtime = $this->fetchTimestamp($id, $name, $cached->cache_id, $cached->compile_id);
        if ($mtime === null) {
            $this->fetch($id, $name, $cached->cache_id, $cached->compile_id, $content, $mtime);
        }
        return $mtime && ($t = time()) - $mtime < $smarty->locking_timeout;
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
<<<<<<< HEAD

        $id = $cached->filepath;
        $name = $cached->source->name . '.lock';
        $this->save($id, $name, null, null, $smarty->locking_timeout, '');
=======
        $id = $cached->lock_id;
        $name = $cached->source->name . '.lock';
        $this->save($id, $name, $cached->cache_id, $cached->compile_id, $smarty->locking_timeout, '');
>>>>>>> gosa-core_v2.8
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
<<<<<<< HEAD

        $name = $cached->source->name . '.lock';
        $this->delete($name, null, null, null);
    }
}
?>
=======
        $name = $cached->source->name . '.lock';
        $this->delete($name, $cached->cache_id, $cached->compile_id, null);
    }
}
>>>>>>> gosa-core_v2.8
