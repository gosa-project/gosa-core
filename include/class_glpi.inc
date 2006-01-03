<?php

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

	function deviceExists($attr)
	{
		if($this->is_connected){	
			$arr = array_flip(array(	"glpi_device_case"		=> "case",
						"glpi_device_control"	=> "control",	
						"glpi_device_drive"		=> "drive",
						"glpi_device_gfxcard"	=> "gfxcard",	
						"glpi_device_hdd"		=> "hdd",		
						"glpi_device_iface"		=> "iface",
						"glpi_device_moboard"	=> "moboard",
						"glpi_device_pci"		=> "pci",
						"glpi_device_power"		=> "power",
						"glpi_device_processor"	=> "processor",
						"glpi_device_ram"		=> "ram",
						"glpi_device_sndcard"	=> "sndcard"));

			$tbl_name = $arr[$attr['device_type']];
			if(!isset($attr['ID'])){
				return(false);
			}else{
				$qry = "SELECT * FROM ".$tbl_name." WHERE ID=".$attr['ID'].";";
				$res = $this->query($qry);
				if(count($res) != 0){
					return(true);
				}
			}
		}else{
			echo "not connected";
			return(false);
		}

		return(false);
	}

	function deleteDevice($attr)
	{
		if($this->is_connected){
			$arr = array_flip(array(	"glpi_device_case"		=> "case",
						"glpi_device_control"	=> "control",	
						"glpi_device_drive"		=> "drive",
						"glpi_device_gfxcard"	=> "gfxcard",	
						"glpi_device_hdd"		=> "hdd",		
						"glpi_device_iface"		=> "iface",
						"glpi_device_moboard"	=> "moboard",
						"glpi_device_pci"		=> "pci",
						"glpi_device_power"		=> "power",
						"glpi_device_processor"	=> "processor",
						"glpi_device_ram"		=> "ram",
						"glpi_device_sndcard"	=> "sndcard"));
			
			$device_type = $attr['device_type'];
			unset($attr['device_type']);			

			$tbl_name = $arr[$device_type];

			$this->query("DELETE FROM ".$tbl_name." WHERE ID=".$attr['ID'].";");	
		}else{
			echo "not connected";
			return(false);
		}
	}
	function updateDevices($attr)
	{
		if($this->is_connected){
			$arr = array_flip(array(	"glpi_device_case"		=> "case",
						"glpi_device_control"	=> "control",	
						"glpi_device_drive"		=> "drive",
						"glpi_device_gfxcard"	=> "gfxcard",	
						"glpi_device_hdd"		=> "hdd",		
						"glpi_device_iface"		=> "iface",
						"glpi_device_moboard"	=> "moboard",
						"glpi_device_pci"		=> "pci",
						"glpi_device_power"		=> "power",
						"glpi_device_processor"	=> "processor",
						"glpi_device_ram"		=> "ram",
						"glpi_device_sndcard"	=> "sndcard"));
			
			$device_type = $attr['device_type'];
			unset($attr['device_type']);			

			$tbl_name = $arr[$device_type];

			$str = "UPDATE ".$tbl_name." SET ";
			foreach($attr as $name => $value){
				$str.=$name."='".$value."', ";
			}
			$str = preg_replace("/, $/","",$str);
			$str .= " WHERE ID=".$attr['ID'].";";
			$this->query($str);	
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getRAMTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_dropdown_ram_type;"));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}
	

	function getGlpiDeviceControlTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_dropdown_hdd_type;"));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}
	
	function getGlpiGfxControlTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_dropdown_hdd_type;"));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}
	
	
	function addDevice($attr)
	{
		if($this->is_connected){	
			$arr = array_flip(array(	"glpi_device_case"		=> "case",
						"glpi_device_control"	=> "control",	
						"glpi_device_drive"		=> "drive",
						"glpi_device_gfxcard"	=> "gfxcard",	
						"glpi_device_hdd"		=> "hdd",		
						"glpi_device_iface"		=> "iface",
						"glpi_device_moboard"	=> "moboard",
						"glpi_device_pci"		=> "pci",
						"glpi_device_power"		=> "power",
						"glpi_device_processor"	=> "processor",
						"glpi_device_ram"		=> "ram",
						"glpi_device_sndcard"	=> "sndcard"));
			
			$device_type = $attr['device_type'];
			unset($attr['device_type']);			

			$tbl_name = $arr[$device_type];
            $v = "";
            $a = "";
            foreach($attr as $name => $value){
            	$a .= $name.", ";
            	$v .= "'".$value."', ";
            }
            if(empty($v)){
                echo "addDevice: no attributes given ";
                return(false);
            }else{
                $a = preg_replace("/, $/","",$a);
                $v = preg_replace("/, $/","",$v);
                return($this->query("INSERT INTO ".$tbl_name." (".$a.") VALUES (".$v.");"));
            }

        }else{
            echo "not connected";
            return(false);
        }
	}

	function getDevices()
	{
		if($this->is_connected){
			$arr = array(	"glpi_device_case"		=> "case",
							"glpi_device_control"	=> "control",	
							"glpi_device_drive"		=> "drive",
							"glpi_device_gfxcard"	=> "gfxcard",	
							"glpi_device_hdd"		=> "hdd",		
							"glpi_device_iface"		=> "iface",
							"glpi_device_moboard"	=> "moboard",
							"glpi_device_pci"		=> "pci",
							"glpi_device_power"		=> "power",
							"glpi_device_processor"	=> "processor",
							"glpi_device_ram"		=> "ram",
							"glpi_device_sndcard"	=> "sndcard");
		
			$res = array();
			foreach($arr as $glpi => $gosa){
				$qry = "SELECT * FROM ".$glpi.";";
				$ret = $this->query($qry);
				foreach($ret as $id => $entry){
					$entry['device_type'] = $gosa;
					$res[$entry['designation']."-".$gosa] = $entry;
				}
			}
			return($res);
		}else{
			echo "not connected";
			return(false);
		}
	}

	function removeComputerInformations($id)
	{
/*		
		if($this->is_connected){
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
