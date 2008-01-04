<?php

class hotplugDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $hotplugDevices           = ""; 
  var $hotplugDeviceList        = array(); 
  var $regex                    = "*";
  var $depselect                = "/";
  var $deplist                  = "/";
  var $skipThese                = array();

  function hotplugDialog (&$config,$skipThese = array())
  {
    $this->skipThese  = $skipThese;
    $this->config     = $config;
    $this->depselect   = session::get('CurrentMainBase');
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

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
    return($display);
  }


  /* Save to LDAP */
  function save()
  {
    $entries = array();
    foreach($_POST['hotplugName'] as $name){
      $entries[$name] = $this->hotplugDeviceList[$name];
    }
    return $entries;
  }

  function getHotplugs()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->cd(get_ou('deviceou').$this->depselect);
    $ldap->search("(&(objectClass=gotoDevice)(|(cn=".$this->regex.")(description=".$this->regex.")))",array("gotoHotplugDevice","cn","dn")); 

    $a_return = array();  
    $this->hotplugDeviceList = array();
    while($attr = $ldap->fetch()){

      if(isset($attr['gotoHotplugDevice'][0])){

        $hot_plug = $attr['gotoHotplugDevice'][0];
        $tmp      = preg_split("/\|/",$hot_plug);
        $tmp2     = array();
          
        if(in_array($attr['cn'][0],$this->skipThese)){
          continue;
        }

        $tmp2['name']         = $attr['cn'][0];
        $tmp2['description']  = $tmp[0];
        $tmp2['id']           = $tmp[1];
        $tmp2['produkt']      = $tmp[2];
        $tmp2['vendor']       = $tmp[3];
        $tmp2['dn']           = $attr['dn'];

        $a_return[$attr['cn'][0]]= $attr['cn'][0]." [".$tmp[0]."] ".$tmp[1];

        $this->hotplugDeviceList[$attr['cn'][0]]=$tmp2;
      }
    }
    return($a_return);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
