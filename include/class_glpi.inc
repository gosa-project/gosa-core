<?php


class glpiDB{

	var $user		="";
	var $password	="";
	var $server		="";
	var $db			="";

	var $is_connected		= 0;
	var $handle 			= NULL;

	var $lasterror	="";

	var $deviceMappingGOsaGlpi;
	var $deviceMappingTableNameID;

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
		$this->deviceMappingGOsaGlpi = array(
				"glpi_device_case"      => "case",
				"glpi_device_control"   => "control",
				"glpi_device_drive"     => "drive",
				"glpi_device_gfxcard"   => "gfxcard",
				"glpi_device_hdd"       => "hdd",
				"glpi_device_iface"     => "iface",
				"glpi_device_moboard"   => "moboard",
				"glpi_device_pci"       => "pci",
				"glpi_device_power"     => "power",
				"glpi_device_processor" => "processor",
				"glpi_device_ram"       => "ram",
				"glpi_monitors"       => "monitor",
				"glpi_device_sndcard"   => "sndcard");

		$this->deviceMappingTableNameID = array(		"moboard"	=> 1,										
				"processor"	=> 2,										
				"ram"		=> 3,										
				"hdd"		=> 4,										
				"iface"		=> 5,										
				"drive"		=> 6,										
				"control"	=> 7,										
				"gfxcard"	=> 8,										
				"sndcard"	=> 9,										
				"pci"		=> 10,										
				"case"		=> 11,										
				"power"		=> 12);


	}

	function SelectDB()
	{
		if($this->is_connected){
			mysql_select_db($this->db,$this->handle);
		}
	}


	/* This functions checks if the selected computer/network 
	   device is already available in the db
	 */
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

	/* this function queries everything 
	 */
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

	/* System types 
       Returns all defined system types 
	 */
	function getSystemTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_type_computers;"));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			asort($ret);
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* System types 
	   Update a system type 	
	*/
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

	/* System types 
       Add one entry to the system types 
     */
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

	/* System types 
       Remove one entry from the system types (specified by ID=$id)
     */
	function removeSystemType($id)
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

	/* System type is used */
	function is_systemTypeUsed($ID){
		if($this->is_connected){
			$ret = array();
			$qry="SELECT name,type FROM glpi_computers WHERE type=".$ID." LIMIT 3;";
			$res = $this->query($qry);
			foreach($res as $val){
				$ret[$val['name']] = $val['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}


	/* Manufacturer 
	   Returns all defined manufacturers
	*/	
	function getEnterprises()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = $this->query("SELECT * FROM glpi_enterprises ORDER BY name;");
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Manufacturer 
	   Returns single manufacturer
	*/	
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

	/* Manufacturer 
       Updates already existing manufacturer
    */
	function updateEnterprise($array,$id)
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

	/* Manufacturer
	   Add new manufacturer
	 */
	function addEnterprise($array)
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

	/*	Manufacturer
		remove manufacturer
	 */
	function removeEnterprise($id)
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

	/* Operating systems 
	   Returns all OSs
	 */
	function getOSTypes($keys = false)
	{
		if($this->is_connected){
			$ret = array();
			$tmp=($this->query("SELECT * FROM glpi_dropdown_os ORDER by name;"));

			if($keys){
				foreach($tmp as $t){
					$ret[$t['name']]=$t['ID'];
				}
			}else{
				foreach($tmp as $t){
					$ret[$t['ID']]=$t['name'];
				}
			}
			return($ret);

		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Operating system is used ? */
	function is_osUsed($ID){
		if($this->is_connected){
			$ret = array();
			$qry="SELECT name,type FROM glpi_computers WHERE os=".$ID." LIMIT 3;";
			$res = $this->query($qry);
			foreach($res as $val){
				$ret[$val['name']] = $val['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}


	/*  Operating systems
		Add a new operating system to the dropdown menus   
	 */
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

    /* Operating systems 
       remove one OS entry
     */
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

	/* Operating systems 
	   Update existing OS entry
	*/
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

	/* This returns all available glpi users 
     */
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

	/* this function adds a new glpi user
     */
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

	/* This function updates a glpi user 
       with the given data
     */
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

	/* This function returns all available data 
       from a specified dn
     */
	function getComputerInformations($name)
	{
		if($this->is_connected){	
			$ret = $this->query("SELECT * FROM glpi_computers WHERE name='".$name."';");
			return($ret);		
		}else{
			echo "not connected";
			return(false);
		}
	}

	/*  This fucntions updates an already existing entry 
     */
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

	/* This function adds a new inventory device (computer phone etc)
     */
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

	/* this functions checks if the given Device 
     * already exists 
     */
	function deviceExists($attr)
	{
		$deviceMappingGOsaGlpi = $this->deviceMappingGOsaGlpi;
		if($this->is_connected){	
			$arr = array_flip($deviceMappingGOsaGlpi);

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

		
	/* Check if given device is used by some accounts
     *  (helpfull to avoid removement of used devices)
     */
	function is_deviceUsed($item)
	{
		$deviceMappingGOsaGlpi 		= array_flip($this->deviceMappingGOsaGlpi);
		$deviceMappingTableNameID 	= $this->deviceMappingTableNameID;
		if($this->is_connected){
			$tablename =  $deviceMappingGOsaGlpi[$item['device_type']];
			$type = $item['device_type'];

			$ret = array();

			if($type=="monitor"){
				$str = "SELECT c.name FROM glpi_connect_wire as w, glpi_computers as c WHERE w.end1=".$item['ID']." AND w.end2 = c.ID AND w.type=4;"; 
			}else{
				$str = "SELECT c.name FROM glpi_computer_device as d, glpi_computers as c WHERE d.FK_computers=c.ID AND FK_device=".$item['ID']." AND device_type=".$deviceMappingTableNameID[$type]." ;";
			}

			$res = $this->query($str);
	
			foreach($res as $val){
				$ret[$val['name']] = $val['name'];
			}	

			return($ret);//count($this->query($str)));
		}else{
			echo "not connected";
			return(false);
		}
			
	}


	/* This functions deletes a specified entry 
     * from our device tables 
     */
	function deleteDevice($attr)
	{
		$deviceMappingGOsaGlpi = $this->deviceMappingGOsaGlpi;
		if($this->is_connected){
			$arr = array_flip($deviceMappingGOsaGlpi);
			
			$device_type = $attr['device_type'];
			unset($attr['device_type']);			

			$tbl_name = $arr[$device_type];

			$this->query("DELETE FROM ".$tbl_name." WHERE ID=".$attr['ID'].";");	
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* This funtions updated an already existing device
     */
	function updateDevices($attr)
	{
		$deviceMappingGOsaGlpi = $this->deviceMappingGOsaGlpi;
		if($this->is_connected){
			$arr = array_flip($deviceMappingGOsaGlpi);
			
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

	/* Returns all possible RAM types 
     * like SDRAM , DIMM .....
     */
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
	
	/* Returns all possible HDD connection types 
     * like IDE SCSI ...
     */
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
	
	/* Returns all possible gfx card connection types
     * like PCI-X PCI AGP ....
 	 */
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
	
	/* Devices 
       Adds a new single device to our db
    */	
	function addDevice($attr)
	{
		$deviceMappingGOsaGlpi = $this->deviceMappingGOsaGlpi;
		if($this->is_connected){	
			$arr = array_flip($deviceMappingGOsaGlpi);
			
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

	/* Return all available devices 
     */
	function getDevices()
	{
		$deviceMappingGOsaGlpi = $this->deviceMappingGOsaGlpi;
		if($this->is_connected){
			$arr = $deviceMappingGOsaGlpi; 
		
			$res = array();
			foreach($arr as $glpi => $gosa){
				$qry = "SELECT * FROM ".$glpi.";";
				$ret = $this->query($qry);
				foreach($ret as $id => $entry){
					$entry['device_type'] = $gosa;
	
					if(isset($entry['designation'])){
						$res[$entry['designation']."-".$gosa] = $entry;
					}else{
						$res[$entry['name']."-".$gosa] = $entry;
					}
				}
			}
			return($res);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* This function returns all used devices 
     */
	function getUsedDevices($computerID)
	{
		$deviceMappingGOsaGlpi = array_flip($this->deviceMappingGOsaGlpi);
		$deviceMappingTableNameID	= $this->deviceMappingTableNameID;
	
		if($this->is_connected){
			$qry = "SELECT * FROM glpi_computer_device WHERE FK_computers=".$computerID.";";
			$res = $this->query($qry);
		
			$ret = array();
			foreach($deviceMappingGOsaGlpi as $GOsa => $glpi){
				$ret[$GOsa] = array();
			}

			$tbls = array_flip($deviceMappingTableNameID);

			foreach($res as $device){
				$devtype = $tbls[$device['device_type']];
				$tbl_name = $deviceMappingGOsaGlpi[$devtype];
				$qry = ("SELECT * FROM ".$tbl_name." WHERE ID=".$device['FK_device'].";");
				$res2 = $this->query($qry);
				if(count($res2)!=0){
					$ret[$devtype][$res2[0]['designation']]=$res2[0];
				}

			$qry = "SELECT * FROM glpi_connect_wire WHERE type=4 AND end2=".$computerID.";";
			$res2 = $this->query($qry);
			foreach($res2 as $monitor){
				$qry = "SELECT * FROM glpi_monitors WHERE ID=".$monitor['end1'].";";
				$res3 = $this->query($qry);
				foreach($res3 as $moni){
					$ret['monitor'][$moni['name']]=$moni;
				}
			}



			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* This function removes all given devices from a computer, specified by $id
       In the next step all devices specified by devices will be added.
	 */
	function addDevicesToComputer($devices, $id)
	{
		$deviceMappingGOsaGlpi = array_flip($this->deviceMappingGOsaGlpi);
		$deviceMappingTableNameID	= $this->deviceMappingTableNameID;
	
		if(($id == "" )||(!is_numeric($id))){
			return (false); 
		}
		if($this->is_connected){
			$qry = "DELETE FROM glpi_computer_device WHERE FK_computers=".$id.";";
			$this->query($qry);	
		
			foreach($devices as $type => $entries){
				foreach($entries as $entry){
					if($type=="monitor"){
						$str = "INSERT INTO glpi_connect_wire (end1,end2,type) 
						VALUES (".$entry['ID'].",".$id.",4);";
					}else{
						$str = "INSERT INTO glpi_computer_device (device_type,FK_device,FK_computers) 
						VALUES (".$deviceMappingTableNameID[$type].",".$entry['ID'].",".$id.");";
					}
					$this->query($str);
				}
			}
		

		}else{
			echo "not connected";
			return(false);
		}

	}

	function removeComputerInformations($name)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_computers WHERE name='".$name."';");
			if(isset($tmp[0])){
				$id = $tmp[0]['ID'];
				$this->query("DELETE FROM glpi_connect_wire WHERE end2=".$id.";");
				$this->query("DELETE FROM glpi_computer_device WHERE FK_computers=".$id.";");
				return($this->query("DELETE FROM glpi_computers WHERE ID=".$id.";"));
			}else{
				echo "can't remove not existing entry";
				return(false);
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	function is_connected()
	{
		return($this->is_connected);
	}

	function addAttachmentsToComputer($attr,$id)
	{
        if(($id == "" )||(!is_numeric($id))){
            return (false);
        }
        if($this->is_connected){
            $qry = "DELETE FROM glpi_doc_device WHERE (FK_device=".$id.") AND (device_type=1);";
            $this->query($qry);
			
			foreach($attr as $aid => $entry){
				$str = "INSERT INTO glpi_doc_device (FK_doc,FK_device,device_type,is_template) 
												VALUES
													($aid,$id,1,'0');";
				$this->query($str);
			}
        }else{
            echo "not connected";
            return(false);
        }
	}

	function getAssignAttachments($id)
	{

		if($this->is_connected){
			$qry= "SELECT * FROM glpi_doc_device WHERE (device_type=1) AND (FK_device=".$id.");";
			$ret = $this->query($qry);
			return($ret);
		}else{
            echo "not connected";
            return(false);
        }
	}

	function deleteAttachment($id)
	{
		if($this->is_connected){
			$qry = "DELETE FROM glpi_docs WHERE ID=".$id."";
			$this->query($qry);
		}else{
            echo "not connected";
            return(false);
        }
	}
	
	function getAttachments()
	{
		$ret = array();
		if($this->is_connected){
			$qry = "SELECT * FROM glpi_docs WHERE name!='';";
			$re = $this->query($qry);

			foreach($re as $entry){
				$ret[$entry['ID']]=$entry;
			}

			return($ret);
		}else{
            echo "not connected";
            return(false);
        }
	}
	
	function saveAttachments($attrs,$id = -1)
	{
		if($this->is_connected){
			$atr = array("name","filename","rubrique","mime","date_mod","comment","deleted","link");
			$tmp = array();
			foreach($atr as $at){
				if(isset($attrs[$at])){
					$tmp[$at] = $attrs[$at];
				}
			}
			if(count($tmp)==0){
				return(false);
			}else{

				// Add
				if($id == -1){
					$str = "INSERT INTO glpi_docs ";
					$namen = "";
					$values= "";
					foreach($tmp as $name => $value){	
						$namen .= $name.", ";
						if(is_numeric($value)){
							$values .= $value.", ";
						}else{
							$values .= "'".$value."', ";
						}
					}
					$values = preg_replace("/, $/","",$values);
					$namen  = preg_replace("/, $/","",$namen);
					$str .= "(".$namen.") VALUES (".$values.");";
				}else{
					$str = "UPDATE glpi_docs SET ";
					foreach($tmp as $name => $value){	
						$str .= $name."= ";
						if(is_numeric($value)){
							$str .= $value.", ";
						}else{
							$str .= "'".$value."', ";
						}
					}
					$str = preg_replace("/, $/","",$str);
					$str .= " WHERE ID=".$id.";";
				}
				$this->query($str);
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	
	/* Check if given attachment id is used in any Device 
		( - avoid removing of used attachments)
     */
	function is_attachmentUsed($id)
	{
		if($this->is_connected){
			$ret = array();
			$qry = "SELECT t.name FROM glpi_computers as t, glpi_doc_device WHERE t.ID = glpi_doc_device.FK_device AND FK_doc =".$id." LIMIT 3;";
			$res = $this->query($qry);
			foreach($res as $val){
				$ret[$val['name']] = $val['name'];
			}
			return($ret);
		}else{
            echo "not connected";
            return(false);
        }
	}

		
	/* Monitor handling 
     */
    function getMonitors()
	{
		if($this->is_connected){
			$qry= "SELECT * FROM glpi_monitors;";
			return($this->query($qry));	
		
		}else{
            echo "not connected";
            return(false);
        }
	}

    function updatedMonitor()
	{
		if($this->is_connected){
//			$qry= "SELECT * FROM glpi_monitors;";
//			return($this->query($qry));	
		
		}else{
            echo "not connected";
            return(false);
        }
	}

    function addMonitor()
	{
		if($this->is_connected){
//			$qry= "SELECT * FROM glpi_monitors;";
//			return($this->query($qry));	
		
		}else{
            echo "not connected";
            return(false);
        }
	}

    function removeMonitor($id)
	{
		if($this->is_connected){
			$qry= "DELETE FROM glpi_monitors WHERE ID=".$id.";";
			$this->query($qry);	
		}else{
            echo "not connected";
            return(false);
        }
	}

    function getMonitorTypes()
	{
		if($this->is_connected){
			$qry= "SELECT * FROM glpi_type_monitors;";
			return($this->query($qry));	
		
		}else{
            echo "not connected";
            return(false);
        }
	}

    function getLocationTypes()
	{
		if($this->is_connected){
			$qry= "SELECT * FROM glpi_dropdown_locations;";
			return($this->query($qry));	
		
		}else{
            echo "not connected";
            return(false);
        }
	}

    function getStateTypes()
	{
		if($this->is_connected){
			$qry= "SELECT * FROM glpi_dropdown_state;";
			return($this->query($qry));	
		}else{
            echo "not connected";
            return(false);
        }
	}

	
	/* Printer functions
     */

	/* is printer type used ? 
     */
	function is_printerTypeUsed($id)
	{
		if($this->is_connected){
			$qry = "SELECT * FROM glpi_printers WHERE type=".$id.";";
			$res = $this->query( $qry);
			$ret =array();
			foreach($res as $entry){
				$ret[$entry['ID']] = $entry['name'];
			}
			return($ret);
		}else{
            echo "not connected";
            return(false);
        }
	}

	/* This functions checks if the selected computer/network 
	   device is already available in the db
	 */
	function is_printer_account($dn)
	{
		if(!$this->is_connected){
			$this->lasterror ="Can't query anything, if we aren't connected.";
			return(false);
		}else{
			$qry = "SELECT * FROM glpi_printers WHERE name='".$dn."';";
			$res = $this->query($qry);
			if(count($res)==0){
				return(false);
			}else{
				return(true);
			}
		}
	}

	/* This function returns all available data 
       from a specified dn
     */
	function getPrinterInformations($name)
	{
		if($this->is_connected){	
			$ret = $this->query("SELECT * FROM glpi_printers WHERE name='".$name."';");
			return($ret);		
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Get Printer attachments
     */
	function getAssignPrinterAttachments($id)
	{

		if($this->is_connected){
			$qry= "SELECT * FROM glpi_doc_device WHERE (device_type=3) AND (FK_device=".$id.");";
			$ret = $this->query($qry);
			return($ret);
		}else{
            echo "not connected";
            return(false);
        }
	}

	/* Printer types 
       Returns all defined printer types 
	 */
	function getPrinterTypes()
	{
		if($this->is_connected){
			$ret = array();
			$tmp = ($this->query("SELECT * FROM glpi_type_printers ORDER BY name; "));
			foreach($tmp as $t){
				$ret[$t['ID']]=$t['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Add pritner types 
       Add one entry to the printer types 
     */
	function addPrinterType($name)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_printers WHERE name='".$name."';");
			if(isset($tmp[0])){
				//echo "such an entry already exists";
				return(false);
			}else{	
				return($this->query("INSERT INTO glpi_type_printers (name) VALUES ('".$name."');"));
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* remove printer types 
       Remove one entry from the printer types (specified by ID=$id)
     */
	function removePrinterType($id)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_printers WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("DELETE FROM glpi_type_printers WHERE ID=".$id.";"));	
			}else{
				echo "can't remove not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Update printer types 
	   Update a printer type 	
	*/
	function updatePrinterType($name,$id)
	{

		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_type_printers WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("UPDATE glpi_type_printers SET name='".$name."' WHERE ID=".$id.";"));	
			}else{
				echo "can't update not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}


	/*  This fucntions updates an already existing entry 
     */
	function updatePrinterInformations($array,$name)
	{
		if(!is_array($array)){
			echo "updatePrinterInformations: first paraeter must be an array";
		}elseif($this->is_connected){
			$tmp = $this->query("SELECT * FROM  glpi_printers WHERE name='".$name."';");
			if(isset($tmp[0])){

				$atr = array(	"ID","name","serial","otherserial","contact","contact_num",
						"tech_num","comments","date_mod","location","domain","network","ramSize","flags_serial","flags_par","flags_usb",
						"model","type","is_template","tplname","FK_glpi_enterprise","deleted");
				$v = "";
				foreach($atr as $at){
					if(isset($array[$at])){
						$v .= " ".$at."='".$array[$at]."', ";
					}
				}
				if(empty($v)){
					echo "updateSystemInformations: no attributes given ";
					return(false);
				}else{
					$v = preg_replace("/, $/","",$v);
					return($this->query("UPDATE glpi_printers SET ".$v." WHERE name='".$name."';"));
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

	/* This function adds a new inventory settings for printers
     */
	function addPrinterInformations($array)
	{
		if(!is_array($array)){
			echo "updateComputerInformations: first paraeter must be an array";
		}elseif($this->is_connected){
			$atr = array(	"ID","name","serial","otherserial","contact","contact_num",
					"tech_num","comments","date_mod","os","location","domain","network","ramSize","flags_serial","flags_par","flags_usb",
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
				return($this->query("INSERT INTO glpi_printers (".$a.") VALUES (".$v.");"));
			}
		
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* add atachment to given printer */
	function addAttachmentsToPrinter($attr,$id)
	{
        if(($id == "" )||(!is_numeric($id))){
            return (false);
        }
        if($this->is_connected){
            $qry = "DELETE FROM glpi_doc_device WHERE (FK_device=".$id.") AND (device_type=3);";
            $this->query($qry);
			
			foreach($attr as $aid => $entry){
				$str = "INSERT INTO glpi_doc_device (FK_doc,FK_device,device_type,is_template) 
												VALUES
													($aid,$id,3,'0');";
				$this->query($str);
			}
        }else{
            echo "not connected";
            return(false);
        }
	}

	function removePrinterInformations($name)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_printers WHERE name='".$name."';");
			if(isset($tmp[0])){
				$id = $tmp[0]['ID'];
//				$this->query("DELETE FROM glpi_connect_wire WHERE end2=".$id.";");
				$this->query("DELETE FROM glpi_doc_device WHERE FK_device=".$id." AND device_type=3;");
				return($this->query("DELETE FROM glpi_printers WHERE ID=".$id.";"));
			}else{
				echo "can't remove not existing entry";
				return(false);
			}
		}else{
			echo "not connected";
			return(false);
		}
	}


	/* Cartridges 
     */
	/* return all assigned cartridges */
	function getUsedCartridges($printerID)
	{
		if($this->is_connected){
			$ret = array();
			$qry = "SELECT 
						c.date_use			as date_use,
						c.ID 				as ID, 
						t.ID 				as type_ID,
						t.name				as name, 
						c.FK_glpi_printers	as FK_glpi_printers, 
						d.name 				as type_name 
					FROM 
						glpi_dropdown_cartridge_type as d,
						glpi_cartridges as c, 
						glpi_cartridges_type as t 
					WHERE 	c.FK_glpi_cartridges_type = t.ID 
						AND t.type = d.ID 
						AND c.FK_glpi_printers = ".$printerID.";"; 
			$res = $this->query($qry);
			foreach($res as $entry){
				$ret[$entry['ID']] = $entry;
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* return all assigned cartridges */
	function getAvailableCartridgeTypes($printerTypeID)
	{
		if($this->is_connected){
			$ret = array();
			$qry= "
	SELECT 	
			ct.ID 			as cartridgeID,
			ct.name 		as cartridgeName,
			pt.ID 			as printerTypeID,
			pt.name 		as printerTypeName,
			ct.type 		as cartridgeTypeID,
			dt.name 		as cartridgeTypeName 
	FROM 	
			glpi_type_printers 				as pt, 
			glpi_cartridges_type 			as ct, 
			glpi_dropdown_cartridge_type 	as dt, 
			glpi_cartridges_assoc 			as ac
	 WHERE 
				ac.FK_glpi_type_printer = pt.ID 
			AND ac.FK_glpi_cartridges_type = ct.ID 
			AND ct.type=dt.ID 
			AND pt.ID=".$printerTypeID.";";
			$res = $this->query($qry);
			foreach($res as $entry){
				$ret[$entry['cartridgeID']] = $entry;
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	function removeCartridgeFromPrinter($cartridgeID)
	{
		if($this->is_connected){
			$qry = "DELETE FROM glpi_cartridges WHERE ID=".$cartridgeID.";";
			return($this->query($qry));
		}else{
			echo "not connected";
			return(false);
		}
	}

	function addCartridgeFromPrinter($printerID,$cartridgeID)
	{
		if($this->is_connected){
			$qry ="INSERT INTO 
						glpi_cartridges (FK_glpi_cartridges_type,FK_glpi_printers,date_in,date_use) 
				   VALUES 
						(".$cartridgeID.",".$printerID.",'".date("Y-m-d")."','".date("Y-m-d")."');";	
			return($this->query($qry));
		}else{
			echo "not connected";
			return(false);
		}
	}

	function is_cartridgeTypeUsed($id){
		if($this->is_connected){
			$qry = "SELECT p.ID,p.name as name FROM glpi_cartridges as c,glpi_printers as p WHERE p.ID=c.FK_glpi_printers AND c.FK_glpi_cartridges_type=".$id.";";
			$res = $this->query($qry);
			$ret =array();
			foreach($res as $entry){
				$ret[$entry['ID']] = $entry['name'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getCartridgeTypeInformations($id = "all"){
		if($this->is_connected){
			$ret = array();			
			if($id != "all"){
				$qry = "SELECT * FROM glpi_cartridges_type WHERE ID = ".$id.";";
			}else{
				$qry = "SELECT * FROM glpi_cartridges_type;";
			}
		
			$res = ($this->query($qry));
			foreach($res as $entry){
				$ret[$entry['ID']] = $entry;
			}	
			return($ret);

		}else{
			echo "not connected";
			return(false);
		}
	}

	function getCartridgeTypes(){
		if($this->is_connected){
			$ret = array();			
			$qry = "SELECT * FROM glpi_dropdown_cartridge_type;";
			$res = ($this->query($qry));
			foreach($res as $entry){
				$ret[$entry['ID']] = $entry['name'];
			}	
			return($ret);

		}else{
			echo "not connected";
			return(false);
		}
	}



	/* check if given manufacturer ID ist still in use.
	   The problem is, that nearly every table uses manufacturers .... 
     */
	function is_manufacturerUsed($id)
	{
		if($this->is_connected){
			$tables = array();
			foreach($this->deviceMappingGOsaGlpi as $entry => $table){
				$tables[] = $entry;
			}
			$tables[] ="glpi_computers";
			$tables[] ="glpi_cartridges_type";
			$ret = array();
			$i = 3;
			foreach($tables as $tbl){
				if($i <= 0 ) continue;
				$qry = "SELECT * FROM ".$tbl." WHERE FK_glpi_enterprise = ".$id.";";
				$res = $this->query($qry);
				foreach($res as $entry){
					if($i <= 0 ) continue;
					if(isset($entry['designation'])){
						$entry['name'] = $entry['designation'];
					}
					$i --;
						$ret[] = $entry['name'];
				}
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Manufacturer 
	   Updates already existing manufacturer
	 */
	function Add_UpdateCatrigdeType($array,$array_printer_types)
	{
		if(!is_array($array)){
			echo "Add_UpdateCatrigdeType: first paraeter must be an array";
		}elseif($this->is_connected){


			$atr = array("name","ref","location","type","FK_glpi_enterprise","tech_num","deleted","comments","alarm");

			/* Entry was edited */
			if($array['ID']>0){
				$qry = "DELETE FROM glpi_cartridges_assoc WHERE FK_glpi_cartridges_type=".$array['ID'].";";

				$v = "";
				foreach($atr as $at){
					if(isset($array[$at])){
						$v .= " ".$at."='".$array[$at]."', ";
					}
				}
				if(empty($v)){
					echo "Add_UpdateCatrigdeType: no attributes given ";
					return(false);
				}else{
					$v = preg_replace("/, $/","",$v);
					$qry = "UPDATE glpi_cartridges_type SET ".$v." WHERE ID='".$array['ID']."';";
					$this->query($qry);	
				}
			}else{

				/* skip if name is in use*/
				$qry = "SELECT * FROM glpi_cartridges_type WHERE name='".$array['name']."';";
				if(count($this->query($qry))){
					return;
				}

				$str = "INSERT INTO glpi_cartridges_type ";
				$namen = "";
				$values= "";
				foreach($array as $name => $value){	
					$namen .= $name.", ";
					if(is_numeric($value)){
						$values .= $value.", ";
					}else{
						$values .= "'".$value."', ";
					}
				}
				$values = preg_replace("/, $/","",$values);
				$namen  = preg_replace("/, $/","",$namen);
				$str .= "(".$namen.") VALUES (".$values.");";
				$this->query($str);	
				$IDs = $this->query("SELECT ID FROM glpi_cartridges_type WHERE name='".$array['name']."';");
				if(count($IDs) > 1){
					echo "internal db error";
					return;
				}
				$array['ID'] = $IDs[0]['ID'];
			}

			foreach($array_printer_types as $id){
				$qry = "INSERT INTO glpi_cartridges_assoc 
					(FK_glpi_cartridges_type,FK_glpi_type_printer) 
					VALUES 
					(".$array['ID'].",".$id.")";

				$this->query($qry);
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getSupportedPrinterTypeIDsForCartridge($cid)
	{
		if($this->is_connected){
			$ret = array();
			$qry = "SELECT FK_glpi_type_printer FROM glpi_cartridges_assoc WHERE FK_glpi_cartridges_type = ".$cid.";";	
			$res = $this->query($qry);
				
			foreach($res as $entry => $value){
				$ret[$value['FK_glpi_type_printer']] = $value['FK_glpi_type_printer'];
			}
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}

	function removeCartridgeType($id){
		if($this->is_connected){
			$qry = "DELETE FROM glpi_cartridges_assoc WHERE FK_glpi_cartridges_type=".$id.";";
			$this->query($qry);	
			$qry = "DELETE FROM glpi_cartridges_type WHERE ID=".$id.";";
			return($this->query($qry));	
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getCartridgesWhichUseThisType($id)
	{
		if($this->is_connected){
			$qry = "SELECT * FROM glpi_cartridges WHERE FK_glpi_cartridges_type=".$id.";";
			$ret  = $this->query($qry);
			return($ret);
		}else{
			echo "not connected";
			return(false);
		}
	}


	/* Add pritner types 
       Add one entry to the cartridgeType types 
     */
	function addCartridgeDropdownType($name)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_dropdown_cartridge_type WHERE name='".$name."';");
			if(isset($tmp[0])){
				//echo "such an entry already exists";
				return(false);
			}else{	
				return($this->query("INSERT INTO glpi_dropdown_cartridge_type (name) VALUES ('".$name."');"));
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* remove cartridgeType types 
       Remove one entry from the cartridgeType types (specified by ID=$id)
     */
	function removeCartridgeDropdownType($id)
	{
		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_dropdown_cartridge_type WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("DELETE FROM glpi_dropdown_cartridge_type WHERE ID=".$id.";"));	
			}else{
				echo "can't remove not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	/* Update cartridgeType  
	   Update a cartridgeType  	
	*/
	function updateCartridgeDropdownType($name,$id)
	{

		if($this->is_connected){
			$tmp = $this->query("SELECT * FROM glpi_dropdown_cartridge_type WHERE ID=".$id.";");
			if(isset($tmp[0])){
				return($this->query("UPDATE glpi_dropdown_cartridge_type SET name='".$name."' WHERE ID=".$id.";"));	
			}else{
				echo "can't update not existing entry";
				return(false);	
			}
		}else{
			echo "not connected";
			return(false);
		}
	}

	function getUsedDropdownTypes($id=false)
	{
		if($this->is_connected){
			if($id){
				$qry = "SELECT distinct(type) FROM glpi_cartridges_type WHERE type = ".$id.";";
			}else{
				$qry = "SELECT distinct(type) FROM glpi_cartridges_type;";
			}
			return($this->query($qry));
		}else{
			echo "not connected";
			return(false);
		}
	}

}
//$s = new glpiDB("vserver-01","glpi","tester","glpi");
//print_r($s->query("SELECT * FROM glpi_computers"));
//$s->getComputerInformations("1 OR (c.ID<10000)");
?>
