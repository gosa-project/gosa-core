<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007  Fabian Hickert

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*! 
  \brief   This class represents the host action queue.
  \author  Fabian Hickert <hickert@gonicus.de>
  \version 2.6
  \date    24.10.2007

  This class is a queue handler, which allows adding,removing,
   priority settings,stop and resume actions... for each queue entry
 */
class hostActionQueue {

  private $o_config       = NULL;
  private $s_queue_file   = ""; 
  private $s_error        = "";
  private $b_error        = FALSE;

  private $a_queue        = array();
  private $i_pointer      = 0;
  private $b_queue_loaded = false;
  private $i_fileversion  = 0;

  private $id_entry_map   = array();

  public function __construct($config)
  {
    $this->o_config= $config;

    /* Define source file */
    $this->s_queue_file = CONFIG_DIR."/gotomasses_machines";

    $file = $this->o_config->search("gotomasses", "STORAGE_FILE",array('menu'));
    if(!empty($file)){
      $this->s_queue_file = $file;
    }
  }

  private function _reload_queue()
  {
    $this->_reset_error();
    $this->b_queue_loaded = FALSE;
    $this->b_error        = FALSE;
    $this->s_error        = "";
    $this->i_pointer      = 0;

    /* Check file accessibility */
    if(!file_exists($this->s_queue_file)){
      $this->set_error(sprintf(_("Can't locate gotomasses queue file '%s'."),$this->s_queue_file));
      return(FALSE);
    }
    if(!is_readable($this->s_queue_file)){
      $this->set_error(sprintf(_("Can't read gotomasses queue file '%s'."),$this->s_queue_file));
      return(FALSE);
    }

    /* Check if file contents could be read */
    $fp = @fopen($this->s_queue_file,"r");
    if(!$fp){
      $this->set_error(sprintf(_("Can't read gotomasses storage file '%s'."),$this->s_queue_file));
      return(FALSE);
    }
    $this->i_fileversion = filemtime($this->s_queue_file);
    clearstatcache();
 
    /* Get file contents */
    $data ="";
    while(!feof($fp)){
      $data.= fread($fp,512);
    }
    
    /* Get lines from file */
    $this->a_queue  = array();
    $comment        = "";
    $rows           = split("\n",$data);
 
    $used_ids = array();
 
    /* Walk trough rows and parse data */
    foreach($rows as $row){

      /* Skip empty lines */
      $row = trim($row);
      if(empty($row)){
        continue;
      }

      /* Get comment, if available */
      if(preg_match("/^#/",$row)){
        $comment = preg_replace("/^#/","",$row);
        continue;
      }

      $entry_id = $task_id= "";    
  
      /* Comment must be set correctly */
      if(empty($comment)){
        $desc     = "";
      }else{
        $task_id =preg_replace("/^.*taskid:([0-9a-z]*).*$/i","\\1",$comment);
        $entry_id=preg_replace("/^.*entryid:([0-9a-z]*).*$/i","\\1",$comment);
        $desc    =preg_replace("/^.*desc:(.*)$/","\\1",$comment);
      }
      if($task_id == "" || empty($task_id)){ 
        $task_id  = $this->get_new_id();
      }
      if($entry_id == "" || empty($entry_id)){
        $entry_id = $this->get_new_id();
      }
   
      /* Get unige id */ 
      while(in_array($entry_id,$used_ids)){
        $entry_id = $this->get_new_id();
      }
      $used_ids[] = $entry_id;
 
      /* Split row into minutes/ hours ...*/
      $row    = preg_replace('/[\t ]/umi'," ",$row);
      $row    = preg_replace('/  */umi'," ",$row);
      $parts  = split(" ",$row);

      if(count($parts) != 10){
        print_red(_("Entry broken, skipped."));
      }else{

        $entry = array();
        $entry['TASK_ID'] = $task_id;
        $entry['ID']      = $entry_id;
        $entry['Minute']  = $parts[0];
        $entry['Hour']    = $parts[1];
        $entry['Day']     = $parts[2];
        $entry['Month']   = $parts[3];
        $entry['Weekday'] = $parts[4];
        $entry['Action']  = $parts[5];
        $entry['OGroup']  = $parts[6];
        $entry['Zone']    = $parts[7];
        $entry['Section'] = $parts[8];
        if($entry['Action'] == "initial_install"){
          $tmp2 = split(";",$parts[9]);
          foreach($tmp2 as $target){
            $tmp = split(",",$target);
            $entry['Initial_Target'][]  = array(
                          "MAC"     => $tmp[0],
                          "IP"      => $tmp[1],
                          "NAME"    => $tmp[2]);
          }
          $entry['Target']  = array();
        }else{
          $entry['Initial_Target']  = array();
          $entry['Target']  = split(";",$parts[9]);
        }
        $entry['Comment'] = $desc;
        $this->a_queue[]   = $entry;
      }
      $comment = "";
    }
   
    /* Udpate ENTRY_ID -> id mapping */ 
    reset($this->a_queue);
    foreach($this->a_queue as $id => $entry){
      $this->id_entry_map[$entry['ID']] = $id;
    }   
 
    return(TRUE);
  }


