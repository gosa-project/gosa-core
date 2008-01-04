<?php

/* Create a class (name must be unique inside GOsa) which extends plugin. The plugin base
    class contains all methods that are used by GOsa and it provides the mechanism to load data
    from LDAP to local variables and prepare the save to ldap routine for you. */
class dfsManagment extends plugin {
  /* Definitions */
  var $plHeadline = "DFS Managment";
  var $plDescription = "This does something";
   
  /* These contain attributes to be loaded. We're not doing an LDAP plugin currently, so we don't
     care... */
  var $attributes = array();
  var $objectclasses = array();
  var $dfstab = NULL;

  /* The constructor just saves a copy of the config. You may add what ever you need. */
  function dfsManagment (&$config, &$ui) {
    /* save config for later use */
    $this->config = &$config;
    $this->ui = &$ui;
    
  }

  /* Execute is the function all plugins need. It fills the plugin with life and produces the output. */
  function execute() {
	
    /* Normally you would react to user input here. */
    plugin::execute();    

    $action = "";
    $s_action= "";
    $options = "";
    $regex = "";
	
    $icon         = 'folder.gif';
    $expandedIcon = 'folder-expanded.gif';
    $base = get_base_from_people($this->ui->dn);
    $smarty= get_smarty();

    foreach ($_POST as $key => $val) {
      # delete
      if (preg_match("/dfs_del.*/", $key)) {
        $s_action = "del";
        $s_entry = preg_replace("/dfs_".$s_action."_/i", "", $key);
        $s_entry = preg_replace("/_.*$/", "", $s_entry);
        $s_entry = base64_decode($s_entry);
      # edit  
      } elseif (preg_match("/dfs_edit.*/", $key)) {
        $s_action = "edit";
        $s_entry = preg_replace("/dfs_".$s_action."_/i", "", $key);
        $s_entry = preg_replace("/_.*$/", "", $s_entry);
        $s_entry = base64_decode($s_entry);
      # new
      } elseif (preg_match("/dfs_new.*/", $key)) {
        $s_action = "new";
      # back
      } elseif (preg_match("/dfs_back.*/", $key)) {
        $s_action = "back";
      # home
      } elseif (preg_match("/dfs_home.*/", $key)) {
        $s_action = "home";
      # root
      } elseif (preg_match("/dfs_root.*/", $key)) {
        $s_action = "root";
      }
    }  

    $tree  = new HTML_TreeMenu();

    if ((isset($_GET['act'])) && ($_GET['act'] == "edit_entry")) {
      $s_action = "edit";
      $dfs_share = $_GET['id'];
    }

    # new
    if ($s_action == "new") {
      $this->dn = "new";
      $this->dfstab = new dfstabs($this->config, $this->config->data['TABS']['DFSTABS'], $this->dn);
    }

    # edit
    if (($_SERVER['REQUEST_METHOD'] == "POST") || ($_SERVER['REQUEST_METHOD'] == "GET")) {
      if ($s_action == "edit") {

        $this->dn = $dfs_share;

        if (($user = get_lock($this->dn)) != "") {  
          return (gen_locked_message ($user, $this->dn));
        } else {
          add_lock ($this->dn, $this->ui->dn);
        }

        $this->acl = get_permissions($this->dn, $this->ui->subtreeACL);

        $this->dfstab = new dfstabs($this->config, $this->config->data['TABS']['DFSTABS'], $this->dn);
        $this->dfstab->set_acl($this->acl);
        $this->dfstab->save_object();
        session::set('objectinfo',$this->dn);
      }
    } 
      
    # save pressed
    if (isset($_POST['edit_finish'])) {
      $this->dfstab->last = $this->dfstab->current;
      $this->dfstab->save_object(); 
      #$disp = $this->by_object[''];
      #var_dump($disp);
    
      $message = $this->dfstab->check();
     
      # any errors?
      if (count($message) == 0) {
        # write to ldap
        $this->dfstab->save();
      } else {
        show_errors($message);
      }
    }

    # cancel pressed
    if (isset($_POST['edit_cancel'])) {
      del_lock ($_SESSION['objectinfo']);
      unset ($this->dfstab);
      $this->dfstab = NULL;
      $this->dn = "";
      session::un_set('objectinfo');
    }

    # delete pressed
    if (isset($_POST['edit_delete'])) {
      
      # get the current values
      $this->sambasharename = $_POST['sambaShareName'];
      $this->sharedescription = $_POST['description'];
      $this->fileserver = $_POST['fileserver'];
      $this->share = $_POST['fileservershare'];
      $this->loc = $_POST['location'];
          
      $base = get_base_from_people($this->ui->dn);
      $ou = get_ou("DFS");
      $this->basedn = "sambaShareName=$this->sambasharename,ou=$this->loc,$ou$base";

      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->basedn);
      $ldap->rmdir($this->basedn);
			show_ldap_error($ldap->get_error(), sprintf(_("Removing of DFS share with dn '%s' failed."),$this->dn));
      
      del_lock ($_SESSION['objectinfo']);
      unset ($this->dfstab);
      $this->dfstab = NULL;
      $this->dn = "";
      session::un_set('objectinfo');
    }

