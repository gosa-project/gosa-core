<?php

class pathNavigator
{

  static function registerPlugin($class)
  {
    
    $str = $title = "";

    // Display headline of management plugins
    if($class instanceOf management && isset($class->plHeadline)){
      $str= _($class->plHeadline);
    }

    // Shown title of sub dialogs (They have no plHeadline set.)
    if($class instanceOf plugin && !isset($class->plHeadline)){
      if(empty($class->pathTitle)){
        $str = "Missing: ".get_class($class);
      }else{
        $str = _($class->pathTitle);
      }
    }
  
    // In case of tabs add the 'dn' of the entry 
    if($class instanceOf tabs){

      // Convert dn to cn
      if(isset($class->dn)){
        if(!session::is_set("pathNavigator::registerPlugin:{$class->dn}")){
          global $config;
          $ldap = $config->get_ldap_link();
          $ldap->cat($class->dn, array('cn'));
          if($ldap->count()){
            $attrs = $ldap->fetch();
            $str = $attrs['cn'][0];
          }
          session::set("pathNavigator::registerPlugin:{$class->dn}", $str);
        }
        $str = session::get("pathNavigator::registerPlugin:{$class->dn}");
        $title = $class->dn;
      }
    }

    // Simple string given 
    if(is_string($class)){
      $str = $class;
    }
    
    if(!empty($str)){
      $cur = session::get('pathNavigator::position');

      if(!empty($title)) $title = " title='{$title}' ";

      if(empty($cur)){
        $cur.= "\n<li {$title} class='left path-element'>{$str}</li>";
      }else{
        $cur.= "\n<li {$title} class='left path-element'>{$str}</li>";
      }
      session::set('pathNavigator::position', $cur);
    }
  }

  static function getCurrentPath()
  {
    $path = session::get('pathNavigator::position');
    if(empty($path)){
      $path = "<li class='left path-element'>"._("Welcome to GOsa")."</li>";
    }
    return($path);
  }

  static function clear()
  {
    session::set('pathNavigator::position','');
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
