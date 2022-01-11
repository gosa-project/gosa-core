<?php
/**
 * Smarty Internal Plugin Resource File
 *
 * @package Smarty
 * @subpackage TemplateResources
 * @author Uwe Tews
 * @author Rodney Rehm
 */

/**
 * Smarty Internal Plugin Resource File
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Implements the file system as resource for Smarty templates
 *
 * @package Smarty
 * @subpackage TemplateResources
 */
<<<<<<< HEAD
class Smarty_Internal_Resource_File extends Smarty_Resource {

=======
class Smarty_Internal_Resource_File extends Smarty_Resource
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
     * @throws \SmartyException
>>>>>>> gosa-core_v2.8
     */
    public function populate(Smarty_Template_Source $source, Smarty_Internal_Template $_template=null)
    {
        $source->filepath = $this->buildFilepath($source, $_template);
<<<<<<< HEAD

        if ($source->filepath !== false) {
            if (is_object($source->smarty->security_policy)) {
                $source->smarty->security_policy->isTrustedResourceDir($source->filepath);
            }

            $source->uid = sha1($source->filepath);
            if ($source->smarty->compile_check && !isset($source->timestamp)) {
                $source->timestamp = @filemtime($source->filepath);
                $source->exists = !!$source->timestamp;
            }
=======
        if ($source->filepath !== false) {
            if (isset($source->smarty->security_policy) && is_object($source->smarty->security_policy)) {
                $source->smarty->security_policy->isTrustedResourceDir($source->filepath, $source->isConfig);
            }
            $source->exists = true;
            $source->uid = sha1(
                $source->filepath . ($source->isConfig ? $source->smarty->_joined_config_dir :
                    $source->smarty->_joined_template_dir)
            );
            $source->timestamp = filemtime($source->filepath);
        } else {
            $source->timestamp = $source->exists = false;
>>>>>>> gosa-core_v2.8
        }
    }

    /**
     * populate Source Object with timestamp and exists from Resource
     *
     * @param Smarty_Template_Source $source source object
     */
    public function populateTimestamp(Smarty_Template_Source $source)
    {
<<<<<<< HEAD
        $source->timestamp = @filemtime($source->filepath);
        $source->exists = !!$source->timestamp;
=======
        if (!$source->exists) {
            $source->timestamp = $source->exists = is_file($source->filepath);
        }
        if ($source->exists) {
            $source->timestamp = filemtime($source->filepath);
        }
>>>>>>> gosa-core_v2.8
    }

    /**
     * Load template's source from file into current template object
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
<<<<<<< HEAD
        if ($source->timestamp) {
            return file_get_contents($source->filepath);
        }
        if ($source instanceof Smarty_Config_Source) {
            throw new SmartyException("Unable to read config {$source->type} '{$source->name}'");
        }
        throw new SmartyException("Unable to read template {$source->type} '{$source->name}'");
=======
        if ($source->exists) {
            return file_get_contents($source->filepath);
        }
        throw new SmartyException(
            'Unable to read ' . ($source->isConfig ? 'config' : 'template') .
            " {$source->type} '{$source->name}'"
        );
>>>>>>> gosa-core_v2.8
    }

    /**
     * Determine basename for compiled filename
     *
     * @param Smarty_Template_Source $source source object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string resource's basename
     */
    public function getBasename(Smarty_Template_Source $source)
    {
<<<<<<< HEAD
        $_file = $source->name;
        if (($_pos = strpos($_file, ']')) !== false) {
            $_file = substr($_file, $_pos + 1);
        }
        return basename($_file);
    }

}

?>
=======
        return basename($source->filepath);
    }

    /**
     * build template filepath by traversing the template_dir array
     *
     * @param Smarty_Template_Source   $source    source object
     * @param Smarty_Internal_Template $_template template object
     *
     * @return string fully qualified filepath
     * @throws SmartyException
     */
    protected function buildFilepath(Smarty_Template_Source $source, Smarty_Internal_Template $_template = null)
    {
        $file = $source->name;
        // absolute file ?
        if ($file[ 0 ] === '/' || $file[ 1 ] === ':') {
            $file = $source->smarty->_realpath($file, true);
            return is_file($file) ? $file : false;
        }
        // go relative to a given template?
        if ($file[ 0 ] === '.' && $_template && $_template->_isSubTpl()
            && preg_match('#^[.]{1,2}[\\\/]#', $file)
        ) {
            if ($_template->parent->source->type !== 'file' && $_template->parent->source->type !== 'extends'
                && !isset($_template->parent->_cache[ 'allow_relative_path' ])
            ) {
                throw new SmartyException("Template '{$file}' cannot be relative to template of resource type '{$_template->parent->source->type}'");
            }
            // normalize path
            $path =
                $source->smarty->_realpath(dirname($_template->parent->source->filepath) . DIRECTORY_SEPARATOR . $file);
            // files relative to a template only get one shot
            return is_file($path) ? $path : false;
        }
        // normalize DIRECTORY_SEPARATOR
        if (strpos($file, DIRECTORY_SEPARATOR === '/' ? '\\' : '/') !== false) {
            $file = str_replace(DIRECTORY_SEPARATOR === '/' ? '\\' : '/', DIRECTORY_SEPARATOR, $file);
        }
        $_directories = $source->smarty->getTemplateDir(null, $source->isConfig);
        // template_dir index?
        if ($file[ 0 ] === '[' && preg_match('#^\[([^\]]+)\](.+)$#', $file, $fileMatch)) {
            $file = $fileMatch[ 2 ];
            $_indices = explode(',', $fileMatch[ 1 ]);
            $_index_dirs = array();
            foreach ($_indices as $index) {
                $index = trim($index);
                // try string indexes
                if (isset($_directories[ $index ])) {
                    $_index_dirs[] = $_directories[ $index ];
                } elseif (is_numeric($index)) {
                    // try numeric index
                    $index = (int)$index;
                    if (isset($_directories[ $index ])) {
                        $_index_dirs[] = $_directories[ $index ];
                    } else {
                        // try at location index
                        $keys = array_keys($_directories);
                        if (isset($_directories[ $keys[ $index ] ])) {
                            $_index_dirs[] = $_directories[ $keys[ $index ] ];
                        }
                    }
                }
            }
            if (empty($_index_dirs)) {
                // index not found
                return false;
            } else {
                $_directories = $_index_dirs;
            }
        }
        // relative file name?
        foreach ($_directories as $_directory) {
            $path = $_directory . $file;
            if (is_file($path)) {
                return (strpos($path, '.' . DIRECTORY_SEPARATOR) !== false) ? $source->smarty->_realpath($path) : $path;
            }
        }
        if (!isset($_index_dirs)) {
            // Could be relative to cwd
            $path = $source->smarty->_realpath($file, true);
            if (is_file($path)) {
                return $path;
            }
        }
        // Use include path ?
        if ($source->smarty->use_include_path) {
            return $source->smarty->ext->_getIncludePath->getIncludePath($_directories, $file, $source->smarty);
        }
        return false;
    }
}
>>>>>>> gosa-core_v2.8
