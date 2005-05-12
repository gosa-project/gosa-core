<?php

class apptabs extends tabs
{

  function apptabs($config, $data, $dn)
  {
	tabs::tabs($config, $data, $dn);
  }

  function save()
  {
	$baseobject= $this->by_object['application'];

	/* Check for new 'dn', in order to propagate the
	   'dn' to all plugins */
	$new_dn= "cn=".$baseobject->cn.",ou=apps,".$baseobject->base;

        /* Move group? */
        if ($this->dn != $new_dn){

                /* Write entry on new 'dn' */
                if ($this->dn != "new"){
                        $baseobject->move($this->dn, $new_dn);
                        $this->by_object['application']= $baseobject;
                }

                /* Happen to use the new one */
                $this->dn= $new_dn;
        }

	tabs::save();
  }

}

?>
