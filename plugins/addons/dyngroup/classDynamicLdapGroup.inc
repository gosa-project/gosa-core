<?php

/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 * Copyright (C) 2010 Thomas CHEMINEAU
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * This class will allow user to parameter dynamic group.
 * @author Thomas Chemineau - thomas.chemineau<at>gmail.com
 * @version 0.01
 */
class DynamicLdapGroup extends plugin
{

    /**
     * The attribute that will use GOsa to store LDAP URI.
     * @var array
     */
    public $attributes = array('labeledURI');

    /**
     * The objectClass that will use GOsa to identify a group as dynamic.
     * @var array
     */
    public $objectclasses = array('labeledURIObject');

    /**
     * Default value for the corresponding attribute found in the $this->attributes
     * array of this plugin.
     * @var string
     */
    public $labeledURI = array();
    public $labeledURIparsed = array();
    public $labeledURIdefault = 'ldap:///dc=example,dc=com?memberUid?sub?(objectClass=posixGroup)';
    
    public $scopes = array('base','one','sub');

    /**
     * Store values of memberUrl.
     * @var Array
     */
    private $_memberUrls = Array();

    public $orig_dn ="";

    /**
     * Create this object.
     * @param Array $config GOsa config.
     * @param string $dn Current DN.
     */
    public function __construct ($config, $dn)
    {
        parent::__construct($config, $dn);

        // Load labeledURI values.
        $this->labeledURI = array();
        if(!$this->is_account){
            $this->labeledURI[] = str_replace('dc=example,dc=com', LDAP::fix($this->dn), $this->labeledURIdefault);
        }elseif(isset($this->attrs['labeledURI'])){
            for($i =0; $i < $this->attrs['labeledURI']['count']; $i++) {
                $this->labeledURI[] = $this->attrs['labeledURI'][$i];
            }
        }

        // Parse labeledURI entries
        $this->labeledURIparsed = array();
        foreach($this->labeledURI as $entry){
            list($base,$attr,$scope,$filter) = preg_split("/\?/",$entry); 
            
            // Ignore entries that do not have a valid scope value (one,base,sub)
            if(!in_array($scope,array('base','one','sub'))) continue;

            // Append parsed uri
            $scope = array_search($scope,$this->scopes);
            $this->labeledURIparsed[] = array('base' => $base, 'attr' => $attr, 'scope' => $scope,'filter' => $filter);
        }
   
        // Save dn, to be able the check for object movements - put this in plugin::move 
        $this->orig_dn = $this->dn;
    }


    public function check ()
    {
        $messages = plugin::check();
        
        // At least one entry is required. 
        if(!count($this->labeledURIparsed)){
            $messages[] = msgPool::required(_("Labeled URI"));
        }  

        // Check entries
        foreach($this->labeledURIparsed as $key => $entry){
            $nr = $key +1;

            // A base is required
            if(empty($entry['base'])){
                $messages[] = msgPool::required(_("Base")." {$nr}");
            }

            // Check for invalid attributes
            if(empty($entry['attr'])){
                $messages[] = msgPool::required(_("Attribute")." {$nr}");
            }elseif(in_array(strtolower($entry['attr']), array('objectclass'))){
                $messages[] = msgPool::reserved(_("Attribute")." {$nr}");
            }

            // A filter is required
            if(empty($entry['filter'])){
                $messages[] = msgPool::required(_("Filter")." {$nr}");
            }elseif(!preg_match("/^\(/", $entry['filter'])){
                $messages[] = msgPool::invalid(_("Filter")." {$nr}",  $entry['filter'],'', '(objectClass=gosaAccount)'." - "._("Surrounding brackets are required!"));
            }else{

                // Check if filter is valid
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($this->config->current['BASE']);
                $ldap->search($entry['filter']);
                if(!$ldap->success()){
                    $messages[] = sprintf(_("The given filter '%s' for entry %s seems to be invalid!"),
                            bold($entry['filter']), $nr);
                }
            }
        }
 
        return($messages);
    }


    /**
     * Execute this plugin.
     * @return string HTML to print.
     */
    public function execute ()
    {
        //
        // Are we trying to modify state of this group ? If so,
        // we can edit the current object.
        //
        if (isset($_POST['modify_state']))
        {
            $this->is_account = !$this->is_account;
        }

        //
        // Display a message if this feature is disabled.
        //
        if (!$this->is_account)
        {
            return $this->show_disable_header(msgPool::addFeaturesButton(_("Dynamic object")), msgPool::featuresDisabled(_("Dynamic object")));
        }
        $display = $this->show_disable_header(msgPool::removeFeaturesButton(_("Dynamic object")), msgPool::featuresEnabled(_("Dynamic object")));

        // Display values.
        //
        $smarty = get_smarty();
        $smarty->assign('labeledURIparsed',  set_post($this->labeledURIparsed));
        $smarty->assign('scopes', set_post($this->scopes));
        $display .= $smarty->fetch(get_template_path('dyngroup.tpl', TRUE, dirname(__FILE__)));
        return $display;
    }



