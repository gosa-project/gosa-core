<?php

class dfstabs extends tabs { 
  var $base;

  function dfstabs($config, $data, $dn) {
    tabs::tabs($config, $data, $dn);
    #$this->base = $this->by_object['dfsManagment']->base;
  }

  #function save_object() {
  #  if (isset($_POST['base'])) {
  #    plugin::save_object();
  #  } else {
  #    $this->base = $_POST['base'];
  #  }
  #}




  function check($ignore_account= FALSE) {
    return (tabs::check(TRUE));
  }

  function save($ignore_account= FALSE)
  {
    $baseobject = $this->by_object['dfsManagment'];
    return tabs::save(TRUE);
  }

  function del() {
    $baseobject = $this->by_object['dfsManagment'];
    return tabs::delete(TRUE); 
  }

}

?>
