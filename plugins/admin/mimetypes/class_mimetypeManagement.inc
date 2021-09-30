<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_mimetypeManagement.inc 14766 2009-11-05 14:30:35Z hickert $$
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

class mimetypeManagement extends management
{
    var $plHeadline     = "Mime types";
    var $plDescription  = "Manage file associations that are usable in managed software releases";
    var $plIcon  = "plugins/goto/images/mimetypes.png";
    var $matIcon = "file_copy";

    var $mime_release = ""; // The currently selected release while in release management mode!
    var $mime_base = ""; // The currently selected release while in release management mode!

    // Tab definition 
    protected $tabClass = "mimetabs";
    protected $tabType = "MIMETABS";
    protected $aclCategory = "mimetypes";
    protected $aclPlugin   = "mimetype";
    protected $objectName   = "mimetype";

    var $releaseSelector = NULL;

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;
        $this->mime_base = get_ou("mimetype", "mimetypeRDN").$this->config->current['BASE'];
        $this->mime_release = get_ou("mimetype", "mimetypeRDN").$this->config->current['BASE'];

        if($this->IsReleaseManagementActivated()){
            $filter_xml = "mimetype-filter-release.xml";
            $list_xml   = "mimetype-list-release.xml";
            $this->storagePoints= array("");
            $releases = faiManagement::getReleaseStates($this->config,$this->mime_base);
            $this->releaseSelector = new releaseSelector($releases, $this->mime_release, $this->mime_release);
        }else{
            $filter_xml = "mimetype-filter.xml";
            $list_xml = "mimetype-list.xml";
            $this->storagePoints= array(get_ou("mimetype", "mimetypeRDN"));
        }

        // Build filter
        if (0 && session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path($filter_xml, true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path($list_xml, true));
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }
        parent::__construct($config, $ui, "mimetypes", $headpage);
    }


    /*! \brief   Overridden render method of class mangement.
     *            this allows us to add a release selection box.
     */
    function renderList()
    {
        $smarty = get_smarty();
        $filter = $this->getFilter();
        $headpage = $this->getHeadpage();

        // Display release selector
        if($this->IsReleaseManagementActivated()){
            $this->releaseSelector->update();
            $this->mime_release = $this->releaseSelector->getBase();
            $headpage->setBase($this->mime_release);
            $smarty->assign("RELEASE", $this->releaseSelector->render());
        }

        $headpage->update();
        $display = $headpage->render();
        return($this->getHeader().$display);
    }


    /*! \brief  This method opens an existing object or a list of existing objects to be edited.
     *
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        management::editEntry($action,$target,$all,$altTabClass,$altTabType,$altAclCategory);
        if($this->tabObject){
            if(isset($this->tabObject->by_object['mimetype']->attrs['FAIstate'][0])){
                $state = $this->tabObject->by_object['mimetype']->attrs['FAIstate'][0];
                $this->tabObject->setReadOnly(preg_match("/freeze/i", $state));
            }
        }
    }


    /*! \brief   Someone wants to remove some object(s)
     *            ask for a confirmation now.
     */
    function removeEntryRequested($action="",$target=array(),$all=array(),
            $altTabClass ="", $altTabType = "",             $altAclCategory="")
    {
        // Close dialogs and remove locks for currently handled dns
        $this->cancelEdit();

        // Do not allow to remove objects with state freeezed
        $errors = $disallowed = array();
        $headpage = $this->getHeadpage();
        foreach($target as $dn){

            $obj = $headpage->getEntry($dn);
            $acl  = $this->ui->get_permissions($obj['dn'],"mimetypes/mimetype");
            if(!preg_match("/d/",$acl)){
                $disallowed[] = $obj['dn'];
            }elseif(isset($obj['FAIstate']) && preg_match('/freeze/i', $obj['FAIstate'][0])){
                $errors[] = $obj['dn'];
            }else{
                $this->dns[] = $obj['dn'];
            }
        }
        if(count($errors)){
            msg_dialog::display(_("Branch locked"),
                    sprintf(_("The following entries are locked, you can't remove them %s."),
                        msgPool::buildList($errors)),INFO_DIALOG);
        }
        if(count($disallowed)){
            msg_dialog::display(_("Permission error"), msgPool::permDelete($disallowed), ERROR_DIALOG);
        }
        return(management::removeEntryRequested($action, $this->dns));
    }


    function IsReleaseManagementActivated()
    {
        /* Check if we should enable the release selection */
        return($this->config->pluginEnabled("faiManagement"));
    }

} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
