<?php
/*  This class is used for glpi account management.
	ADD // EDIT // DELETE of computers or changng 
	something else in the glpi database should be done here.


	function glpiDB($server,$user,$pwd,$db){
	function SelectDB()
	function query($qry)

// return all available Sytemtypes 
function getSystemTypes()

// Update system type specifid by id
function updateSystemType($name,$id)

// Add system type
function addSystemType($name)

// Delete system type by name / id 
function removeSystemType_byID($id)
function removeSystemType_byNAME($name)

// Get all manufacturer 
function getEnterprisesTypes()

// Update with specified attributes, on entry $id
function updateEnterprisesType($array,$id)

// remove entry with id=$id
function removeEnterprisesType($id)

// return all os types 
function getOSTypes()

// return all users 
function getUsers()

// Return available devices 
function getDevices()

// return computer informations  (phone, terminal,ws ...)
function getComputerInformations_byID($id)
function getComputerInformations_byNAME($name)


 */
class glpiDB{

	var $user		="";
	var $password	="";
	var $server		="";
	var $db			="";

	var $is_connected		= 0;
	var $handle 			= NULL;

	var $lasterror	="";

	function glpiDB($server,$user,$pwd,$db){
		$this->server 	= $server;
		$this->user 	= $user;
		$this->password = $pwd;
		$this->db		= $db;

		$this->handle 	= @mysql_connect($this->server,$this->user,$this->password);

		if($this->handle){
			$this->is_connected = true;
			$this->SelectDB($this->db);
		}	
	}

	function SelectDB()
	{
		if($this->is_connected){
			mysql_select_db($this->db,$this->handle);
		}
	}

	function is_account($dn)
	{
		if(!$this->is_connected){
            $this->lasterror ="Can't query anything, if we aren't connected.";
            return(false);
        }else{
			$qry = "SELECT * FROM glpi_computers WHERE name='".$dn."';";
			$res = $this->query($qry);
			if(count($res)==0){
				return(false);
			}else{
				return(true);
			}
		}
	}

	function query($qry)
	{
		if(!$this->is_connected){
			$this->lasterror ="Can't query anything, if we aren't connected.";
			return(false);
		}else{
			$ret =array();
			$res = mysql_query($qry,$this->handle);

			while($rs = @mysql_fetch_array($res,MYSQL_ASSOC)){
				$ret[]=$rs;
			}
			return($ret);
		}
	}

