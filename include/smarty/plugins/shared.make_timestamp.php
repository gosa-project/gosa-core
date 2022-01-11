<?php
/**
 * Smarty shared plugin
 *
 * @package Smarty
 * @subpackage PluginsShared
 */
<<<<<<< HEAD

/**
 * Function: smarty_make_timestamp<br>
 * Purpose:  used by other smarty functions to make a timestamp from a string.
 *
 * @author   Monte Ohrt <monte at ohrt dot com>
 * @param DateTime|int|string $string  date object, timestamp or string that can be converted using strtotime()
=======
/**
 * Function: smarty_make_timestamp
 * Purpose:  used by other smarty functions to make a timestamp from a string.
 *
 * @author Monte Ohrt <monte at ohrt dot com>
 *
 * @param DateTime|int|string $string date object, timestamp or string that can be converted using strtotime()
 *
>>>>>>> gosa-core_v2.8
 * @return int
 */
function smarty_make_timestamp($string)
{
    if (empty($string)) {
        // use "now":
        return time();
<<<<<<< HEAD
    } elseif ($string instanceof DateTime) {
        return $string->getTimestamp();
    } elseif (strlen($string) == 14 && ctype_digit($string)) {
        // it is mysql timestamp format of YYYYMMDDHHMMSS?
        return mktime(substr($string, 8, 2),substr($string, 10, 2),substr($string, 12, 2),
                       substr($string, 4, 2),substr($string, 6, 2),substr($string, 0, 4));
=======
    } elseif ($string instanceof DateTime
              || (interface_exists('DateTimeInterface', false) && $string instanceof DateTimeInterface)
    ) {
        return (int)$string->format('U'); // PHP 5.2 BC
    } elseif (strlen($string) === 14 && ctype_digit($string)) {
        // it is mysql timestamp format of YYYYMMDDHHMMSS?
        return mktime(
            substr($string, 8, 2),
            substr($string, 10, 2),
            substr($string, 12, 2),
            substr($string, 4, 2),
            substr($string, 6, 2),
            substr($string, 0, 4)
        );
>>>>>>> gosa-core_v2.8
    } elseif (is_numeric($string)) {
        // it is a numeric string, we handle it as timestamp
        return (int) $string;
    } else {
        // strtotime should handle it
        $time = strtotime($string);
<<<<<<< HEAD
        if ($time == -1 || $time === false) {
=======
        if ($time === -1 || $time === false) {
>>>>>>> gosa-core_v2.8
            // strtotime() was not able to parse $string, use "now":
            return time();
        }
        return $time;
    }
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
