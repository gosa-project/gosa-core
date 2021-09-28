<?php
class goSpamServerRule extends plugin {

    var $attributes = array("name","rule");
    var $name = "";
    var $rule = "";

    var $orig_name = "";
    var $view_logged  =FALSE;

    function __construct(&$config,$dn,$name = "",$rule ="")
    {
        plugin::__construct($config,$dn);
        $this->name = $this->orig_name= $name;
        $this->rule = $rule;
    }


    function execute()
    {
        plugin::execute();

        $smarty = get_smarty();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }
        return($smarty->fetch(get_template_path("goSpamServerRule.tpl",TRUE,dirname(__FILE__))));			
    }

    function save_object()
    {
        foreach($this->attributes as $attr){
            if(isset($_POST[$attr])){
                $this->$attr = get_post($attr);
            }
        }
    }

    function acl_is_writeable($attribute,$skip_write = FALSE)
    {
        if($this->read_only) return(FALSE);
        $ui= get_userinfo();
        return preg_match('/w/', $ui->get_permissions($this->acl_base, $this->acl_category."gospamserver", $attribute,      $skip_write));
    }

    function save()
    {
        $ret =array();
        $ret['orig_name'] = $this->orig_name;
        $ret['name'] = $this->name;
        $ret['rule'] = $this->rule;
        return($ret);
    }

    function check()
    {
        $messages = plugin::check();
        return($messages);
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
