<?php

class pgre_sql{

	var $handle;
	var $query_log;
	var $user;
	var $server;
	var $db;
	var $pwd;
	var $is_connected = false;

	function __construct($user,$pwd,$server,$db)
	{
		$this->user   = $user;
		$this->pwd  = $pwd;
		$this->server = $server;
		$this->db   = $db;

		if($this->_connect()){
			$this->is_connected = true;
		}else{
			$this->is_connected = false;
		}
	}

	function _connect()
	{
		error_reporting(E_ALL | E_STRICT);
		if(is_callable("pg_connect")){
			if(empty($this->pwd)){
				$this->handle = @pg_connect("dbname=".$this->db." host=".$this->server." user=".$this->user);
			}else{
				$this->handle = @pg_connect("dbname=".$this->db." host=".$this->server." user=".$this->user." password=".$this->pwd);
			}
			if(!$this->handle){
				$this->handle = false;
			}
		}else{
			$this->handle = false;
		}
		return($this->handle);
	}

	function Query($a_query)
	{
		if(is_array($a_query)){
			foreach($a_query as $nr => $query){
				return($this->_query($query));
			}
		}else{
			return($this->_query($a_query));
		}
	}

	function _query($query)
	{
		return(pg_query($this->handle,$query));
	}

	function FetchAllRows($res)
	{
		return(pg_fetch_all($res))  ;
	}

	function gen_id()
	{
		$tmp = $this->_query("select nextval('key_generator');");
		$tmp = ($this->FetchAllRows($tmp));
		return($tmp[0]['nextval']);
	}


	function GetTemplateUser(){
		$data = array();
		$qry = "SELECT description,name,company_id FROM company WHERE is_template_user=1;";
		$res = $this->_query($qry);
		$tmp = $this->FetchAllRows($res);
		foreach($tmp as $attr){
			$data[$attr['name']] = $attr;
		}
		return $data;
	}
	function GetLocationTeam(){
		$data = array();
		$qry = "SELECT description,name,company_id FROM team WHERE is_location_team=1;";
		$res = $this->_query($qry);
		$tmp = $this->FetchAllRows($res);
		if(is_array($tmp)){
			foreach($tmp as $attr){
				$data[$attr['description']] = $attr;
			}
		}
		return $data;
	}
	function GetTeams(){
		$data = array();
		$qry = "SELECT description,name,company_id FROM team 
			WHERE (is_team=1) AND company_id 
			NOT IN (SELECT company_id FROM company WHERE is_location_team=1);";
		$res = $this->_query($qry);
		$tmp = $this->FetchAllRows($res);
		foreach($tmp as $attr){
			$data[$attr['description']] = $attr;
		}
		return $data;
	}
}


function gen_syntax($array,$tablename,$act,$ist)
{
	if($act == "EDIT"){
		$str = "UPDATE ".$tablename." SET ";
		$company_id = $ist[0]['company_id'];

		foreach($array as $name => $value){
			if((empty($value))&&(!preg_match("/^is_/i",$name))) continue;

			if((empty($value))&&(preg_match("/^is_/i",$name))){
				$value= 0;
			}

			if(!is_numeric($value)){
				$str.= " ".$name."='".$value."', ";
			}else{
				$str.= " ".$name."=".$value.", ";
			}
		}
		$str = preg_replace("/, $/","",$str);
		$str .= " WHERE company_id=".$company_id.";\n";
		return $str;
	}
	if($act == "ADD"){
		$str = "INSERT into ".$tablename." (";
		$attrs  = "";
		$values = "";
		foreach($array as $name => $attribute){
			if((empty($attribute))&&(!preg_match("/^is_/i",$name))) continue;

			if((empty($attribute))&&(preg_match("/^is_/i",$name))){
				$attribute= 0;
			}

			if(is_numeric($attribute)){
				$attrs  .= $name.", ";
				$values .= $attribute.", ";
			}else{
				$attrs  .= $name.", ";
				$values .= "'".$attribute."', ";
			}
		}
		$attrs = preg_replace("/, $/","",$attrs);
		$values= preg_replace("/, $/","",$values);
		$str .= $attrs." ) \nVALUES\n (".$values.");\n";
		return $str;
	}
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
