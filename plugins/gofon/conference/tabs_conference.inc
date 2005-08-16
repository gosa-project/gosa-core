<?php

class conferencetabs extends tabs
{
  var $base= "";

  function conferencetabs($config, $data, $dn)
  {
	tabs::tabs($config, $data, $dn);
	$this->base= $this->by_object['conference']->base;
  }

  function check()
  {
	return (tabs::check(TRUE));
  }
  

  function save()
  {
  	$baseobject= $this->by_object['conference'];
	$new_dn= 'ou='.$baseobject->ou.','.$baseobject->base;

 	if ($this->dn != $new_dn && $this->dn != "new"){
		$baseobject->recursive_move($this->dn, $new_dn);
	}
	$this->dn= $new_dn;

	tabs::save(TRUE);
  }

}

?>