  private function _save_data()
  {
    $this->_reset_error();

    /* Check file accessibility */
    if(!file_exists($this->s_queue_file)){
      $this->set_error(sprintf(_("Can't locate gotomasses queue file '%s'."),$this->s_queue_file));
      return(FALSE);
    }
    if(!is_writeable($this->s_queue_file)){
      $this->set_error(sprintf(_("Can't write gotomasses queue file '%s'."),$this->s_queue_file));
      return(FALSE);
    }
    if($this->i_fileversion != filemtime($this->s_queue_file)){
      $this->set_error(_("The queue file was modified since last reload. Can't save changes."));
      return(FALSE);
    }
    $fp = @fopen($this->s_queue_file,"w");
    if(!$fp){
      $this->set_error(sprintf(_("Can't write gotomasses queue file '%s'."),$this->s_queue_file));
      return(FALSE);
    }

    $str = "#GOsa generated file, please just modify if you really know what you do.";
    reset($this->a_queue);
    foreach($this->a_queue as $task){
      $str .= "\n#taskid:".trim($task['TASK_ID']).";entryid:".$task['ID'].";desc:".trim($task['Comment']);
      $str .= "\n";
      if($task['Action'] == "initial_install"){
        $str .= "*     *     *     *     *     ";
      }else{
        $str .= str_pad($task['Minute'] ,5," ")." ";
        $str .= str_pad($task['Hour']   ,5," ")." ";
        $str .= str_pad($task['Day']    ,5," ")." ";
        $str .= str_pad($task['Month']  ,5," ")." ";
        $str .= str_pad($task['Weekday'],5," ")." ";
      }
      $str .= str_pad($task['Action'] ,5," ")." ";
      $str .= str_pad($task['OGroup'] ,5," ")." ";
      $str .= str_pad($task['Zone']   ,5," ")." ";
      $str .= str_pad($task['Section'],5," ")." ";
      if($task['Action'] == "initial_install"){
        foreach($task['Initial_Target'] as $target){
          $str .= trim($target['MAC']).",".trim($target['IP']).",".trim($target['NAME']).";";
        }
      }else{
        foreach($task['Target'] as $target){
          $str .= $target.";";
        }
      }
      $str = preg_replace("/;$/","",$str);
    }

    /* Write contents */
    $str .= "\n";
    fwrite($fp,$str);
    fclose($fp);
    clearstatcache();
    $this->i_fileversion = filemtime($this->s_queue_file);
    clearstatcache();

    /* Update ENTRY_ID -> id mapping */ 
    reset($this->a_queue);
    foreach($this->a_queue as $id => $entry){
      $this->id_entry_map[$entry['ID']] = $id;
    }   
 
    return(TRUE);
  }

  private function _add_entry($entry,$task_id = "")
  {
    if($task_id == "" || empty($task_id)){
      $task_id = $this->get_new_id();
    }
    $entry['TASK_ID'] = $task_id;
    if(!isset($entry['ID']) || empty($entry['ID']) || !isset($entry['ID']) || empty($entry['ID'])){
      $entry['ID'] = $this->get_new_id();
    }
    $this->a_queue[] = $entry;
    
    return(true);
  }

  public function get_entry($id)
  {
    if(isset($this->a_queue[$this->id_entry_map[$id]])){
      return($this->a_queue[$this->id_entry_map[$id]]);
    }else{
      $this->set_error(sprintf(_("Entry with id '%s' not found."),$id));
    }
  }

  public function update_entry($id,$entry)
  {
    if(isset($this->id_entry_map[$id])){
      $this->a_queue[$this->id_entry_map[$id]] = $entry;
      return($this->_save_data());
    }else{
      $this->set_error(sprintf(_("Could not update entry, entry with id '%s' not found."),$id));
    }
  }

