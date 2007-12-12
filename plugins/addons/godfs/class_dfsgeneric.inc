<?php

  class dfsgeneric extends plugin {
    /* CLI vars */
    var $cli_summary      = "Manage terminal base objects";
    var $cli_description  = "Some longer text\nfor help";
    var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

    /* Needed values and lists */
    var $base             = "";
    var $cn               = "";

    /* attribute list for save action */
    var $attributes     = array();
    var $objectclasses  = array();

    function dfsgeneric(&$config, $dn) {
      plugin::plugin($config, $dn);
      $this->dn = $dn;
      $this->orig_dn = $dn;
    }
 
    function execute() {
      /* Call parent execute */
      plugin::execute();
      
      $smarty= get_smarty();

      if (($this->dn == "new") || ($this->dn == "")) {
        $smarty->assign("sambasharename", "");
        $smarty->assign("sharedescription", "");
        $smarty->assign("fileserver", "");
        $smarty->assign("fileservershare", "");
        $smarty->assign("location", "");
        $smarty->assign("dfsdescription", "");
      } else {
        $ldap = $this->config->get_ldap_link();

        $base = get_base_from_people($this->dn);
        $ou = get_ou("DFS");

        $tmp = preg_split('/\//', $this->dn, 2);
        $this->loc = $tmp[0];
        $this->sambasharename = $tmp[1];

        $ldap->cd("$ou $base");
        $ldap->search(("ou=$this->loc"), array("description"));
        $dfs_desc = $ldap->fetch();
        $this->dfsdescription = $dfs_desc['description'][0];

        $ldap->cd("ou=$this->loc, $ou $base");
        $ldap->search("(&(sambaShareName=$this->sambasharename)(objectclass=sambaShare))", array("description", "documentLocation"));

        $details = $ldap->fetch();
        $this->sharedescription = $details['description'][0];
        $tmp = preg_split('/\\\\/', $details['documentLocation'][0], 2);

        $this->fileserver = preg_replace("/msdfs:/", "", $tmp[0]);
        $this->share = preg_replace("/\\\/", "", $tmp[1]);
      
        #var_dump($this->dn);
        #echo "<BR>\n";

        /* Fill array */
        #$this->reload();
        $smarty->assign("sambasharename", $this->sambasharename);
        $smarty->assign("sharedescription", $this->sharedescription);
        $smarty->assign("fileserver", $this->fileserver);
        $smarty->assign("fileservershare", $this->share);
        $smarty->assign("location", $this->loc);
        $smarty->assign("dfsdescription", $this->dfsdescription);
      }

      /* Show main page */
      return ($smarty->fetch (get_template_path('generic.tpl', TRUE)));
      #, dirname(__FILE__))));
    }

    function check() 
	{
      plugin::check(); 
      $message = array();

      ## permission
      #if (chkacl($this->acl, "create") != "") {
      #  $message[] = _("You have no premissions to create a dfs share.");
      #}

      # existance
      
      $ldap = $this->config->get_ldap_link();
      $base = get_base_from_people($this->ui->dn);
      $ou = get_ou("DFS");
      $dn_explode = explode("/", $this->dn);
      $sub_ou = $dn_explode[0];
      $sambaShareName = $dn_explode[1] . "/" . $dn_explode[2] . "/" . $dn_explode[3];
      $dn = "sambaShareName=$sambaShareName,ou=$sub_ou,$ou$base";
      $ldap->cat($dn);
      $attrs = $ldap->fetch();
      
      if ($this->orig_dn == "new" && !($attrs == FALSE)) {
        $message[] = _("Dfs share already exists.");
      } elseif ($this->orig_dn != $this->dn && !($attrs == FALSE)) {
        $message[] = _("Dfs share already exists.");
      }
      
      if ($this->dn == "new" || $this->dn == "") {
        $this->sambasharename = $_POST['sambaShareName'];
        $this->sharedescription = $_POST['description'];
        $this->fileserver = $_POST['fileserver'];
        $this->share = $_POST['fileservershare'];
        $this->loc = $_POST['location'];
      }
      
      # required fields set?
      if ($this->sambasharename == "") {
        $message[] = _("Required Field \"Name of dfs Share\" is not set.");
      }
      if ($this->sharedescription == "") {
        $message[] = _("Required Field \"Description\" is not set.");
      }
      if ($this->fileserver == "") {
        $message[] = _("Required Field \"Fileserver\" is not set.");
      }
      if ($this->share == "") {
        $message[] = _("Required Field \"Share on fileserver\" is not set.");
      }
      if ($this->loc == "") {
        $message[] = _("Required Field \"Location\" is not set.");
      }

      return $message;
    }
    
    function save() {
      
      plugin::save();

      # get the current values
      $this->sambasharename = $_POST['sambaShareName'];
      $this->sharedescription = $_POST['description'];
      $this->fileserver = $_POST['fileserver'];
      $this->share = $_POST['fileservershare'];
      $this->loc = $_POST['location'];
          
      # set the attribs
      $this->attrs["sambaShareName"]   = "$this->sambasharename,ou=$this->loc,$ou$base";
      $this->attrs["objectClass"][]    = "top";
      $this->attrs["objectClass"][]    = "SambaShare";
      $this->attrs["objectClass"][]    = "extensibleObject"; 
      $this->attrs["sambaShareName"]   = $this->sambasharename;
      $this->attrs["description"]      = $this->sharedescription;
      $this->attrs["documentLocation"] = "msdfs:$this->fileserver\\\\$this->share";

      $ldap = $this->config->get_ldap_link();

      if ($this->dn == "new" || $this->dn == "") {
        echo "new<br>\n";
        $base = get_base_from_people($this->ui->dn);
        $ou = get_ou("DFS");
        $this->basedn = "sambaShareName=$this->sambasharename,ou=$this->loc,$ou$base";
        $ldap->cd($this->basedn);
        $ldap->add($this->attrs);
      } else {
        # try to find entry
        $base = get_base_from_people($this->ui->dn);
        $ou = get_ou("DFS");
        #$dn_explode = explode("/", $this->sambasharename);
        #$sub_ou = $dn_explode[0];
        #$sambaShareName = $dn_explode[1] . "/" . $dn_explode[2] . "/" . $dn_explode[3];
        $dn = "sambaShareName=$this->sambasharename,ou=$this->loc,$ou$base";
        $ldap->cat($dn);
        $attrs = $ldap->fetch();
          
        $this->basedn = "sambaShareName=$this->sambasharename,ou=$this->loc,$ou$base";
        $ldap->cd($this->basedn);
        $nr = count($attrs);
        
        if (count($attrs)) {
          # modify if found
          $ldap->modify($this->attrs);
        } else {
          # add
          $ldap->add($this->attrs);
        }
				show_ldap_error($ldap->get_error(), sprintf(_("Saving dfs/generic with dn '%s' failed."),$this->basedn));
      }
    }

    function save_object() {
      #if (isset($_POST['base'])) {
        plugin::save_object();
        #echo "base = ".$_POST['base']."<br>\n";
      #}
#      if (chkacl($this->acl, "create") == "") {
        $this->base = $_POST['base'];
#      }
    }

    function delete() {
    
      plugin::delete();

      # get the current values
      $this->sambasharename = $_POST['sambaShareName'];
      $this->sharedescription = $_POST['description'];
      $this->fileserver = $_POST['fileserver'];
      $this->share = $_POST['fileservershare'];
      $this->loc = $_POST['location'];
          
      $base = get_base_from_people($this->ui->dn);
      $ou = get_ou("DFS");
      $this->basedn = "sambaShareName=$this->sambasharename,ou=$this->loc,$ou$base";
      echo "BASEDN: $this->basedn<br>\n";

      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->basedn);
      $ldap->rmdir($this->basedn);
			show_ldap_error($ldap->get_error(), sprintf(_("Removing dfs/generic with dn '%s' failed."),$this->basedn));
    }
      
  }

