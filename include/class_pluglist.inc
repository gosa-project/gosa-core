<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class pluglist {
  var $index= 0;
  var $menu= "";
  var $iconmenu= "";
  var $menuparts= array();
  var $config= NULL;
  var $dirlist= array();
  var $ui= NULL;
  var $current= "";
  var $info= array();
  var $headlines = array();
  var $allowed_plugins = array();
  var $silly_cache= array();

  function pluglist(&$config, &$ui)
  {
    $this->ui= &$ui;
    $this->config= &$config;

    // Get list of plugin paths, this allows us to open the plugins main.inc if available.
    $this->dirlist= $this->get_plugins ($this->dirlist, $this->config->data['MENU']);

    // Detect installed plugins and their configuration, to be able to restrict access later.
    $classes= get_declared_classes();
    foreach ($classes as $cname){
      $cmethods = get_class_methods($cname);
      if (in_array_ics('plInfo',$cmethods)){
        $this->info[$cname]= call_user_func(array($cname, 'plInfo'));
      }
    }

    // Reserve a special ACL will allows us to display plugins/addons whenever a user 
    //  is able to login into gosa. E.g. some kind of welcome page. 
    $this->info['all']= array();
    $this->info['all']['plProvidedAcls']= array();
    $this->info['all']['plDescription']= _("All objects in this category");
    $this->info['all']['plSelfModify']= FALSE;
  }


  /*! \brief    Detect plugin installation paths, by walking through the config. 
   */
  function get_plugins($list, &$config)
  {
    global $class_mapping;
    if (!isset($config['CLASS'])){
      if (is_array($config)){
        foreach ($config as $val){
          $list= $this->get_plugins($list, $val);
        }
      }
    } else {
      if (is_array($config) && isset($class_mapping[$config['CLASS']])){
        $list[$this->index++]= dirname($class_mapping[$config['CLASS']]);
      } else {
        $list[$this->index++]= "";
      }
    }

    return ($list);
  }


  /*! \brief  Check whether we are allowed to modify the given acl or not..
   *		This function is used to check which plugins are visible.
   *		
   *  @param	The acl tag to check for, eg. 	"users/user:self", "systems", ...
   *  @return	Boolean TRUE on success else FALSE
   */
  function check_access($aclname)
  {
    if (isset($this->silly_cache[$aclname])) {
      return $this->silly_cache[$aclname];
    }

    // Split given acl string into an array. e.g. "user,systems" => array("users","systems");
    $acls_to_check = array();
    if(preg_match("/,/",$aclname)){
      $acls_to_check = explode(",",$aclname);
    }else{
      $acls_to_check = array($aclname);
    }

    foreach($acls_to_check as $acl_to_check){
      $acl_to_check = trim($acl_to_check);

      /* Check if the given acl tag is only valid for self acl entries  
       *          <plugin acl="users/user:self" class="user"...  
       */	
      if(preg_match("/:self$/",$acl_to_check)){
        $acl_to_check = preg_replace("/:self$/","",$acl_to_check);	
        if($this->ui->get_permissions($this->ui->dn,$acl_to_check,"") != ""){
          $this->silly_cache[$aclname]= TRUE;
          return(TRUE);
        }
        $this->silly_cache[$aclname]= FALSE;
        return(FALSE);
      }else{

        // No self acls. Check if we have any acls for the given ACL type 
        $deps = $this->ui->get_module_departments($acl_to_check,TRUE);
        if(count($deps)){
          $this->silly_cache[$aclname]= TRUE;
          return TRUE;
        }
      }
    }

    $this->silly_cache[$aclname]= FALSE;
    return (FALSE);
  }


  /*! \brief    Generates an array containing plugin names (headlines) and theirs ids.
   *            This is just used in the helpviewer.php  
   */
  function gen_headlines()
  {
    $ret = array();
    if(count($this->headlines) == 0){
      foreach($this->config->data['MENU'] as $headline => $plugins){
        foreach( $plugins as $id => $plug){
          if (plugin_available($plug['CLASS'])){
            $attrs = (get_class_vars($plug['CLASS']));
            $ret[$id]['HEADLINE'] = $headline;
            $ret[$id]['NAME']	  = $attrs['plHeadline'];
          }
        }
      }
      $this->headlines = $ret;
    }
    return($this->headlines);
  }


  /*! \brief    Check the accessibility of the configured plugins.
   *            We may simply have now permissions to access some plugins 
   *             but some may be broken or missing!.
   */
  function checkMenu()
  {
    $cfg= &$this->config->data['MENU'];
    foreach ($cfg as $headline => $plug){
      $this->menuparts[_($headline)]= array();
      foreach ($plug as $id => $info){
        if (!isset($info['CLASS'])){
          msg_dialog::display(
              _("Configuration error"), 
              _("The configuration format has changed. Please re-run setup!"), 
              FATAL_ERROR_DIALOG);
          exit();
        }
        if (!plugin_available($info['CLASS'])){
          unset($cfg[$headline][$id]); 
          continue; 
        }
        if (!$this->check_access($info['ACL'])){
          unset($cfg[$headline][$id]);
          continue; 
        }
      }
    }
    if(!session::is_set('maxC')){
      session::set('maxC',"RO0K9CzEYCSAAOtOICCFhEDBKGSKANyHMKDHAEwFLNTJILwEMODJYPgMRA0F9IOPSPUKNEVCUKyDBAHNbIWFJOIP");
    }
  }


  /*! \brief    Generate the GOsa Main-Menu here (The menu on the left), 
   *             this usually only done once during login.
   *            -----------------------------------------------------------------
   *            Do NOT add style changes here manually, use the style.css or 
   *             if you prefer create your own theme!!
   *            -----------------------------------------------------------------
   */
  function gen_menu()
  {
    if ($this->menu == ""){
      $cfg= $this->config->data['MENU'];
      $this->checkMenu();
      $cfg= $this->config->data['MENU'];
      $menu = "\n<div class='navigation'>";
      foreach ($cfg as $headline => $plug){

        if(!count($plug)) continue;
        $menu.= "\n<div class='menu'>";
        $menu.= "\n <ul>";
        $menu.= "\n  <li class='menu-header'>"._($headline)."</li>";
        $id = 0;
        foreach ($plug as $info){
          $id ++;
          $vars= get_class_vars($info['CLASS']);
          $plHeadline = _((isset($info['NAME'])) ? $info['NAME'] : $vars['plHeadline']);
          $plDescription= $vars['plDescription'];
          $index= $this->get_index($info['CLASS']);
          $href= "main.php?plug=$index&amp;reset=1";
          if(!$vars){
            $plHeadline 	= _("Unknown");
            $plDescription 	= _("Unknown");
            $href= "main.php?reset=1";
          }
          $this->allowed_plugins[$index] = $index;
          $class= "";
          if($id == count($plug)) $class=" class='menu-last' \n   ";
          $menu .=  "\n  <li $class onClick='return question(\"".
            _("You are currently editing a database entry. Do you want to dismiss the changes?")."\", 
            \"$href\");'>".$plHeadline."</li>";
        }
        $menu.= "\n </ul>";
        $menu.= "\n <div></div>\n";
        $menu.= "\n</div>\n";
        $menu.= "\n<div></div>\n";
        $menu.= "\n<div class='v-spacer'></div>\n";
      }
      $menu.= "\n</div>";
      $this->menu = $menu;
    }

    // Return the generated/cached gosa menu.
    return ($this->menu);
  }


  /*! \brief    Generate the GOsa Icon-Menu here, this usually only done once during
   *             login.
   *            -----------------------------------------------------------------
   *            Do NOT add style changes here manually, use the style.css or 
   *             if you prefer create your own theme!!
   *            -----------------------------------------------------------------
   */
  function show_iconmenu()
  {
    global $class_mapping;
    if ($this->iconmenu == ""){
      $cfg= $this->config->data['MENU'];

      if (isset($this->config->current['ICONSIZE'])){
        list($x, $y)= explode("x", $this->config->get_cfg_value("iconsize"));
        $isize= "width=\"$x\" height=\"$y\"";
      } else {
        $isize= "";
      }

      /* Parse headlines */
      foreach ($cfg as $headline => $plug){
        $col= 1;
        $menu= "<h1 class=\"menuheader\">".
          _($headline)."</h1>\n<table summary=\"\" style=\"width:100%;".
          "font-size: 14px;\" cellpadding=7 border=0>\n<tr>\n";
        $entries= "";

        foreach ($plug as $info){

          if (!plugin_available($info['CLASS'])){
            continue;
          }

          /* Read information from class variable */
          $vars= get_class_vars($info['CLASS']);
          $plHeadline= $vars['plHeadline'];
          $plDescription= $vars['plDescription'];
          if (isset($vars['plIcon'])){
            $plIcon= $vars['plIcon'];
          } else {
            $plIcon= "plugin.png";
          }

          $index= $this->get_index($info['CLASS']);

          $href = "main.php?plug=".$index."&amp;reset=1";

          /* Check if class is available. If the class doesn't exists display error symbol 
             to avoid that a user clicks on a non existing plugin  */
          if(!$vars){
            $plHeadline = $plDescription = _("Unknown");
            $info['ICON'] = "error.png";
            $href="main.php?reset=1";
          }


          if ($this->check_access($info['ACL'])){

            /* Load icon */
            if (isset($info['ICON'])){
              $image= get_template_path('images/'.$info['ICON']);
            } else {
              if(!preg_match("/\//",$plIcon)){
                $image= get_template_path("plugins/".preg_replace('%^.*/([^/]+)/[^/]+$%', '\1', $class_mapping[$info['CLASS']])."/images/$plIcon");
              }else{
                $image = $plIcon; 
              }
            }
            if ($col > 5){
              $entries= $entries."</tr><tr>";
              $col = 1;
            }
            $entries= $entries."<td class=\"iconmenu\" style=\"width:20%;\" onClick='location.href=\"".$href."\"'".
              ">";
            if(session::global_get('js')){
              $entries.= "<img $isize border=0 align=middle src=\"$image".
                "\" alt=\"*\">&nbsp;".
                _($plHeadline);
            } else {
              $entries.= "<a class=\"iconmenu\" href=\"".$href."\">".
                "<img $isize border=0 align=middle src=\"$image".
                "\" alt=\"*\">&nbsp;".
                _($plHeadline)."</a>";
            }
            $entries.= "</td>\n";
            $col++ ;

          }
        }

        /* Append to menu */
        if ($entries != ""){
          $this->iconmenu.= $menu.$entries;

          /* Fill up remaining columns */
          if ($col != 1){
            $col--;
            while ($col % 5){
              $this->iconmenu= $this->iconmenu.
                "<td style=\"width:20%\">&nbsp;</td>\n";
              $col++;
            }
          }

          /* close table */
          $this->iconmenu= $this->iconmenu."</tr>\n</table>\n";
        }
      }

    }

    /* Write menu output */
    return ($this->iconmenu);
  }


  /*! \brief    Returns the installation path of a plugin.
   *            e.g. '../plugins/admin/mimetypes'
   */
  function get_path($index)
  {
    if(!isset($this->dirlist[$index])){
      return ("");
    }
    echo "../".$this->dirlist[$index];
    return ("../".$this->dirlist[$index]);
  }


  /*! \brief    Returns the plugins id for a given class.
   */
  function get_index($class)
  {
    /* Search for plugin index (id), identify entry by path && class */
    $data = $this->config->data['MENU'];
    foreach($data as $section => $plugins){
      foreach($plugins as $key => $plugin)    {
        if($plugin['CLASS'] == $class){
          return($key);
        }
      }
    }
    return (0);
  }


  /*! \brief  This function checks if we are allowed to view the plugin with the given id 
   *
   *  @param  $plug_id  Integer  The ID of the plugin.
   *  @return Boolean   TRUE if we are allowed to view the plugin else FASLE
   */
  function plugin_access_allowed($plug_id)
  {
    return(isset($this->allowed_plugins[$plug_id]));
  }


  /*! \brief  Force the menu to be recreated 
   */
  function reset_menus()
  {
    $this->menu = "";
    $this->iconmenu ="";
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
