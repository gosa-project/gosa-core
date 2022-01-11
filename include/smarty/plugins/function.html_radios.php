<?php
/**
 * Smarty plugin
 * 
 * @package Smarty
 * @subpackage PluginsFunction
 */
<<<<<<< HEAD

/**
 * Smarty {html_radios} function plugin
 * 
 * File:       function.html_radios.php<br>
 * Type:       function<br>
 * Name:       html_radios<br>
 * Date:       24.Feb.2003<br>
 * Purpose:    Prints out a list of radio input types<br>
 * Params:
 * <pre>
=======
/**
 * Smarty {html_radios} function plugin
 * File:       function.html_radios.php
 * Type:       function
 * Name:       html_radios
 * Date:       24.Feb.2003
 * Purpose:    Prints out a list of radio input types
 * Params:
 *
>>>>>>> gosa-core_v2.8
 * - name       (optional) - string default "radio"
 * - values     (required) - array
 * - options    (required) - associative array
 * - checked    (optional) - array default not set
 * - separator  (optional) - ie <br> or &nbsp;
 * - output     (optional) - the output next to each radio button
 * - assign     (optional) - assign the output as an array to this variable
 * - escape     (optional) - escape the content (not value), defaults to true
<<<<<<< HEAD
 * </pre>
 * Examples:
 * <pre>
 * {html_radios values=$ids output=$names}
 * {html_radios values=$ids name='box' separator='<br>' output=$names}
 * {html_radios values=$ids checked=$checked separator='<br>' output=$names}
 * </pre>
=======
 *
 * Examples:
 *
 * {html_radios values=$ids output=$names}
 * {html_radios values=$ids name='box' separator='<br>' output=$names}
 * {html_radios values=$ids checked=$checked separator='<br>' output=$names}
>>>>>>> gosa-core_v2.8
 * 
 * @link http://smarty.php.net/manual/en/language.function.html.radios.php {html_radios}
 *      (Smarty online manual)
 * @author Christopher Kvarme <christopher.kvarme@flashjab.com> 
 * @author credits to Monte Ohrt <monte at ohrt dot com> 
 * @version 1.0
<<<<<<< HEAD
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 * @return string 
 * @uses smarty_function_escape_special_chars()
 */
