<?php

class faiPartitionTableEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("DISKcn","DISKdescription");
  var $objectclasses= array();

  var $DISKcn              = "";
  var $DISKdescription     = "";
  
  var $partitions           = array();
  var $UsedAttrs            = array();
  var $is_edit              = false;
  var $old_cn               = "";
  var $status               = "new";
  var $deletePartitions     = array();

  var $FAIstate             = "";

  function faiPartitionTableEntry (&$config, $dn= NULL,$usedDiskNames=array(),$disk=false)
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

          if (preg_match('/^_/', $disk['partitions'][$name]['FAIfsType'])){
            $disk['partitions'][$name]['FAIfsType']= preg_replace('/^_/', '', $disk['partitions'][$name]['FAIfsType']);
            $disk['partitions'][$name]['FAIencrypted']= true;
          } else {
            $disk['partitions'][$name]['FAIencrypted']= false;
          }
        }
      }

      /* Set default attributes */
      $this->DISKcn          = $disk['cn'];
      $this->DISKdescription = $disk['description'];
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
      if((preg_match("/Delete_.*/",$name)) && $this->acl_is_removeable()){
        $tmp = split("_",$name);
        $s_action = "remove";
        $s_entry  = $tmp[1]; 
      }
    }

    /* To remove a partition we unset the index posted.
     * We must sort the index again, else we possibly got problems 
     * with partitions order.
     */
    if($s_action == "remove" && $this->acl_is_removeable()){
      if($this->partitions[$s_entry]['status'] == "edited"){
        $this->deletePartitions[$s_entry]= $this->partitions[$s_entry];
        $this->deletePartitions[$s_entry]['FAIpartitionNr']=$s_entry;
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
    if(isset($_POST['AddPartition']) && $this->acl_is_createable()){
      foreach($this->UsedAttrs as $attr){
        $tmp[$attr]                = "";     
      }
      $tmp["old_cn"]               = "";     
      $tmp["FAIencrypted"] = FALSE;     
      $tmp['status']="new";
      $this->partitions[count($this->partitions)+1]=$tmp;
    }

    /* $setup contains a table with the partitions. 
     */
    $smarty->assign("setup", $this->generateParts());
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    $tmp = $this->plInfo();
    $sacl = "";
    foreach($tmp['plProvidedAcls'] as $name => $translated){

      $acl = $this->getacl($name);
      $sacl .= $acl;
      if($this->FAIstate == "freezed"){
        $acl = preg_replace("/w/","",$acl);
      }
      $smarty->assign($name."ACL",$acl);
    }
    $smarty->assign("allowSave",$sacl);
   
    $smarty->assign("sub_object_is_createable",$this->acl_is_createable());
 
    /* Fetch template and show the result
     */
    $display.= $smarty->fetch(get_template_path('faiPartitionTableEntry.tpl', TRUE));
    return($display);
  }

  function generateParts()
  {
    /* Define Arrays with allowed syntax */
    $PartitionTypes   = array("primary"=>_("primary"),"logical"=>_("logical"));
    $FAIfsTypes       = array("ext2","ext3","xfs","swap","reiser","dosfat16","winfat32");

    /* Display Header */
    $str = "";
    if (count($this->partitions)){
      $str .= "<tr>";
      $str .= "<td><b>"._("Type")."</b></td>";
      $str .= "<td><b>"._("FS type")."</b></td>";
      $str .= "<td><b>"._("Mount point")."</b></td>";
      $str .= "<td><b>"._("Size in MB")."</b></td>";
      $str .= "<td><b>"._("Mount options")."</b></td>";
      $str .= "<td><b>"._("FS option")."</b></td>";
      $str .= "<td><b>"._("Preserve")."</b></td>";
      $str .= "<td><b>"._("Encrypted")."</b></td>";
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
        $PartitionType = "";
        if($this->acl_is_writeable("FAIpartitionType")){
          $PartitionType = "<select name='FAIpartitionType_".$key."' id='FAIpartitionType_".$key."' ".$disableALL.">";
          foreach($PartitionTypes as $type => $PTname){
            if($part['FAIpartitionType'] == $type){
              $PartitionType .="<option value='".$type."' selected >".$PTname."</option>";
            }else{
              $PartitionType .="<option value='".$type."'>".$PTname."</option>";
            }
          }        
          $PartitionType.="</select>";   
        }elseif($this->acl_is_readable("FAIpartitionType")){
          $PartitionType = $part['FAIpartitionType'];
        }


        /* Generate fsType select box  
         */
        $FAIfsType= "";
        if($this->acl_is_writeable("FAIfsType")){
          $FAIfsType= "<select name='FAIfsType_".$key."' id='FAIfsType_".$key."' ".$dis." ".$disableALL.">";
          foreach($FAIfsTypes as $type){
            if($part['FAIfsType'] == $type){
              $FAIfsType  .="<option value='".$type."' selected >".$type."</option>";
            }else{
              $FAIfsType .="<option value='".$type."'>".$type."</option>";
            }
          }        
          $FAIfsType.="</select>";   
        }elseif($this->acl_is_readable("FAIfsType")){
          $FAIfsType = $part['FAIfsType'];
        }

        foreach(array("FAImountPoint","FAIpartitionSize","FAImountOptions","FAIfsOptions") as $attr){
          $$attr  = "";
          if($this->acl_is_writeable($attr)){
            $$attr = "<input name='".$attr."_".$key."'  ".$disableALL."  ".$dis."
                        value='".$part[$attr]."'    id='".$attr."_".$key."'>";

          }elseif($this->acl_is_readable($attr)){
            $$attr = $part[$attr];
          }
        }


        $str .= "\n<tr>";
        $str .= "\n<td>".$PartitionType."</td>";
        $str .= "\n<td>".$FAIfsType."</td>";
        $str .= "\n<td>".$FAImountPoint."</td>";
        $str .= "\n<td>".$FAIpartitionSize."</td>";
        $str .= "\n<td>".$FAImountOptions."</td>";
        $str .= "\n<td>".$FAIfsOptions."</td>";

        $changeState = "onClick=\"changeState('FAImountPoint_".$key."') ; ".
                                 "changeState('FAIpartitionSize_".$key."') ; ".
                                 "changeState('FAImountOptions_".$key."') ; ".
                                 "changeState('FAIfsType_".$key."') ; ".
                                 "changeState('FAIfsOptions_".$key."') ; \"";
  
        if($this->acl_is_writeable("FAIpartitionFlags")){
          if($part['FAIpartitionFlags']!=false){
            $str .= "\n<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve' checked ".$changeState." ".$disableALL."></td>";
          }else{
            $str .= "\n<td><input type='checkbox' name='FAIpartitionFlags_".$key."' value='preserve' ".$changeState." ".$disableALL."></td>";
          }
          if($part['FAIencrypted']!=false){
            $str .= "\n<td><input type='checkbox' name='FAIencrypted_".$key."' value='encrypted' checked></td>";
          }else{
            $str .= "\n<td><input type='checkbox' name='FAIencrypted_".$key."' value='encrypted'></td>";
          }
        }else{
          if($part['FAIpartitionFlags']!=false){
            $str .= "<td>"._("False")."</td>";
          }else{
            $str .= "<td>"._("True")."</td>";
          }
          if($part['FAIencrypted']!=false){
            $str .= "<td>"._("False")."</td>";
          }else{
            $str .= "<td>"._("True")."</td>";
          }
        }
        if($this->acl_is_removeable()){
          $str .= "\n<td><input type='submit' name='Delete_".$key."' value='"._("Remove")."' ".$disableALL."></td>";    
        }else{
          $str .= "\n<td><input type='submit' name='restricted' disabled value='"._("Remove")."'></td>";    
        }
        $str .= "\n</tr>";    
      }
    }
    
    if(!empty($str)){
      $str = "<table summary='' style='width:100%'>".$str."</table>";
    }
    return($str);

  }

  function save()
  {
    $tmp = array();
    $tmp['cn']          = $this->DISKcn;

    /* Attach partitions */
    foreach($this->partitions as $key=>$val) {
      if (isset($this->partitions[$key]['FAIencrypted']) && $this->partitions[$key]['FAIencrypted']){
        unset ($this->partitions[$key]['FAIencrypted']);
        $this->partitions[$key]['FAIfsType']= "_".$this->partitions[$key]['FAIfsType'];
      }
      $this->partitions[$key]['FAIpartitionNr']=$key;
    }

    /* Attach deleted */
    foreach($this->deletePartitions as $key=>$val) {
      $this->partitions[$key."-delete"]=$val;
      $this->partitions[$key."-delete"]['status']="delete";
    }

    $tmp['description'] = $this->DISKdescription;
    $tmp['partitions']  = $this->partitions;
    $tmp['status']      = $this->status;

    /* If hdd name has changed, tell partitionTable to rename it */
    if(($this->is_edit)&&($this->old_cn != $this->DISKcn)){
      $tmp['rename']['from']  = $this->old_cn;
      $tmp['rename']['to']    = $this->DISKcn;
    }
    return($tmp);
  }


  /* Save data to object */
  function save_object()
  {


    if((isset($_POST['TableEntryFrameSubmitted'])) && ($this->FAIstate != "freeze") ){
      plugin::save_object();

      foreach($this->partitions as $key => $part){
        foreach($this->UsedAttrs as $attrs){

          if($this->acl_is_writeable($attrs)){

            if(isset($_POST[$attrs."_".$key])){
              $this->partitions[$key][$attrs] = $_POST[$attrs."_".$key];
            }else{
              $this->partitions[$key][$attrs] = false;
            }
          }
        }

        if($this->acl_is_writeable("FAIencrypted")){

          if(isset($_POST["FAIencrypted_".$key])){
            $this->partitions[$key]["FAIencrypted"] = $_POST["FAIencrypted_".$key];
          }else{
            $this->partitions[$key]["FAIencrypted"] = false;
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
  
    /* Check for an empty disk name */
    if(empty($this->DISKcn)){
      $message[] = _("Please specify a valid disc name.");
    }
       
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

      if($part['FAIfsType']!="swap" && $part['FAImountPoint'] != "-"){
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
                if (!tests::is_id(is_id($tmp[0])) &&(!empty($tmp[1]))){
                  $message[]= sprintf(_("Please enter a valid partition size for partition %s."),($key));
                }
                break;
                
        case 2:
                if((!tests::is_id($tmp[0]))&&(!tests::is_id($tmp[1])) &&(!empty($tmp[1]))){
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

  

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName" => _("Partition table entry"),
          "plDescription" => _("FAI partition table entry"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 27,
          "plSection"     => array("administration"),
          "plCategory"    => array("fai"),
          "plProvidedAcls" => array(
            "DISKcn"           => _("Name"),
            "DISKdescription"  => _("Description"),
            "FAIpartitionType"  => _("Partition type"),
            "FAIpartitionNr"    => _("Partition no."),
            "FAIfsType"         => _("File system type"),
            "FAImountPoint"     => _("Mount point"),
            "FAIpartitionSize"  => _("Partition size"),
            "FAImountOptions"   => _("Mount options"),
            "FAIfsOptions"      => _("File system options"),
            "FAIencrypted"      => _("File system encryption"),
            "FAIpartitionFlags" => _("Partition flags"))
          ));
  }

 
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
