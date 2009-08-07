<?php

class filterLDAP {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "", $objectBase= "")
  {
    $config= session::global_get('config');
    $ldap= $config->get_ldap_link(TRUE);
    $result= filterLDAP::get_list($base, $scope, $filter, $attributes,
                                  $category, $objectStorage, $objectBase,
                                  GL_SUBSEARCH | GL_SIZELIMIT);
    return $result;
  }


  static function get_list($base, $scope, $filter, $attributes, $category,
                           $objectStorage= array(), $objectBase= "", $flags= GL_SUBSEARCH)
  {
    $config= session::global_get('config');
    $ui= session::global_get('ui');
    $departments= array();

    /* Get LDAP link */
    $ldap= $config->get_ldap_link($flags & GL_SIZELIMIT);

    /* If we do no subsearch, adapt */
    if ($scope != "sub") {
      if ($objectBase != "") {
        $base= preg_replace('/,$/', '', $objectBase).",".$base;
      } elseif (is_string($objectStorage)) {
        $base= preg_replace('/,$/', '', $objectStorage).",".$base;
      }
    }
  
    /* Set search base to configured base if $base is empty */
    if ($base == ""){
      $base = $config->current['BASE'];
    }
    $ldap->cd ($base);
  
    /* Ensure we have an array as storage list */
    if(is_string($objectStorage)){
      $objectStorage = array($objectStorage);
    }
  
    /* Remove ,.*$ ("ou=1,ou=2.." => "ou=1") */
    $sub_bases = array();
    foreach($objectStorage as $key => $sub_base){
      if(empty($sub_base)){
  
        /* Subsearch is activated and we got an empty sub_base.
         *  (This may be the case if you have empty people/group ous).
         * Fall back to old get_list().
         * A log entry will be written. */
        if($flags & GL_SUBSEARCH){
          $sub_bases = array();
          break;
        }else{
  
          /* Do NOT search within subtrees is requested and the sub base is empty.
           * Append all known departments that matches the base. */
          $departments[$base] = $base;
        }
      }else{
        $sub_bases[$key] = preg_replace("/,.*$/","",$sub_base);
      }
    }
  
    // If there is no sub_department specified, fall back to old method, get_list().
    if(!count($sub_bases) && !count($departments)){
  
      // Log this fall back, it may be an unpredicted behaviour.
      if(!count($sub_bases) && !count($departments)){
        new log("debug","all",__FILE__,$attributes,
            sprintf("filterLDAP::get_list(): falling back to filterLDAP::get_list_old() because objectStorage is empty.".
              " This may slow down GOsa. Filter was: '%s'", $filter));
      }
      return (ldapFILTER::get_list_old($filter, $category,$base,$attributes,$flags));
    }
  
    /* Get all deparments matching the given sub_bases */
    $base_filter= "";
    foreach($sub_bases as $sub_base){
      $base_filter .= "(".$sub_base.")";
    }
    $base_filter = "(&(objectClass=organizationalUnit)(|".$base_filter."))";
    $ldap->search($base_filter, array("dn"));
    while($attrs = $ldap->fetch()){
      foreach($objectStorage as $sub_dep){
  
        /* Only add those departments that match the reuested list of departments.
         *
         * e.g.   sub_deps = array("ou=servers,ou=systems,");
         *
         * In this case we have search for "ou=servers" and we may have also fetched
         *  departments like this "ou=servers,ou=blafasel,..."
         * Here we filter out those blafasel departments.
         */
        if(preg_match("/".preg_quote($sub_dep, '/')."/",$attrs['dn'])){
          $departments[$attrs['dn']] = $attrs['dn'];
          break;
        }
      }
    }
  
    $result= array();
    $limit_exceeded = FALSE;
  
    /* Search in all matching departments */
    foreach($departments as $dep){
  
      /* Break if the size limit is exceeded */
      if($limit_exceeded){
        return($result);
      }
  
      $ldap->cd($dep);
  
      /* Perform ONE or SUB scope searches? */
      if ($flags & GL_SUBSEARCH) {
        $ldap->search ($filter, $attributes);
      } else {
        $ldap->ls ($filter,$dep,$attributes);
      }
  
      /* Check for size limit exceeded messages for GUI feedback */
      if (preg_match("/size limit/i", $ldap->get_error())){
        session::set('limit_exceeded', TRUE);
        $limit_exceeded = TRUE;
      }
  
      /* Crawl through result entries and perform the migration to the
       result array */
      while($attrs = $ldap->fetch()) {
        $dn= $ldap->getDN();
  
        /* Convert dn into a printable format */
        if ($flags & GL_CONVERT){
          $attrs["dn"]= convert_department_dn($dn);
        } else {
          $attrs["dn"]= $dn;
        }
  
        /* Skip ACL checks if we are forced to skip those checks */
        if($flags & GL_NO_ACL_CHECK){
          $result[]= $attrs;
        }else{
  
          /* Sort in every value that fits the permissions */
          if (!is_array($category)){
            $category = array($category);
          }
          foreach ($category as $o){
            if((preg_match("/\//",$o) && preg_match("/r/",$ui->get_permissions($dn,$o))) ||
                (!preg_match("/\//",$o) && preg_match("/r/",$ui->get_category_permissions($dn, $o)))){
              $result[]= $attrs;
              break;
            }
          }
        }
      }
    }

    return($result);
  }


  function get_list_old($filter, $category, $base= "", $attributes= array(), $flags= GL_SUBSEARCH)
  {
    $config= session::global_get('config');
    $ui= session::global_get('ui');
  
    /* Get LDAP link */
    $ldap= $config->get_ldap_link($flags & GL_SIZELIMIT);
  
    /* Set search base to configured base if $base is empty */
    if ($base == ""){
      $ldap->cd ($config->current['BASE']);
    } else {
      $ldap->cd ($base);
    }
  
    /* Perform ONE or SUB scope searches? */
    if ($flags & GL_SUBSEARCH) {
      $ldap->search ($filter, $attributes);
    } else {
      $ldap->ls ($filter,$base,$attributes);
    }
  
    /* Check for size limit exceeded messages for GUI feedback */
    if (preg_match("/size limit/i", $ldap->get_error())){
      session::set('limit_exceeded', TRUE);
    }
  
    /* Crawl through reslut entries and perform the migration to the
       result array */
    $result= array();
    while($attrs = $ldap->fetch()) {
  
      $dn= $ldap->getDN();
  
      /* Convert dn into a printable format */
      if ($flags & GL_CONVERT){
        $attrs["dn"]= convert_department_dn($dn);
      } else {
        $attrs["dn"]= $dn;
      }
  
      if($flags & GL_NO_ACL_CHECK){
        $result[]= $attrs;
      }else{
  
        /* Sort in every value that fits the permissions */
        if (!is_array($category)){
          $category = array($category);
        }
        foreach ($category as $o){
          if((preg_match("/\//",$o) && preg_match("/r/",$ui->get_permissions($dn,$o))) ||
              (!preg_match("/\//",$o) && preg_match("/r/",$ui->get_category_permissions($dn, $o)))){
            $result[]= $attrs;
            break;
          }
        }
      }
    }
    return ($result);
  }

}

?>
