<?php

class msgPool
{
  public static function permDelete($name= "")
  {
    if ($name == "") { 
      return (_("You have no permission to delete this object!"));
    }

    if (!is_array($name)){
      return (_("You have no permission to delete the object:")."<br><br><i>$name</i>");
    }

    msgPool::buildList($name);
    if (count($name) == 1){
      return (_("You have no permission to delete the object:")."<br>".msgPool::buildList($name));
    }

    return (_("You have no permission to delete these objects:")."<br>".msgPool::buildList($name));
  }


  public static function permCreate($name= "")
  {
    if ($name == "") { 
      return (_("You have no permission to create this object!"));
    }

    if (!is_array($name)){
      return (_("You have no permission to create the object:")."<br><br><i>$name</i>");
    }

    if (count($name) == 1){
      return (_("You have no permission to create the object:")."<br>".msgPool::buildList($name));
    }

    return (_("You have no permission to create these objects:")."<br>".msgPool::buildList($name));
  }


  public static function permModify($name= "")
  {
    if ($name == "") { 
      return (_("You have no permission to modify this object!"));
    }

    if (!is_array($name)){
      return (_("You have no permission to modify the object:")."<br><br><i>$name</i>");
    }

    if (count($name) == 1){
      return (_("You have no permission to modify the object:")."<br>".msgPool::buildList($name));
    }

    return (_("You have no permission to modify these objects:")."<br>".msgPool::buildList($name));
  }


  public static function permView($name= "")
  {
    if ($name == "") { 
      return (_("You have no permission to view this object!"));
    }

    if (!is_array($name)){
      return (_("You have no permission to view the object:")."<br><br><i>$name</i>");
    }

    if (count($name) == 1){
      return (_("You have no permission to view the object:")."<br>".msgPool::buildList($name));
    }

    return (_("You have no permission to view these objects:")."<br>".msgPool::buildList($name));
  }


  public static function permMove($name= "")
  {
    if ($name == "") { 
      return (_("You have no permission to move this object!"));
    }

    if (!is_array($name)){
      return (_("You have no permission to move the object:")."<br><br><i>$name</i>");
    }

    if (count($name) == 1){
      return (_("You have no permission to move the object:")."<br>".msgPool::buildList($name));
    }

    return (_("You have no permission to move these objects:")."<br>".msgPool::buildList($name));
  }


  public static function dbconnect($name, $error= "", $dbinfo= "")
  {
    if ($error != ""){
      $error= "<br><br><i>"._("Error").":</i> ".$error;
    }
    if ($dbinfo != ""){
      $error.= "<br><br><i>"._("Connection information").":</i> ".$dbinfo;
    }
    return (sprintf(_("Cannot connect to %s database!"), $name).$error);
  }


  public static function dbselect($name, $error= "", $dbinfo= "")
  {
    if ($error != ""){
      $error= "<br><br><i>"._("Error").":</i> ".$error;
    }
    if ($dbinfo != ""){
      $error.= "<br><br><i>"._("Connection information").":</i> ".$dbinfo;
    }
    return (sprintf(_("Cannot select %s database!"), $name).$error);
  }


  public static function noserver($name)
  {
    return (sprintf(_("No %s server defined!"), $name));
  }


  public static function dbquery($name, $error= "", $dbinfo= "")
  {
    if ($error != ""){
      $error= "<br><br><i>"._("Error").":</i> ".$error;
    }
    if ($dbinfo != ""){
      $error.= "<br><br><i>"._("Connection information").":</i> ".$dbinfo;
    }
    return (sprintf(_("Cannot query %s database!"), $name).$error);
  }


  public static function reserved($name)
  {
    return (sprintf(_("The field '%s' contains a reserved keyword!"), $name));
  }


  public static function cmdnotfound($type, $plugin)
  {
    return (sprintf(_("Command specified as %s hook for plugin '%s' does not exist!"), $type, $plugin));
  }


  public static function toobig($name, $min= "")
  {
    if ($min == ""){
      return (sprintf(_("Value for '%s' is too big!"), $name));
    } else {
      return (sprintf(_("'%s' must be smaller than %d!"), $name, $min));
    }
  }


  public static function toosmall($name, $min= "")
  {
    if ($min == ""){
      return (sprintf(_("Value for '%s' is too small!"), $name));
    } else {
      return (sprintf(_("'%s' must be %d or above!"), $name, $min));
    }
  }


  public static function bigger($name1, $name2)
  {
    return (sprintf(_("'%s' should be bigger than '%s'!"), $name1, $name2));
  }


