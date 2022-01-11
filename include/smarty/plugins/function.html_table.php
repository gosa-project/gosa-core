<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsFunction
 */
<<<<<<< HEAD

/**
 * Smarty {html_table} function plugin
 *
 * Type:     function<br>
 * Name:     html_table<br>
 * Date:     Feb 17, 2003<br>
 * Purpose:  make an html table from an array of data<br>
 * Params:
 * <pre>
=======
/**
 * Smarty {html_table} function plugin
 * Type:     function
 * Name:     html_table
 * Date:     Feb 17, 2003
 * Purpose:  make an html table from an array of data
 * Params:
 *
>>>>>>> gosa-core_v2.8
 * - loop       - array to loop through
 * - cols       - number of columns, comma separated list of column names
 *                or array of column names
 * - rows       - number of rows
 * - table_attr - table attributes
 * - th_attr    - table heading attributes (arrays are cycled)
 * - tr_attr    - table row attributes (arrays are cycled)
 * - td_attr    - table cell attributes (arrays are cycled)
 * - trailpad   - value to pad trailing cells with
 * - caption    - text for caption element
 * - vdir       - vertical direction (default: "down", means top-to-bottom)
 * - hdir       - horizontal direction (default: "right", means left-to-right)
 * - inner      - inner loop (default "cols": print $loop line by line,
 *                $loop will be printed column by column otherwise)
<<<<<<< HEAD
 * </pre>
 * Examples:
 * <pre>
 * {table loop=$data}
 * {table loop=$data cols=4 tr_attr='"bgcolor=red"'}
 * {table loop=$data cols="first,second,third" tr_attr=$colors}
 * </pre>
=======
 *
 * Examples:
 *
 * {table loop=$data}
 * {table loop=$data cols=4 tr_attr='"bgcolor=red"'}
 * {table loop=$data cols="first,second,third" tr_attr=$colors}
>>>>>>> gosa-core_v2.8
 *
 * @author Monte Ohrt <monte at ohrt dot com>
 * @author credit to Messju Mohr <messju at lammfellpuschen dot de>
 * @author credit to boots <boots dot smarty at yahoo dot com>
 * @version 1.1
 * @link http://www.smarty.net/manual/en/language.function.html.table.php {html_table}
 *          (Smarty online manual)
<<<<<<< HEAD
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 * @return string
 */
function smarty_function_html_table($params, $template)
=======
 *
 * @param array $params parameters
 *
 * @return string
 */
