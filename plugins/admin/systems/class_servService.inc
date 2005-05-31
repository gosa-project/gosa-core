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
  var $goXdmcpIsEnabled ="";
  var $goFontPath= "";
  var $goNTPServer="";
  var $goLdapServer="";
  var $goTerminalServer="";
  var $goSyslogServer="";
  var $goCupsServer="";

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes       = array("goExportEntry","goTimeSource","goLdapBase","goXdmcpIsEnabled","goFontPath" );
  var $objectclasses    = array("top","goServer"); 
  var $additionaloc     = array(
                
                          "goNfsServer"     => array("goExportEntry"),
                          "goNtpServer"     => array("goTimeSource"),
                          "goLdapServer"    => array("goLdapBase"),
                          "goTerminalServer"=> array("goXdmcpIsEnabled", "goFontPath"),
                          "goSyslogServer"  => array(),
                          "goCupsServer"    => array()                           
                          );









  function servservice ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

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
    if(isset($this->attrs['goExportEntry']))
      {
      $this->goExportEntry = ($this->attrs['goExportEntry']);
      unset($this->goExportEntry['count']);
      }


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
    if(isset($this->attrs['goTimeSource']))
      {
      $this->goTimeSource = ($this->attrs['goTimeSource']);
      unset($this->goTimeSource['count']);
      }



 
    /* Attributes... */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign("$attr"."ACL", chkacl($this->acl, $attr));
      $smarty->assign($attr."State","");
    }


    /* Classes... */
foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->objectclasses[$oc])){
        $smarty->assign("$oc", "checked");
        $smarty->assign("$oc"."State", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));

      } else {
        $smarty->assign("$oc", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));
        $smarty->assign("$oc"."State", "disabled");
      }
    }


  if(isset($this->attrs['goXdmcpIsEnabled'])&&($this->attrs['goXdmcpIsEnabled']=="1"))
    $smarty->assign("goXdmcpIsEnabled","checked");

  return($smarty->fetch (get_template_path('servservice.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['servicetab'])){
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
    plugin::save();
 
    $test = $this->objectclasses;
    $tmp = array_flip($this->attrs['objectClass']);
   
    
 
    foreach($this->additionaloc as $key=>$val) {
      unset($tmp[$key]);
      }
   

    $classes = (array_flip(array_merge(array_flip($test),$tmp)));

    unset($this->attrs['objectClass']);

    foreach($classes as $class){
      $this->attrs['objectClass'][]=$class;
    }
      

    /* Remove unneeded attributes */
    foreach ($this->additionaloc as $oc => $attrs){
      if (!in_array($oc, $this->attrs['objectClass'])){
        foreach ($attrs as $attr){
          $this->attrs[$attr]= array();
        }
      }
    }
    $this->attrs = array_reverse($this->attrs);

    /* Check if we are able to set these attributes */
#fixme : It Would be better to check ObjectClass rights to, but what is to do if there are insuficient rights 
    foreach($this->additionaloc as $oc => $attrs)
    {
      foreach($attrs as $val)
      {
      if(chkacl($this->acl,$val)=="")
        {
        unset($this->attrs[$val]);
        unset($this->$val);
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
