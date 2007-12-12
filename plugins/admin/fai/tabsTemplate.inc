<?php

class tabsTemplate extends tabs
{
  var $base= "";

  function tabsTemplate($config, $data, $dn,$category)
  {
	tabs::tabs($config, $data, $dn, $category);

	/* Add references/acls/snapshots */
	$this->addSpecialTabs();
  }

  function check($ignore_account= FALSE)
  {
	return (tabs::check(TRUE));
  }
  

  function save($ignore_account= FALSE)
  {
  	$baseobject= $this->by_object['faiTemplate'];
	
	$new_dn= 'cn='.$baseobject->cn.",ou=templates,ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];

	if($_SESSION['faifilter']['branch']!="main"){
		$new_dn ='cn='.$baseobject->cn.",ou=templates,".$_SESSION['faifilter']['branch'];
	}

 	if ($this->dn != $new_dn && $this->dn != "new"){
		
		/* if( new_dn is subtree of this->dn ) */
		$cnt1 = count(split(",",$this->dn));
		$cnt2 = count(split(",",$new_dn));
		if((strstr($new_dn,$this->dn))&&($cnt1<$cnt2)){
			print_red(_("Try to move tree failed. Destination tree is subtree of source tree."));
		}else{
//			$baseobject->recursive_move($this->dn, $new_dn);
		}
	}
	$this->dn= $new_dn;

	tabs::save(TRUE);
  }

}

?>
