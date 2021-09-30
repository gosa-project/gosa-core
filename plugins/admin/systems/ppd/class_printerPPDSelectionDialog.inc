<?php

class printerPPDSelectionDialog extends plugin
{
    /* attribute list for save action */
    var $ignore_account       = FALSE;
    var $attributes           = array();
    var $objectclasses        = array("whatever");

    var $list                 =array();
    var $header               =array();
    var $current_del_id       = "";
    var $Vendor               = "";
    var $selectedPPD = "";

    function __construct(&$config, $dn= NULL,$list=false,$headers=false,$ppd=false)
    {
        plugin::__construct ($config, $dn);
        $this->list       = $list;
        $this->header     = $headers;
        $this->depselect  = $this->config->current['BASE'];

        // Prepare lists
        $this->ppdList = new sortableListing();
        $this->ppdList->setDeleteable(true);
        $this->ppdList->setInstantDelete(false);
        $this->ppdList->setEditable(true);
        $this->ppdList->setWidth("100%");
        $this->ppdList->setColspecs(array('*','20px'));
        $this->ppdList->setHeight("200px");
        $this->ppdList->setHeader(array(_("PPD")));
        $this->ppdList->setDefaultSortColumn(0);
    }

    function isSelected(){
        return(!empty($this->selectedPPD));
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Fill templating stuff */
        $smarty= get_smarty();
        $display= "";


        $this->ppdList->save_object();
        $action = $this->ppdList->getAction();
        if($action['action'] == "delete"){
            $data = $this->ppdList->getData($action['targets'][0]);
            if($data['TYPE'] == 'PPD'){
                $found = "";
                foreach($this->list[$this->Vendor] as $key => $ppd){
                    if($ppd['link'] == $data['ID']){
                        $found = $key;
                        break;
                    }
                }
                if(!empty($found)){
                    $this->current_del_id = $ppd['link'];
                    $smarty->assign("warning", sprintf(msgPool::deleteInfo($found)));
                    return($smarty->fetch(get_template_path('remove_ppd.tpl', TRUE, dirname(__FILE__))));
                }else{
                    msg_dialog::display(_("PPD error"), 
                            msgPool::fileDoesNotExist($this->current_del_id), ERROR_DIALOG);
                }
            }
        }

        /* Deletion confirmed */
        if(isset($_POST['delete_ppd_confirm']) && !empty($this->current_del_id)){
            $is_ok = false;
            if(is_writeable($this->current_del_id)){
                $is_ok = @unlink($this->current_del_id);

                /* Update ppd list */
                if($is_ok){
                    foreach($this->list as $vendor => $ppds){
                        foreach($ppds as $ppd => $data){
                            if($data['link'] == $this->current_del_id){
                                unset($this->list[$vendor][$ppd]);
                            }
                        }
                    }
                }
            }
            if(!$is_ok){
                msg_dialog::display(_("PPD error"), 
                        msgPool::cannotDeleteFile($this->current_del_id), ERROR_DIALOG);
            }
        }
        // Build up ppd 
        $list = array();
        foreach($this->list as $cat => $ppds){
            foreach($ppds as $ppd){
                $name = $ppd['ppd']['name'];
                $list[$ppd['link']] = $ppd;
            }
        }


        $data = $lData = array();
        if(empty($this->Vendor)){
            
            $this->ppdList->setDeleteable(false);
            foreach($this->header as $key => $entry){
                $data[$key] = array('TYPE' => 'VENDOR' , 'ID' => $key);
                $lData[$key] = array('data'=>array($key));
            } 

        }else{
            $data[] = array('TYPE' => 'VENDOR' , 'ID' => '..');
            $lData[] = array('data'=>array(".. ["._("back")."]"));

            $this->ppdList->setDeleteable(true);
            foreach($list as $key => $ppd){
                $name = $ppd['ppd']['name'];
                if(preg_match("/^".$this->Vendor."/", $name)){
                    $data[$key] = array('TYPE' => 'PPD' , 'ID' => $key);
                    $lData[$key] = array('data'=>array($name));
                }
            }
        }

        $this->ppdList->setAcl('rwcdm');
        $this->ppdList->setListData($data,$lData);
        $this->ppdList->update();

        $smarty->assign("List", $this->ppdList->render());

        $display.= $smarty->fetch(get_template_path('printerPPDSelectionDialog.tpl', TRUE,dirname(__FILE__)));
        return($display);
    }

    function save_object()
    {
        $this->ppdList->save_object();
        $action = $this->ppdList->getAction();
        if($action['action'] == "edit"){
            $data = $this->ppdList->getData($action['targets'][0]);
            if($data['TYPE'] == "VENDOR"){
                $this->Vendor = $data['ID'];
                if($this->Vendor == "..") $this->Vendor = "";
            }
            if($data['TYPE'] == "PPD"){
                $this->selectedPPD = $data['ID'];
            }
        } 

    } 

    function check(){
    }

    /* Save to LDAP */
    function save()
    {
        return $this->selectedPPD;
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
