<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_sudo.inc 9975 2008-03-25 14:09:30Z hickert $$
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


/*! \brief  Sudo option class.
  Allows setting flags/options for a sudo role.
 */
class sudoOption extends plugin
{
    /* Group attributes */
    protected $sudoOption = array();
    public $attributes    = array("sudoOption");
    private $options = array();
    public $ignore_account = TRUE;

    /*! \brief  Initializes this class
      @param  Object $config  The GOsa configuration object.
      @param  String $dn      The object dn.
     */
    function __construct(&$config, $dn= NULL)
    {
        plugin::__construct ($config, $dn);

        /****
          Create a list of known options
         ****/
        $options = array();
        $option['long_otp_prompt']= array('NAME' =>'long_otp_prompt' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['ignore_dot']=  array('NAME' =>'ignore_dot' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['mail_always']= array('NAME' =>'mail_always' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['mail_badpass']=  array('NAME' =>'mail_badpass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['mail_no_user']=  array('NAME' =>'mail_no_user' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['mail_no_host']=  array('NAME' =>'mail_no_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['mail_no_perms']= array('NAME' =>'mail_no_perms' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['tty_tickets']= array('NAME' =>'tty_tickets' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['authenticate']=  array('NAME' =>'authenticate' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['root_sudo']= array('NAME' =>'root_sudo' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['log_host']=  array('NAME' =>'log_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['log_year']=  array('NAME' =>'log_year' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['shell_noargs']=  array('NAME' =>'shell_noargs' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['set_home']=  array('NAME' =>'set_home' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['always_set_home']= array('NAME' =>'always_set_home' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['path_info']= array('NAME' =>'path_info' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['preserve_groups']= array('NAME' =>'preserve_groups' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['fqdn']=  array('NAME' =>'fqdn' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['insults']= array('NAME' =>'insults' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['requiretty']=  array('NAME' =>'requiretty' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['env_editor']=  array('NAME' =>'env_editor' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['rootpw']=  array('NAME' =>'rootpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['runaspw']= array('NAME' =>'runaspw' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['targetpw']=  array('NAME' =>'targetpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['set_logname']= array('NAME' =>'set_logname' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['stay_setuid']= array('NAME' =>'stay_setuid' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['env_reset']= array('NAME' =>'env_reset' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'TRUE');
        $option['use_loginclass']=  array('NAME' =>'use_loginclass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['noexec']=  array('NAME' =>'noexec' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['ignore_local_sudoers']=  array('NAME' =>'ignore_local_sudoers' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => 'FALSE');
        $option['passwd_tries']=  array('NAME' =>'passwd_tries' ,  'TYPE' => 'INTEGER' ,   'DEFAULT' => 3);
        $option['loglinelen']=  array('NAME' =>'loglinelen' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => 80);
        $option['timestamp_timeout']= array('NAME' =>'timestamp_timeout' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => 0);
        $option['passwd_timeout']=  array('NAME' =>'passwd_timeout' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => 15);
        $option['umask']= array('NAME' =>'umask' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => "0022");
        $option['mailsub']= array('NAME' =>'mailsub' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '*** SECURITY information for %h ***');
        $option['badpass_message']= array('NAME' =>'badpass_message' ,   'TYPE' => 'STRING' ,   'DEFAULT' => 'Sorry, try again');
        $option['timestampdir']=  array('NAME' =>'timestampdir' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '/var/run/sudo');
        $option['timestampowner']=  array('NAME' =>'timestampowner' ,  'TYPE' => 'STRING' ,   'DEFAULT' => 'root');
        $option['passprompt']=  array('NAME' =>'passprompt' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '[sudo] password for %p: ');
        $option['runas_default']= array('NAME' =>'runas_default' ,   'TYPE' => 'STRING' ,   'DEFAULT' => 'root');
        $option['syslog_goodpri']=  array('NAME' =>'syslog_goodpri' ,  'TYPE' => 'STRING' ,   'DEFAULT' => 'notice');
        $option['syslog_badpri']= array('NAME' =>'syslog_badpri' ,   'TYPE' => 'STRING' ,   'DEFAULT' => 'alert');
        $option['editor']=  array('NAME' =>'editor' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '/usr/bin/vi');
        $option['noexec_file']= array('NAME' =>'noexec_file' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '/usr/lib/sudo/sudo_noexec.so');
        $option['lecture']= array('NAME' =>'lecture' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'once');
        $option['lecture_file']=  array('NAME' =>'lecture_file' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
        $option['logfile']= array('NAME' =>'logfile' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'syslog');
        $option['syslog']=  array('NAME' =>'syslog' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'authpriv');
        $option['mailerpath']=  array('NAME' =>'mailerpath' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
        $option['mailerflags']= array('NAME' =>'mailerflags' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '-t');
        $option['mailto']=  array('NAME' =>'mailto' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'root');
        $option['exempt_group']=  array('NAME' =>'exempt_group' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'root');
        $option['verifypw']=  array('NAME' =>'verifypw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'all');
        $option['listpw']=  array('NAME' =>'listpw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => 'any');
        $option['env_check']= array('NAME' =>'env_check' ,   'TYPE' => 'LISTS' ,   'DEFAULT' => '');
        $option['env_delete']=  array('NAME' =>'env_delete' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
        $option['env_keep']=  array('NAME' =>'env_keep' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
        ksort($option);
        $this->options = $option;

        $this->load_options();
    }


    private function load_options()
    {

        /****
          Parse given sudoOption attributes 
         ****/
        $this->sudoOption = array();
        if(isset($this->attrs['sudoOption'])){
            for($i = 0 ; $i < $this->attrs['sudoOption']['count']; $i++){

                /****
                  Detect attribute name/value/negation
                 ****/
                $opt = $this->attrs['sudoOption'][$i];

                /* Get negation */ 
                $negation = FALSE;
                if(preg_match("/^!/",$opt)){
                    $negation = TRUE; 
                    $opt = preg_replace("/^!/","",$opt);
                }

                /* Get value / name*/
                $value    = "";
                if(preg_match("/=/",$opt)){
                    $value  = preg_replace("/^[^=]*+=/","",$opt);
                    $opt    = preg_replace("/=.*$/","",$opt);
                }

                /* Special chars are escaped, remove escape char now.
                   \\ => \
                   \: => :
                   \, => ,
                   \= => = 
                 */
                $value = $this->unescape_command($value);

                /* Check if the given value is part of our options list.
                   If it is not, add it as type STRING and display a warning.  
                 */
                if(!isset($this->options[$opt])){
                    $this->options[$opt]=array('NAME'=>$opt,'TYPE'=>'STRING','DEFAULT' => '');
                    msg_dialog::display(_("Unknown option"),
                            sprintf(_("The Sudo option '%s' is invalid!"),
                                $opt),WARNING_DIALOG);
                }

                /* Create internal sudoOption object */
                $option = array();
                $option['NAME']   = $opt;
                $option['VALUE']  = $value;
                $option['NEGATE'] = $negation;

                /*  Special handling for mixed flag types. 
                    Some attributes like (BOOL_INTEGER) can be TRUE/FALSE and INTEGER.
                    This means, if the value is empty it is BOOL and $negation defines its boolean value.
                 */
                if(in_array_strict($this->options[$opt]['TYPE'],array("BOOL_INTEGER","STRING_BOOL"))){
                    if(empty($value)){
                        $option['NEGATE'] = FALSE;
                        if($negation){
                            $option['VALUE'] = "FALSE";
                        }else{
                            $option['VALUE'] = "TRUE";
                        }
                    }
                }

                /* Special handling for BOOLEAN values */
                if(in_array_strict($this->options[$opt]['TYPE'],array("BOOLEAN"))){
                    $option['NEGATE'] = FALSE;
                    if($negation){
                        $option['VALUE'] = "FALSE";
                    }else{
                        $option['VALUE'] = "TRUE";
                    }
                }

                /* Append values */
                $this->sudoOption[$opt][] = $option;
            }
        }
    }



    /*! \brief  Create HTML output for this plugin 
      @return String  HTML output for this plugin.
     */
    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /*****
          Handle Posts 
         *****/
        if($this->acl_is_writeable("")){

            foreach($_POST as $name => $value){
    
                $value = get_post($name);

                if(preg_match("/^negOption_/",$name)){

                    $opt = preg_replace("/^negOption_(.*)_[0-9]*$/","\\1", $name);
                    $id  = preg_replace("/^negOption_.*_([0-9])*$/","\\1", $name);

                    if(isset($this->sudoOption[$opt][$id])){
                        $val = $this->sudoOption[$opt][$id]["VALUE"];

                        /*****
                          Negate STRING_BOOL && BOOL_INTEGER
                         *****/
                        if(in_array_strict($this->options[$opt]['TYPE'],array('STRING_BOOL','BOOL_INTEGER'))){
                            if(in_array_strict($val, array("TRUE","FALSE"))){
                                if($val == "TRUE"){
                                    $this->sudoOption[$opt][$id]["VALUE"] = "FALSE";
                                }else{
                                    $this->sudoOption[$opt][$id]["VALUE"] = "TRUE";
                                }
                            }else{
                                $this->sudoOption[$opt][$id]['NEGATE'] = !$this->sudoOption[$opt][$id]['NEGATE']; 
                            }
                        }

                        /*****
                          Negate STRING / INTEGER
                         *****/
                        if(in_array_strict($this->options[$opt]['TYPE'],array('STRING','INTEGER','LISTS'))){
                            $this->sudoOption[$opt][$id]['NEGATE'] = !$this->sudoOption[$opt][$id]['NEGATE']; 
                        }

                        /*****
                          Negate BOOLEAN
                         *****/
                        if(in_array_strict($this->options[$opt]['TYPE'],array('BOOLEAN'))){
                            if($val == "TRUE"){
                                $this->sudoOption[$opt][$id]["VALUE"] = "FALSE";
                            }else{
                                $this->sudoOption[$opt][$id]["VALUE"] = "TRUE";
                            }
                        }
                    }
                    break;
                }

                /*****
                  Remove options
                 *****/
                if(preg_match("/^delOption/",$name)){
                    $opt = preg_replace("/^delOption_/","",$name);
                    $opt = preg_replace("/_[^_]*$/","",$opt);
                    $id  = preg_replace("/^.*_([0-9])*$/","\\1",$opt);
                    $opt = preg_replace("/_[0-9]*$/","",$opt);

                    if(isset($this->sudoOption[$opt][$id])){
                        unset($this->sudoOption[$opt][$id]);
                    }
                    if(!count($this->sudoOption[$opt])){
                        unset($this->sudoOption[$opt]);
                    }
                    break;
                }
            }
        }


        $smarty = get_smarty();
        $smarty->assign("ACL",$this->getacl(""));
        $smarty->assign("map",  
                    set_post(    array(
                    "STRING" => _("string"), "BOOLEAN" => _("boolean"),
                    "INTEGER" => _("integer") , "BOOL_INTEGER" => _("integer")."-"._("boolean") ,
                    "STRING_BOOL" => _("string")."-"._("boolean"),"LISTS" => _("list"))));
        $smarty->assign("sudoOption",$this->prepare_for_html($this->sudoOption));
        $smarty->assign("options",$this->options);
        return($smarty->fetch(get_template_path('options.tpl', TRUE)));
    }


    /*! \brief  Prepare options array to be used in HTML.
      @param  Array   The options array ($this->sudoOption) 
      @return Array   HTML ready sudoOption. Can now be used in smarty templates
     */
    function prepare_for_html($a_options)
    {
        foreach($a_options as $name => $options){
            foreach($options as $key => $option){
                $a_options[set_post($name)][$key]['VALUE'] = set_post($option['VALUE']);
            }
        }
        return($a_options);
    }


    /*! \brief  Removes this plugin 
     */
    function remove_from_parent()
    {
    }


    /*! \brief  Saves all relevant HTML post values for this plugin 
     */
    function save_object()
    {
        if($this->acl_is_writeable("")){
            plugin::save_object();

            if(isset($_POST['add_option']) && isset($_POST['option'])){
                $opt = get_post("option");

                /* Append attribute only once, lists are handled below */
                if(isset($this->options[$opt])){
                    $type = $this->options[$opt]['TYPE'];
                    $val  = $this->options[$opt]['DEFAULT'];
                    $option = array("NAME" => $opt, "VALUE" => $val , "NEGATE" => FALSE);
                    $this->sudoOption[$opt][] = $option;
                }
            }

            foreach($this->sudoOption as $name => $opts){
                foreach($opts as $id => $opt){

                    /****
                      Get posted value for BOOLEAN
                     ****/
                    if(in_array_strict($this->options[$name]['TYPE'],array("BOOLEAN"))){
                        if(isset($_POST['option_value__'.$name.'_'.$id])){
                            $this->sudoOption[$name][$id]["VALUE"] = get_post('option_value__'.$name.'_'.$id);
                        }
                    }

                    /****
                      Get posted value for STRING / INTEGER
                     ****/
                    if(in_array_strict($this->options[$name]['TYPE'],array("STRING","INTEGER"))){
                        if(isset($_POST['option_value__'.$name.'_'.$id])){
                            $this->sudoOption[$name][$id]["VALUE"] = get_post('option_value__'.$name.'_'.$id);
                        }
                    }

                    /****
                      Get posted value for STRING_BOOL / BOOL_INTEGER
                     ****/
                    if(in_array_strict($this->options[$name]['TYPE'],array("BOOL_INTEGER","STRING_BOOL"))){
                        if(isset($_POST['option_selection__'.$name.'_'.$id])){
                            $sel = get_post('option_selection__'.$name.'_'.$id);
                            $val = "";
                            if(isset($_POST['option_value__'.$name.'_'.$id])){
                                $val = get_post('option_value__'.$name.'_'.$id);
                            }

                            if($sel == "FALSE" || $sel == "TRUE"){
                                $this->sudoOption[$name][$id]['VALUE'] = $sel;
                                $this->sudoOption[$name][$id]['NEGATE'] = FALSE;
                            }else{
                                $this->sudoOption[$name][$id]['VALUE'] = $val;
                            }
                        }
                    }

                    /****
                      Get posted value for LISTS
                     ****/
                    if(in_array_strict($this->options[$name]['TYPE'],array("LISTS"))){
                        foreach($this->sudoOption[$name] as $entry_key => $entry){
                            if(isset($_POST['list_value__'.$name.'_'.$entry_key])){
                                $val = get_post('list_value__'.$name.'_'.$entry_key);
                                $this->sudoOption[$name][$entry_key]["VALUE"] = $val;
                            }
                        } 
                    }
                }
            }
        }
    }


    /*! \brief  Save changes to ldap 
     */
    function save()
    {
        plugin::save(); 

        $this->attrs['sudoOption'] = array();
        foreach($this->sudoOption as $name => $opts){
            foreach($opts as $id => $opt){

                $type   = $this->options[$name]['TYPE'];
                $neg    = $opt['NEGATE'];
                $value  = $opt['VALUE'];
                $option = "";

                /* Escape special chars */
                $value = $this->escape_command($value);

                /****
                  Save LISTS 
                 ****/
                if($type=="LISTS"){
                    if($value == ""){
                        $option = $name;
                    }else{
                        $option = $name."=".$value;
                    }
                    if($neg){
                        $option = "!".$option;
                    }
                }

                /****
                  Save BOOLEAN
                 ****/
                if(in_array_strict($type,array("BOOLEAN"))){ 
                    $option = $name;
                    if($value == "FALSE"){
                        $option = "!".$option;
                    }
                }

                /****
                  Save STRING / INTEGER
                 ****/
                if(in_array_strict($type,array("STRING","INTEGER"))){ 
                    if($value != ""){
                        $option = $name."=".$value;
                    }else{
                        $option = $name; 
                    }
                    if($neg){
                        $option = "!".$option;
                    }
                }

                /****
                  Save STRING_BOOL / BOOL_INTEGER
                 ****/
                if(in_array_strict($type,array("STRING_BOOL","BOOL_INTEGER"))){
                    if($value == "FALSE"){
                        $option = "!".$name;
                    }elseif($value == "TRUE"){
                        $option = $name;
                    }else{
                        if($value != ""){
                            $option = $name."=".$value;
                        }else{
                            $option = $name; 
                        }
                        if($neg){
                            $option = "!".$option;
                        }
                    }
                }

                $this->attrs['sudoOption'][] = $option;
            }
        }
        $this->cleanup();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->modify($this->attrs);;
    }


    /*! \brief  Checks input validity
     */
    function check()
    {
        $message = plugin::check();

        foreach($this->sudoOption as $name => $options){
            foreach($options as $id => $option){
                switch($this->options[$name]['TYPE']){

                    /* Check for a valid integer value */
                    case 'INTEGER' : 
                        {
                            if(!preg_match("/^[0-9]*$/",$option['VALUE'])){
                                $message[] = msgPool::invalid($name,$option['VALUE'],"/[0-9]/");
                            }
                        } break;
                }
            }
        }
        return ($message);
    }


    /*! \brief  This function will be called if an object gets copied.
      This function adapts attributes from the source object.
      @param  Array The source object.
     */
    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);
        if(isset($source['sudoOption'])){
            $this->attrs['sudoOption'] = $source['sudoOption'];
            $this->load_options();
        }
    }


    /*!  \brief   Escape special chars in function parameters.
      @param   String the string to that must be escaped.
     */
    private function escape_command($str)
    {
        /* Check if given value is a command (/[a-z]/ ..)
         */
        if(preg_match("/^\//",$str)){
            $cmd = preg_replace("/^([^ ]*).*$/","\\1",$str);
            $val = preg_replace("/^[^ ]*(.*)$/","\\1",$str);
            $str = $cmd.addcslashes($val,":.,\\");
        }
        return($str);
    }


    /*! \brief  Add ACL object
      @return Returns the ACL object.
     */
    static function plInfo()
    {
        return (array(  
                    "plShortName" => _("Options"),
                    "plDescription" => _("Sudo options"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 2,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("sudo"),
                    "plProvidedAcls"    => array(
                        "sudoOption"  => _("Sudo options") 
                        )
                    ));
    }

    /*!  \brief   Unescape special chars in function parameters.
      @param   String the string to that must be unescaped.
     */
    private function unescape_command($str)
    {
        /* Check if given value is a command (/[a-z]/ ..)
         */
        if(preg_match("/^\//",$str)){
            $cmd = preg_replace("/^([^ ]*).*$/","\\1",$str);
            $val = preg_replace("/^[^ ]*(.*)$/","\\1",$str);
            $val = preg_replace(array("/\\\\\\\\/","/\\\\,/","/\\\\:/","/\\\\=/"),
                    array("\\",",",":","="),$val);
            $str = $cmd.$val;
        }
        return($str);
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
