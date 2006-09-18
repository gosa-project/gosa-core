<?php

class grouptabs extends tabs
{

	function grouptabs($config, $data, $dn)
	{
		tabs::tabs($config, $data, $dn);
		$baseobject= $this->by_object['group'];
		foreach ($this->by_object as $name => $obj){
			if(isset($obj->parent)){
				$this->by_object[$name]->parent  = $this;
			}
		}
	}

	function save_object($save_current= FALSE)
	{
		tabs::save_object($save_current);

		/* Update reference, transfer variables */
		$baseobject= $this->by_object['group'];
		foreach ($this->by_object as $name => $obj){
			/* Don't touch base object */
			if ($name != 'group'){
				$obj->parent= &$this;
				$obj->cn= $baseobject->cn;
				$this->by_object[$name]= $obj;
			}
		}
	}

	function save()
	{
		$baseobject= $this->by_object['group'];

		/* Check for new 'dn', in order to propagate the
		   'dn' to all plugins */
		$new_dn= 'cn='.$baseobject->cn.','.get_groups_ou().$baseobject->base;

		/* Put baseobjects 'cn' to mailobjects 'uid' */
		if (isset($this->by_object['mailgroup'])){
			$this->by_object['mailgroup']->uid= $baseobject->cn;
		}


		/* Update reference, transfer variables */
		foreach ($this->by_object as $name => $obj){
			/* Transfer attributes for mailgroup account */
			if ($name == 'mailgroup'){
				$this->by_object['mailgroup']->members= $baseobject->memberUid;;
			}
		}

		/* Move group? */
		if ($this->dn != $new_dn){

			/* Write entry on new 'dn' */
			if ($this->dn != "new"){
				$baseobject->move($this->dn, $new_dn);
				$this->by_object['group']= $baseobject;
			}

			/* Happen to use the new one */
			$this->dn= $new_dn;
		}

		$ret= tabs::save();

		/* Fix tagging if needed */
		$baseobject->dn= $new_dn;
		$baseobject->handle_object_tagging();

		return $ret;
	}

	function saveCopyDialog()
	{
		tabs::saveCopyDialog();
		
        /* Update reference, transfer variables */
        $baseobject= $this->by_object['group'];
        foreach ($this->by_object as $name => $obj){
            /* Don't touch base object */
            if ($name != 'group'){
                $obj->parent= &$this;
                $obj->cn= $baseobject->cn;
                $this->by_object[$name]= $obj;
            }
        }
	}
	
}

?>
