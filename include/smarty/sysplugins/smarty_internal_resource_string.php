<?php
/**
 * Smarty Internal Plugin Resource String
 *
 * @package Smarty
 * @subpackage TemplateResources
 * @author Uwe Tews
 * @author Rodney Rehm
 */

/**
 * Smarty Internal Plugin Resource String
<<<<<<< HEAD
 *
 * Implements the strings as resource for Smarty template
 *
=======
 * Implements the strings as resource for Smarty template
>>>>>>> gosa-core_v2.8
 * {@internal unlike eval-resources the compiled state of string-resources is saved for subsequent access}}
 *
 * @package Smarty
 * @subpackage TemplateResources
 */
<<<<<<< HEAD
class Smarty_Internal_Resource_String extends Smarty_Resource {

=======
class Smarty_Internal_Resource_String extends Smarty_Resource
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
<<<<<<< HEAD
        $source->uid = $source->filepath = sha1($source->name);
        $source->timestamp = 0;
        $source->exists = true;
=======
        $source->uid = $source->filepath = sha1($source->name . $source->smarty->_joined_template_dir);
        $source->timestamp = $source->exists = true;
>>>>>>> gosa-core_v2.8
    }

    /**
     * Load template's source from $resource_name into current template object
     *
     * @uses decode() to decode base64 and urlencoded template_resources
<<<<<<< HEAD
     * @param Smarty_Template_Source $source source object
=======
     *
     * @param Smarty_Template_Source $source source object
     *
>>>>>>> gosa-core_v2.8
     * @return string template source
     */
    public function getContent(Smarty_Template_Source $source)
    {
        return $this->decode($source->name);
    }
    
    /**
     * decode base64 and urlencode
     *
     * @param string $string template_resource to decode
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string decoded template_resource
     */
    protected function decode($string)
    {
        // decode if specified
        if (($pos = strpos($string, ':')) !== false) {
            if (!strncmp($string, 'base64', 6)) {
                return base64_decode(substr($string, 7));
            } elseif (!strncmp($string, 'urlencode', 9)) {
                return urldecode(substr($string, 10));
            }
        }
<<<<<<< HEAD
        
=======
>>>>>>> gosa-core_v2.8
        return $string;
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
        return get_class($this) . '#' .$this->decode($resource_name);
    }

    /**
     * Determine basename for compiled filename
<<<<<<< HEAD
     *
     * Always returns an empty string.
     *
     * @param Smarty_Template_Source $source source object
     * @return string resource's basename
     */
    protected function getBasename(Smarty_Template_Source $source)
=======
     * Always returns an empty string.
     *
     * @param Smarty_Template_Source $source source object
     *
     * @return string                 resource's basename
     */
    public function getBasename(Smarty_Template_Source $source)
>>>>>>> gosa-core_v2.8
    {
        return '';
    }

<<<<<<< HEAD
}

?>
=======
    /*
        * Disable timestamp checks for string resource.
        *
        * @return bool
        */
    /**
     * @return bool
     */
    public function checkTimestamps()
    {
        return false;
    }
}
>>>>>>> gosa-core_v2.8
