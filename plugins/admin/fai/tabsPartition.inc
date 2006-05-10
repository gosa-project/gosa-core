<?php

class tabsPartition extends tabs
{
  var $base= "";

  function tabsPartition($config, $data, $dn)
  {
	tabs::tabs($config, $data, $dn);
  }

  function check()
  {
	return (tabs::check(TRUE));
  }
  

  function save()
  {
  	$baseobject= $this->by_object['faiPartitionTable'];

	$new_dn= 'cn='.$baseobject->cn.",ou=disk,ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
	
	if($_SESSION['faifilter']['branch']!="main"){
		$new_dn ='cn='.$baseobject->cn.",ou=disk,".$_SESSION['faifilter']['branch'];
	}

 	if ($this->dn != $new_dn && $this->dn != "new"){
		
		/* if( new_dn is subtree of this->dn ) */
		$cnt1 = count(split(",",$this->dn));
		$cnt2 = count(split(",",$new_dn));
		if((strstr($new_dn,$this->dn))&&($cnt1<$cnt2)){
			print_red(_("Try to move tree failed. Destination tree is subtree of source tree."));
		}else{
			$baseobject->recursive_move($this->dn, $new_dn);
		}
	}
	$this->dn= $new_dn;

	tabs::save(TRUE);
  }

}

?>
