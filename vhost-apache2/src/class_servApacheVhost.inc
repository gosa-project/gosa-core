<?php

class servapache extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = FALSE;
  var $attributes       = array(); 
  var $objectclasses    = array("whatever");

  var $RecordTypes      = array();
  var $Vhosts            = array();
  var $dialog           = NULL;

  var $orig_dn          = "";

  var $APACHEinitially_was_account;


  function servapache ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    $this->orig_dn = $dn;

    /* Get all vhost Informations
     */
    $this->Vhosts = getVhostEntries($config,$dn);

    /* If there is at least one entry in this -> types, we have apache vhosts enabled 
     */
    if(count($this->Vhosts) == 0){
      $this->is_account = false;
    }else{
      $this->is_account = true;
    }
    $this->APACHEinitially_was_account = $this->is_account;
  }


  function execute()
  {
    /* Call parent execute 
     */
    plugin::execute();

    /* Fill templating stuff 
     */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? 
     */
    if (isset($_POST['modify_state'])){

      /* Only change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }
    }

    if ($this->is_account){
      $display= $this->show_header(_("Remove Apache service"),
          _("This server has Apache features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add Apache service"),
          _("This server has Apache features disabled. You can enable them by clicking below."));
      return ($display);
    }

    /* Edited or Added vhost 
     */
    if((isset($_POST['SaveVhostChanges'])) && is_object($this->dialog)){
      $this->dialog->save_object();

      /* Check for errors  
       */
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msgs){
          print_red($msgs); 
        }
      }else{
        /* add new/edited vhost 
         */
        $ret = $this->dialog->save();
        if(!$this->dialog->isNew){
          unset($this->Vhosts[$this->dialog->OldApacheServerName]);
        }
        $this->Vhosts[$ret['apacheServerName']] = $ret;
        $this->dialog = NULL;
      }
    }

    /* Cancel vhost edit / new 
     */
    if(isset($_POST['CancelVhostChanges'])){
      $this->dialog = NULL;
    }

    /* Add empty new vhost 
     */
    if(isset($_POST['AddVhost']) && chkacl($this->acl,"servapache") == ""){
      $this->dialog = new servapacheVhostEdit($this->config,$this->dn);
    }

    /* Check for edit vhost request 
     */
    $once = false;
    foreach( $_POST as $name => $value){
  	//user_error(print_r($this->Vhosts,true));
      /* check all post for edit request 
       */
      if(preg_match("/^editVhost_/",$name)&&!$once && chkacl($this->acl,"servapache") == ""){
        $once =true;
        $tmp = preg_replace("/^editVhost_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        $this->dialog= new servapacheVhostEdit($this->config,$this->dn,$this->Vhosts[$tmp]);
      }

      /* check posts for delete vhost 
       */
      if(preg_match("/^delVhost_/",$name)&&!$once && chkacl($this->acl,"servapache") == ""){

        $once =true;
        $tmp = preg_replace("/^delVhost_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
     
        /* Initiate deletion
         */ 
        $this->RemoveVhost($tmp);
      }
    }

    /* Show dialog 
     */
    if($this->dialog!= NULL){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Vhosts 
     */
    $VhostList = new divSelectBox("apacheConfigs");
    $VhostList -> SetHeight(254);
    
    /* Add entries to divlist
     */
    $editImg = "<input type='image' src='images/edit.png' name='editVhost_%s'>
      <input type='image' src='images/edittrash.png' name='delVhost_%s'>";
    foreach($this->Vhosts as $vhost => $values ){
      $VhostList->AddEntry(array(
            array("string" => $vhost),
            array("string" => str_replace("%s",base64_encode($vhost),$editImg))
            ));
    }    
  
    $smarty->assign("servapacheACL",chkacl($this->acl,"servapache"));
  
    /* Display template 
     */
    $smarty->assign("VhostList",$VhostList->DrawList());
    $display.= $smarty->fetch(get_template_path('servApacheVhost.tpl', TRUE));
    return($display);
  }


  /* Delete specified vhost
   */  
  function RemoveVhost($id)
  {
      unset($this->Vhosts[$id]);
      return(true);
  }


  /* This function returns all used Vhostnames 
   */
  function getUsedServerNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=apacheConfig)(apacheServerName=*))",array("apacheServerName"));
    while($attr = $ldap->fetch()){
      $ret[$attr['apacheServerName'][0]][] = $attr['dn'];
    }
    return($ret);
  }


    /* Remove apache service
   */
  function remove_from_parent()
  {
    if($this->APACHEinitially_was_account){
      $bool = true;
      foreach($this->Vhosts as $key => $vhost){
        $bool= $bool & $this->RemoveVhost($key);
      }

      if($bool){
        $this->save();
      }
      return($bool);
    }
  }


  /* Save to LDAP */
  function save()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);  
  
    /* Get differences 
     */

    $old_dn =  $this->orig_dn;
    if($old_dn == "new"){
      $old_dn = $this->dn;
    }

    $tmp = getVhostEntriesDiff($this->config,$this->Vhosts,$old_dn);

    /* Updated vhost entries if reverser or forward name has changed  
     * Must be done before moving entries, else the given dn is invalid
     */
    if(isset($tmp['vhostUpdates'])){
      foreach($tmp['vhostUpdates'] as $dn => $attrs){
        $ldap->cd($dn);
        $ldap->modify($attrs);
        show_ldap_error("Vhost:".$ldap->get_error(), _("Updating Apache service failed"));
      }
    }

    /* Delete apache vhost
     */
    foreach($tmp['del'] as $dn => $del){
      $ldap->cd($dn);
      $ldap->rmdir_recursive($dn);
      show_ldap_error($ldap->get_error(), _("Removing Apache entries failed"));
    }

    /* move follwoing entries
     */
    foreach($tmp['move'] as $src => $dst){
      $this->recursive_move($src,$dst);
    }

    /* Add || Update new apache entries
     */
    foreach($tmp['add'] as $dn => $attrs){
      $ldap->cd($dn);
//      user_error(print_r($dn,true));
      $ldap->cat($dn, array('dn'));
      if(count($ldap->fetch())){
        $ldap->cd($dn);
        $ldap->modify ($attrs);
      }else{
        $ldap->cd($dn);
        $ldap->add($attrs);
      }
      show_ldap_error($ldap->get_error(), _("Saving apache entries failed"));
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
