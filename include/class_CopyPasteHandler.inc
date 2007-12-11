<?php

class CopyPasteHandler {

  var $config;
  var $current;

  var $copyCurrent = false;
  var $cutCurrent  = false;
  var $dialogOpen  = false;
  var $objectdn    = false;

  var $lastdn      = "";

  var $was_successfull = false;

  /* Create CP handler  */
  function CopyPasteHandler($config){
    $this->config = $config;	
    $this->current= NULL;
  }


  /* Returns wether the current object can be pasted */	
  function isCurrentObjectPastAble(){

    /* Check if we got a valid object */
    if($this->current == NULL){
      return(false);
    }
    return(true);
  }	


  /* Clears all copy & paste informations */
  function Clear()
  {
    $this->copyCurrent 	= false;
    $this->cutCurrent  	= false;
    $this->dialogOpen   = false;
    $this->current 	   	= NULL;
  }


  /* return current obejct dn */
  function GetCurrentDn()
  {
    return($this->objectdn);
  }


  /* Add Object which should be copied  */
  function Copy($obj,$emptyObj)
  {
    $this->copyCurrent = true;
    $this->objectdn    = $obj->dn;
    $this->current = $emptyObj;
    foreach($obj->by_object as $name => $obj){
    
      /* Prepare every single class, to be copied  */
      $this->current->by_object[$name]->PrepareForCopyPaste($obj);
 
      /* handle some special vars */ 
      foreach(array("is_account") as $attr){
        if(isset($obj->$attr)){
          $this->current->by_object[$name]->$attr = $obj->$attr;
        }
      }
    }
    if($this->isCurrentObjectPastAble()){
      return(true);
    }else{
      $this->cutCurrent = $this->copyCurrent = false;
      $this->obj = NULL;
    }
    return(false);
  }


  /* Add Object which should be cutted  */
  function Cut($obj){
    $this->cutCurrent = true;
    $this->current 	  = $obj;
    $this->objectdn    = $obj->dn;
    if($this->isCurrentObjectPastAble()){
      return(true);
    }else{
      $this->cutCurrent = $this->copyCurrent = false;
      $this->obj = NULL;
    }
    return(false);
  }


  /* Returns true if current object 
   *  is cutted. And awaits to be pasted anywhere  */
  function isCurrentCutted(){
    return($this->cutCurrent);
  }


  /* Returns true if current object  
   *  was copied, and awaits to be pasted again  */
  function isCurrentCopied(){
    return($this->copyCurrent);
  }


  /* Returns true if the copy$paste dialog is still open  */
  function stillOpen(){	
    if(isset($_POST['AbortCopyPaste'])){
      $this->dialogOpen = false;
    }
    return($this->dialogOpen);
  }


  /* Displays a dialog which allows the user to fix all dependencies of this object.
     Create unique names, ids, or what ever */
  function execute()
  {
    $this->dialogOpen = true;

    /* Cut & paste
     */
    if($this->cutCurrent){

      $msgs = $this->check();	
      if(count ($msgs) ){
        foreach( $msgs as $msg){
          print_red($msg);
        }
        $this->dialogOpen =false;
      }else{
        $this->current->save();
        $this->dialogOpen =false;
        $this->Clear();
      }
      /* Copy & paste
       */
    }else{
      if(isset($_POST['PerformCopyPaste'])){
        $msgs = $this->check();	
        if(count ($msgs) ){
          foreach( $msgs as $msg){
            print_red($msg);
          }
        }else{
          $this->current->save();
          $this->lastdn = $this->current->dn;
          $this->dialogOpen =false;
          $this->Clear();
        }
      }
      if($this->current){
        $smarty = get_smarty();	
        $smarty->assign("Complete",false);
        $smarty->assign("AttributesToFix",$this->generateAttributesToFix());	
        $smarty->assign("SubDialog",$this->current->SubDialog);
        $smarty->assign("objectDN"		 ,$this->objectdn);	
        $smarty->assign("message", sprintf(_("You are going to copy the entry '%s'."), $this->objectdn));	
        return($smarty->fetch(get_template_path("copyPasteDialog.tpl",FALSE)));
      }
    }
  }

  /* Create dialog which asks unique attributes/values ... 
   *  call tabs -> getCopyDialog() 
   *    which calls tab -> getCopyDialog()  */
  function generateAttributesToFix()
  {
    if($this->current){
      return($this->current->getCopyDialog());  
    }
  }


  /* Set a single attribute to specified value
   *  example :   ("base", $newBase );    */
  function SetVar($name,$value)
  {
    foreach($this->	current->by_object as $key => $obj){
      if(isset($this->current->by_object[$key]->$name)){
        $this->current->by_object[$key]->$name = $value;
      }
    }
  }


  /* Save new values posted by copy & paste dialog */
  function save_object()
  {
    /* Assign posted var to all tabs
     */
    if($this->current){
      $this->current->saveCopyDialog();
    }
  }


  /* Returns errors from including tabs. */
  function check()
  {
    $ret = array();
    foreach($this->	current->by_object as $obj){
      if($obj->is_account){
        $ret = array_merge($ret , $obj->check());
      }
    }
    return($ret);
  }

  
  /* returns the paste icon for headpages */ 
  function generatePasteIcon()
  {
    $Copy_Paste= "&nbsp;<img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    if($this->isCurrentObjectPastAble()){
      if($this->isCurrentCutted()){
        $img= "images/cutpaste.png";
      }else{
        $img= "images/copypaste.png";
      }
      $Copy_Paste.= "<input type='image' name='editPaste' class='center'
        src='".$img."' alt='"._("Paste")."' title='".$this->GetCurrentDn()."'>&nbsp;";
    }else{
      $Copy_Paste.= "<img class='center' src='images/cant_editpaste.png' alt=\""._("Can't paste")."\" >&nbsp;";
    }

    return ($Copy_Paste);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
