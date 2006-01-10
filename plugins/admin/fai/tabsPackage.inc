<?php

class tabsPackage extends tabs
{
  var $base= "";

  function tabsPackage($config, $data, $dn)
  {
	tabs::tabs($config, $data, $dn);
  }

  function check()
  {
	return (tabs::check(TRUE));
  }
  

  function save()
  {
  	$baseobject= $this->by_object['faiPackage'];

	$branch = "";
	if($_SESSION['faifilter']['branch']!="main"){
		$branch = "ou=".$_SESSION['faifilter']['branch'].",";
	}

	$new_dn= 'cn='.$baseobject->cn.",ou=packages,".$branch."ou=fai,ou=configs,ou=systems,".$_SESSION['faifilter']['base'];
	
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
