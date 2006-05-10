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




  function check() {
    return (tabs::check(TRUE));
  }

  function save() {
    $baseobject = $this->by_object['dfsManagment'];
    #$this->dn = $new_dn;
    return tabs::save(TRUE);
  }

  function del() {
    $baseobject = $this->by_object['dfsManagment'];
    return tabs::delete(TRUE); 
    #tabs::del();
  }

}

?>
