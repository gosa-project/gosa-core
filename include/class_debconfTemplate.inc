<?php

class debconf
{
  var $package= "";
  var $language= "";
  var $has_template= FALSE;
  var $template_directory= "/var/lib/dpkg/info";
  var $template= array();


  function debconf($package= "", $language= "")
  {
    $this->set_language($language);
    $this->set_package($package);
  }


  function set_package($package)
  {
    $this->package= $package;
    return ($this->load());
  }
  

  function set_template_directory($directory)
  {
    if (is_dir($directory) && is_readable($directory)){
      $this->template_directory($directory);
      return TRUE;
    }

    $this->template_directory= "";
    return FALSE;
  }

  
  function set_language($language)
  {
    $this->language= $language;
  }


  function load()
  {
    /* Reject requests, if parameters are not set */
    if ($this->package == "" || $this->template_directory == ""){
      return (FALSE);
    }

    /* Try to load package based template file */
    $filename= preg_replace("/\/+/", "/", $this->template_directory."/".$this->package.".templates");
    if (is_file($filename) && is_readable($filename)){
      $this->template= array();

      /* Read template array */
      $name= "";
      $langcode= $this->language.".UTF-8";
      $in_description= FALSE;
      $got_local_description= FALSE;
      $fh= fopen($filename, 'r');

      while (!feof($fh)){
        $line= fgets($fh, 1024);

        /* Reset description flag */
        if ($in_description && !preg_match("/^ /", $line)){
          $in_description= FALSE;
        }
        
        /* Template header */
        if (preg_match("/^Template: /", $line)){
          $name= trim(preg_replace("/^Template: (.*)$/", "\\1", $line));
          $this->template[$name]= array();
          $got_local_description= FALSE;
          continue;
        }

        /* Get type */
        if (preg_match("/^Type: /", $line)){
          $type= trim(preg_replace("/^Type: (.*)$/", "\\1", $line));
          $this->template[$name]['Type']= $type;
          continue;
        }

        /* Get default */
        if (preg_match("/^Default: /", $line)){
          $default= trim(preg_replace("/^Default: (.*)$/", "\\1", $line));
          $this->template[$name]['Default']= $default;
          continue;
        }

        /* Get description */
        if (!$got_local_description && preg_match("/^Description: /", $line)){
          $description= trim(preg_replace("/^Description: (.*)$/", "\\1", $line));
          $this->template[$name]['Topic']= $description;
          $this->template[$name]['Description']= "";
          $in_description= TRUE;
          continue;
        }

        /* Fill description */
        if (!$got_local_description && $in_description){
          $description= preg_replace("/^ (.*)$/", "\\1", $line);
          $this->template[$name]['Description'].= $description;
          continue;
        }

        /* Get local description */
        if (preg_match("/^Description-$langcode: /", $line)){
          $description= trim(preg_replace("/^Description-$langcode: (.*)$/", "\\1", $line));
          $this->template[$name]['Topic']= $description;
          $in_description= TRUE;
          $got_local_description= TRUE;
          $this->template[$name]['Description']= "";
          continue;
        }

        /* Fill local description */
        if ($got_local_description && $in_description){
          $description= preg_replace("/^ (.*)$/", "\\1", $line);
          $this->template[$name]['Description'].= $description;
          continue;
        }

        /* Get native choices */
        if (preg_match("/^Choices: /", $line)){
          $type= trim(preg_replace("/^Choices: (.*)$/", "\\1", $line));
          $this->template[$name]['Choices']= $type;
        }

        /* Get local choices */
        if (preg_match("/^Choices-$langcode: /", $line)){
          $type= trim(preg_replace("/^Choices-$langcode: (.*)$/", "\\1", $line));
          $this->template[$name]['Choices-local']= $type;
        }

      }

      fclose($fh);
      $this->has_template= TRUE;
      return (TRUE);
    }

    $this->has_template= FALSE;
    return (FALSE);
  }


  function has_template()
  {
    return ($this->has_template);
  }


  function get_dialog()
  {
    if ($this->has_template){
      $result= "";

      foreach ($this->template as $name => $entry){

        $types= array("boolean" => "", "multiselect" => "", "note" => "",
                      "password" => "", "select" => "", "string" => "", "text" => "", "title" => "");
        
        /* Check if type is available */
        if (isset($types[$entry['Type']])){

          /* Produce type specific output */
          $fn= "render_".$entry['Type'];
          $result.= $this->$fn($entry);
          
        } else {
          php_error(E_WARNING, "An unknown type has been specified in the debconf template. Please fix.");
        }
      }
      
      return ($result);
    } else {
      return _("This package has no debconf options.");
    }
  }


  function render_boolean($data)
  {
    $result=  "<table width='100%' border=1>";
    $result.= "<tr><td valign='top'>";
    $result.= "<b>".$data['Topic']."</b><br><br>";
    $result.= $data['Description'];
    $result.= "</td><td width='10%' valign='top'>O Yes<br>O No</td>";
    $result.= "</table>";

    return ($result);
  }


  function render_multiselect($data)
  {
    if (preg_match('/\$\{/', $data['Choices'])){
      $choices= "Need to use some text...";
    } else {
      $choices= "";
      foreach (split(", ", $data['Choices']) as $choice){
        $choices.= "[ ] ".$choice."<br>";
      }
    }
  
    $result=  "<table width='100%' border=1>";
    $result.= "<tr><td valign='top'>";
    $result.= "<b>".$data['Topic']."</b><br><br>";
    $result.= $data['Description'];
    $result.= "</td><td width='30%' valign='top'>$choices</td>";
    $result.= "</table>";

    return ($result);
  }


  function render_note($data)
  {
    /* Ignore notes, they do not makes sense, since we don't get any
       chance to test entered values... */
    return ("");
  }


  function render_password($data)
  {
    $result=  "<table width='100%' border=1>";
    $result.= "<tr><td valign='top'>";
    $result.= "<b>".$data['Topic']."&nbsp;[......................]</b><br><br>";
    $result.= $data['Description'];
    $result.= "</td></table>";

    return ($result);
  }


  function render_select($data)
  {
    if (preg_match('/\$\{/', $data['Choices'])){
      $choices= "Need to use some text...";
    } else {
      $choices= "";
      foreach (split(", ", $data['Choices']) as $choice){
        $choices.= "[ ] ".$choice."<br>";
      }
    }
  
    $result=  "<table width='100%' border=1>";
    $result.= "<tr><td valign='top'>";
    $result.= "<b>".$data['Topic']."</b><br><br>";
    $result.= $data['Description'];
    $result.= "</td><td width='30%' valign='top'>$choices</td>";
    $result.= "</table>";

    return ($result);
  }


  function render_string($data)
  {
    $result=  "<table width='100%' border=1>";
    $result.= "<tr><td valign='top'>";
    $result.= "<b>".$data['Topic']."&nbsp;[......................]</b><br><br>";
    $result.= $data['Description'];
    $result.= "</td></table>";

    return ($result);
  }


  function render_text($data)
  {
    /* Ignore text messages, they are normally used for status hints. */
    return ("");
  }


  function render_title($data)
  {
    /* Ignore text messages, they are normally used for status hints. */
    return ("");
  }


  function get_template_packages()
  {
  }

  
  function set_default($default)
  {
  }


}

# Example:
$debconf= new debconf("libnss-ldap", "de");
echo $debconf->get_dialog();

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
