<?php
/**
 * Smarty plugin
<<<<<<< HEAD
 * @package Smarty
 * @subpackage PluginsFunction
 */

/**
 * Smarty {counter} function plugin
 *
 * Type:     function<br>
 * Name:     counter<br>
=======
 *
 * @package    Smarty
 * @subpackage PluginsFunction
 */
/**
 * Smarty {counter} function plugin
 * Type:     function
 * Name:     counter
>>>>>>> gosa-core_v2.8
 * Purpose:  print out a counter value
 *
 * @author Monte Ohrt <monte at ohrt dot com>
 * @link http://www.smarty.net/manual/en/language.function.counter.php {counter}
 *       (Smarty online manual)
<<<<<<< HEAD
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
=======
 *
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 *
>>>>>>> gosa-core_v2.8
 * @return string|null
 */
function smarty_function_counter($params, $template)
{
    static $counters = array();
<<<<<<< HEAD

    $name = (isset($params['name'])) ? $params['name'] : 'default';
    if (!isset($counters[$name])) {
        $counters[$name] = array(
            'start'=>1,
            'skip'=>1,
            'direction'=>'up',
            'count'=>1
            );
    }
    $counter =& $counters[$name];

    if (isset($params['start'])) {
        $counter['start'] = $counter['count'] = (int)$params['start'];
    }

    if (!empty($params['assign'])) {
        $counter['assign'] = $params['assign'];
    }

    if (isset($counter['assign'])) {
        $template->assign($counter['assign'], $counter['count']);
    }
    
=======
    $name = (isset($params[ 'name' ])) ? $params[ 'name' ] : 'default';
    if (!isset($counters[ $name ])) {
        $counters[ $name ] = array('start' => 1, 'skip' => 1, 'direction' => 'up', 'count' => 1);
    }
    $counter =& $counters[ $name ];
    if (isset($params[ 'start' ])) {
        $counter[ 'start' ] = $counter[ 'count' ] = (int)$params[ 'start' ];
    }
    if (!empty($params[ 'assign' ])) {
        $counter[ 'assign' ] = $params[ 'assign' ];
    }
    if (isset($counter[ 'assign' ])) {
        $template->assign($counter[ 'assign' ], $counter[ 'count' ]);
    }
>>>>>>> gosa-core_v2.8
    if (isset($params['print'])) {
        $print = (bool)$params['print'];
    } else {
        $print = empty($counter['assign']);
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if ($print) {
        $retval = $counter['count'];
    } else {
        $retval = null;
    }
<<<<<<< HEAD

    if (isset($params['skip'])) {
        $counter['skip'] = $params['skip'];
    }
    
    if (isset($params['direction'])) {
        $counter['direction'] = $params['direction'];
    }

    if ($counter['direction'] == "down")
        $counter['count'] -= $counter['skip'];
    else
        $counter['count'] += $counter['skip'];
    
    return $retval;
    
}

?>
=======
    if (isset($params[ 'skip' ])) {
        $counter[ 'skip' ] = $params[ 'skip' ];
    }
    if (isset($params[ 'direction' ])) {
        $counter[ 'direction' ] = $params[ 'direction' ];
    }
    if ($counter[ 'direction' ] === 'down') {
        $counter[ 'count' ] -= $counter[ 'skip' ];
    } else {
        $counter[ 'count' ] += $counter[ 'skip' ];
    }
    return $retval;
}
>>>>>>> gosa-core_v2.8
