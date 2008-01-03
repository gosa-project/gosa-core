<?php

class conferencetabs extends tabs
{
  var $base= "";

  function conferencetabs($config, $data, $dn, $category)
  {
	  tabs::tabs($config, $data, $dn,$category);
	  $this->base= $this->by_object['conference']->base; 

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function check($ignore_account= FALSE)
  {
	return (tabs::check(TRUE));
  }
  

  function save($ignore_account= FALSE)
  {
  	$baseobject= $this->by_object['conference'];
	$new_dn= 'cn='.$baseobject->cn.','.get_ou('conferenceou').$baseobject->base;

	if(strtolower($this->dn)==strtolower($new_dn)){
		$this->dn=$new_dn;
	}

	if($this->dn != $new_dn){
		$baseobject->recursive_move($this->dn, $new_dn);
	}
	$this->dn= $new_dn;

	tabs::save(TRUE);
  }

}

?>
