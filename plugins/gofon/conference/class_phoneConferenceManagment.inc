<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
require "tabs_conference.inc";

class phoneConferenceManagment extends plugin
{
  /* Definitions */
  var $plHeadline				  = "Phone conferences";
  var $plDescription			= "Management";

  /* CLI vars */
  var $cli_summary			  = "Handling of LDAP subtrees";
  var $cli_description		= "Some longer text\nfor help";
  var $cli_parameters			= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Headpage attributes */
  var $conferences			  = array();

  /* attribute list for save action */
  var $attributes				  = array();
  var $objectclasses			= array();
  var $conftab            = false;

  var $departments  =array();

  /* Initialise Class */
  function phoneConferenceManagment ($config, $ui)
  {
    $this->ui			= $ui;
    $this->dn			= "";
    $this->config		= $config;

    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "conference", $ui->dn);

    /* Get global filter config */
    if (!isset($_SESSION["conferencefilter"])){
      $base= get_base_from_people($ui->dn);
      $conferencefilter= array(
          "regex"           => "*");
      $_SESSION["conferencefilter"] = $conferencefilter;
    }
  }

  /* Execute class and display something */
  function execute()
  {

	/* Call parent execute */
	plugin::execute();

    /* Reload departments */
    $smarty						          = get_smarty();
    $display 					          = "";
    $s_action 					        = "";  // Will contain an action, like del or edit
    $s_entry  					        = "";  // The entry name for edit delete -...
    $conferencefilter           = $_SESSION["conferencefilter"]; 

    if(isset($_POST['regexit'])){
      $conferencefilter['regex']=$_POST['regexit'];
    }

    if(isset($_GET['search'])){
      if($_GET['search']=="*"){
        $conferencefilter['regex']=$_GET['search'];
      }else{
        $conferencefilter['regex']=$_GET['search']."*";
      }
    }

    $smarty->assign("regex",$conferencefilter['regex']); 

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
      $s_action="open";
      $s_entry = base64_decode($_GET['dep_id']);
      $_SESSION['CurrentMainBase']= $this->config->departments[trim($s_entry)];
    }

    foreach($_POST as $key => $post){
      if(preg_match("/.*new.*/i",$key)){
        $s_action = "new";
        // Post for delete
      }elseif(preg_match("/conf_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/conf_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
        // Post for edit
      }elseif(preg_match("/conf_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/conf_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      } 
    }

    /* Edit Entry */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action = "edit";
      $s_entry  = $_GET['id'];
    }

    /* Department changed? */
    if(isset($_POST['CurrentMainBase']) && $_POST['CurrentMainBase']){
      $_SESSION['CurrentMainBase']= $_POST['CurrentMainBase'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $_SESSION['CurrentMainBase'] =preg_replace("/^[^,]+,/","",$this->ui->dn);
      $_SESSION['CurrentMainBase'] =preg_replace("/^[^,]+,/","",$_SESSION['CurrentMainBase']);
    }
    
    /* back to root */
    if($s_action=="root"){
      $_SESSION['CurrentMainBase']=($this->config->current['BASE']);
    }

    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$_SESSION['CurrentMainBase']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $_SESSION['CurrentMainBase']= $this->config->departments[trim($base_back)];
      }else{
        $_SESSION['CurrentMainBase']= $this->config->departments["/"];
      }
    }

    register_global("conferencefilter",$conferencefilter);
    $this->reload();

    /* Reset requested? */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      if (isset($this->conftab)){
        del_lock ($this->conftab->dn);
        unset ($this->conftab);
      }
      $this->conftab= NULL;
      $this->lognames= array();;
      $this->cn= "";
      unset ($_SESSION['objectinfo']);
    }

    /* Delete Entry if Posted action (s_action) == del
     * The entry which will be deleted is defined in $s_entry
     */
    if ($s_action =="del"){
      $this->dn= $this->conferences[$s_entry]['dn'];

      /* Check locking */
      if (($conf= get_lock($this->dn)) != ""){
        $_SESSION['dn']= $this->dn;
        return(gen_locked_message($conf, $this->dn));
      } else {
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), $this->dn));
        $display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
        return ($display);
      }
    }

    /* If department deletion is accepted ...
     * Finally delete department
     */
    if (isset($_POST['delete_department_confirm'])){
      if (chkacl($this->acl, "delete") == ""){
        $this->remove_from_parent();
        gosa_log ("Department object'".$this->dn."' has been removed");
        $this->reload ();
      } else {
        print_red (_("You have no permission to remove this department."));
      }
    }

    /* Edit Entry if Posted action (s_action) == edit
     * The entry which will be edited is defined in $s_entry
     */
    if (($s_action=="edit") && (!isset($this->conftab->config))){

      $this->dn= $this->conferences[$s_entry]['dn'];

      if (($conf= get_lock($this->dn)) != ""){
        return(gen_locked_message ($conf, $this->dn));
      }

      /* Lock the current entry, so everyone will get the  above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Register conftab to trigger edit dialog */
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Insert new entry*/    
    if($s_action == "new"){
      $this->dn= "new";
      /* Set up the users ACL's for this 'dn' */
      $this->dn= "new";
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn);
      unset($_SESSION['objectinfo']);//
    }

    /* Edit finished, check and save changes */
    if ((isset($_POST['edit_finish'])) && (isset($this->conftab->config))){
      /* Check tabs, will feed message array */
      $this->conftab->last= $this->conftab->current;
      $this->conftab->save_object();
      $message= $this->conftab->check();

      if (count($message) == 0){
        if($this->conftab->save() == 1){
          gosa_log ("goFonConference object '".$this->dn."' saving failed.");
          return;
        }
        gosa_log ("goFonConference object '".$this->dn."' has been saved");

        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        $this->conftab->save ();
        del_lock ($this->conftab->dn);;
        unset ($this->conftab);
        $this->conftab= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        show_errors($message);
      }
      
      $this->reload();
    }

    /* if edit or new, show dialog */
    if(($this->conftab) && (isset($this->conftab->config))){
      $display= $this->conftab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->conftab->by_object[$this->conftab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Header + Departmentlist*/
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($_SESSION['CurrentMainBase'] == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_new_conference.png' title='"._("Create new conference")."' alt='"._("New conference")."' name='user_new'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit").     "'>&nbsp;".
      "</div>";

    /* Images for delete / edit */
    $actions = "<input class='center' type='image' src='images/edit.png' 		 alt='"._("edit")."' 	  name='conf_edit_%KEY%' title='"._("Edit this entry")."'>";
    $actions.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 	name='conf_del_%KEY%'  title='"._("Delete this entry")."'>";
    
    $actions2= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 	name='conf_del_%KEY%'  title='"._("Delete this entry")."'>";
    /* open an entry */
    $linkopen= "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    /* Set Header */
    $divlist  = new divlist("conferenceTabs");
    $divlist->SetSummary(_("This table displays all available conference rooms."));
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" =>_("Name - Number"), "attach" => "style=''"),
          array("string" => _("Owner"), "attach" => "style='width:200px;'"),
          array("string" => _("PIN"), "attach" => "style='width:50px;'"),
          array("string" =>_("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'")));

    /* Insert departments in divlist*/
    foreach($this->departments as $key=> $val){
      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/".$keys."\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      /* Prepare and insert fields*/
      $a_field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $a_field2 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style=''");
      $a_field3 = array("string" => "&nbsp;", "attach" => "style='width:200px;'");
      $a_field4 = array("string" => "&nbsp;", "attach" => "style='width:50px;'");
      $a_field5 = array("string" => "&nbsp;",  "attach" => "style='width:60px;border-right:0px;text-align:right;'");

      /* Add entries */
      $divlist->AddEntry(array($a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
    }

    /* Image shown in divlist */
    $userimg  = "<img class='center' src='images/select_conference.png' alt='User' title='%s'>";

    /* Edit entry link*/
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Insert conferneces*/
    foreach($this->conferences as $conferencekey => $conference ){

      if((isset($conference['goFonPIN'][0]))&&(($this->ui->dn==$conference['goFonConferenceOwner'][0])||($this->acl=="#all#"))){
        $pin = "<img class='center' src='images/list_password.png' title='PIN : ".$conference['goFonPIN'][0]."' alt='PIN'>";
      }else{
        $pin = "&nbsp;";
      }

      if(isset($conference['telephoneNumber'][0])){
        $number = " - ".$conference['telephoneNumber'][0];
      }else{
        $number = "&nbsp; - ?";
      }

      $ldap= $this->config->get_ldap_link();
      $ldap->cat($conference['goFonConferenceOwner'][0], array('cn'));
      $data = $ldap->fetch();
      $cn = $data['cn'][0];
 
      $a_field1 = array("string"=>sprintf($userimg,_("Conference")), "attach" => "style='text-align:center;width:20px;'");

      if(($this->acl=="#all#")||(($this->ui->dn==$conference['goFonConferenceOwner'][0])&&(chkacl($this->acl,"goFonConferenceOwner")==""))){
        $a_field2 = array("string"=>sprintf($editlink,$conferencekey,$conference['cn'][0].$number), "attach" => "style=''");
      }else{  
        $a_field2 = array("string"=>$conference['cn'][0].$number, "attach" => "style=''");
      }
      $a_field3 = array("string"=> $cn, "attach" => "style='width:200px;'");
      $a_field4 = array("string"=> $pin, "attach" => "style='width:50px;'");

      if(($this->ui->dn==$conference['goFonConferenceOwner'][0])&&(chkacl($this->acl,"goFonConferenceOwner")=="")){
        $a_field5 = array("string"=>preg_replace("/%KEY%/",$conferencekey,$actions), "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      }elseif(chkacl($this->acl,"goFonConferenceOwner")==""){
        $a_field5 = array("string"=>preg_replace("/%KEY%/",$conferencekey,$actions2), "attach" => "style='width:52px;border-right:0px;text-align:right;'");
      }else{
        $a_field5 = array("string"=>"&nbsp;", "attach" => "style='width:52px;border-right:0px;text-align:right;'");
      }

      $divlist->AddEntry(array($a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
    }

    $smarty->assign("search_image",     get_template_path('images/search.png'));
    $smarty->assign("tree_image",       get_template_path('images/tree.png'));
    $smarty->assign("infoimage",        get_template_path('images/info.png'));
    $smarty->assign("launchimage",      get_template_path('images/launch.png'));
    $smarty->assign("conferences",      $divlist->DrawList());
    $smarty->assign("conferencehead",  $listhead);
    $smarty->assign("deplist",          $this->config->idepartments);
    $smarty->assign("apply",            apply_filter());
    $smarty->assign("alphabet",         generate_alphabet());
    $smarty->assign("hint",             print_sizelimit_warning());

    $display= $smarty->fetch(get_template_path('headpage.tpl', TRUE));
    return($display);
  }


  /* Reload entries for divlist.
   * First, reload all conferences for the current base, with the given regex
   * Second, get all departments for the given base and regex
   * Convert departments dn
   */
  function reload()
  {
    $conferencefilter = $_SESSION["conferencefilter"];
    $base             = $_SESSION['CurrentMainBase'];
    $regex            = $conferencefilter['regex'];

    $this->confgerences= get_list("(&(|(cn=$regex)(description=$regex))(objectClass=goFonConference))",
                                  $this->ui->subtreeACL, $base, array("*"), GL_SIZELIMIT);
    $res3= get_list("(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))", $this->ui->subtreeACL,
                    $base, array("ou", "description"), GL_SIZELIMIT | GL_CONVERT);

    $this->departments= array();

    $tmp = array();
    foreach ($res3 as $value){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if(isset($value["description"][0])){
        $this->departments[$value['dn']]= get_sub_department($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]= get_sub_department($value['dn']);//$value["description"][0];
      }
    }
  }

  function remove_from_parent()
  {
    $cfg = new conference($this->config, $this->dn);

    $cfg->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->recursive_remove();

    /* Optionally execute a command after we're done */
    $this->postremove();

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new conftab($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }

  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
