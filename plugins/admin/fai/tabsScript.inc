<?php

class tabsScript extends tabs
{
  var $base= "";

  function tabsScript($config, $data, $dn, $new_cn = "")
  {
	tabs::tabs($config, $data, $dn);

	/* Directly assign a valid dn 
     * cn can't be changed anymore, since the object was created
     */
	$baseobject= $this->by_object['faiScript'];
    $use_dn= 'cn='.$new_cn.",ou=scripts,ou=fai,ou=configs,ou=systems,".$_SESSION['faifilter']['base'];
	$this->by_object['faiScript']->use_dn=$use_dn;
  }

  function check()
  {
	return (tabs::check(TRUE));
  }

  function save()
  {
  	$baseobject= $this->by_object['faiScript'];

	$new_dn= 'cn='.$baseobject->cn.",ou=scripts,ou=fai,ou=configs,ou=systems,".$_SESSION['faifilter']['base'];
	
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