	/* System types */
	function getSystemTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_type_computers;"));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* System types */
	function updateSystemType($name,$id)
	{

		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_computers WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("UPDATE glpi_type_computers SET name='".$name."' WHERE ID=".$id.";"));	
			}else{
				echo "can't update not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* System types */
	function addSystemType($name)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_computers WHERE name='".$name."';");
			if(isset($tmp[0])){
				echo "such an entry already exists";
				return(false);
			}else{	
				return($this->query("INSERT INTO glpi_type_computers (name) VALUES ('".$name."');"));
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* System types */
	function removeSystemType_byID($id)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_computers WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("DELETE FROM glpi_type_computers WHERE ID=".$id.";"));	
			}else{
				echo "can't remove not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Manufacturer */	
	function getEnterprisesTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = $this->query("SELECT * FROM glpi_enterprises;");
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Manufacturer */	
	function getEnterprise($id)
	{
		if($this->is_connected){
			$ret = array();
			$tmp = $this->query("SELECT * FROM glpi_enterprises WHERE ID=".$id.";");
			return($tmp);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* System types */
	function updateEnterprisesType($array,$id)
	{
		if(!is_array($array)){
			echo "updateEnterprisesType: first paraeter must be an array";
		}elseif($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_enterprises WHERE ID='".$id."';");
			if(isset($tmp[0])){
				$atr = array("ID","name","type","address","website","phonenumber","comments","deleted","fax","email");

				$v = "";
				foreach($atr as $at){
					if(isset($array[$at])){
						$v .= " ".$at."='".$array[$at]."', ";
					}
				}
				if(empty($v)){
					echo "updateEnterprisesType: no attributes given ";
					return(false);
				}else{
					$v = preg_replace("/, $/","",$v);
					return($this->query("UPDATE glpi_enterprises SET ".$v." WHERE ID='".$id."';"));	
				}
			}else{
				echo "can't update not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	function addEnterprisesType($array)
	{
		if(!is_array($array)){
			echo "addUser: first paraeter must be an array";
		}elseif($this->is_connected){
			$atr = array("ID","name","type","address","website","phonenumber","comments","deleted","fax","email");
			$v = "";
			$a = "";
			foreach($atr as $at){
				if(isset($array[$at])){
					$a .= $at.", ";
					$v .= "'".$array[$at]."', ";
				}
			}
			if(empty($v)){
				echo "addUser: no attributes given ";
				return(false);
			}else{
				$a = preg_replace("/, $/","",$a);
				$v = preg_replace("/, $/","",$v);
				return($this->query("INSERT INTO glpi_enterprises (".$a.") VALUES (".$v.");"));
			}
		
		}else{
			echo "not connected";
			return(false);
		}

	}



	function removeEnterprisesType($id)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_enterprises WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("DELETE FROM glpi_enterprises WHERE ID=".$id.";"));
			}else{
				echo "can't remove not existing entry";
				return(false);
			}
		}else{
			echo "not connected";
			return(false);
		}
	}


	/* Operating systems */
	function getOSTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp=($this->query("SELECT * FROM glpi_dropdown_os;"));

			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}

			return($ret);
				
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* os */
    function addOS($name)
    {
        if($this->is_connected){
            $tmp = $this->query("SELECT * FROM glpi_dropdown_os WHERE name='".$name."';");
            if(isset($tmp[0])){
                echo "such an entry already exists";
                return(false);
            }else{
                return($this->query("INSERT INTO glpi_dropdown_os (name) VALUES ('".$name."');"));
            }
        }else{
            echo "not connected";
            return(false);
        }
    }

	   /* System types */
    function removeOS_byID($id)
    {
        if($this->is_connected){
            $tmp = $this->query("SELECT * FROM glpi_dropdown_os WHERE ID=".$id.";");
            if(is_array($tmp[0])){
                return($this->query("DELETE FROM glpi_dropdown_os WHERE ID=".$id.";"));
            }else{
                echo "can't remove not existing entry";
                return(false);
            }
        }else{
            echo "not connected";
            return(false);
        }
    }


	
	/* System types */
	function updateOS($name,$id)
	{

		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_dropdown_os WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("UPDATE glpi_dropdown_os SET name='".$name."' WHERE ID=".$id.";"));	
			}else{
				echo "can't update not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}


	/* Technical / Responsible person / glpi users  */
	function getUsers()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_users"));
			foreach($tmp as $user){
				$ret[$user['ID']]=$user['name'];
			}
			return($ret);

		}else{
			echo "not connected";
			return(false);
		}
	}

	
	function addUser($array,$dn)
	{
		if(!is_array($array)){
			echo "addUser: first paraeter must be an array";
		}elseif($this->is_connected){
			$array['name']=$dn;
			$atr = array("name","phone","email");
			$v = "";
			$a = "";
			foreach($atr as $at){
				if(isset($array[$at])){
					$a .= $at.", ";
					$v .= "'".$array[$at]."', ";
				}
			}
			if(empty($v)){
				echo "addUser: no attributes given ";
				return(false);
			}else{
				$a = preg_replace("/, $/","",$a);
				$v = preg_replace("/, $/","",$v);
				return($this->query("INSERT INTO glpi_users (".$a.") VALUES (".$v.");"));
			}
		
		}else{
			echo "not connected";
			return(false);
		}

	}

	function updateUser($array,$dn)
	{
		if(!is_array($array)){
            echo "updateUser: first paraeter must be an array";
        }elseif($this->is_connected){
            $tmp = $this->query("SELECT * FROM  glpi_users WHERE name='".$dn."';");
            if(isset($tmp[0])){

				$atr = array("name","phone","email");
                $v = "";
                foreach($atr as $at){
                    if(isset($array[$at])){
                        $v .= " ".$at."='".$array[$at]."', ";
                    }
                }
                if(empty($v)){
                    echo "UpdateUser: no attributes given ";
                    return(false);
                }else{
                    $v = preg_replace("/, $/","",$v);
                    return($this->query("UPDATE glpi_users SET ".$v." WHERE name='".$dn."';"));
                }
            }else{
                echo "can't update not existing entry";
                return(false);
            }
        }else{
            echo "not connected";
            return(false);
        }

	}



	/* Gets all pulldown and needed tableinformations */
	function getDevices()
	{
		if($this->is_connected){
			$ret = array();
			$ret['devices']['glpi_device_moboard'] 		= $this->query("SELECT * FROM glpi_device_moboard;");
			$ret['devices']['glpi_device_case'] 		= $this->query("SELECT * FROM glpi_device_case;");
			$ret['devices']['glpi_device_control'] 		= $this->query("SELECT * FROM glpi_device_control;");
			$ret['devices']['glpi_device_drive'] 		= $this->query("SELECT * FROM glpi_device_drive;");
			$ret['devices']['glpi_device_gfxcard'] 		= $this->query("SELECT * FROM glpi_device_gfxcard;");
			$ret['devices']['glpi_device_hdd'] 			= $this->query("SELECT * FROM glpi_device_hdd;");
			$ret['devices']['glpi_device_iface'] 		= $this->query("SELECT * FROM glpi_device_iface;");
			$ret['devices']['glpi_device_pci'] 			= $this->query("SELECT * FROM glpi_device_pci;");
			$ret['devices']['glpi_device_power'] 		= $this->query("SELECT * FROM glpi_device_power;");
			$ret['devices']['glpi_device_processor'] 	= $this->query("SELECT * FROM glpi_device_processor;");
			$ret['devices']['glpi_device_ram'] 			= $this->query("SELECT * FROM glpi_device_ram;");
			$ret['devices']['glpi_device_sndcard'] 		= $this->query("SELECT * FROM glpi_device_sndcard;");
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getComputerInformations($name)
	{
		if($this->is_connected){	
			/*
			$ret							= $this->query(	'	
					SELECT
					c.ID,
					c.name      as "Name",
					c.comments  as "Commets",
					d.name      as "OS",
					e.name      as "Manufacturer",
					dc.name     as "Type",
					u.name      as "Technical responsible"
					FROM
					glpi_computers as c
					left join   glpi_dropdown_os    as d    on (c.os                    = d.ID)
					left join   glpi_enterprises    as e    on (c.FK_glpi_enterprise    = e.ID)
					left join   glpi_type_computers as dc   on (c.type                  = dc.ID)
					left join   glpi_users          as u    on (c.tech_num              = u.ID)

					WHERE c.Name!="" AND c.ID='.$id.';');
		*/
			$ret = $this->query("SELECT * FROM glpi_computers WHERE name='".$name."';");
			return($ret);		
		}else{
			echo "not connected";
			return(false);
		}
	}

	function updateComputerInformations($array,$name)
	{
		if(!is_array($array)){
			echo "updateComputerInformations: first paraeter must be an array";
		}elseif($this->is_connected){
			$tmp = $this->query("SELECT * FROM  glpi_computers WHERE name='".$name."';");
			if(isset($tmp[0])){

				$atr = array(	"ID","name","serial","otherserial","contact","contact_num",
						"tech_num","comments","date_mod","os","location","domain","network",
						"model","type","is_template","tplname","FK_glpi_enterprise","deleted");
				$v = "";
				foreach($atr as $at){
					if(isset($array[$at])){
						$v .= " ".$at."='".$array[$at]."', ";
					}
				}
				if(empty($v)){
					echo "updateComputerInformations: no attributes given ";
					return(false);
				}else{
					$v = preg_replace("/, $/","",$v);
					return($this->query("UPDATE glpi_computers SET ".$v." WHERE name='".$name."';"));
				}
			}else{
				echo "can't update not existing entry";
				return(false);
			}
		}else{
			echo "not connected";
			return(false);
		}

	}

	function addComputerInformations($array)
	{
		if(!is_array($array)){
			echo "updateComputerInformations: first paraeter must be an array";
		}elseif($this->is_connected){
			$atr = array(	"ID","name","serial","otherserial","contact","contact_num",
					"tech_num","comments","date_mod","os","location","domain","network",
					"model","type","is_template","tplname","FK_glpi_enterprise","deleted");
			$v = "";
			$a = "";
			foreach($atr as $at){
				if(isset($array[$at])){
					$a .= $at.", ";
					$v .= "'".$array[$at]."', ";
				}
			}
			if(empty($v)){
				echo "updateComputerInformations: no attributes given ";
				return(false);
			}else{
				$a = preg_replace("/, $/","",$a);
				$v = preg_replace("/, $/","",$v);
				return($this->query("INSERT INTO glpi_computers (".$a.") VALUES (".$v.");"));
			}
		
		}else{
			echo "not connected";
			return(false);
		}

	}

	function removeComputerInformations($id)
	{
/*		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_computers WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("DELETE FROM glpi_computers WHERE ID=".$id.";"));
			}else{
				echo "can't remove not existing entry";
				return(false);
			}
		}else{
			echo "not connected";
			return(false);
		}
*/
	}

	function is_connected()
	{
		return($this->is_connected);
	}

}
//$s = new glpiDB("vserver-01","glpi","tester","glpi");
//print_r($s->query("SELECT * FROM glpi_computers"));
//$s->getComputerInformations("1 OR (c.ID<10000)");
?>
