<?php


class mailMethodKolab22 extends mailMethodKolab
{

  /* Allow modification of account_ids for existing mail accounts */
  protected $modifyableMail = FALSE;

  /* Enforces same value for 'mail' as used for 'cn' */
  protected $mailEqualsCN   = TRUE; 

  /* Enabled mail domain selection. If enabled getMailDomains must the domain parts */ 
  protected $enableDomainSelection= TRUE;
  protected $enableQuota          = TRUE;
  protected $enableSieveManager   = TRUE;
  protected $enableVacationRange  = TRUE;
  protected $enableFolderTypes    = TRUE;

  protected $userObjectClasses = array("kolabInetOrgPerson");
  protected $shareObjectClasses = array("kolabSharedFolder");

  protected $acl_map = array(
//     "lrs"           => "read",
//     "lrsp"          => "post",
//     "lrsip"         => "append",
//     "lrswipkxte"    => "write",
//     "lrxte"         => "delete",
//     "lrswipkxtecda" => "admin",
//     " "             => "none");
"none"             => "none",
"post"             => "post",             
"read"             => "read",
"read/post"        => "read/post",
"append"           => "append",
"write"            => "write",
"read anon"        => "read anon",
"read anon/post"   => "read anon/post",
"read hidden"      => "read hidden",
"read hidden/post" => "read hidden/post",
"all"              => "all");

  protected $default_acls = array("__anyone__" => "none", "__member__" => "write");


  /*! \brief  Returns configured acls
   */
  public function  getFolderACLs($ldap_acls)
  {
    $this->reset_error();
    $imap_acls = mailMethodKolab::getFolderACLs(array());

    foreach($imap_acls as $user => $acl){
      if(!isset($ldap_acls[$user])){
        $ldap_acls[$user] = $acl;
      }else{
        // Ldap attributes come first in kolab22.
      }
    }

    /* Merge given ACL with acl mapping
       This ensures that no ACL will accidentally overwritten by gosa.
     */
    foreach($ldap_acls as $user => $acl){
      if(!isset($this->acl_mapping[$acl])){
        $this->acl_mapping[$acl] = $acl;
      }
    }

    return($ldap_acls);
  }


  /*! \brief Returns a list of configured mail domains 
      @return Array A list of mail domains
   */
  public function getMailDomains()
  {
    $domains = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=kolab)(k=kolab)(postfix-mydestination=*))",array("postfix-mydestination"));

    if($ldap->count() == 1){
      $attrs = $ldap->fetch();
      for($i=0; $i <$attrs['postfix-mydestination']['count']; $i ++ ){
        $domains[] = $attrs['postfix-mydestination'][$i];
      }
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<b>Found: ".trim(implode($domains,", "),", ")."</b>",
          "Revceiving 'Mail domains for kolab22'.");
    }elseif($ldap->count() == 0){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<b>No domains found. Check k=kolab and postfix-mydestination</b>",
          "Revceiving 'Mail domains for kolab22'.");
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<b>Found more than one kolab configuration object. Giving up.</b>",
          "Revceiving 'Mail domains for kolab22'.");
    }

    return($domains);
  } 
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
