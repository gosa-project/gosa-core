<?php
/**
* Smarty Internal Plugin Resource Stream
<<<<<<< HEAD
*
=======
>>>>>>> gosa-core_v2.8
* Implements the streams as resource for Smarty template
*
* @package Smarty
* @subpackage TemplateResources
* @author Uwe Tews
* @author Rodney Rehm
*/

/**
* Smarty Internal Plugin Resource Stream
<<<<<<< HEAD
*
=======
>>>>>>> gosa-core_v2.8
* Implements the streams as resource for Smarty template
*
* @link http://php.net/streams
* @package Smarty
* @subpackage TemplateResources
*/
<<<<<<< HEAD
class Smarty_Internal_Resource_Stream extends Smarty_Resource_Recompiled {

=======
class Smarty_Internal_Resource_Stream extends Smarty_Resource_Recompiled
{
>>>>>>> gosa-core_v2.8
    /**
    * populate Source Object with meta data from Resource
    *
    * @param Smarty_Template_Source   $source    source object
    * @param Smarty_Internal_Template $_template template object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
    * @return void
    */
    public function populate(Smarty_Template_Source $source, Smarty_Internal_Template $_template=null)
    {
        if(strpos($source->resource, '://') !== false) {
            $source->filepath = $source->resource;
        } else {
            $source->filepath = str_replace(':', '://', $source->resource);
        }
        $source->uid = false;
        $source->content = $this->getContent($source);
<<<<<<< HEAD
        $source->timestamp = false;
        $source->exists = !!$source->content;
=======
        $source->timestamp = $source->exists = !!$source->content;
>>>>>>> gosa-core_v2.8
    }

    /**
    * Load template's source from stream into current template object
    *
    * @param Smarty_Template_Source $source source object
<<<<<<< HEAD
    * @return string template source
    * @throws SmartyException if source cannot be loaded
=======
     *
     * @return string template source
>>>>>>> gosa-core_v2.8
    */
    public function getContent(Smarty_Template_Source $source)
    {
        $t = '';
        // the availability of the stream has already been checked in Smarty_Resource::fetch()
        $fp = fopen($source->filepath, 'r+');
        if ($fp) {
            while (!feof($fp) && ($current_line = fgets($fp)) !== false) {
                $t .= $current_line;
            }
            fclose($fp);
            return $t;
        } else {
            return false;
        }
    }

    /**
    * modify resource_name according to resource handlers specifications
    *
    * @param Smarty $smarty        Smarty instance
    * @param string $resource_name resource_name to make unique
<<<<<<< HEAD
    * @return string unique resource name
    */
    protected function buildUniqueResourceName(Smarty $smarty, $resource_name)
=======
     * @param boolean $isConfig      flag for config resource
     *
     * @return string unique resource name
     */
    public function buildUniqueResourceName(Smarty $smarty, $resource_name, $isConfig = false)
>>>>>>> gosa-core_v2.8
    {
        return get_class($this) . '#' . $resource_name;
    }
}
