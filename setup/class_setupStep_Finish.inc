<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


class Step_Finish extends setup_step
{
  var $gosa_conf_contrib   = "/gosa.conf";
  var $cfg_file_written = FALSE;
  var $header_image = "images/setup/server.png";

  var $b_displayCheckbutton = TRUE;

  function __construct()
  {
    $this->update_strings();
  }

  
  function update_strings()
  {
    $this->s_title      = _("Finish");
    $this->s_info       = _("Write configuration file");
    $this->s_title_long = _("Finish - write the configuration file");
  }


  function get_conf_data()
  {
    $smarty = get_smarty();
    $smarty->assign("cv",xmlentities($this->parent->captured_values));
    $smarty->assign("config_checksum", md5(file_get_contents(CONFIG_TEMPLATE_DIR.$this->gosa_conf_contrib)));
    $str =  $smarty->fetch(CONFIG_TEMPLATE_DIR.$this->gosa_conf_contrib);
    return($str);
  }  


  function execute()
  {

    if(!function_exists("posix_getgrgid")){
      $webgroup = "";
    }else{
      $info= posix_getgrgid(posix_getgid());
      $webgroup = $info['name'];
    }

    /* Check if there is currently an active gosa.conf 
     */
    $exists = file_exists(CONFIG_DIR."/".CONFIG_FILE); 

    /* Redirect to GOsa login, if : 
     *   - gosa.conf exists
     *   - Permisssion are set correctly 
     */ 
    if(isset($_POST['next']) && $exists && !$this->is_world_readable(CONFIG_DIR."/".CONFIG_FILE)){
      session::destroy();
      header("Location: index.php");
      exit();
    }

    /* Downlaod config */
    if(isset($_POST['getconf'])){

      header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
      header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
      header("Cache-Control: no-cache");
      header("Pragma: no-cache");
      header("Cache-Control: post-check=0, pre-check=0");
      header("Content-type: text/plain");
     
      if (preg_match('/MSIE 5.5/', $_SERVER['HTTP_USER_AGENT']) ||
          preg_match('/MSIE 6.0/', $_SERVER['HTTP_USER_AGENT'])){
        header('Content-Disposition: filename="'.CONFIG_FILE.'"');
      } else {
        header('Content-Disposition: attachment; filename="'.CONFIG_FILE.'"');
      }

      $str = $this->get_conf_data();
      $str = preg_replace("/\n[ ]*\n/","",$str);

      echo $this->get_conf_data();
      exit();
    }
   
    $err_msg= "";
    
    if($exists && $this->is_world_readable(CONFIG_DIR."/".CONFIG_FILE)){
      $err_msg = _("Your configuration file is currently world readable. Please update the file permissions!");
    }elseif(!$exists){
      $err_msg = _("The configuration is currently not readable or it does not exists.");
    }

    $smarty = get_smarty();
    $smarty->assign("err_msg",$err_msg);
    $smarty->assign("webgroup", $webgroup);
    $smarty->assign("CONFIG_DIR",CONFIG_DIR);
    $smarty->assign("CONFIG_FILE" , CONFIG_FILE);

    $smarty->assign("msg2", sprintf(_("Download gosa.conf and place it under %s."),CONFIG_DIR));
    $smarty->assign("msg3", sprintf(_("After that, please make sure that the user under which the web server is running can read the %s. Other users must not be able to do this. To meet these requirements, you can run these commands in the console."), CONFIG_FILE));

    $smarty->assign("cv",$this->parent->captured_values);
    return($smarty -> fetch (get_template_path("../setup/setup_finish.tpl")));
  }


  /* check if given file is world readable */
  function is_world_readable($file)
  {
    clearstatcache();
    $p = fileperms($file);
    $w_r = (decbin($p & 4) == TRUE);
    return($w_r);
  }


  function save_object()
  {
    if(isset($_POST['step8_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
