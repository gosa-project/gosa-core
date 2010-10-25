<?php

class ConfigManagementDataModel
{
    private $data = array();
    private $typeToPath = array();
    private $idToPath = array();

    function __construct()
    {
        $this->data = array();
        $this->data['linear'] = array();
        $this->data['recursive'] = array();
        $this->addItem('root', '','root', array());
    }

    function getDataModel()
    {
        return($this->data);
    }

    


    function addItem($type, $path, $name, $values = array(), $status = "")
    {
        if($path == ""){
            $parentItem = &$this->data['recursive'];
            $parentId = NULL;
            $parentDn = "";
        }elseif(isset($this->data['linear'][$path])){
            $parentItem = &$this->data['linear'][$path]['children'];
            $parentId = $this->data['linear'][$path]['id'];
            $parentDn = $this->data['linear'][$path]['dn'];
        }else{
            echo "No parent";
            return(FALSE);
        }

        $this->currentID ++;
        $entryPath = "{$path}/{$name}";
        $entryDn  = rtrim("cn={$name},{$parentDn}",',');
        $entryId =  $this->currentID;
        
        $entry['path'] = $entryPath;
        $entry['dn'] = $entryDn;
        $entry['id'] = $entryId;
        $entry['parentId'] = $parentId;
        $entry['name'] = $name;
        $entry['path'] = $entryPath;
        $entry['parentPath'] = $path;
        $entry['type'] = $type;
        $entry['status'] = $status;
        $entry['values'] = $values;
        $entry['children'] = array();

        $parentItem[$entryPath] = &$entry;
        $this->data['linear'][$entryPath] = &$parentItem[$entryPath];

        $this->idToPath[$entryId] = $entryPath;
        $this->typeToPath[$type][$name] = $entryPath;
    
        return($entryId);
    }

    function pathToDn($path)
    {
        
    }

    function itemExistsByPath($path)
    {
        return(isset($this->data['linear'][$path]));
    }
    
    function getItemByPath($path)
    {
        if(isset($this->data['linear'][$path])){
            return($this->data['linear'][$path]);
        }
        return(NULL);
    }

    function setItemStatus($path, $status)
    {
        if(!$this->itemExistsByPath($path)){
            trigger_error("Invalid item path '{$path}'!");
        }else{
            $this->data['linear'][$path]['status'] = $status;
        }
    }

    function getItemsByType($types)
    {
        $res = array();
        foreach($this->data['linear'] as $path => $item){
            if(in_array($item['type'],$types)) $res[] = $item;
        }
        return($res);
    }
}

?>
