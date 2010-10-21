<?php

class ConfigManagementDataModel
{
    private $data = array();

    function __construct()
    {
        $this->data = array();
        $this->data['linear'] = array();
        $this->data['recursive'] = array();
        $this->addItem('root', '','root', array());
    }


    function addItem($type, $base, $name, $values, $status = "")
    {
        if($base == ""){
            $parentItem = &$this->data['recursive'];
            $parentId = NULL;
        }elseif(isset($this->data['linear'][$base])){
            $parentItem = &$this->data['linear'][$base]['children'];
            $parentId = $this->data['linear'][$base]['id'];
        }else{
            echo "No parent";
            return(FALSE);
        }

        $this->currentID ++;
        $entryBase = "{$base}/{$name}";
        $entryId =  $this->currentID;
        
        $entry['dn'] = $entryBase;
        $entry['id'] = $entryId;
        $entry['parentId'] = $parentId;
        $entry['name'] = $name;
        $entry['base'] = $base;
        $entry['status'] = $status;
        $entry['values'] = $values;
        $entry['children'] = array();

        $parentItem[$entryBase] = &$entry;
        $this->data['linear'][$entryBase] = &$parentItem[$entryBase];
        return(TRUE);
    }
}

?>
