<?php

class pathNavigator
{

  static function registerPlugin($class)
  {
    $str = "";
    if($class instanceOf management){
      $str = "<b>".$class->plHeadline."</b>";
    }elseif($class instanceOf plugin){
      $str = $class->plHeadline;
    }elseif($class instanceOf tabs){
      $str = $class->dn;
    }
    
    if(!empty($str)){
      $cur = session::get('pathNavigator::position');
      $cur.= "<li class='left'>{$str}</li>";
      session::set('pathNavigator::position', $cur);
    }
  }

  static function getCurrentPath()
  {
    return(session::get('pathNavigator::position'));
  }

  static function clear()
  {
    session::set('pathNavigator::position','');
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
