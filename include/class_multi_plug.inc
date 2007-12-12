<?php
/*
   This code is part of GOsa (https://oss.gonicus.de/labs/gosa/)
   Copyright (C) 2007 Fabian Hickert

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

/*

Data structure : 
================ 
  |->o_tab                      <-- dummy object, collects HTML posts, displays ui 
  |->a_handles                  <-- tab object for each given dn
     |->tab object for dn 1 
     |->tab object for dn 2
      ...
     |->tab object for dn n

Using this class: 
=================
  Simple Example:

    $dn   = array(dn1,dn2,dn3);
    $tmp  = new multi_plug($config,"usertabs",$config->data['TABS']['USERTABS'],$dn);
    echo $tmp->execute();

  $tmp can now be used like the normal tab class, execute, save_object ...
  
  To enable multipe edit for a specific plugin, 
  just set the plugin variable 'multiple_support' to true:

    var $multiple_support = TRUE;

  If plugin::multiple_support is true, the member function 
  multiple_execute() will be called and displayed, NOT execute().

  (I will put this in the wiki, later. This are just notes for me.)
  
*/


/*! \brief   Handles multiple edits for a given set of dns.
    \author  Fabian Hickert
    \version 1.01
    \date    2007/12/07

    This class edits multiple tab objects at once. 
    1. There is a dummy tab object initialized that collects the user input.
    2. All given objects specified by '$dn' will be initialized and the collected
     data from the dummy object will be populated to them.
 */
class multi_plug
{
  /* Tab handler for each given dn entry */
	public $a_handles = array();

  /* Dummy handler which collects the data */
  private $o_tab     = NULL;  

  public $dn      = array();
  public $config  = NULL;
  private $s_class= "";
  public $current = "";

  /*! \brief    Creates a multi_plug object
     @param   object  $config GOsa Configuration object
     @param   string  $class  The class name of the tab we want to edit. e.g. usertabs
     @param   string  $tab    The config tab name e.g. USERTABS 
     @param   array   $dns    The object dns we want to edit.
     @return  object  multi_plug
   */
  public function __construct($config,$class,$tab,$dns,$acl_base,$acl_category)
  {
    if(!count($dns)){
      return;
    }

    $this->dn       = $dns;
    $this->config   = $config;
    $this->s_class  = $class;

    /* Initialize collector object 
     * Used to display the ui and to collect the user input.
     */
    $this->o_tab    = new $class($config,$tab,"new",$acl_category);
    $this->o_tab->set_acl_base($acl_base);

    /* Check if the specified tab object supports multiple edits 
     */
    if($this->o_tab->multiple_support_available()){

      /* Enable multiple actions for the collector object 
       */ 
      $this->o_tab->enable_multiple_support();

      /* Initialize the objects we want to edit at once 
       */
      foreach($dns as $dn){
        $obj = new $class($config,$tab,$dn,$acl_category);
        $obj->set_acl_base($acl_base);
        $this->a_handles[] = $obj;
      }
    }

    /* Detect attributes used by all plugins and set 
     *  those values as default in edit handle 
     */
    $this->detect_multiple_used_attributes();
  }

  
  /*! \brief    Detect values that are used in all edited objects.
   * @returns   array   All multiple used attributes
   */  
  private function detect_multiple_used_attributes()
  {
    $attrs = array();
    $all   = array();
    $first = $this->o_tab->current;
    foreach($this->a_handles as $handle){
      if(count($attrs) == 0){
        $attrs = $handle->by_object[$first]->attrs;
      }else{
        foreach($attrs as $key => $attr){
          if(!isset($handle->by_object[$first]->attrs[$key]) || !($attr === $handle->by_object[$first]->attrs[$key])){
            unset($attrs[$key]);
          }
          if(!is_numeric($key)){
            if(!isset($all[$key])){
              $all[$key] = $attr; 
            }elseif(isset($attr['count'])){
              for($i = 0; $i < $attr['count'] ; $i ++){
                $all[$key][] = $attr[$i];
              }
              $all[$key] = array_unique($all[$key]);
            }
          }
        }
      }
    }
    foreach($this->o_tab->by_object as $name => $obj){
      $this->o_tab->by_object[$name]->init_multiple_support($attrs,$all);
    }
  }


