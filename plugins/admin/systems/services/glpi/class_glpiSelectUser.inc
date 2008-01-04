<?php

class glpiSelectUser extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  var $users = array();
  var $departments = array();

  var $ui;

  var $PostVarName      = "";

  function glpiSelectUser (&$config, $dn= NULL,$postvar)
  {
    plugin::plugin ($config, $dn);

    $this->PostVarName = $postvar;

    if(!session::is_set('glpi_user_filter')){
      $tmp['users_regex'] = "*";
      $tmp['base']       = $this->config->current['BASE'];
      session::set('glpi_user_filter',$tmp);
    }

    $this->ui = get_userinfo();  
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if(isset($_POST['depselect'])){
      $_SESSION['glpi_user_filter']['base'] = $_POST['depselect'];
    }

    $filter = $_SESSION['glpi_user_filter'];

    /* Filter settings, remove double* */
    if(isset($_GET['search'])){
      $filter['users_regex'] = preg_replace("/\*\**/","*",$_GET['search']."*");
    }elseif(isset($_POST['regex'])){
      $filter['users_regex'] = preg_replace("/\*\**/","*",$_POST['regex']);
    }
    if(empty($filter['users_regex'])) {
      $filter = "*";
    }


    $s_action="";
    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $filter['base']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $filter['base']=(preg_replace("/^[^,]+,/","",$filter['base']));
    }

    if($s_action=="root"){
      $filter['base']=($this->config->current['BASE']);
    }


    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$filter['base']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $filter['base']= $this->config->departments[trim($base_back)];
      }else{
        $filter['base']= $this->config->departments["/"];
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="dep_open")){
      $entry = base64_decode($_GET['dep_id']);
      $filter['base']= ($this->config->departments[$entry]); 
    }    

    session::set('glpi_user_filter',$filter);

    $this->reload();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $divlist = new divlist("glpi users");
    $divlist->SetPluginMode();
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("Users")." / "._("Departments"), "attach" => "style=''"),
          array("string" => _("Use"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
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

      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style=''");
      $field3 = array("string" => "&nbsp;", "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    $useruse = "<a href='?plug=".$_GET['plug']."&amp;act=".$this->PostVarName."&amp;id=%s'>%s</a>";
    
    foreach($this->users as $key=>$user){
      $field1 = array("string" => "<img src='images/select_user.png' alt='user' >", "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($useruse,base64_encode($key),$user), "attach" => "style=''");
      $field3 = array("string" => sprintf($useruse,base64_encode($key),"<img title='"._("Use")."' border=0 src='images/crossref.png' alt='"._("use")."'>"), 
        "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry(array($field1,$field2,$field3));
      
    }

     /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */

    
    /* Get all departments within this subtree */
    $this->base = $_SESSION['glpi_user_filter']['base'];
    $options  ="";
    $base = $this->config->current['BASE'];
    $deps= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", "users", $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);

    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("users");
    $ids = $this->config->idepartments;
    foreach($deps as $dep){
      if(isset($ids[$dep['dn']]) && in_array_ics($dep['dn'], $tdeps)){
        $value = $ids[$dep['dn']];
        if ($this->base == $dep['dn']){
          $options.= "<option selected='selected' value='".$dep['dn']."'>$value</option>";
        } else {
          $options.= "<option value='".$dep['dn']."'>$value</option>";
        }
      }
    }

    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' src='images/list_root.png' 
      align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;"._("Base")."&nbsp;".
    " <select name='depselect' onChange='mainform.submit()' class='center'>$options</select>".
    " <input class='center' type='image' src='images/list_submit.png' align='middle'
        title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;".
      "</div>";


    $filter= $_SESSION['glpi_user_filter'];
    $smarty->assign("usershead", $listhead);
    $smarty->assign("users", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info_small.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("users_regex", $filter['users_regex']);


    $display.= $smarty->fetch(get_template_path('glpiSelectUser.tpl', TRUE));
    return($display);
  }

  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Optionally execute a command after we're done */
#$this->handle_post_events($mode);
  }

  function reload()
  {

    $filter= $_SESSION['glpi_user_filter'];

    $base = $filter['base'];
    $regex= $filter['users_regex'];
    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */

    $res3= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))","users",
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

    /* END NEW LIST MANAGMENT
     */
    $ou = get_people_ou();

    $res2= get_list("(&(|(cn=$regex)(sn=$regex))(objectClass=person))", "users",
                    $ou.$base, array("cn", "sn", "uid", "givenName", "description"),
                    GL_SUBSEARCH | GL_SIZELIMIT);

    $tmp = array();

    foreach($res2 as $val){
      $str = "";
      if(isset($val['givenName'][0])){
        $str .= $val['givenName'][0];
      }
      if(isset($val['sn'][0])){
        $str .= $val['sn'][0];
      }
      if(isset($val['uid'][0])){
        $str .= $val['uid'][0];
      }

      $tmp[$str] = $val;
    }

    ksort($tmp);
    $this->users = array();
    foreach($tmp as $value){
      if (isset($value["givenName"][0]) && isset($value["sn"][0])){
        $this->users[$value['dn']]= $value["sn"][0].", ".
          $value["givenName"][0].
          " [".$value["uid"][0]."]";
      } else {
        $this->users[$value["dn"]]= "[".$value["uid"][0]."]";
      }
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
