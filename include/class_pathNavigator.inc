<?php

class pathNavigator
{

  static function registerPlugin($class, $title= "")
  {
    $str = "";

    // Display headline of management plugins
    if($class instanceOf management && isset($class->plHeadline)){
      $str= _($class->plHeadline);
    }

    // Shown title of sub dialogs (They have no plHeadline set.)
    if($class instanceOf plugin && !isset($class->plHeadline)){
      if(!empty($class->pathTitle)){
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

          if(!empty($class->dn)){
            $namingAttr = preg_replace("/^([^=]*)=.*$/","\\1",$class->dn);

            $ldap->cat($class->dn, array($namingAttr));
            if($ldap->count()){
              $attrs = $ldap->fetch();
              $str = $attrs[$namingAttr][0];
            }
            session::set("pathNavigator::registerPlugin:{$class->dn}", $str);
          }
        }
        $str = session::get("pathNavigator::registerPlugin:{$class->dn}");
        if(empty($title)){
          $title = $class->dn;
        }
      }
    }

    // Simple string given 
    if(is_string($class)){
      $str = $class;
    }
    
    if(!empty($str)){
      $cur = session::get('pathNavigator::position');
      $entry = array('TITLE' => $title, 'NAME' => $str);
      $cur[] = $entry;
      session::set('pathNavigator::position', $cur);
    }
  }

  static function getCurrentPath()
  {
    // Ensure that we've a position value initialized. 
    if(!session::is_set('pathNavigator::position')) {
        session::set('pathNavigator::position',array());
    }

    // Get position and validate its value
    $list = session::get('pathNavigator::position');
    if(!is_array($list)){
        $list = array();
    }

    // Build up title and path string.
    $path = "";
    $sTitle = "";
    foreach($list as $entry){
        $title = (!empty($entry['TITLE']))? 'title="'.htmlentities(LDAP::fix($entry['TITLE']),ENT_COMPAT,'UTF-8').'"': '';
        $path.= "\n<li {$title} class='left path-element'>{$entry['NAME']}</li>";
        $sTitle .= " - ".$entry['NAME'];
    }

    // If no path is given then show a simple 'Welcome to GOsa' string.
    if(empty($path)){
      $path = "<li class='left path-element'>"._("Welcome to GOsa")."</li>";
    }

    $smarty = get_smarty();
    $smarty->assign('title', 'GOsa '.$sTitle);
    return($path);
  }

  static function clear()
  {
    session::set('pathNavigator::position', array());
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