  /*! \brief    Returns the edit ui for multiple edit.
     @return    string  HTML User interface for given tab object.
   */
  public function execute()
  {
    return($this->o_tab->execute());
  }


  /*! \brief    Checks if one of the objects we want to edit is locked. 
     @return    boolean   Returns TRUE if at least one entry is locked, else false.
   */
  public function entries_locked()
  {
    $ui = get_userinfo();
    foreach($this->dn as $dn){
      if(get_lock($dn) != ""){
        return(TRUE);
      }
    }
    return(FALSE);
  }


  /*! \brief    Generates a lock message that can be displayed if an entry is locked.
     @return    string  Returns a list of locked entries 
   */
  public function display_lock_message()
  {
    $ui = get_userinfo();
    $lock_msg = "";
    $lock_msg.=  gen_locked_message ($ui->dn, $this->dn);
    return($lock_msg);
  }


  /*! \brief    Locks all currently managed objects (array $this->dn) 
     @return    boolean   Returns TRUE
   */
  public function lock_entries($uid)
  {
    foreach($this->dn as $dn)
    add_lock($dn,$uid);
    return(TRUE);
  }


  /*! \brief    Checks if the given tab object supports multiple edit.
     @return    boolean   Returns TRUE if the given tab objects supports multiple edit else FALSE.
   */
  public function multiple_available()
  { 
    if(isset($this->o_tab) && is_object($this->o_tab)){
      return($this->o_tab->multiple_support_available());
    }else{
      return(FALSE);
    }
  }


  /*! \brief    Sets the currently active tab. The tab that will be displayed by $this->execute(). 
   */
  public function set_active_tab($str)
  {
    $this->current = $str;
  }


  /*! \brief    Returns the object info string, that can be displayed in the tab header.
      @return   string  Returns an information string, containing the list of currently edited dns.
   */
  public function get_object_info()
  {
    return(_("You are currently editing mutliple entries."));
  }


  /*! \brief    Handles all HTML posts from the dummy tab object. 
   */
  public function save_object()
  {
    $this->o_tab->save_object(); 
  }


  /*! \brief    Checks if the values fetched by $this->save_object() are valid.
      @param    array Returns an array containig all error messages.
   */
  public function check()
  {
    $this->populate_values();
    $messages = $this->o_tab->check();
    return($messages);
  }


  /*! \brief    Currently not implemented, later be used to trigger password changes. 
      @param    boolean Returns TRUE if a password change is needed.
   */
  public function password_change_needed()
  {
    foreach($this->a_handles as $i_id => $o_handle){
      if($o_handle->password_change_needed() && isset($o_handle->by_object['user'])){
        new msg_dialog(_("Password reset"),_("The user password was resetted, please set a new password value!"),WARNING_DIALOG);
        change_password ($o_handle->dn, "",0, $o_handle->by_object['user']->pw_storage);
      }
    }
    return(FALSE);
  }


  /*! \brief    Populate all collected values to the target tab objects ($this->o_handles)
   */
  public function populate_values()
  {

    if($this->multiple_available() && is_array($this->a_handles)){
      foreach($this->o_tab->by_object as $name => $obj){

        $values = $this->o_tab->by_object[$name]->get_multi_edit_values();
        foreach($this->a_handles as $i_id => $o_handle){
          foreach($values as $a_name => $a_value){
            $this->a_handles[$i_id]->by_object[$name]->$a_name = $a_value;
          }
        }
      }
    }
  }


  /*! \brief    Save all edited tab objects ($this->o_handles). 
   */
  public function save()
  {
    if($this->multiple_available() && is_array($this->a_handles)){
      $this->populate_values();
      foreach($this->a_handles as $i_id => $o_handle){
        $o_handle->save();
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