  public function remove_entry($id)
  {
    if(isset($this->id_entry_map[$id])){
      unset($this->a_queue[$this->id_entry_map[$id]]);
      unset($this->id_entry_map[$id]);
      return($this->_save_data());
    }else{
      $this->set_error(sprintf(_("Could not remove entry, entry with id '%s' not found."),$id));
    }
  }

  public function id_exists($id)
  {
    return(isset($this->id_entry_map[$id]));
  }


  public function add($entry)
  {
    if(!$this->_add_entry($entry)){
      return(FALSE);
    }
    return($this->_save_data());
  }

  public function add_multiple($array)
  {
    $task_id = $this->get_new_id();
    foreach($array as $entry){
      if(!$this->_add_entry($entry,$task_id)){
        return(FALSE);
      }
    }
    return($this->_save_data());
  }

  public function fetch()
  {
    if(isset($this->a_queue[$this->i_pointer])){
      $p = $this->a_queue[$this->i_pointer];
      $this->i_pointer ++;
      return($p);
    }   
    return(FALSE);    
  }

  public function load()
  {
    return($this->_reload_queue());    
  }

  public function save()
  {
    return($this->_save_data());    
  }

  private function _reset_error()
  {
    $this->b_error = FALSE;
    $this->s_error = "";
  }
 
  private function set_error($str)
  {
    $this->b_error = TRUE;
    $this->s_error = $str;
  }

  public function is_error()
  {
    return($this->b_error);
  }

  public function get_error()
  {
    return($this->s_error);
  }

  public function max_entry_priority($id)
  {
    if(!$this->id_exists($id)){
      $this->set_error(sprintf(_("Can't set priority for ID '%s'. ID does not exist."),$id));
      return(FALSE);
    }
    $tmp = array();
    $tmp[]= $this->get_entry($id);
    reset($this->a_queue);
    foreach($this->a_queue as $key => $entry){
      if($id != $entry['ID']){
        $tmp[] = $entry;
      }
    }
    $this->a_queue=$tmp;
    return($this->_save_data());
  }

  public function increase_entry_priority($id)
  {
    if(!$this->id_exists($id)){
      $this->set_error(sprintf(_("Can't set priority for ID '%s'. ID does not exist."),$id));
      return(FALSE);
    }
    $tmp = array();
    $skip = NULL;
    $next = NULL;
    reset($this->a_queue);
    foreach($this->a_queue as $key => $entry){
      if($next != NULL){
        if($id == $entry['ID']){
          $tmp[] = $entry; 
          $tmp[] = $next;
          $next = NULL;
        }else{
          $tmp[] = $next;
          $next = $entry;
        }
      }else{
        $next = $entry;
      }
    }
    if($next != NULL){
      $tmp[] = $next;
    }
    $this->a_queue=$tmp;
    return($this->_save_data());
  }

  public function decrease_entry_priority($id)
  {
    if(!$this->id_exists($id)){
      $this->set_error(sprintf(_("Can't set priority for ID '%s'. ID does not exist."),$id));
      return(FALSE);
    }
    $tmp = array();
    $skip = NULL;
    reset($this->a_queue);
    foreach($this->a_queue as $key => $entry){
      if($id != $entry['ID']){
        $tmp[] = $entry;
        if($skip != NULL){
          $tmp[] = $skip;
          $skip = NULL;
        }
      }else{
        $skip = $entry;
      }
    }
    if($skip != NULL){
      $tmp[] = $skip;
    }
    $this->a_queue=$tmp;
    return($this->_save_data());
  }

  private function get_new_id()
  {
    $mt = microtime();
    $sec  = preg_replace("/^0.([0-9]*) ([0-9]*)$/","\\2",$mt);
    $msec = preg_replace("/^0.([0-9]*) ([0-9]*)$/","\\1",$mt);
    return(strtoupper(base_convert($sec,10,36).base_convert($msec,10,36)));
  }

  public function min_entry_priority($id)
  {
    if(!$this->id_exists($id)){
      $this->set_error(sprintf(_("Can't set priority for ID '%s'. ID does not exist."),$id));
      return(FALSE);
    }
    reset($this->a_queue);
    $tmp = array();
    foreach($this->a_queue as $key => $entry){
      if($id != $entry['ID']){
        $tmp[] = $entry;
      }
    }
    $tmp[]= $this->get_entry($id);
    $this->a_queue=$tmp;
    return($this->_save_data());
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
