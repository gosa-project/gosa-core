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

  var $DISK_cn              = "";
  var $DISK_description     = "";
  
  var $partitions           = array();
  var $UsedAttrs            = array();
  var $is_edit              = false;
  var $old_cn               = "";
  var $status               = "new";
  var $deletePartitions     = array();

  var $FAIstate             = "";

  function faiPartitionTableEntry ($config, $dn= NULL,$usedDiskNames=array(),$disk=false)
  {
    plugin::plugin ($config, $dn);
    $this->UsedAttrs  = array("cn","description","FAIpartitionType","FAIpartitionNr","FAIfsType","FAImountPoint","FAIpartitionSize",
                         "FAImountOptions","FAIfsOptions","FAIpartitionFlags");

    /* Default status is new */
    $this->status = "new";    
 
    /* We want to edit an entry */
    if($disk){

      /* Set disk status */      
      $this->status = $disk['status'];

      /* Walk through partitions */
      foreach($disk['partitions'] as $name => $values){

        /* If a partition is already marked as delete, attach it to deletePartitions only. */
        if($values['status'] == "delete"){
          unset($disk['partitions'][$name]);
          $this->deletePartitions[]=$values;
        }else{

          /* Set status, to know which partition must be deleted from ldap
             new    : Neu partition entry // save
             edited : Update partition entry in ldap
             deleted: Remove partition from ldap
           */
    
          /* If status is not new, set to edit mode. 
           * New means that this partition currently wasn't saved to ldap.
           */
          if($disk['partitions'][$name]['status']!="new"){
            $disk['partitions'][$name]['status']="edited";
          }
     
          $disk['partitions'][$name]['old_cn']= $disk['partitions'][$name]['cn'];
 
          /* Assign empty attributes, if attribute is missing */
          foreach($this->UsedAttrs as $attr){
            if(!isset($values[$attr])){
              $disk['partitions'][$name][$attr]="";  
            }
          }
        }
      }

      /* Set default attributes */
      $this->DISK_cn          = $disk['cn'];
      $this->DISK_description = $disk['description'];
      $this->partitions       = $disk['partitions'];
      $this->is_edit          = true;
      $this->old_cn           = $disk['cn'];
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

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
      if($this->partitions[$s_entry]['status'] == "edited"){
        $this->deletePartitions[]= $this->partitions[$s_entry];
        unset($this->partitions[$s_entry]);
      }else{
        unset($this->partitions[$s_entry]);
      }
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
      $tmp["old_cn"]               = "";     
      $tmp['status']="new";
      $this->partitions[count($this->partitions)+1]=$tmp;
    }

    /* $setup contains a table with the partitions. 
     */
    $smarty->assign("setup", $this->generateParts());
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    foreach($this->UsedAttrs as $attr){
      if(($this->FAIstate == "freeze") || (chkacl($this->acl,$attr)!= "")){
        $smarty->assign($attr."ACL"," disabled ");
      }else{
        $smarty->assign($attr."ACL","  ");
      }
    }


    /* The user can't change a diskname 
     *  if we are currently in edit mode for the specified disk 
     */
//    if($this->is_edit){
//      $smarty->assign("DISK_cnACL"," disabled ");
//    }else{
      $smarty->assign("DISK_cnACL","");
//    }

  

    /* Fetch template and show the result
     */
    $display.= $smarty->fetch(get_template_path('faiPartitionTableEntry.tpl', TRUE));
    return($display);
  }

  function generateParts()
  {
    /* Define Arrays with allowed syntax */
    $PartitionTypes   = array("primary"=>_("primary"),"logical"=>_("logical"));
    $FAIfsTypes       = array("ext2","ext3","xfs","swap","reiserfs","dosfat16","winfat32");

    /* Display Header */
    $str = "<table summary='' style='width:100%'>";
    if (count($this->partitions)){
      $str .= "<tr>";
      $str .= "<td>"._("Type")."</td>";
      $str .= "<td>"._("FS type")."</td>";
      $str .= "<td>"._("Mount point")."</td>";
      $str .= "<td>"._("Size in MB")."</td>";
      $str .= "<td>"._("Mount options")."</td>";
      $str .= "<td>"._("FS option")."</td>";
      $str .= "<td>"._("Preserve")."</td>";
      $str .= "<td>&nbsp;</td>";
      $str .= "</tr>";
    }
    
    /* Walk through all defined partitions.
     * Create a new row for each partition and append it to 
     *  the header defined above.
     * To be able to check the posts later, we append a key to each single postfield. like cn_1 ... cn_2
     */
    foreach($this->partitions as $key => $part){
      
      $dis = "";
      if($part['FAIpartitionFlags'] == "preserve"){
        $dis = " disabled ";
      }

      $disableALL = "";
      if($this->FAIstate =="freeze"){
        $disableALL = " disabled ";
      }

      if($part['status']!="delete"){
        /* Generate Partition select box  
         */
        $PartitionType = "<select name='FAIpartitionType_".$key."' id='FAIpartitionType_".$key."' ".$disableALL.">";
        foreach($PartitionTypes as $type => $PTname){
          if($part['FAIpartitionType'] == $type){
            $PartitionType .="<option value='".$type."' selected >".$PTname."</option>";
          }else{
            $PartitionType .="<option value='".$type."'>".$PTname."</option>";
          }
        }        
        $PartitionType.="</select>";   


        /* Generate fsType select box  
         */
        $FAIfsType= "<select name='FAIfsType_".$key."' id='FAIfsType_".$key."' ".$dis." ".$disableALL.">";
        foreach($FAIfsTypes as $type){
          if($part['FAIfsType'] == $type){
            $FAIfsType  .="<option value='".$type."' selected >".$type."</option>";
          }else{
            $FAIfsType .="<option value='".$type."'>".$type."</option>";
          }
        }        
        $FAIfsType.="</select>";   

        $str .= "\n<tr>";
        $str .= "\n<td>".$PartitionType."</td>";
        $str .= "\n<td>".$FAIfsType."</td>";
        $str .= "\n<td><input name='FAImountPoint_".$key."'  ".$disableALL."  ".$dis."  value='".$part['FAImountPoint']."'    id='FAImountPoint_".$key."'></td>";
        $str .= "\n<td><input name='FAIpartitionSize_".$key."' ".$disableALL." ".$dis." value='".$part['FAIpartitionSize']."' id='FAIpartitionSize_".$key."' size=12></td>";
        $str .= "\n<td><input name='FAImountOptions_".$key."' ".$disableALL."  ".$dis."  value='".$part['FAImountOptions']."'  id='FAImountOptions_".$key."' style='width:100px;'></td>";
        $str .= "\n<td><input name='FAIfsOptions_".$key."'   ".$disableALL."  ".$dis."  value='".$part['FAIfsOptions']."'     id='FAIfsOptions_".$key."' style='width:100px;'></td>";

        $changeState = "onClick=\"changeState('FAImountPoint_".$key."') ; ".
                                 "changeState('FAIpartitionSize_".$key."') ; ".
                                 "changeState('FAImountOptions_".$key."') ; ".
                                 "changeState('FAIfsType_".$key."') ; ".
                                 "changeState('FAIfsOptions_".$key."') ; \"";
  

        if($part['FAIpartitionFlags']!=false){
          $str .= "\n<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve' checked ".$changeState." ".$disableALL."></td>";
        }else{
          $str .= "\n<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve' ".$changeState." ".$disableALL."></td>";
        }
        $str .= "\n<td><input type='submit' name='Delete_".$key."' value='"._("Remove")."' ".$disableALL."></td>";
        $str .= "\n</tr>";    
      }
    }
    $str.="</table>";
    return($str);

  }

  function save()
  {
    $tmp = array();
    $tmp['cn']          = $this->DISK_cn;

    /* Attach partitions */
    foreach($this->partitions as $key=>$val) {
      $this->partitions[$key]['FAIpartitionNr']=$key;
    }

    /* Attach deleted */
    foreach($this->deletePartitions as $key=>$val) {
      $this->partitions[$key."-delete"]=$val;
      $this->partitions[$key."-delete"]['status']="delete";
    }

    $tmp['description'] = $this->DISK_description;
    $tmp['partitions']  = $this->partitions;
    $tmp['status']      = $this->status;

    /* If hdd name has changed, tell partitionTable to rename it */
    if(($this->is_edit)&&($this->old_cn != $this->DISK_cn)){
      $tmp['rename']['from']  = $this->old_cn;
      $tmp['rename']['to']    = $this->DISK_cn;
    }
    return($tmp);
  }


  /* Save data to object */
  function save_object()
  {
    if((isset($_POST['TableEntryFrameSubmitted'])) && ($this->FAIstate != "freeze") ){
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
    /* Call common method to give check the hook */
    $message= plugin::check();
   
    /* check every partition.
     * if there is an invalid value defined, append an errorstr to message
     */

    /* Array that contain every partitionname mountpoint etc already assigned */
    $alreadyUsed    = array();
    foreach($this->UsedAttrs as $attrs){
      $alreadyUsed[$attrs] = array();
    }      

    foreach($this->partitions as $key => $part){
  
      /* Skip all checks, if preserve is set */ 
      if($part['FAIpartitionFlags'] == "preserve"){
        $this->partitions[$key]['FAIfsType']        = "preserve";
        $this->partitions[$key]['FAIpartitionSize'] = "preserve";
        continue;
      }
 
      if((in_array($part['FAImountPoint'],$alreadyUsed['FAImountPoint']))&&($part['FAIfsType']!="swap")){
        $message[]=sprintf(_("please enter a unique mount point for partition %s"),($key));
      }

      if($part['FAIfsType']!="swap"){
        if((empty($part['FAImountPoint']))||(!((preg_match("/^\/.*/",$part['FAImountPoint']))||(preg_match("/^swap$/",$part['FAImountPoint']))))){
          $message[]=sprintf(_("Please enter a valid mount point for partition %s." ),($key));
        }
      }
      if($part['FAIfsType'] == "swap"){
        if(in_array($part['FAIfsType'],$alreadyUsed['FAIfsType'])){
          $message[]=sprintf(_("File system type 'swap' is already used, change file system type for partition %s."),$key);
        }
      }
      if(($part['FAIfsType'] == "swap")&&(!empty($part['FAImountPoint']))&&($part['FAImountPoint']!="swap")){
        $message[]=_("Please use 'swap' as mount point, if 'swap' is used as fs-type.");
      }

      $tmp = split("-",$part['FAIpartitionSize']);
      switch (count($tmp)){
        case 0:
                $message[]= sprintf(_("Please enter a valid partition size for partition %s."),($key));
                break;
        case 1:
                if (!is_id(is_id($tmp[0]))){
                  $message[]= sprintf(_("Please enter a valid partition size for partition %s."),($key));
                }
                break;
                
        case 2:
                if((!is_id($tmp[0]))||(!is_id($tmp[1]))){
                  $message[]=sprintf(_("Please enter a valid range for partition %s."),($key)); 
                }elseif($tmp[0]>=$tmp[1]){
                  $message[]=sprintf(_("Please enter a valid range for partition %s."),($key));
                }
                break;

        default:
                $message[]=sprintf(_("Please enter a range for partition size for partition %s."),($key));
      }

      foreach($this->UsedAttrs as $attrs){
        $alreadyUsed[$attrs][$key] = $part[$attrs];
      }      
    }

    $cnt = 0;
    foreach($this->partitions as $key => $part){
      if($part['FAIpartitionType'] == "primary"){
        $cnt ++ ; 
      }
    }
    if($cnt > 3){
      $message[] = _("You have more than 3 primary partition table entries in your configuration, please check your configuration twice.");
    }

    return ($message);
  }
 
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
