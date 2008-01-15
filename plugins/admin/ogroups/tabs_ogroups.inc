<?php

class ogrouptabs extends tabs
{
  var $base= "";
  
  function reload($dd){
    $objects= preg_replace('/[\[\]]/', '', $dd);
    
    /* If there is a phonequeue,
     * but there is no user left with goPhoneAccount ... remove it.
     */
    $usePhoneTab = false;
    $o_cache = $this->by_object['ogroup']->objcache;
    foreach($this->by_object['ogroup']->memberList as $dn => $val){
      if(isset($o_cache[$dn]['objectClass'])){
        if(in_array("goFonAccount",$o_cache[$dn]['objectClass'])){
          $usePhoneTab = true;
        }
      }
    }
    if(((!$usePhoneTab)&&(isset($this->by_object['phonequeue'])))||((!preg_match("/U/",$objects))&&(isset($this->by_object['phonequeue'])))){
      $this->by_object['phonequeue']->remove_from_parent();
      unset($this->by_object['phonequeue']);
      unset($this->by_name['phonequeue']);
    }
    /* Remove mail group if there is no user anymore */
    if((!preg_match("/U/",$objects))&&(isset($this->by_object['mailogroup']))){
      $this->by_object['mailogroup']->remove_from_parent();
      unset($this->by_object['mailogroup']);
      unset($this->by_name['mailogroup']);
    }
    
    /* Remove terminal group, if theres no terminal left in the object list */
    if(((!preg_match("/T/",$objects)) && (!preg_match("/W/",$objects)))&&(isset($this->by_object['termgroup']))){
      $this->by_object['termgroup']->remove_from_parent();
      unset($this->by_object['termgroup']);
      unset($this->by_name['termgroup']);
    }
    
    /* Remove ws tabs, if theres no ws left in the object list */
    if((!preg_match("/W/",$objects))&&(isset($this->by_object['workservice']))){
      $this->by_object['workservice']->remove_from_parent();
      unset($this->by_object['workservice']);
      unset($this->by_name['workservice']);
    }
    if((!preg_match("/S/",$objects) && !preg_match("/W/",$objects))&&(isset($this->by_object['workstartup']))){
      $this->by_object['workstartup']->remove_from_parent();
      unset($this->by_object['workstartup']);
      unset($this->by_name['workstartup']);
      $this->by_object['faiSummary']->remove_from_parent();
      unset($this->by_object['faiSummary']);
      unset($this->by_name['faiSummary']);
    }
  
    /* Create goPhoneAccount if theres an user with goPhoneAccount
     * but only if there is currently no queue enabled.
     */
    $o_cache = $this->by_object['ogroup']->objcache;
    if(!isset($this->by_object['phonequeue'])){
      foreach($this->by_object['ogroup']->memberList as $dn => $val){
        if(isset($o_cache[$dn]['objectClass'])){
          if(in_array("goFonAccount",$o_cache[$dn]['objectClass'])){
            require_once("class_phonequeue.inc");
            $this->by_name['phonequeue']= _("Phone queue");
            $this->by_object['phonequeue']= new phonequeue($this->config, $this->dn);
            $this->by_object['phonequeue']->acl = "#all#";
            $this->by_object['phonequeue']->parent= &$this;
          }
        }
      }
    }

    /* Add mail group tab , if there is curerntly no mail tab defined */ 
    if((preg_match("/U/",$objects))&&(!isset($this->by_object['mailogroup']))){
      if(isset($this->config->current['MAILMETHOD'])){
        if (preg_match('/kolab/i', $this->config->current['MAILMETHOD'])){
          require_once("class_mailogroup.inc");
          $this->by_name['mailogroup']= _("Mail");
          $this->by_object['mailogroup']= new mailogroup($this->config, $this->dn);
          $this->by_object['mailogroup']->acl = "#all#";
          $this->by_object['mailogroup']->parent= &$this;
        }
      }
    }

    /* Add Terminal tab */
    if(((preg_match("/T/",$objects)) || (preg_match("/W/",$objects)))&&(!isset($this->by_object['termgroup']))){
      require_once("class_termgroup.inc");
      if(!isset($this->by_object['termgroup'])){
        $this->by_name['termgroup']= _("System");
        $this->by_object['termgroup']= new termgroup($this->config, $this->dn);
        $this->by_object['termgroup']->acl = "#all#";
        $this->by_object['termgroup']->inheritTimeServer = false;
        $this->by_object['termgroup']->parent= &$this;
      }
    }

    /* Add Workstation tabs */
    if((preg_match("/W/",$objects))&&(!isset($this->by_object['workservice']))){
      $this->by_name['workservice']= _("Devices");
      $this->by_object['workservice']= new workservice($this->config, $this->dn);
      $this->by_object['workservice']->inheritTimeServer = false;
      $this->by_object['workservice']->parent= &$this;
      $this->by_object['workservice']->acl = "#all#";
    }
    if((preg_match("/S/",$objects) || preg_match("/W/",$objects))&&(!isset($this->by_object['workstartup']))){
      $this->by_name['workstartup']= _("Startup");
      $this->by_object['workstartup']= new workstartup($this->config, $this->dn);
      $this->by_object['workstartup']->parent= &$this;
      $this->by_object['workstartup']->acl = "#all#";
      $this->by_name['faiSummary']= _("Summary");
      $this->by_object['faiSummary']= new faiSummaryTab($this->config, $this->dn);
      $this->by_object['faiSummary']->parent= &$this;
    }

    /* Add application tab if user or group is member in this object group*/ 
    if((preg_match("/G/",$objects) || preg_match("/U/",$objects)) && !isset($this->by_name['appgroup'])){
      $this->by_name['appgroup']= _("Application");
      $this->by_object['appgroup']= new appgroup($this->config, $this->dn);
      $this->by_object['appgroup']->acl = "#all#";
      $this->by_object['appgroup']->parent= &$this;
    }

    /* Remove application tab if not required any longer */
    if(!preg_match("/G/",$objects) && !preg_match("/U/",$objects) && isset($this->by_name['appgroup'])){
      $this->by_object['appgroup']->remove_from_parent();
      unset($this->by_name['appgroup']);
      unset($this->by_object['appgroup']);
    }  
 
 
    /* Add environment tab if user or group is member in this object group*/ 
    if((preg_match("/G/",$objects) || preg_match("/U/",$objects)) && !isset($this->by_name['environment'])){
      $this->by_name['environment']= _("Environment");
      $this->by_object['environment']= new environment($this->config, $this->dn);
      $this->by_object['environment']->acl = "#all#";
      $this->by_object['environment']->parent= &$this;
    }

    /* Remove environment tab if not required any longer */
    if(!preg_match("/G/",$objects) && !preg_match("/U/",$objects) && isset($this->by_name['environment'])){
      $this->by_object['environment']->remove_from_parent();
      unset($this->by_name['environment']);
      unset($this->by_object['environment']);
    }  

 
    /* Move reference tab to last position*/
    if(isset($this->by_name['reference'])){
      $tmp = $this->by_name['reference'];
      unset($this->by_name['reference']);
      $this->by_name['reference'] = $tmp;
    }
  }