function smarty_function_html_table($params)
>>>>>>> gosa-core_v2.8
{
    $table_attr = 'border="1"';
    $tr_attr = '';
    $th_attr = '';
    $td_attr = '';
    $cols = $cols_count = 3;
    $rows = 3;
    $trailpad = '&nbsp;';
    $vdir = 'down';
    $hdir = 'right';
    $inner = 'cols';
    $caption = '';
    $loop = null;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if (!isset($params['loop'])) {
        trigger_error("html_table: missing 'loop' parameter",E_USER_WARNING);
        return;
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    foreach ($params as $_key => $_value) {
        switch ($_key) {
            case 'loop':
                $$_key = (array)$_value;
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'cols':
                if (is_array($_value) && !empty($_value)) {
                    $cols = $_value;
                    $cols_count = count($_value);
                } elseif (!is_numeric($_value) && is_string($_value) && !empty($_value)) {
                    $cols = explode(',', $_value);
                    $cols_count = count($cols);
                } elseif (!empty($_value)) {
                    $cols_count = (int)$_value;
                } else {
                    $cols_count = $cols;
                }
                break;
<<<<<<< HEAD

            case 'rows':
                $$_key = (int)$_value;
                break;

=======
            case 'rows':
                $$_key = (int)$_value;
                break;
>>>>>>> gosa-core_v2.8
            case 'table_attr':
            case 'trailpad':
            case 'hdir':
            case 'vdir':
            case 'inner':
            case 'caption':
                $$_key = (string)$_value;
                break;
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            case 'tr_attr':
            case 'td_attr':
            case 'th_attr':
                $$_key = $_value;
                break;
        }
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    $loop_count = count($loop);
    if (empty($params['rows'])) {
        /* no rows specified */
        $rows = ceil($loop_count / $cols_count);
    } elseif (empty($params['cols'])) {
        if (!empty($params['rows'])) {
            /* no cols specified, but rows */
            $cols_count = ceil($loop_count / $rows);
        }
    }
<<<<<<< HEAD

    $output = "<table $table_attr>\n";

    if (!empty($caption)) {
        $output .= '<caption>' . $caption . "</caption>\n";
    }

    if (is_array($cols)) {
        $cols = ($hdir == 'right') ? $cols : array_reverse($cols);
        $output .= "<thead><tr>\n";

=======
    $output = "<table $table_attr>\n";
    if (!empty($caption)) {
        $output .= '<caption>' . $caption . "</caption>\n";
    }
    if (is_array($cols)) {
        $cols = ($hdir === 'right') ? $cols : array_reverse($cols);
        $output .= "<thead><tr>\n";
>>>>>>> gosa-core_v2.8
        for ($r = 0; $r < $cols_count; $r++) {
            $output .= '<th' . smarty_function_html_table_cycle('th', $th_attr, $r) . '>';
            $output .= $cols[$r];
            $output .= "</th>\n";
        }
        $output .= "</tr></thead>\n";
    }
<<<<<<< HEAD

    $output .= "<tbody>\n";
    for ($r = 0; $r < $rows; $r++) {
        $output .= "<tr" . smarty_function_html_table_cycle('tr', $tr_attr, $r) . ">\n";
        $rx = ($vdir == 'down') ? $r * $cols_count : ($rows-1 - $r) * $cols_count;

        for ($c = 0; $c < $cols_count; $c++) {
            $x = ($hdir == 'right') ? $rx + $c : $rx + $cols_count-1 - $c;
            if ($inner != 'cols') {
                /* shuffle x to loop over rows*/
                $x = floor($x / $cols_count) + ($x % $cols_count) * $rows;
            }

=======
    $output .= "<tbody>\n";
    for ($r = 0; $r < $rows; $r++) {
        $output .= "<tr" . smarty_function_html_table_cycle('tr', $tr_attr, $r) . ">\n";
        $rx = ($vdir === 'down') ? $r * $cols_count : ($rows - 1 - $r) * $cols_count;
        for ($c = 0; $c < $cols_count; $c++) {
            $x = ($hdir === 'right') ? $rx + $c : $rx + $cols_count - 1 - $c;
            if ($inner !== 'cols') {
                /* shuffle x to loop over rows*/
                $x = floor($x / $cols_count) + ($x % $cols_count) * $rows;
            }
>>>>>>> gosa-core_v2.8
            if ($x < $loop_count) {
                $output .= "<td" . smarty_function_html_table_cycle('td', $td_attr, $c) . ">" . $loop[$x] . "</td>\n";
            } else {
                $output .= "<td" . smarty_function_html_table_cycle('td', $td_attr, $c) . ">$trailpad</td>\n";
            }
        }
        $output .= "</tr>\n";
    }
    $output .= "</tbody>\n";
    $output .= "</table>\n";
<<<<<<< HEAD

    return $output;
}

=======
    return $output;
}

/**
 * @param $name
 * @param $var
 * @param $no
 *
 * @return string
 */
>>>>>>> gosa-core_v2.8
function smarty_function_html_table_cycle($name, $var, $no)
{
    if (!is_array($var)) {
        $ret = $var;
    } else {
        $ret = $var[$no % count($var)];
    }
<<<<<<< HEAD

    return ($ret) ? ' ' . $ret : '';
}

?>
=======
    return ($ret) ? ' ' . $ret : '';
}
>>>>>>> gosa-core_v2.8
