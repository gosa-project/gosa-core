<?php
/**
 * Smarty plugin
 * 
 * @package Smarty
 * @subpackage PluginsFunction
 */
<<<<<<< HEAD

/**
 * Smarty {html_image} function plugin
 * 
 * Type:     function<br>
 * Name:     html_image<br>
 * Date:     Feb 24, 2003<br>
 * Purpose:  format HTML tags for the image<br>
 * Examples: {html_image file="/images/masthead.gif"}<br>
 * Output:   <img src="/images/masthead.gif" width=400 height=23><br>
 * Params:
 * <pre>
=======
/**
 * Smarty {html_image} function plugin
 * Type:     function
 * Name:     html_image
 * Date:     Feb 24, 2003
 * Purpose:  format HTML tags for the image
 * Examples: {html_image file="/images/masthead.gif"}
 * Output:   <img src="/images/masthead.gif" width=400 height=23>
 * Params:
 *
>>>>>>> gosa-core_v2.8
 * - file        - (required) - file (and path) of image
 * - height      - (optional) - image height (default actual height)
 * - width       - (optional) - image width (default actual width)
 * - basedir     - (optional) - base directory for absolute paths, default is environment variable DOCUMENT_ROOT
 * - path_prefix - prefix for path output (optional, default empty)
<<<<<<< HEAD
 * </pre>
=======
>>>>>>> gosa-core_v2.8
 * 
 * @link http://www.smarty.net/manual/en/language.function.html.image.php {html_image}
 *      (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com> 
 * @author credits to Duda <duda@big.hu> 
 * @version 1.0
<<<<<<< HEAD
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 * @return string 
 * @uses smarty_function_escape_special_chars()
 */
function smarty_function_html_image($params, $template)
{
    require_once(SMARTY_PLUGINS_DIR . 'shared.escape_special_chars.php');
 
=======
 *
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 *
 * @throws SmartyException
 * @return string
 * @uses    smarty_function_escape_special_chars()
 */
function smarty_function_html_image($params, Smarty_Internal_Template $template)
{
    $template->_checkPlugins(
        array(
            array(
                'function' => 'smarty_function_escape_special_chars',
                'file'     => SMARTY_PLUGINS_DIR . 'shared.escape_special_chars.php'
            )
        )
    );
>>>>>>> gosa-core_v2.8
    $alt = '';
    $file = '';
    $height = '';
    $width = '';
    $extra = '';
    $prefix = '';
    $suffix = '';
    $path_prefix = '';
    $basedir = isset($_SERVER['DOCUMENT_ROOT']) ? $_SERVER['DOCUMENT_ROOT'] : '';
    foreach($params as $_key => $_val) {
        switch ($_key) {
            case 'file':
            case 'height':
            case 'width':
            case 'dpi':
            case 'path_prefix':
            case 'basedir':
                $$_key = $_val;
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'alt':
                if (!is_array($_val)) {
                    $$_key = smarty_function_escape_special_chars($_val);
                } else {
<<<<<<< HEAD
                    throw new SmartyException ("html_image: extra attribute '$_key' cannot be an array", E_USER_NOTICE);
                } 
                break;

=======
                    throw new SmartyException(
                        "html_image: extra attribute '{$_key}' cannot be an array",
                        E_USER_NOTICE
                    );
                }
                break;
>>>>>>> gosa-core_v2.8
            case 'link':
            case 'href':
                $prefix = '<a href="' . $_val . '">';
                $suffix = '</a>';
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            default:
                if (!is_array($_val)) {
                    $extra .= ' ' . $_key . '="' . smarty_function_escape_special_chars($_val) . '"';
                } else {
<<<<<<< HEAD
                    throw new SmartyException ("html_image: extra attribute '$_key' cannot be an array", E_USER_NOTICE);
=======
                    throw new SmartyException(
                        "html_image: extra attribute '{$_key}' cannot be an array",
                        E_USER_NOTICE
                    );
>>>>>>> gosa-core_v2.8
                } 
                break;
        } 
    } 
<<<<<<< HEAD

    if (empty($file)) {
        trigger_error("html_image: missing 'file' parameter", E_USER_NOTICE);
        return;
    } 

    if ($file[0] == '/') {
=======
    if (empty($file)) {
        trigger_error('html_image: missing \'file\' parameter', E_USER_NOTICE);
        return;
    }
    if ($file[ 0 ] === '/') {
>>>>>>> gosa-core_v2.8
        $_image_path = $basedir . $file;
    } else {
        $_image_path = $file;
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    // strip file protocol
    if (stripos($params['file'], 'file://') === 0) {
        $params['file'] = substr($params['file'], 7);
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    $protocol = strpos($params['file'], '://');
    if ($protocol !== false) {
        $protocol = strtolower(substr($params['file'], 0, $protocol));
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    if (isset($template->smarty->security_policy)) {
        if ($protocol) {
            // remote resource (or php stream, …)
            if(!$template->smarty->security_policy->isTrustedUri($params['file'])) {
                return;
            }
        } else {
            // local file
<<<<<<< HEAD
            if(!$template->smarty->security_policy->isTrustedResourceDir($params['file'])) {
=======
            if (!$template->smarty->security_policy->isTrustedResourceDir($_image_path)) {
>>>>>>> gosa-core_v2.8
                return;
            }
        }
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if (!isset($params['width']) || !isset($params['height'])) {
        // FIXME: (rodneyrehm) getimagesize() loads the complete file off a remote resource, use custom [jpg,png,gif]header reader!
        if (!$_image_data = @getimagesize($_image_path)) {
            if (!file_exists($_image_path)) {
<<<<<<< HEAD
                trigger_error("html_image: unable to find '$_image_path'", E_USER_NOTICE);
                return;
            } else if (!is_readable($_image_path)) {
                trigger_error("html_image: unable to read '$_image_path'", E_USER_NOTICE);
                return;
            } else {
                trigger_error("html_image: '$_image_path' is not a valid image file", E_USER_NOTICE);
                return;
            } 
        }

=======
                trigger_error("html_image: unable to find '{$_image_path}'", E_USER_NOTICE);
                return;
            } elseif (!is_readable($_image_path)) {
                trigger_error("html_image: unable to read '{$_image_path}'", E_USER_NOTICE);
                return;
            } else {
                trigger_error("html_image: '{$_image_path}' is not a valid image file", E_USER_NOTICE);
                return;
            }
        }
>>>>>>> gosa-core_v2.8
        if (!isset($params['width'])) {
            $width = $_image_data[0];
        } 
        if (!isset($params['height'])) {
            $height = $_image_data[1];
        } 
    } 
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if (isset($params['dpi'])) {
        if (strstr($_SERVER['HTTP_USER_AGENT'], 'Mac')) {
            // FIXME: (rodneyrehm) wrong dpi assumption
            // don't know who thought this up… even if it was true in 1998, it's definitely wrong in 2011.
            $dpi_default = 72;
        } else {
            $dpi_default = 96;
        } 
        $_resize = $dpi_default / $params['dpi'];
        $width = round($width * $_resize);
        $height = round($height * $_resize);
    } 
<<<<<<< HEAD

    return $prefix . '<img src="' . $path_prefix . $file . '" alt="' . $alt . '" width="' . $width . '" height="' . $height . '"' . $extra . ' />' . $suffix;
} 

?>
=======
    return $prefix . '<img src="' . $path_prefix . $file . '" alt="' . $alt . '" width="' . $width . '" height="' .
           $height . '"' . $extra . ' />' . $suffix;
}
>>>>>>> gosa-core_v2.8