  function execute(){
    $str = "";
    $this->by_object['ogroup']->AddDelMembership();
    $this->reload($this->by_object['ogroup']->gosaGroupObjects);
    $str .= tabs::execute();
    return ( $str);
  }  

  function ogrouptabs($config, $data, $dn)
  {

    tabs::tabs($config, $data, $dn);
    $this->base= $this->by_object['ogroup']->base;

    /* Insert extra tabs for several object types - if present */

    $objects= preg_replace('/[\[\]]/', '', $this->by_object['ogroup']->gosaGroupObjects);
    
    for ($n= 0; $n<strlen($objects); $n++){
      switch ($objects[$n]){
        case "T":
          /* Add a terminal tab */
          require_once("class_termgroup.inc");
          $this->by_name['termgroup']= _("Terminals");
          $this->by_object['termgroup']= new termgroup($this->config, $this->dn);
          $this->by_object['termgroup']->parent= &$this;

          break;

          case "U":
            /* Append a PhoneQueue, if objectClass = goFonAccount */
            $use = false;
          $o_cache = $this->by_object['ogroup']->objcache;
          foreach($this->by_object['ogroup']->memberList as $dn => $val){
            if(isset($o_cache[$dn]['objectClass'])){
              if(in_array("goFonAccount",$o_cache[$dn]['objectClass'])){
                $use = true; 
              }
            }
          }

          /* We found goFonAccount in users objectClasses*/
          if($use){
            require_once("class_phonequeue.inc");
            $this->by_name['phonequeue']= _("Phone queue");
            $this->by_object['phonequeue']= new phonequeue($this->config, $this->dn);
            $this->by_object['phonequeue']->parent= &$this;

          } 
 
          /* Add a user tab used for mail distribution lists */
          if(isset($this->config->current['MAILMETHOD'])){
            if (preg_match('/kolab/i', $this->config->current['MAILMETHOD'])){
              require_once("class_mailogroup.inc");
              $this->by_name['mailogroup']= _("Mail");
              $this->by_object['mailogroup']= new mailogroup($this->config, $this->dn);
              $this->by_object['mailogroup']->parent= &$this;
            }
          }

          break;
      }
    }
    $this->reload($this->by_object['ogroup']->gosaGroupObjects);
  }


  function check()
  {
    return (tabs::check(FALSE));
  }


  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    /* Update reference, transfer variables */
    $baseobject= $this->by_object['ogroup'];
    foreach ($this->by_object as $name => $obj){

      /* Don't touch base object */
      if ($name != 'ogroup'){
        $obj->parent    = &$this;
        $obj->uid       = $baseobject->uid;
        $obj->cn        = $baseobject->cn;
        $obj->sn        = $baseobject->uid;
        $obj->givenName = $baseobject->uid;
        $this->by_object[$name]= $obj;
      }

      /* Update parent in base object */
      $this->by_object['ogroup']->parent= &$this;
    }
  }


  function save()
  {
    $baseobject= $this->by_object['ogroup'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $new_dn= 'cn='.$baseobject->cn.','.get_groups_ou().$baseobject->base;

    /* Move group? */
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['ogroup']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    if ($this->dn == "new"){
      $this->dn= $new_dn;
    }

    tabs::save();
  }

  function saveCopyDialog()
  {
    tabs::saveCopyDialog();
    $baseobject= $this->by_object['ogroup'];
    $cn = $baseobject->cn;
    foreach($this->by_object as  $name => $obj){
      $this->by_object[$name]->cn = $cn;
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
