<?php

class hotplugDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array("HOT_name","HOT_description","HOT_id","HOT_produkt","HOT_vendor");
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  

  var $HOT_name                 = "";
  var $HOT_description          = "";
  var $HOT_id                   = "";
  var $HOT_produkt              = "";
  var $HOT_vendor               = "";

  var $hotplugDevices           = ""; 
  var $hotplugDeviceList        = array(); 
  var $regex                    = "*";
  var $depselect                = "/";
  var $deplist                  = "/";
  var $skipThese                = array();

  function hotplugDialog ($config, $dn= NULL,$use_existing=false ,$skipThese = array())
  {
    $this->use_existing = $use_existing;
    $this->skipThese    = $skipThese;
    plugin::plugin ($config, $dn);
    $this->depselect = $this->config->current['BASE'];
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    foreach($this->attributes as $s_attr){
      $smarty->assign($s_attr,$this->$s_attr);
    }

    if($this->use_existing){

      if(isset($_POST['dialogissubmitted'])){
        $this->regex=$_POST['regexHot'];
        $this->depselect = $_POST['depselectHot'];
      }

      if((isset($_GET['search']))&&(!empty($_GET['search']))){
        $this->regex=$_GET['search']."*";
        $this->regex=preg_replace("/\*\*/","*",$this->regex);
      }

      $this->deplist=$this->config->idepartments;

      $this->hotplugDevices = $this->getHotplugs();
      $smarty->assign("regexHot"    ,$this->regex);
      $smarty->assign("deplistHot"  ,$this->deplist);
      $smarty->assign("depselectHot",$this->depselect);
      $smarty->assign("apply", apply_filter());
      $smarty->assign("alphabet", generate_alphabet());
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("deplist", $this->config->idepartments);


      $smarty->assign("hotplugDevices",$this->hotplugDevices);
      $smarty->assign("hotplugDeviceKeys",array_flip($this->hotplugDevices));
      $display.= $smarty->fetch(get_template_path('hotplugDialog.tpl', TRUE,dirname(__FILE__)));
    }else{
      $display.= $smarty->fetch(get_template_path('hotplugDialogNew.tpl', TRUE,dirname(__FILE__)));
    }
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['dialogissubmitted'])){
      foreach($this->attributes as $s_attr){
        if(isset($_POST[$s_attr])){
          $this->$s_attr = stripslashes($_POST[$s_attr]);
        }else{
          $this->$s_attr = false;
        }
      }
      plugin::save_object();
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!$this->use_existing){
      if((empty($this->HOT_name))||(preg_match("/[^a-z0-9]/i",$this->HOT_name))){
        $message[]=_("Please specify a valid name. Only 0-9 a-Z is allowed.");
      }
      if((empty($this->HOT_description))||(preg_match("/[^a-z0-9!\"?.,;:-_\(\) ]/i",$this->HOT_description))){
        $message[]=_("Invalid character in description. Please specify a valid description.");
      }
      if((empty($this->HOT_id))||(preg_match("/[\|]/i",$this->HOT_id))){
        $message[]=_("Please specify a valid id.");
      }
      if((empty($this->HOT_vendor))||(preg_match("/[\|]/i",$this->HOT_vendor))){
        $message[]=_("Please specify a valid id.");
      }
      if((empty($this->HOT_produkt))||(preg_match("/[\|]/i",$this->HOT_produkt))){
        $message[]=_("Please specify a valid id.");
      }

    $ldap = $this->config->get_ldap_link();
    $ldap->search("(&(objectClass=gotoEnvironment)(gotoHotplugDevice=".$this->HOT_name."*))",array("gotoHotplugDevice"));
    if($ldap->count()){
      $message[]=_("An Entry with this name already exists.");
    }

    }else{
      if((!isset($_POST['hotplugName']))||(empty($_POST['hotplugName']))){
        $message[] = _("Please select an entry or press cancel.");
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* return generated entry from input fields*/
    if(!$this->use_existing){
      $a_return=array();
      $a_return[$this->HOT_name]['name']        = $this->HOT_name; 
      $a_return[$this->HOT_name]['description'] = $this->HOT_description; 
      $a_return[$this->HOT_name]['id']          = $this->HOT_id;
      $a_return[$this->HOT_name]['produkt']     = $this->HOT_produkt;
      $a_return[$this->HOT_name]['vendor']      = $this->HOT_vendor;
      return($a_return); 
    }else{
      $entries = array();
      foreach($_POST['hotplugName'] as $name){
        $entries[$name] = $this->hotplugDeviceList[$name];
      }
      return $entries;
    }
  }

  function getHotplugs(){
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->depselect);

    $ldap->search("(&(objectClass=gotoEnvironment)(gotoHotplugDevice=".$this->regex."))",array("gotoHotplugDevice")); 

    $a_return = array();  
    $this->hotplugDeviceList = array();
    while($attr = $ldap->fetch()){
      if(isset($attr['gotoHotplugDevice'])){
        unset($attr['gotoHotplugDevice']['count']);
        foreach($attr['gotoHotplugDevice'] as $device){
          $tmp =$tmp2= array();
          $tmp = split("\|",$device);
          
          if(preg_match("/^".str_replace("*","",$this->regex).".*/i",$tmp[0])){
  
            if(in_array($tmp[0],$this->skipThese)) continue;

            $a_return[$tmp[0]]= $tmp[0]." [".$tmp[1]."] ".$tmp[2];
            
            $tmp2['name']         = $tmp[0];
            $tmp2['description']  = $tmp[1];
            $tmp2['id']           = $tmp[2];

            /* Produkt ID */
            if(!isset($tmp[3])){
              $tmp[3] = "";
            }
            /* Vendor ID */
            if(!isset($tmp[4])){
              $tmp[4] = "";
            }

            $tmp2['produkt']      = $tmp[3];
            $tmp2['vendor']       = $tmp[4];

            $this->hotplugDeviceList[$tmp[0]]=$tmp2;
          }
        }
      }
    }
  return($a_return);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