function smarty_function_html_radios($params, $template)
{
    require_once(SMARTY_PLUGINS_DIR . 'shared.escape_special_chars.php');

=======
 *
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 *
 * @return string
 * @uses    smarty_function_escape_special_chars()
 * @throws \SmartyException
 */
function smarty_function_html_radios($params, Smarty_Internal_Template $template)
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
    $name = 'radio';
    $values = null;
    $options = null;
    $selected = null;
    $separator = '';
    $escape = true;
    $labels = true;
    $label_ids = false;
    $output = null;
    $extra = '';
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    foreach($params as $_key => $_val) {
        switch ($_key) {
            case 'name':
            case 'separator':
                $$_key = (string) $_val;
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'checked':
            case 'selected':
                if (is_array($_val)) {
                    trigger_error('html_radios: the "' . $_key . '" attribute cannot be an array', E_USER_WARNING);
                } elseif (is_object($_val)) {
<<<<<<< HEAD
                    if (method_exists($_val, "__toString")) {
                        $selected = smarty_function_escape_special_chars((string) $_val->__toString());
                    } else {
                        trigger_error("html_radios: selected attribute is an object of class '". get_class($_val) ."' without __toString() method", E_USER_NOTICE);
=======
                    if (method_exists($_val, '__toString')) {
                        $selected = smarty_function_escape_special_chars((string)$_val->__toString());
                    } else {
                        trigger_error(
                            'html_radios: selected attribute is an object of class \'' . get_class($_val) .
                            '\' without __toString() method',
                            E_USER_NOTICE
                        );
>>>>>>> gosa-core_v2.8
                    }
                } else {
                    $selected = (string) $_val;
                } 
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'escape':
            case 'labels':
            case 'label_ids':
                $$_key = (bool) $_val;
                break;
<<<<<<< HEAD

            case 'options':
                $$_key = (array) $_val;
                break;

=======
            case 'options':
                $$_key = (array)$_val;
                break;
>>>>>>> gosa-core_v2.8
            case 'values':
            case 'output':
                $$_key = array_values((array) $_val);
                break;
<<<<<<< HEAD

            case 'radios':
                trigger_error('html_radios: the use of the "radios" attribute is deprecated, use "options" instead', E_USER_WARNING);
                $options = (array) $_val;
                break;

            case 'assign':
                break;

            case 'strict': break;

=======
            case 'radios':
                trigger_error(
                    'html_radios: the use of the "radios" attribute is deprecated, use "options" instead',
                    E_USER_WARNING
                );
                $options = (array)$_val;
                break;
            case 'assign':
                break;
            case 'strict':
                break;
>>>>>>> gosa-core_v2.8
            case 'disabled':
            case 'readonly':
                if (!empty($params['strict'])) {
                    if (!is_scalar($_val)) {
<<<<<<< HEAD
                        trigger_error("html_options: $_key attribute must be a scalar, only boolean true or string '$_key' will actually add the attribute", E_USER_NOTICE);
                    }

                    if ($_val === true || $_val === $_key) {
                        $extra .= ' ' . $_key . '="' . smarty_function_escape_special_chars($_key) . '"';
                    }

                    break;
                }
                // omit break; to fall through!

=======
                        trigger_error(
                            "html_options: {$_key} attribute must be a scalar, only boolean true or string '$_key' will actually add the attribute",
                            E_USER_NOTICE
                        );
                    }
                    if ($_val === true || $_val === $_key) {
                        $extra .= ' ' . $_key . '="' . smarty_function_escape_special_chars($_key) . '"';
                    }
                    break;
                }
            // omit break; to fall through!
            // no break
>>>>>>> gosa-core_v2.8
            default:
                if (!is_array($_val)) {
                    $extra .= ' ' . $_key . '="' . smarty_function_escape_special_chars($_val) . '"';
                } else {
<<<<<<< HEAD
                    trigger_error("html_radios: extra attribute '$_key' cannot be an array", E_USER_NOTICE);
=======
                    trigger_error("html_radios: extra attribute '{$_key}' cannot be an array", E_USER_NOTICE);
>>>>>>> gosa-core_v2.8
                } 
                break;
        } 
    } 
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if (!isset($options) && !isset($values)) {
        /* raise error here? */
        return '';
    }
<<<<<<< HEAD

    $_html_result = array();

    if (isset($options)) {
        foreach ($options as $_key => $_val) {
            $_html_result[] = smarty_function_html_radios_output($name, $_key, $_val, $selected, $extra, $separator, $labels, $label_ids, $escape);
=======
    $_html_result = array();
    if (isset($options)) {
        foreach ($options as $_key => $_val) {
            $_html_result[] =
                smarty_function_html_radios_output(
                    $name,
                    $_key,
                    $_val,
                    $selected,
                    $extra,
                    $separator,
                    $labels,
                    $label_ids,
                    $escape
                );
>>>>>>> gosa-core_v2.8
        }
    } else {
        foreach ($values as $_i => $_key) {
            $_val = isset($output[$_i]) ? $output[$_i] : '';
<<<<<<< HEAD
            $_html_result[] = smarty_function_html_radios_output($name, $_key, $_val, $selected, $extra, $separator, $labels, $label_ids, $escape);
        } 
    } 

=======
            $_html_result[] =
                smarty_function_html_radios_output(
                    $name,
                    $_key,
                    $_val,
                    $selected,
                    $extra,
                    $separator,
                    $labels,
                    $label_ids,
                    $escape
                );
        }
    }
