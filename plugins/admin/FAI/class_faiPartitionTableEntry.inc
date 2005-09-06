<?php

class faiPartitionTableEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("DISK_cn","DISK_description");
  var $objectclasses= array();

  var $DISK_cn          = "";
  var $DISK_description = "";
  
  var $partitions           = array();
  var $UsedAttrs            = array();
  var $AvailableDiskNames   = array();
  var $is_edit              = false;
  var $old_cn               = "";

  function faiPartitionTableEntry ($config, $dn= NULL,$disk=false)
  {
    plugin::plugin ($config, $dn);
    $this->UsedAttrs  = array("cn","description","FAIpartitionType","FAIpartitionNr","FAIfsType","FAImountPoint","FAIpartitionSize",
                         "FAImountOptions","FAIfsOptions","FAIpartitionFlags");

    $this->AvailableDiskNames = array();
    foreach(array("a","b","c","d","e","f","g","h") as $ts){
      $this->AvailableDiskNames["hd".$ts]= "hd".$ts;
    }
    
    if($disk){
      foreach($disk['partitions'] as $name => $values){
        foreach($this->UsedAttrs as $attr){
          if(!isset($values[$attr])){
             $disk['partitions'][$name][$attr]="";  
          }
        }
      }


      $this->DISK_cn          = $disk['cn'];
      $this->DISK_description = $disk['description'];
      $this->partitions       = $disk['partitions'];
      $this->is_edit          = true;
      $this->old_cn           = $disk['cn'];
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $s_action   = "";
    $s_entry    = "";
    $display    = "";

    /* Assign values 
     * And Checkbox selection
     */
    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,$this->$attrs);
      if($this->$attrs){
        $smarty->assign($attrs."CHK"," ");
      }else{
        $smarty->assign($attrs."CHK"," disabled ");
      }
    }

    /* Check all Posts if there is something usefull for us,
     * For example : Delete is posted as Delete_1 
     * The number specifies the index we want to delete
     */
    foreach($_POST as $name => $value){
      if(preg_match("/Delete_.*/",$name)){
        $tmp = split("_",$name);
        $s_action = "remove";
        $s_entry  = $tmp[1]; 
      }
    }

    /* To remove a partition we unset the index posted.
     * We must sort the index again, else we possibly got problems 
     * with partitions order.
     */
    if($s_action == "remove"){
      unset($this->partitions[$s_entry]);
      $tmp= array();
      foreach($this->partitions as $part){
        $tmp[count($tmp)+1]=$part;
      }
      $this->partitions = $tmp;

    }

    /* To add a partitions we only append an empty 
     *  array to the already defined partitions.
     */
    if(isset($_POST['AddPartition'])){
      foreach($this->UsedAttrs as $attr){
        $tmp[$attr]                = "";     
      }
      $this->partitions[count($this->partitions)+1]=$tmp;
    }

    /* Tell smarty which Disknames are available.
     */
    $smarty->assign("cns", $this->AvailableDiskNames); 
    
    /* $setup contains a table with the partitions. 
     */
    $smarty->assign("setup", $this->generateParts());
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    /* The user can't change a diskname 
     *  if we are currently in edit mode for the specified disk 
     */
    if($this->is_edit){
      $smarty->assign("DISK_cnACL"," disabled ");
    }else{
      $smarty->assign("DISK_cnACL","");
    }

    /* Fetch template and show the result
     */
    $display.= $smarty->fetch(get_template_path('faiPartitionTableEntry.tpl', TRUE));
    return($display);
  }

  function generateParts()
  {
    /* Define Arrays with allowed syntax */
    $PartitionTypes   = array("primary","logical");
    $FAIfsTypes       = array("ext2","ext3","xfs","swap","reiserfs","dos fat 16","win fat 32");

    /* Display Header */
    $str = "<table>";
    $str .= "<tr>";
    $str .= "<td>"._("Nr")."</td>";
    $str .= "<td>"._("Name")."</td>";
    $str .= "<td>"._("Description")."</td>";
    $str .= "<td>"._("Type")."</td>";
    $str .= "<td>"._("FS type")."</td>";
    $str .= "<td>"._("Mount point")."</td>";
    $str .= "<td>"._("Size in MB")."</td>";
    $str .= "<td>"._("Mount options")."</td>";
    $str .= "<td>"._("FS option")."</td>";
    $str .= "<td>"._("Preserve")."</td>";
    $str .= "<td></td>";
    $str .= "</tr>";

    /* Walk through all defined partitions.
     * Create a new row for each partition and append it to 
     *  the header defined above.
     * To be able to check the posts later, we append a key to each single postfield. like cn_1 ... cn_2
     */
    foreach($this->partitions as $key => $part){

      /* Generate Partition select box  
       */
      $PartitionType = "<select name='FAIpartitionType_".$key."'>";
      foreach($PartitionTypes as $type){
        if($part['FAIpartitionType'] == $type){
          $PartitionType .="<option value='".$type."' selected >".$type."</option>";
        }else{
          $PartitionType .="<option value='".$type."'>".$type."</option>";
        }
      }        
      $PartitionType.="</select>";   

      
      /* Generate fsType select box  
       */
      $FAIfsType= "<select name='FAIfsType_".$key."'>";
      foreach($FAIfsTypes as $type){
        if($part['FAIfsType'] == $type){
          $FAIfsType  .="<option value='".$type."' selected >".$type."</option>";
        }else{
          $FAIfsType .="<option value='".$type."'>".$type."</option>";
        }
      }        
      $FAIfsType.="</select>";   
   
      $str .= "<tr>";
      $str .= "<td>".($key)."</td>";
      $str .= "<td><input name='cn_".$key."'                value='".$part['cn']."'               maxlength='8' style='width:80px;'></td>";
      $str .= "<td><input name='description_".$key."'       value='".$part['description']."'      maxlength='8' style='width:80px;'></td>";
      $str .= "<td>".$PartitionType."</td>";
      $str .= "<td>".$FAIfsType."</td>";
      $str .= "<td><input name='FAImountPoint_".$key."'     value='".$part['FAImountPoint']."'    ></td>";
      $str .= "<td><input name='FAIpartitionSize_".$key."'  value='".$part['FAIpartitionSize']."' style='width:200px;'></td>";
      $str .= "<td><input name='FAImountOptions_".$key."'   value='".$part['FAImountOptions']."'  style='width:100px;'></td>";
      $str .= "<td><input name='FAIfsOptions_".$key."'      value='".$part['FAIfsOptions']."'     style='width:100px;'></td>";
      if($part['FAIpartitionFlags']!=false){
        $str .= "<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve' checked></td>";
      }else{
        $str .= "<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve'></td>";
      }
      $str .= "<td><input type='submit' name='Delete_".$key."' value='"._("Remove")."'></td>";
      $str .= "</tr>";    
    }
    $str.="</table>";
    return($str);

  }

  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['TableEntryFrameSubmitted'])){
      plugin::save_object();

      /* Check base attributes */
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }
      }
     
      foreach($this->partitions as $key => $part){
        foreach($this->UsedAttrs as $attrs){
          if(isset($_POST[$attrs."_".$key])){
            $this->partitions[$key][$attrs] = $_POST[$attrs."_".$key];
          }else{
            $this->partitions[$key][$attrs] = false;
          }
        }
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
   
    /* Check descriptions 
     */ 
    if(empty($this->DISK_description)){
      $message[] = _("Please enter a description for your disk.");
    }
    if(preg_match("/[^0-9a-z]/",$this->DISK_description)){
      $message[] = _("Please enter a valid description for your disk. Only a-Z 0-9 are allowed.");
    }
  
    /* check every partition.
     * if there is an invalid value defined, append an errorstr to message
     */
    foreach($this->partitions as $key => $part){
      if((empty($part['cn']))||(preg_match("/[^0-9a-z]/",$part['cn']))){
        $message[]=sprintf(_("Please enter a valid name for partition %s." ),($key));
      }
    
      if(empty($part['description'])){
        $message[]=sprintf(_("Please enter a valid description for partition %s." ),($key));
      }

      if((empty($part['FAImountPoint']))||(!((preg_match("/^\/.*/",$part['FAImountPoint']))||(preg_match("/^swap$/",$part['FAImountPoint']))))){
        $message[]=sprintf(_("Please enter a valid mount point for partition %s." ),($key));
      }

      $tmp = split("-",$part['FAIpartitionSize']);
      if(count($tmp)!=2){
        $message[]=sprintf(_("Please enter a range for partition size, 'from-to', for partition %s."),($key));
      }elseif((!is_numeric($tmp[0]))||(!is_numeric($tmp[1]))){
        $message[]=sprintf(_("Please enter a valid range for partition %s."),($key)); 
      }elseif($tmp[0]>=$tmp[1]){
        $message[]=sprintf(_("Please enter a valid range for partition %s."),($key));
      }      
    }


    /* Only show 5 errors */
    $cnt = 0;
    if(count($message)>5){
      foreach($message as $msg){
        if($cnt < 5){
          $tmp2[]=$msg;
        }
        $cnt ++;
      }
    $tmp2[] = "...";
    $message = $tmp2;
    }

    return ($message);
  }

  
  function save()
  {
    /* This function returns the new or edited disk with its partitions.
     * If the disk was edited and not a new one, we reset the name of the disk
     *  to keep the correct index.
     */
  
    if($this->is_edit){
      $tmp['cn']          = $this->old_cn;
    }else{
      $tmp['cn']          = $this->DISK_cn;
    }
  
    foreach($this->partitions as $key=>$val) {
      $this->partitions[$key]['FAIpartitionNr']=$key;
    }
  
    $tmp['description'] = $this->DISK_description;
    $tmp['partitions']  = $this->partitions;

    return($tmp);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
