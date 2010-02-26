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
    
    $this->gen_menu();
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
          $menu .=  "\n  <li id='plugMenuId_{$index}' $class onClick='return openPlugin({$index});'>".$plHeadline."</li>";
        }
        $menu.= "\n </ul>";
        $menu.= "\n <div></div>\n";
        $menu.= "\n</div>\n";
        $menu.= "\n<div></div>\n";
        $menu.= "\n<div class='v-spacer'></div>\n";
      }
      $menu.= "\n</div>";
      $this->menu = $menu;

      // Add javascript method to print out warning messages while leaving an unsaved form.
      // We do it in here to get the string translated.
      $this->menu .= "
        <script language='javascript' type='text/javascript'>
        function openPlugin(id){
          return question(\""._("You are currently editing a database entry. Do you want to dismiss the changes?")."\",
              \"main.php?plug=\" + id + \"&amp;reset=1\");
        }
      </script>
        "; 
    }
  
    // Use javascript to mark the currently selected plugin.
    $menu = $this->menu;
    if(isset($_GET['plug'])){
      $menu.= "
        <script language='javascript' type='text/javascript'>
          if($('plugMenuId_".$_GET['plug']."')){
            $('plugMenuId_".$_GET['plug']."').className= 'current'
          }
        </script>
        "; 
    }

    // Return the generated/cached gosa menu.
    return ($menu);
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

    $add_hr =FALSE;
    if ($this->iconmenu == ""){
      $cfg= $this->config->data['MENU'];
      foreach ($cfg as $headline => $plug){
        $col= 0;

        $this->iconmenu .= "\n  <div class='clear'></div>\n";
        if($add_hr){
          $add_hr = FALSE;
          $this->iconmenu .= "\n  <hr>\n";
        }
        $this->iconmenu .= "\n  <h3 class='icon-menu-title'>". _($headline)."</h3>\n";

        foreach ($plug as $info){

          // Get Plugin info
          $vars= get_class_vars($info['CLASS']);
          $plHeadline= _($vars['plHeadline']);
          $plDescription= _($vars['plDescription']);
          $plIcon = (isset($vars['plIcon'])) ? $vars['plIcon']: "plugin.png";
          $index= $this->get_index($info['CLASS']);

          /* Check if class is available. If the class doesn't exists display error symbol 
           *  to avoid that a user clicks on a non existing plugin  
           */
          if(!$vars){
            $plHeadline = $plDescription = _("Unknown");
            $info['ICON'] = "error.png";
            $index = '';
          }

          // Detect the correct position of the plugin icon
          if(!preg_match("/\//",$plIcon)){
            $image= get_template_path("plugins/".preg_replace('%^.*/([^/]+)/[^/]+$%', '\1', 
                  $class_mapping[$info['CLASS']])."/images/$plIcon");
          }else{
            $image = $plIcon; 
          }

          // Add a seperating row
          if (($col % 4) == 0){ 
            $this->iconmenu .= "\n  <div class='clear'></div>\n";
          }

          $this->iconmenu.= "\n  <div class='container-element' style='width: 25%;' onclick='openPlugin({$index})'> ";
          $this->iconmenu.= "\n   <div class='icon-menu-item' style='background-image: url({$image});'>";
          $this->iconmenu.= "\n    <h1>{$plHeadline}</h1>";
          $this->iconmenu.= "\n    <p>{$plDescription}</p>";
          $this->iconmenu.= "\n   </div>";
          $this->iconmenu.= "\n  </div>\n";
          $col++ ;
        }
        $add_hr = TRUE;
      }
    }
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
