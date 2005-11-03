<?php


class ldifexport extends plugin
{
  /* Definitions */
  var $plHeadline= "CSV import";
  var $plDescription= "This does something";
  var $access= "";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function ldifexport ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    $ui= get_userinfo();
    $acl= get_permissions ($config->current['BASE'], $ui->subtreeACL);
    $acl= get_module_permission($acl, "ldifexport", $config->current['BASE']);
    $this->access= $acl;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
    /* Set government mode */
    $smarty= get_smarty();

    $smarty->assign("type",FALSE);

    /* Check permissions for export */
    if ($this->access != '#all#'){
      print_red(_("You've no permission to do LDAP exports."));
    } else {
      // fill in old vars in the Export form
      if((isset($_POST['single']))&&(isset($_POST['sfrmgetsingle']))){
        $smarty->assign("single",$_POST['single']);
        $smarty->assign("type","single");
        $dn = $_POST['single'];
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
    }

    // Set values for optionlist in form
    $smarty->assign("deplist", $this->config->idepartments);

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
      if($ldap->dn_exists($dn) && $dn != ""){
        ;
      }
      else    {
        $smarty->assign("LDIFError",TRUE);
        print_red (_("Error while exporting the requested entries!"));
      }
    }

    // Export a single LDAP entry
    if(isset($_POST['sfrmgetsingle']))
    {
      $smarty->assign("para","?ivbb=2&amp;dn=".base64_encode($_POST['single']));
    }
    elseif(isset($_POST['sfrmgetfull']))
    {
      $smarty->assign("para","?ivbb=3&amp;dn=".base64_encode($_POST['selfull']));
    }
    elseif(isset($_POST['sfrmgetivbb']))
    {
      $smarty->assign("para","?ivbb=4&amp;dn=".base64_encode($_POST['selivbb']));
    }


    /* Show main page */
    return ($smarty->fetch (get_template_path('contentexport.tpl', TRUE)));
  }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
