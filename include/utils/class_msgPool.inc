<?php

class msgPool
{

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
    return (sprintf(_("There is already an entry named '%s' in the system!"), $name));
  }


  public static function required($name)
  {
    return (sprintf(_("The required field '%s' is empty!"), $name));
  }


  public static function invalid($name, $data= "", $regex= "", $example= "")
  {
    /* Stylize example */
    if ($example != ""){
      $example= "<br><br>"._("Example:")." ".$example;
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
          $result.= "<font style='color:red;'>".htmlentities($currentChar)."</font>";
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

}
