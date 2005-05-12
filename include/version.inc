<?php

/* Hard code version number */
$version= "2.3.1";
$cvs    = "";

/* Try to evaluate if we're using CVS */
if (is_dir("$BASE_DIR/CVS")){
  $tmp= stat("$BASE_DIR/CVS");
  $cvs= date ("Y-m-d/H:i:s", $tmp[9]);
} else {
  $cvs= "RELEASE";
}

print_red(sprintf(_("You are currently using GOsa version %s (CVS checkout from %s)"),
      $version, $cvs));

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
