<?php
/*
 * Created on 24-ott-2006 by "Guido Serra aka Zeph" <zeph@purotesto.it>
 * 
 * PuroTesto (freesoftware developers, Milano, IT)
 * http://purotesto.it
 * 
 * This file is part of OpenCA module for GOsa^2.
 *
 * The OpenCA module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * OpenCA module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */
 
class openca extends plugin
{
  /* Definitions */
  var $plHeadline= "Certificates";
  var $plDescription= "Certification Authority control panel";
  var $departments=array();
 
  /* Dialog attributes */
  var $ssltab= NULL;
  var $sslObjects= array();
  var $ui= NULL;
  var $acl= "";
	var $DivListOpenCA;
	
  function openca ($config, $ui){
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Creat dialog object */
    $this->DivListOpenCA = new divListOpenCA($this->config,$this);
  }

  function execute(){
		/* Call parent execute */
		plugin::execute();

		$smarty= get_smarty();

    /* Display dialog with system list */
    $this->DivListOpenCA->parent = $this;
    $this->DivListOpenCA->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListOpenCA->SubSearch){
      $this->DivListOpenCA->AddDepartments($this->DivListOpenCA->selectedBase);
    }
    $this->reload();
    $this->DivListOpenCA->setEntries($this->sslObjects);
    return($this->DivListOpenCA->Draw());
  }
  
  function remove_lock(){
    if (isset($this->ssltab->dn)){
      del_lock ($this->ssltab->dn);
    }
  }
  
  function reload(){
    /* Load terminal shortcuts */
    $responsible= array();
    foreach ($this->config->departments as $key => $value){
      if (get_module_permission(get_permissions ($value, $this->ui->subtreeACL),
            "openca", $value) == "#all#"){
        $responsible[$key]= $value;
      }
    }

    /* some var init */
    $res               = array();
    $this->sslObjects  = array();
    $userregex         = "";

    /* Set base for all searches */
    $base=  $this->DivListOpenCA->selectedBase;
    
    /* This array represents the combination between checkboxes and search filters */
    $objs = array( 
			"ShowSPKACs",
      "ShowPKCS10s", 
      "ShowCertificates",
      "ShowExpired",
      "ShowRevoked");
    
    
    /* Include the 'Display Systems of user' attribute */ 
    if ((!empty($this->DivListOpenCA->UserRegex)) && ($this->DivListOpenCA->UserRegex!= "*")){
      $userregex = "(gotoLastUser=".$this->DivListOpenCA->UserRegex.")";
    }
    

    if(!$this->DivListOpenCA->SubSearch){
    	$filter = "(|(&".$userregex."(objectClass=gosaCA)(cn=".$this->DivListOpenCA->Regex.")))";
      $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, get_people_ou().$base,
      	array("cn", "uid", "SPKAC"), GL_NONE | GL_SIZELIMIT));
    }else{
    	/* User filter? */
      $filter = "(|(&".$userregex."(objectClass=gosaCA)(cn=".$this->DivListOpenCA->Regex.")))";
      $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, get_people_ou().$base,
      	array("cn", "uid", "SPKAC"), GL_NONE | GL_SUBSEARCH | GL_SIZELIMIT));
    }

    /* Search for incoming objects */ 
    $filter = "(|(&".$userregex."(objectClass=gosaCA)(cn=".$this->DivListOpenCA->Regex.")))";
    $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, "ou=incoming,".$base,
          array("cn", "uid", "SPKAC"), GL_NONE | GL_SIZELIMIT));    
    
		foreach($res as $sslObj){
			$this->sslObjects[] = $sslObj["dn"];
		}

    reset ($this->sslObjects);    
  }
  
   /* Save data to object */
  function save_object()
  {
    $this->DivListOpenCA->save_object();
  }

}

?>
