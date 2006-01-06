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

	/* Manufacturer 
	   Returns all defined manufacturers
	*/	
	function getEnterprises()
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
					$res[$entry['designation']."-".$gosa] = $entry;
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
					$str = "INSERT INTO glpi_computer_device (device_type,FK_device,FK_computers) 
					VALUES (".$deviceMappingTableNameID[$type].",".$entry['ID'].",".$id.");";
					$this->query($str);
				}
			}
		

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
		if($this->is_connected){
			$qry = "SELECT * FROM glpi_docs WHERE name!='';";
			$ret = $this->query($qry);
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
					print $str;
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

}
//$s = new glpiDB("vserver-01","glpi","tester","glpi");
//print_r($s->query("SELECT * FROM glpi_computers"));
//$s->getComputerInformations("1 OR (c.ID<10000)");
?>
