<?php

class ldifimport extends plugin
{
  /* Definitions */
  var $plHeadline= "LDIF export";
  var $plDescription= "This does something";
  var $access= "";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function ldifimport ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    $ui= get_userinfo();
    $acl= get_permissions ($config->current['BASE'], $ui->subtreeACL);
    $acl= get_module_permission($acl, "ldifimport", $config->current['BASE']);
    $this->access= $acl;
  }

  function execute()
  {
    /* Set government mode */
    $smarty= get_smarty();

    // Get the LDAP link, to generate the Export
    $ldap = $this->config->get_ldap_link();

    $smarty->assign("LDIFError",FALSE);
    $smarty->assign("type",FALSE);

    /* Check permissions for export */
    if ($this->access != '#all#'){
      print_red(_("You've no permission to do LDAP imports."));
    } else { 
      if((isset($_FILES['userfile']['name']))&&(isset($_POST['fileup'])))
      {


        $smarty->assign("type","importfile");


        // Check if there is a file submitted

        if(!$_FILES['userfile']['size'] > 0 )
        {
          print_red(_("The specified file is empty."));
          $smarty->assign("LDIFError",TRUE);  
        }

        // Is there a tmp file, which we can use ?
        elseif(!file_exists($_FILES['userfile']['tmp_name']))  
        {
          print_red(_("There is no file uploaded."));
          $smarty->assign("LDIFError",TRUE);

        }

        // Can we open the tmp file, for reading
        elseif(!$handle = @fopen($_FILES['userfile']['tmp_name'],"r"))
        {
          print_red(_("There is no file uploaded."));
          $smarty->assign("LDIFError",TRUE);  
        }
        else
        {
          // Everything just fine :)
          $str = ""; 

          // Reading content 
          while(!feof($handle))
          {
            $str .= fread($handle,1024);
          }
          @fclose($handle);

          // Try to Import the Data



          // Should we use Overwrite ?
          if(!empty($_POST['overwrite'])) $overwrite = true; else $overwrite = false;;
          if(!empty($_POST['cleanup']))   $cleanup   = true; else $cleanup = false;

          //

          $check = $ldap->import_complete_ldif($str,$ErrorStr,$overwrite,$cleanup);

          if($check == INSERT_OK  )
            $smarty->assign("LDIFError",FALSE);
          else
            $smarty->assign("LDIFError",TRUE);

          switch($check)
          {
            case INSERT_OK : break;
            case ALREADY_EXISTING_ENTRY      : print_red($ErrorStr); break;
            case UNKNOWN_TOKEN_IN_LDIF_FILE  : print_red($ErrorStr);break;

            default : print_red(_("Unknown Error"));break;
          }
        }
      }
    }
    return ($smarty->fetch (get_template_path('contentimport.tpl', TRUE)));
    
  }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
