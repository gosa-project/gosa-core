<?php
/**
 * Smarty Resource Plugin
 *
 * @package Smarty
 * @subpackage TemplateResources
 * @author Rodney Rehm
 */

/**
 * Smarty Resource Plugin
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Wrapper Implementation for custom resource plugins
 *
 * @package Smarty
 * @subpackage TemplateResources
 */
<<<<<<< HEAD
abstract class Smarty_Resource_Custom extends Smarty_Resource {

=======
abstract class Smarty_Resource_Custom extends Smarty_Resource
{
>>>>>>> gosa-core_v2.8
    /**
     * fetch template and its modification time from data source
     *
     * @param string  $name    template name
     * @param string  &$source template source
     * @param integer &$mtime  template modification timestamp (epoch)
     */
<<<<<<< HEAD
    protected abstract function fetch($name, &$source, &$mtime);

    /**
     * Fetch template's modification timestamp from data source
     *
=======
    abstract protected function fetch($name, &$source, &$mtime);

    /**
     * Fetch template's modification timestamp from data source
>>>>>>> gosa-core_v2.8
     * {@internal implementing this method is optional.
     *  Only implement it if modification times can be accessed faster than loading the complete template source.}}
     *
     * @param string $name template name
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return integer|boolean timestamp (epoch) the template was modified, or false if not found
     */
    protected function fetchTimestamp($name)
    {
        return null;
    }

    /**
     * populate Source Object with meta data from Resource
     *
     * @param Smarty_Template_Source   $source    source object
     * @param Smarty_Internal_Template $_template template object
     */
    public function populate(Smarty_Template_Source $source, Smarty_Internal_Template $_template=null)
    {
<<<<<<< HEAD
        $source->filepath = strtolower($source->type . ':' . $source->name);
        $source->uid = sha1($source->type . ':' . $source->name);

=======
        $source->filepath = $source->type . ':' . substr(preg_replace('/[^A-Za-z0-9.]/', '', $source->name), 0, 25);
        $source->uid = sha1($source->type . ':' . $source->name);
>>>>>>> gosa-core_v2.8
        $mtime = $this->fetchTimestamp($source->name);
        if ($mtime !== null) {
            $source->timestamp = $mtime;
        } else {
            $this->fetch($source->name, $content, $timestamp);
            $source->timestamp = isset($timestamp) ? $timestamp : false;
<<<<<<< HEAD
            if( isset($content) )
                $source->content = $content;
        }
=======
            if (isset($content)) {
                $source->content = $content;
            }
        }
>>>>>>> gosa-core_v2.8
        $source->exists = !!$source->timestamp;
    }

    /**
     * Load template's source into current template object
     *
     * @param Smarty_Template_Source $source source object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string template source
     * @throws SmartyException if source cannot be loaded
     */
    public function getContent(Smarty_Template_Source $source)
    {
        $this->fetch($source->name, $content, $timestamp);
        if (isset($content)) {
            return $content;
        }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
        throw new SmartyException("Unable to read template {$source->type} '{$source->name}'");
    }

    /**
     * Determine basename for compiled filename
     *
     * @param Smarty_Template_Source $source source object
<<<<<<< HEAD
     * @return string resource's basename
     */
    protected function getBasename(Smarty_Template_Source $source)
    {
        return basename($source->name);
    }

}

?>
=======
     *
     * @return string                 resource's basename
     */
    public function getBasename(Smarty_Template_Source $source)
    {
        return basename(substr(preg_replace('/[^A-Za-z0-9.]/', '', $source->name), 0, 25));
    }
}
>>>>>>> gosa-core_v2.8
