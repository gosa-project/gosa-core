<?php

class deptabs extends tabs
{
  var $base= "";

  function deptabs($config, $data, $dn)
  {
	tabs::tabs($config, $data, $dn);
	$this->base= $this->by_object['department']->base;
  }

  function check()
  {
	return (tabs::check(TRUE));
  }
  

  function save($justsave = false)
  {
	if(!$justsave){
		$baseobject= $this->by_object['department'];
		$ou= preg_replace('/,/', '\,', $baseobject->ou);
		$new_dn= @LDAP::convert('ou='.$ou.','.$baseobject->base);
		$disp ="";
		if ($this->dn != $new_dn && $this->dn != "new"){
			
			/* if( new_dn is subtree of this->dn ) */
			$cnt1 = count(split(",",$this->dn));
			$cnt2 = count(split(",",$new_dn));
			if((strstr($new_dn,$this->dn))&&($cnt1<$cnt2)){
				print_red(_("Try to move tree failed. Destination tree is subtree of source tree."));
			}else{
				$disp = ($baseobject->recursive_move($this->dn, $new_dn));
				return($disp);
			}
		}
		$this->dn= $new_dn;
	}
	tabs::save(TRUE);
	return($disp);
  }

}

?>
