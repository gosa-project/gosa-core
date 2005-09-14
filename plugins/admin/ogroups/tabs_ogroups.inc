<?php

class ogrouptabs extends tabs
{
  var $base= "";

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
          foreach($this->by_object['ogroup']->memberList as $dn => $val){
            if(isset($val['objectClass'])){
              if(in_array("goFonAccount",$val['objectClass'])){
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
  }


  function check()
  {
    return (tabs::check(TRUE));
  }


  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    /* Update reference, transfer variables */
    $baseobject= $this->by_object['ogroup'];
    foreach ($this->by_object as $name => $obj){

      /* Don't touch base object */
      if ($name != 'ogroup'){
        $obj->parent= &$this;
        $obj->uid= $baseobject->uid;
        $obj->sn= $baseobject->uid;
        $obj->givenName= $baseobject->uid;
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
      $this->dn= 'cn='.$baseobject->cn.','.get_groups_ou().$baseobject->base;
    }

    tabs::save();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