    /**
     * This plugin does nothing when this method is invoked.
     */
    public function remove_from_parent ()
    {
        parent::remove_from_parent();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->modify($this->attrs);
        if(!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }
        return;
    }


    /**
     * This function is called when tab is undisplayed. For example, the current user
     * wants to change other settings of this group, but not save it to the LDAP
     * directory directly.
     */
    public function save_object ()
    {
        parent::save_object();

        // Add a new labeled Uri
        if(isset($_POST['addUri'])){
            $this->labeledURIparsed[] = 
                array(
                        'base' => 'ldap:///'.$this->dn,
                        'attr' => 'memberUid', 
                        'scope' => 2,
                        'filter' => '(objectClass=posixGroup)');
        }

        // Remove a labeled Uri and get posts
        foreach($this->labeledURIparsed as $key => $data){
            foreach(array('scope','attr','filter','base') as $attr){
                if(isset($_POST[$attr.'_'.$key])){
                    $this->labeledURIparsed[$key][$attr] = get_post($attr.'_'.$key);
                }
            }
        
            // Remove labeled uri if requested
            if(isset($_POST['delUri_'.$key])){
                unset($this->labeledURIparsed[$key]);
            }
        }
        $this->labeledURIparsed = array_values($this->labeledURIparsed);
    }


    /**
     * That will add additionnal information into the current LDAP entry.
     * If this plugin is disable, then it will remove any data that references
     * this plugin into the LDAP directory.
     * @return boolean
     */
    public function save ()
    {
        // Build up labeledUri entries
        $this->labeledURI = array();
        foreach($this->labeledURIparsed as $entry){
            $scope = $this->scopes[$entry['scope']];
            $filter = $entry['filter'];
            $this->labeledURI[] = "{$entry['base']}?{$entry['attr']}?{$scope}?{$filter}";
        }
        $this->labeledURI = array_unique($this->labeledURI);

        parent::save();
        $this->cleanup();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->modify($this->attrs);
        if(!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }
    }


    /*! \brief      Updates labeledURI entries in ldap.
     *              Check whether the given src_dn is part of some labeledURI entries
     *               and then updates the entries to use the dst_dn.
     * @param       $config     The GOsa configuration object.
     * @param       $src_dn     The source 'dn' of the object that was moved.
     * @param       $dst_dn     The target 'dn' of the object that was moved.
     */
    public static function moveDynGroup($config,$src_dn,$dst_dn)
    {
        // Fetch all dynamic group definitions 
        $objs = get_list("(&(objectClass=labeledURIObject)(labeledURI=*))",array("all"),$config->current['BASE'],
                array("dn","labeledURI"),GL_SUBSEARCH | GL_NO_ACL_CHECK);
        $newAttrs = array();
        foreach($objs as $obj){
            $changes = false;
            $attrs = array();
            for($i = 0; $i < $obj['labeledURI']['count']; $i++){
                $c = $obj['labeledURI'][$i];
                $c = preg_replace('/'.preg_quote($src_dn,'/').'/',$dst_dn,$c);
                $attrs['labeledURI'][] = $c;

                // Check if something has changed
                if($c != $obj['labeledURI'][$i]){
                    $changes =TRUE;
                }
            }

            // If at least one line of 'labeledURI' has changed then we have to update the whole entry.
            if($changes)      $newAttrs[$obj['dn']] = $attrs;
        }
 
        // If we've at least one entry to update then 
        if(count($newAttrs)){
            $ldap = $config->get_ldap_link();
            foreach($newAttrs as $dn => $data){
                $ldap->cd($dn);
                $ldap->modify($data);
                if(!$ldap->success()){
                    trigger_error(sprintf("Failed to dynamic group object for %s: %s", bold($dn), $ldap->get_error()));
                    new log("debug",
                            "plugin/plugin::move()",$dn,array(),
                            " -- ERROR -- Failed to update dynamic groups (labeledURI) - ".$ldap->get_error());
                }else{
                    new log("modify",
                            "plugin/plugin::move()",$dn,array_keys($data),
                            "Updated dynamic group entries (labeledURI)");
                }
            }
        }
    }


    /**
     * Static method to set ACL for this plugin.
     */
    public static function plInfo()
    {
        return Array(
                "plShortName"   => _("Dynamic object"),
                "plDescription" => _("Dynamic object"),
                "plSelfModify"  => TRUE,
                "plDepends"     => Array(),
                "plPriority"    => 1,
                "plSection"     => Array("addon"),
                "plCategory"    => Array("groups", "department", "ogroups"),
                "plProvidedAcls" => array(
                    'labeledURI' =>  _('Labeled URI'),
                    )
                );
    }
}

?>
