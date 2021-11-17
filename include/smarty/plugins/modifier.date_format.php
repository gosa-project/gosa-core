<?php
/**
 * Smarty plugin
 * 
 * @package Smarty
 * @subpackage PluginsModifier
 */
<<<<<<< HEAD

/**
 * Smarty date_format modifier plugin
 * 
 * Type:     modifier<br>
 * Name:     date_format<br>
 * Purpose:  format datestamps via strftime<br>
 * Input:<br>
=======
/**
 * Smarty date_format modifier plugin
 * Type:     modifier
 * Name:     date_format
 * Purpose:  format datestamps via strftime
 * Input:
>>>>>>> gosa-core_v2.8
 *          - string: input date string
 *          - format: strftime format for output
 *          - default_date: default date if $string is empty
 * 
 * @link http://www.smarty.net/manual/en/language.modifier.date.format.php date_format (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com> 
<<<<<<< HEAD
=======
 *
>>>>>>> gosa-core_v2.8
 * @param string $string       input date string
 * @param string $format       strftime format for output
 * @param string $default_date default date if $string is empty
 * @param string $formatter    either 'strftime' or 'auto'
<<<<<<< HEAD
=======
 *
>>>>>>> gosa-core_v2.8
 * @return string |void
 * @uses smarty_make_timestamp()
 */
function smarty_modifier_date_format($string, $format=null, $default_date='', $formatter='auto')
{
    if ($format === null) {
        $format = Smarty::$_DATE_FORMAT;
    }
    /**
<<<<<<< HEAD
    * Include the {@link shared.make_timestamp.php} plugin
    */
    require_once(SMARTY_PLUGINS_DIR . 'shared.make_timestamp.php');
    if ($string != '' && $string != '0000-00-00' && $string != '0000-00-00 00:00:00') {
        $timestamp = smarty_make_timestamp($string);
    } elseif ($default_date != '') {
=======
     * require_once the {@link shared.make_timestamp.php} plugin
     */
    static $is_loaded = false;
    if (!$is_loaded) {
        if (!is_callable('smarty_make_timestamp')) {
            include_once SMARTY_PLUGINS_DIR . 'shared.make_timestamp.php';
        }
        $is_loaded = true;
    }
    if (!empty($string) && $string !== '0000-00-00' && $string !== '0000-00-00 00:00:00') {
        $timestamp = smarty_make_timestamp($string);
    } elseif (!empty($default_date)) {
>>>>>>> gosa-core_v2.8
        $timestamp = smarty_make_timestamp($default_date);
    } else {
        return;
    } 
<<<<<<< HEAD
    if($formatter=='strftime'||($formatter=='auto'&&strpos($format,'%')!==false)) {
        if (DS == '\\') {
            $_win_from = array('%D', '%h', '%n', '%r', '%R', '%t', '%T');
            $_win_to = array('%m/%d/%y', '%b', "\n", '%I:%M:%S %p', '%H:%M', "\t", '%H:%M:%S');
=======
    if ($formatter === 'strftime' || ($formatter === 'auto' && strpos($format, '%') !== false)) {
        if (Smarty::$_IS_WINDOWS) {
            $_win_from = array(
                '%D',
                '%h',
                '%n',
                '%r',
                '%R',
                '%t',
                '%T'
            );
            $_win_to = array(
                '%m/%d/%y',
                '%b',
                "\n",
                '%I:%M:%S %p',
                '%H:%M',
                "\t",
                '%H:%M:%S'
            );
>>>>>>> gosa-core_v2.8
            if (strpos($format, '%e') !== false) {
                $_win_from[] = '%e';
                $_win_to[] = sprintf('%\' 2d', date('j', $timestamp));
            } 
            if (strpos($format, '%l') !== false) {
                $_win_from[] = '%l';
                $_win_to[] = sprintf('%\' 2d', date('h', $timestamp));
            } 
            $format = str_replace($_win_from, $_win_to, $format);
        } 
        return strftime($format, $timestamp);
    } else {
        return date($format, $timestamp);
    }
} 
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
