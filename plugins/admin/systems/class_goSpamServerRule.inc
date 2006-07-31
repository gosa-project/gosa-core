<?php
class goSpamServerRule extends plugin {

	var $attributes = array("name","rule");
	var $name = "";
	var $rule = "";
	
	var $orig_name = "";
	
	function goSpamServerRule($config,$dn,$name = "",$rule ="")
	{
		plugin::plugin($config,$dn);
		$this->name = $this->orig_name= $name;
		$this->rule = $rule;
	}


	function execute()
	{
		$smarty = get_smarty();
		foreach($this->attributes as $attr){
			$smarty->assign($attr,$this->$attr);
		}
		return($smarty->fetch(get_template_path("goSpamServerRule.tpl",TRUE,dirname(__FILE__))));			
	}

	function save_object()
	{
		plugin::save_object();
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

?>