    # generate main page no tabs

    $ldap = $this->config->get_ldap_link();

    $ou = get_ou("DFS");
    
    $ldap->cd("$ou$base");
    $ldap->search(("ou=*"), array("dn"));
    
    if ($ldap->count() == 0) {
      $message[] = _("No DFS entries found");
      show_errors ($message);
    } else {
      $have_tree= false;
      while ($dfs = $ldap->fetch()) {
        if (preg_match("/^ou=DFS.*/", $dfs["dn"])) {
          $dfs_root_node = new HTML_TreeNode(array('text' => "DFS", 'icon' => $icon, 'expandedIcon' => $expandedIcon, 'expanded' => false));
	  $have_tree= true;
        } elseif ($have_tree) {
          preg_match("/^ou=(.*),/U", $dfs["dn"], $reg);
          $loc = $reg[1];
          $node = $dfs_root_node->addItem(new HTML_TreeNode(array('text' => "$loc", 'icon' => $icon, 'expandedIcon' => $expandedIcon)));
          
          $sub_node_ldap = $this->config->get_ldap_link();
          $sub_node_ldap->cd("ou=$loc, $ou$base");
          $sub_node_ldap->search("(objectclass=sambaShare)", array("sambaShareName", "description", "documentLocation"));
          
          while ($dfs_loc = $sub_node_ldap->fetch()) {
            $share_name = $dfs_loc["sambaShareName"][0];
            $desc = $dfs_loc["description"][0];
            $srv_loc = preg_replace("/msdfs:/", "", $dfs_loc["documentLocation"][0]);
            #$srv_loc = preg_replace("/\\/", "\\\\", $srv_loc);
            $link = "main.php\?plug=".$_GET['plug']."\&id=$loc\/$share_name\&act=edit_entry"; 
            $share_node = &$node->addItem(new HTML_TreeNode(array('text' => "$share_name - $desc", 'link' => "$link", 'icon' => $icon, 'expandedIcon' => $expandedIcon)));
            $srv_loc_node = &$share_node->addItem(new HTML_TreeNode(array('text' => "$srv_loc", 'icon' => $icon, 'expandedIcon' => $expandedIcon)));
          }
        }
      }
    }

    $tree->addItem($dfs_root_node);

    // Create the presentation class
    $treeMenu = new HTML_TreeMenu_DHTML($tree, array('images' => 'images', 'defaultClass' => 'treeMenuDefault'));
    $tree_html = $treeMenu->toHTML();
    
    if ($this->dfstab === NULL) {

      if (($message = check_sizelimit()) != "") {
        return ($message);
      }
  
      $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
         " <input type='image' class='center' src='images/list_up.png' align='middle' title='"._("Go up one dfsshare")."' name='dfs_back' alt='"._("Up")."'>&nbsp;".
         " <input type='image' class='center' src='images/list_root.png' align='middle' title='"._("Go to dfs root")."' name='dfs_root' alt='"._("Root")."'>&nbsp;".
         " <input class='center' type='image' src='images/dtree.png' align='middle' alt='"._("Create new dfsshare")."' name='dfs_new'>&nbsp;".
         " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
         _("Base")."&nbsp;<select name='depselect' onChange='mainform.submit()' class='center'>$options</select>".
         " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit").
         "'>&nbsp;</div>";


      /* Use the smarty templating engine here... */

      $smarty->assign('tree', $tree_html);
      $smarty->assign('search_image', get_template_path('images/search.png'));
      $smarty->assign('infoimage', get_template_path('images/info.png'));
      $smarty->assign('launchimage', get_template_path('images/launch.png'));
      $smarty->assign('alphabet', generate_alphabet());
      $smarty->assign('hint', print_sizelimit_warning());
      $smarty->assign('apply', apply_filter());
      $smarty->assign('dfshead', $listhead);

      /* Let smarty fetch and process the page. Always seperate PHP and HTML as much as
         you can. */
      return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
    }

    /* TABS */

    $display = $this->dfstab->execute();
   
    $display .= "<p>\n";
    $display .= "  <table width=100\%>\n";
    $display .= "    <tbody>\n";
    $display .= "      <tr>\n";
    $display .= "        <td style=\"text-align: right\">\n";
    $display .= "          <input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
    $display .= "          &nbsp;\n";
    $display .= "          <input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
    $display .= "        </td>\n";
    $display .= "      </tr>\n";
    $display .= "    </tbody>\n";
    $display .= "  </table>\n";
    $display .= "</p>\n";
    
    return ($display);

  }

  function remove_lock() {
    if (isset($this->dn)) {
      del_lock ($this->dn);
    }
  }

}
?>
