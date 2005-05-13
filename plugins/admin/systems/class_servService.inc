<?php

class servservice extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");


  var $goExportEntry  = array();
  var $goTimeSource   = array();
  var $goLdapBase= "";
  var $goXdmcPlsEnabled= "";
  var $goFontPath= "";
  var $goNTPServer="";
  var $goLdapServer="";
  var $goTerminalServer="";
  var $goSyslogServer="";
  var $goCupsServer="";


  /* attribute list for save action */
  var $attributes       = array("goExportEntry","goTimeSource","goLdapBase","goXdmcpIsEnabled","goFontPath");
  var $objectclasses    = array("top","goServer"); 
  var $additionaloc     = array(
                
                          "goNfsServer"     => array("goExportEntry"),
                          "goNtpServer"     => array("goTimeSource"),
                          "goLdapServer"    => array("goLdapBase"),
                          "goTerminalServer"=> array("goXdmcpIsEnabled", "goFontPath")
//                          "goSyslogServer"  => array(""),
//                          "goCupsServer"    => array("")                           
                          );









  function servservice ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Always is account... */

    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->attrs['objectClass']) && in_array($oc, $this->attrs['objectClass'])){
        $this->objectclasses[$oc]= $oc;
      }
    }
    /* Always is account... */
    $this->is_account= TRUE;
  }



  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();

    /*  
    Handling for Nfs Export Entries
    */

    /* Here we add a new entry  */
    if(isset($_POST['NewNfsAdd']))
      {
      $this->attrs['goExportEntry'][]=$_POST['NewNfsExport'];
      }
    
      
     
    /* Deleting an Entry, is a bit more complicated than adding one*/
    if(isset($_POST['DelNfsEnt']))
      {
      /* Go through all entries an skip the selected (The one we want to delete)*/
      foreach($this->attrs['goExportEntry'] as $key => $val)
        {
        /* Add all entries in an new Array, because when deleting one entry, the could be broken*/
        if(($val != $_POST['goExportEntry'] ))
          {
          /* Skip the entry that we want to delete*/
          $new_arr[]=$val;
          }
        }
      /* Deleting complete so add result to the attrs */
      $this->attrs['goExportEntry']=$new_arr;
      }

    /* Set the entry which we want to show in the frontend */
    $this->goExportEntry = ($this->attrs['goExportEntry']);
    unset($this->goExportEntry['count']);
   


   /*
    Handling for NTP Export Entries
    */

    /* Here we add a new entry  */
    if(isset($_POST['NewNTPAdd']))
      {
      $this->attrs['goTimeSource'][]=$_POST['NewNTPExport'];
      }
    
    /*Deleting an Entry, is a bit more complicated than adding one*/
    if(isset($_POST['DelNTPEnt']))
      {
      /* Go through all entries an skip the selected (The one we want to delete)*/
      foreach($this->attrs['goTimeSource'] as $key => $val)
        {
        /* Add all entries in an new Array, because wehn deleting one entry, the could be broken*/
        if($val != $_POST['goTimeSource'] )
          {
          /* Skip the entry that we want to delete*/
          $new_arr[]=$val;
          }
        }
      /* Deleting complete so add result to the attrs */
      $this->attrs['goTimeSource']=$new_arr;
      }

    /* Set the entry which we want to show in the frontend */
    $this->goTimeSource = ($this->attrs['goTimeSource']);
    unset($this->goTimeSource['count']);



 
    /* Attributes... */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign("$attr"."ACL", chkacl($this->acl, $attr));
    }


    /* Classes... */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->objectclasses[$oc])){
        $smarty->assign("$oc", "checked");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));
      } else {
        $smarty->assign("$oc"."State", "disabled");
      }
    }

  return($smarty->fetch (get_template_path('servservice.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
     plugin::save_object();

    /* Save checkbox state */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($_POST[$oc]) && $_POST[$oc] == '1'){
        $this->objectclasses[$oc]= $oc;
      } else {
        unset($this->objectclasses[$oc]);
      }
    }
 
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Normalize lazy objectclass arrays */
    $objectclasses= array();
    foreach($this->objectclasses as $oc){
      $objectclasses[]= $oc;
    }

    $this->objectclasses= $objectclasses;
    plugin::save();

    /* Remove objectclasses */
    unset ($this->attrs['objectClass']);
    foreach($this->objectclasses as $oc){
      $this->attrs['objectClass'][]= $oc;
    }

    /* Adapt sieve server if needed */
    if (in_array('goImapServer', $this->objectclasses)){
      $this->attrs['goImapSieveServer']= preg_replace('/:.*$/', '',
                                         preg_replace('/^{([^}]+)}$/', '\\1',
                                         $this->attrs['goImapConnect']));
    }

    /* Remove unneeded attributes */
    foreach ($this->additionaloc as $oc => $attrs){
      if (!in_array($oc, $this->objectclasses)){
        foreach ($attrs as $attr){
          $this->attrs[$attr]= array();
        }
      }
    }

    /* Write to LDAP */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
