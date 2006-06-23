<?php

class deptabs extends tabs
{
  var $base= "";

  function deptabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
    $this->base= $this->by_object['department']->base;
  }

  function check()
  {
    return (tabs::check(TRUE));
  }


  function save($justsave = false)
  {
    $baseobject= $this->by_object['department'];
    $ou= preg_replace('/,/', '\,', $baseobject->ou);
    $new_dn= @LDAP::convert('ou='.$ou.','.$baseobject->base);
    
    if($justsave){
      if ($this->dn != $new_dn && $this->dn != "new"){
        /* if( new_dn is subtree of this->dn ) */
        $cnt1 = count(split(",",$this->dn));
        $cnt2 = count(split(",",$new_dn));
        if((strstr($new_dn,$this->dn))&&($cnt1<$cnt2)){
          print_red(_("Try to move tree failed. Destination tree is subtree of source tree."));
        }else{
          /* Prepare to be moved */
          $baseobject->recursive_move($this->dn, $new_dn);			
          $this->by_object['department'] = $baseobject;
          return;
        }
      }
    }
    $this->dn= $new_dn;
    $baseobject->dn= $this->dn;
    $this->by_object['department']= $baseobject;
    return(tabs::save(TRUE));
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
