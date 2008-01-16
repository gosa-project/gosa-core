<?php

class xlsexport extends plugin
{
  /* Definitions */
  var $plHeadline= "XLS import";
  var $plDescription= "This does something";
  var $access= "";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $ui;
  var $view_logged = FALSE;

  function xlsexport (&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;

    $this->ui = get_userinfo();
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Log view */
    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","all/".get_class($this),$this->dn);
    }

    $dn = $this->config->current['BASE'];

    /* Set government mode */
    $smarty= get_smarty();
    $smarty->assign("type",FALSE);
    $smarty->assign("depselectivbb","");
    
    if(!isset($_POST['selfull'])) $_POST['selfull'] = "Somehow not posted";

    /* Check permissions for export */
    // fill in old vars in the Export form
    if((isset($_POST['single']))&&(isset($_POST['sfrmgetsingle']))){
      $smarty->assign("single",$_POST['single']);
      $smarty->assign("type","single");
      $a=$_POST['single'];
      $b=$_POST['selfull'];

    }
    if((isset($_POST['selfull']))&&(isset($_POST['sfrmgetfull']))){
      $smarty->assign("depselectfull",$_POST['selfull']);
      $smarty->assign("type","full");
      $dn = $_POST['selfull'];
    } else {
      $smarty->assign("depselectfull", "");
    }
    if((isset($_POST['selivbb']))&&(isset($_POST['sfrmgetivbb']))){
      $smarty->assign("depselectivbb",$_POST['selivbb']);
      $smarty->assign("type","ivbb");
      $dn = $_POST['selivbb'];
    } else {
      $smarty->assign("depselectivbb", "");
    }

    // Set values for optionlist in form
    $bases      = array();
    $acl_bases  = $this->ui->get_module_departments("all");
    foreach($this->config->idepartments as $base_dn => $name){
      if(in_array_ics($base_dn,$acl_bases)){
        $bases[$base_dn] = $name;
      }
    }
    $smarty->assign("deplist", $bases);
    $smarty->assign("choicelist",array( get_people_ou()             =>"users" ,
                                        get_groups_ou()             =>"groups" ,
                                        get_ou('systemsou')         =>"computers",
                                        get_ou('serverou')          =>"servers",
                                        "dc=addressbook,"           =>"addressbook"));

    // Get the LDAP link, to generate the Export
    $ldap = $this->config->get_ldap_link();

    if ($this->config->current['GOVERNMENTMODE'] == 'true'){
      $smarty->assign ("govmode", "true");
    } else {
      $smarty->assign ("govmode", "false");
    }

    $smarty->assign("LDIFError",FALSE);

    if(isset($dn))
    {
      if(!($ldap->dn_exists($dn) && $dn != "")){
        $smarty->assign("LDIFError",TRUE);
	msg_dialog::display(_("Error"), _("Cannot export selected entries!"), ERROR_DIALOG);
      }
    }
   
    /* check alcs for given dn */
    $acls ="";
    if(isset($dn)){
      $acls = $this->ui->get_permissions($dn,"all/all");
    }
 
    if(!preg_match("/r/",$acls) && (isset($_POST['sfrmgetsingle']) || isset($_POST['sfrmgetfull']) || isset($_POST['sfrmgetivbb']))){

      /* Show error msg */      
      msg_dialog::display(_("Error"), sprintf(_("You have no permission to export '%s'!"),@LDAP::fix($dn)), ERROR_DIALOG);

      /* Show main page */
      $smarty->assign("LDIFError",TRUE);
      $smarty->assign("para","");
    }else{

      // Export a single LDAP entry
      if(isset($_POST['sfrmgetsingle']))
      {
        $smarty->assign("para","?ivbb=2&amp;d=".base64_encode($_POST['single'])."&n=".base64_encode($_POST['selfull']));	
      }
      elseif(isset($_POST['sfrmgetfull']))
      {
        $smarty->assign("para","?ivbb=3&amp;dn=".base64_encode($_POST['selfull']));
      }
      elseif(isset($_POST['sfrmgetivbb']))
      {
        $smarty->assign("para","?ivbb=4&amp;dn=".base64_encode($_POST['selivbb']));
      }
    }

    /* Show main page */
    return ($smarty->fetch (get_template_path('contentexportxls.tpl', TRUE)));
  }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