  public static function smaller($name1, $name2)
  {
    return (sprintf(_("'%s' should be smaller than '%s'!"), $name1, $name2));
  }


  public static function depends($name1, $name2)
  {
    return (sprintf(_("'%s' depends on '%s' - please provide both values!"), $name1, $name2));
  }


  public static function duplicated($name)
  {
    return (sprintf(_("There is already an entry with this '%s' attribute in the system!"), $name));
  }


  public static function required($name)
  {
    return (sprintf(_("The required field '%s' is empty!"), $name));
  }


  public static function invalid($name, $data= "", $regex= "", $example= "")
  {
    /* Stylize example */
    if ($example != ""){
      $example= "<br><br><i>"._("Example").":</i> ".$example;
    }

    /* If validChars are posted, take data and paint all invalid
       characters... */
    if ($regex) {
      $result= "";
      $mismatch= "";
      foreach (str_split($data) as $currentChar){
        if (preg_match("$regex", $currentChar)){
          $result.= $currentChar;
        } else {
          $result.= "<font style='color:red;text-decoration:underline;'>".htmlentities($currentChar)."</font>";
          $mismatch.= $currentChar;
        }
      }

      return sprintf(_("The Field '%s' contains invalid characters"), $name).". ".
             (strlen($mismatch)==1?sprintf(_("'%s' is not allowed:"), htmlentities($mismatch)):sprintf(_("'%s' are not allowed."), htmlentities($mismatch))).
             "<br><br> \"$result\"$example";
    } else {
      return sprintf(_("The Field '%s' contains invalid characters"), $name)."!$example";
    }
  }


  public static function missingext($name)
  {
    return sprintf(_("Missing %s PHP extension!"), $name);
  }


  public static function cancelButton()
  {
    return sprintf(_("Cancel"));
  }


  public static function okButton()
  {
    return sprintf(_("Ok"));
  }


  public static function applyButton()
  {
    return sprintf(_("Apply"));
  }


  public static function saveButton()
  {
    return sprintf(_("Save"));
  }


  public static function addButton($what= "")
  {
    return $what == "" ? sprintf(_("Add")): sprintf(_("Add %s"), $what);
  }


  public static function delButton($what= "")
  {
    return $what == "" ? sprintf(_("Delete")): sprintf(_("Delete %s"), $what);
  }


  public static function setButton($what= "")
  {
    return $what == "" ? sprintf(_("Set")): sprintf(_("Set %s"), $what);
  }


  public static function editButton($what= "")
  {
    return $what == "" ? sprintf(_("Edit...")): sprintf(_("Edit %s..."), $what);
  }


  public static function buildList($data)
  {
    $objects= "";
    foreach ($name as $key => $value){
      if (is_numeric($key)){
        $objects.= "<br>\n<i>$value</i>";
      } else {
        $objects.= "<br>\n$value (<i>$key</i>)";
      }
    }
  }

  public static function noValidExtension($name)
  {
	return sprintf(_("This account has no valid %s extensions!", $name));
  }

  public static function featuresEnabled($name, $depends= "")
  {
	if ($depends == ""){
		return sprintf(_("This account has %s settings enabled. You can disable them by clicking below."), $name);
	} else {
		if (is_array($depends)){
			return sprintf(_("This account has %s settings enabled. To disable them, you'll need to remove the %s settings first!"), $depends);
		} else {
			$deps= "";
			foreach ($depends as $dep){
				$deps.= "$dep / ";
			}
			$deps= preg_replace("/ \/ $/", "", $deps);
			return sprintf(_("This account has %s settings enabled. To disable them, you'll need to remove the %s settings first!"), $deps);
		}
	}
  }


  public static function featuresDisabled($name, $depends= "")
  {
	if ($depends == ""){
		return sprintf(_("This account has %s settings disabled. You can enable them by clicking below."), $name);
	} else {
		if (is_array($depends)){
			return sprintf(_("This account has %s settings disabled. To enable them, you'll need to add the %s settings first!"), $depends);
		} else {
			$deps= "";
			foreach ($depends as $dep){
				$deps.= "$dep / ";
			}
			$deps= preg_replace("/ \/ $/", "", $deps);
			return sprintf(_("This account has %s features settings. To disable them, you'll need to add the %s settings first!"), $deps);
		}
	}
  }


  public static function addFeaturesButton($name)
  {
	return sprintf(_("Add %s settings"), $name);
  }


  public static function removeFeaturesButton($name)
  {
	return sprintf(_("Remove %s settings"), $name);
  }


}