>>>>>>> gosa-core_v2.8
    if (!empty($params['assign'])) {
        $template->assign($params['assign'], $_html_result);
    } else {
        return implode("\n", $_html_result);
    } 
} 

<<<<<<< HEAD
function smarty_function_html_radios_output($name, $value, $output, $selected, $extra, $separator, $labels, $label_ids, $escape)
{
    $_output = '';
    
    if (is_object($value)) {
        if (method_exists($value, "__toString")) {
            $value = (string) $value->__toString();
        } else {
            trigger_error("html_options: value is an object of class '". get_class($value) ."' without __toString() method", E_USER_NOTICE);
=======
/**
 * @param $name
 * @param $value
 * @param $output
 * @param $selected
 * @param $extra
 * @param $separator
 * @param $labels
 * @param $label_ids
 * @param $escape
 *
 * @return string
 */
function smarty_function_html_radios_output(
    $name,
    $value,
    $output,
    $selected,
    $extra,
    $separator,
    $labels,
    $label_ids,
    $escape
) {
    $_output = '';
    if (is_object($value)) {
        if (method_exists($value, '__toString')) {
            $value = (string)$value->__toString();
        } else {
            trigger_error(
                'html_options: value is an object of class \'' . get_class($value) .
                '\' without __toString() method',
                E_USER_NOTICE
            );
>>>>>>> gosa-core_v2.8
            return '';
        }
    } else {
        $value = (string) $value;
    }
<<<<<<< HEAD
    
    if (is_object($output)) {
        if (method_exists($output, "__toString")) {
            $output = (string) $output->__toString();
        } else {
            trigger_error("html_options: output is an object of class '". get_class($output) ."' without __toString() method", E_USER_NOTICE);
=======
    if (is_object($output)) {
        if (method_exists($output, '__toString')) {
            $output = (string)$output->__toString();
        } else {
            trigger_error(
                'html_options: output is an object of class \'' . get_class($output) .
                '\' without __toString() method',
                E_USER_NOTICE
            );
>>>>>>> gosa-core_v2.8
            return '';
        }
    } else {
        $output = (string) $output;
    }
<<<<<<< HEAD
    
    if ($labels) {
        if ($label_ids) {
            $_id = smarty_function_escape_special_chars(preg_replace('![^\w\-\.]!' . Smarty::$_UTF8_MODIFIER, '_', $name . '_' . $value));
=======
    if ($labels) {
        if ($label_ids) {
            $_id = smarty_function_escape_special_chars(
                preg_replace(
                    '![^\w\-\.]!' . Smarty::$_UTF8_MODIFIER,
                    '_',
                    $name . '_' . $value
                )
            );
>>>>>>> gosa-core_v2.8
            $_output .= '<label for="' . $_id . '">';
        } else {
            $_output .= '<label>';
        } 
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    $name = smarty_function_escape_special_chars($name);
    $value = smarty_function_escape_special_chars($value);
    if ($escape) {
        $output = smarty_function_escape_special_chars($output);
    }
<<<<<<< HEAD
    
    $_output .= '<input type="radio" name="' . $name . '" value="' . $value . '"';

    if ($labels && $label_ids) {
        $_output .= ' id="' . $_id . '"';
    }

    if ($value === $selected) {
        $_output .= ' checked="checked"';
    }
    
=======
    $_output .= '<input type="radio" name="' . $name . '" value="' . $value . '"';
    if ($labels && $label_ids) {
        $_output .= ' id="' . $_id . '"';
    }
    if ($value === $selected) {
        $_output .= ' checked="checked"';
    }
>>>>>>> gosa-core_v2.8
    $_output .= $extra . ' />' . $output;
    if ($labels) {
        $_output .= '</label>';
    }
<<<<<<< HEAD
    
    $_output .= $separator;
    return $_output;
} 

?>
=======
    $_output .= $separator;
    return $_output;
}
>>>>>>> gosa-core_v2.8
