<?php

require_once("class_pgsql_opengw.inc");

class ogw{
  var $info;
  var $ogo;
  
  var $validLocationTeam;
  var $validTemplateUser;
  var $validTeams;


  var $InfoOK = false;

  var $MUST = array("name","login","template_user_id");
  
  var $MAY = array( "salutation","firstname","description","degree",
                    "birthday","sex","street","zip","country","zipcity",
                    "state","name1","value_string","number","db_status",
                    "object_version","is_locked","LocationTeamID","TeamIDis","password");

  var $LastError  = "";
  var $option     = "";
  var $connected = false;

  function ogw($username,$password,$host,$db)
  {
    $this->ogo  = new pgre_sql($username,$password,$host,$db);

    if($this->ogo->is_connected){
      $this->validLocationTeam  = $this->ogo->GetLocationTeam();
      $this->validTemplateUser  = $this->ogo->GetTemplateUser();
      $this->validTeams         = $this->ogo->GetTeams();
      $this->connected = true;
    }else{
      $this->validLocationTeam  = array();//$this->ogo->GetLocationTeam();
      $this->validTemplateUser  = array();//$this->ogo->GetTemplateUser();
      $this->validTeams         = array();//$this->ogo->GetTeams();
      $this->connected = false;
    }
  }
 
  function SetInfos($infos)
  { 
    if(isset($infos['name']))  {
      $infos['name1']              = $infos['name'];
    }
    $this->info = $infos;
  }

  function GetInfos($uid)
  {  
    $ret = array();
    $qry = "SELECT  is_person,is_account,is_intra_account,is_extra_account,  
                    number,owner_id,object_version,company_id,template_user_id,is_locked,
                    name,firstname,description,salutation,login,degree,birthday,sex  
            FROM person WHERE login='".$uid."';";
    $res = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    $ret = $res[0];

    $qry = "SELECT street,zip,zipcity,country,state FROM address WHERE company_id = ".$ret['company_id']." limit 1;";
    $res = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    $ret = array_merge($ret,$res[0]); 

    $qry = "SELECT company_id FROM company_assignment 
            WHERE (sub_company_id=".$ret['company_id'].") 
              AND company_id IN 
                (SELECT company_id FROM team  WHERE  (is_team=1) 
                AND 
                  company_id NOT IN (SELECT company_id FROM team WHERE is_location_team=1));";    
    $res = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    if(is_array($res)){
      foreach($res as $r){
        $ret['TeamIDis'][]=$r['company_id']; 
      }
    }else{
      $ret['TeamIDis']=array();
    }

    $qry = "SELECT value_string from company_value WHERE company_id=".$ret['company_id'].";";
    $res = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    $ret = array_merge($ret,$res[0]); 

    $qry ="SELECT company_id FROM company_assignment 
           WHERE (sub_company_id=".$ret['company_id'].") 
              AND company_id IN (SELECT company_id FROM team  WHERE  (is_location_team=1));";
    $res = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    $ret['LocationTeamID'] = $res[0]['company_id'];

    return($ret);
  }

  function Perform($option)
  {
    if(!in_array($option,array("ADD","EDIT","REMOVE"))){
      $this->LastError = sprintf("Option '%s' is not allowed, possible options are 'ADD' 'EDIT' 'REMOVE'.",$option);
      return(false);
    }else{
      $this->option = $option;
      if(($this->option == "EDIT")||($this->option=="ADD")){

        /* Static variables */
        if(!isset($this->info['is_person'])){
          $this->info['is_person']          = 1;  
        }
        
        if(!isset($this->is_account)){
          $this->info['is_account']         = 1;  
        }
        
        if(!isset($this->info['is_intra_account'])){
          $this->info['is_intra_account']   = 1;
        }

        if(!isset($this->info['is_extra_account'])){
          $this->info['is_extra_account']   = 0;
        }

        if(!isset($this->info['owner_id'])){
          $this->info['owner_id']           = 10000;
        }

        if(!isset($this->info['is_team'])){
          $this->info['is_team']            = 0;
        }

        $this->InfoOK = $this->checkInfos();

        if($this->InfoOK){
          $qry = "SELECT * FROM person WHERE login='".$this->info['login']."';";
          $ist = $this->ogo->FetchAllRows($this->ogo->Query($qry));

          if($this->option == "ADD"){
            if($this->CheckExistence()){
              $this->LastError="Can't add entry already exists.";
              return(false);
            }else{
              $this->info['db_status']          = "inserted";
              $this->info['object_version']     = 1;
              return($this->ADD());
            }
          }else{
            if(!$this->CheckExistence()){
              $this->LastError="Can't edit entry, entry doesn't exists.";
              return(false);
            }else{
              $this->info['db_status']          = "updated";
              $this->info['object_version']     = $ist[0]['object_version']++;
              return($this->EDIT());
            }
          }
        }else{
          return($this->InfoOK);
        }
      }
      if($this->option == "REMOVE"){
    
        if((!isset($this->info['login']))||(empty($this->info['login']))){
          $this->LastError = "Require login to detect existence";
          return(false);
        }else{
    
          if($this->CheckExistence()){
            return($this->REMOVE());
          }else{
            $this->LastError="Can't remove non existing entry";
            return(false);
          }
        }
      }
    }
  }

  function CheckExistence()
  {
    /* Check if thios entry already exists */
    $qry = "SELECT login,name FROM person WHERE login='".$this->info['login']."';";
    $ist = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    if($ist){
      return(true);
    }else{
      return(false);
    }
  }

  function checkInfos()
  {
    foreach($this->MUST as $name){
      if((!isset($this->info[$name]))||(empty($this->info[$name]))){
        $this->LastError = sprintf("Must attribute '%s' wasn't set.",$name);
        return(false);
      }
    }

    foreach($this->MAY as $name){
      if((!isset($this->info[$name]))||(empty($this->info[$name]))){
        $this->info[$name] = false;
      }
    }

    $tmp = array(false,"",0);
    foreach($this->validLocationTeam as $id){
      $tmp[]= $id['company_id'];
    }
    if(!in_array($this->info['LocationTeamID'],$tmp)){
      $this->LastError = "Given 'Location Team' is invalid.";
      return(false);
    }

    $tmp = array();
    foreach($this->validTemplateUser as $id){
      $tmp[]= $id['company_id'];
    }
    if(!in_array($this->info['template_user_id'],$tmp)){
      $this->LastError = "Given 'Template User ID' is invalid.";
      return(false);
    }

    
    $tmp = array();
    foreach($this->validTeams as $id){
      $tmp[]= $id['company_id'];
    }
    if(is_array($this->info['TeamIDis'])){
      foreach($this->info['TeamIDis'] as $id){
        if(!in_array($id,$tmp)){
          $this->LastError = sprintf("Given 'Team ID':%s is invalid.",$id);
          return(false);
        }
      }
    }
    return(true);
  }

  function REMOVE()
  {
    $qry = "SELECT * FROM person WHERE login='".$this->info['login']."';";
    $ist = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    if((!$ist)||(!isset($ist[0]['company_id']))||($ist[0]['company_id']<=0)){
      $this->LastError(sprintf("Can't get company id for login %s",$this->info['login']));
      return(false);
    }else{

      $company_id     = $ist[0]['company_id'];

      $qry = "UPDATE person SET login='SKY".$company_id.$this->info['login']."', is_account=0, is_intra_account=0 WHERE company_id=".$company_id.";";
      $this->ogo->Query($qry);
      /*
      $this->ogo->Query("DELETE FROM telephone WHERE company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM address  WHERE company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM company_info WHERE company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM company_value WHERE company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM company_assignment WHERE sub_company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM person WHERE company_id=".$company_id.";");
      $this->ogo->Query("DELETE FROM staff WHERE company_id=".$company_id.";");
      */
      return(true);
    }
  }

  function ADD()
  {
    /* 
       Entry settings for personm table 
     */
    $arr = array( "company_id","object_version","owner_id","template_user_id",
                  "is_person","is_account","is_intra_account","is_extra_account",
                  "number","description","is_locked","login","name","name","firstname",
                  "salutation","degree","birthday","sex","db_status","password");
    $this->info['company_id']  = $this->ogo->gen_id();
    $this->info['userID']             = "OGo".$this->info['company_id'];
    foreach($arr as $attr){
      if($attr == "number"){
        $add_user[$attr]  = $this->info['userID'];
      }else{
        $add_user[$attr]  = $this->info[$attr];
      }
    }
    $QUERY[] = gen_syntax($add_user,"person","ADD",false);


    /*
       Entry for staff table
     */
    $arr = array("staff_id","company_id","description","login","is_team","is_account","db_status");
    $this->info['staff_id'] = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_staff[$attr] = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_staff,"staff","ADD",false);


    /* 
       Create entries for company nfo 
     */
    $arr = array("company_info_id","company_id","db_status");
    $this->info['company_info_id']  = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_company_info[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_info,"company_info","ADD",false);


    /* 
       Create entries for company value 
     */
    $arr = array("db_status","value_string","attribute","company_id","company_value_id");
    $this->info['attribute']        = "email1";
    $this->info['company_value_id'] = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_company_value[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_value,"company_value","ADD",false);


    /* 
       address entries 
     */
    $arr = array("company_id","address_id","street","zip","country","zipcity","state","db_status","name1","type");
    foreach(array("private","mailing","location") as  $type){
      
      $this->info['address_id'] = $this->ogo->gen_id();
      $this->info['type']       = $type;
      foreach($arr as $attr){
        $add_address[$attr]  = $this->info[$attr];
      }
      $QUERY[] = gen_syntax($add_address,"address","ADD",false);
    }


    /*
       telephone entries 
     */
    $arr = array("telephone_id","object_version","company_id","number","type","db_status");
    foreach(array("01_tel","02_tel","03_tel_funk","05_tel_private","10_fax","fax_private") as $type){
      $this->info['type']       = $type;
      $this->info['telephone_id']     = $this->ogo->gen_id();
      foreach($arr as $attr){
        $add_telephone[$attr]  = $this->info[$attr];
      }
      $QUERY[] = gen_syntax($add_telephone,"telephone","ADD",false);
    }

    /*
       company_assignment entries (Location Team) 
     */
    $this->info['old_company_id']       = $this->info['company_id'];
    $this->info['sub_company_id']       = $this->info['old_company_id'];

    $this->info['company_assignment_id']= $this->ogo->gen_id();
    $this->info['company_id']           = $this->info['LocationTeamID'];
    $arr = array("company_assignment_id","company_id","sub_company_id","db_status");
    foreach($arr as $attr){
      $add_company_assignment[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_assignment,"company_assignment","ADD",false);


    /*
       company_assignment entries (Teams) 
     */
    $arr = array("company_assignment_id","company_id","sub_company_id","db_status");
    foreach($this->info['TeamIDis'] as $TeamID){
      
      $this->info['company_id']           = $TeamID;
      $this->info['sub_company_id']       = $this->info['old_company_id'];
      $this->info['company_assignment_id']= $this->ogo->gen_id();
      foreach($arr as $attr){
        $add_company_assignment[$attr]  = $this->info[$attr]; 
      }    
      $QUERY[] = gen_syntax($add_company_assignment,"company_assignment","ADD",false);
    }

    $remove_all = false;
    foreach($QUERY as $q ){
      if(!$this->ogo->Query($q)){
        $remove_all = true;
        break;
      }
    }
    
    if($remove_all== true){
      $this->ogo->Query("DELETE FROM telephone WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM address WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM company_assignment WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM company_info WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM company_value WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM staff WHERE company_id=".$this->info['company_id'].";");
      $this->ogo->Query("DELETE FROM person WHERE company_id=".$this->info['company_id'].";");
      $this->LastError="Query failed, removed all added entries";
      return(false);
    }
    return(true);
  }


  function EDIT()
  {
    $qry = "SELECT * FROM person WHERE login='".$this->info['login']."';";
    $ist = $this->ogo->FetchAllRows($this->ogo->Query($qry));
    /* 
       Entry settings for personm table 
     */
    $arr = array( "company_id","object_version","owner_id",
                  "template_user_id","is_person","is_account","is_intra_account",
                  "is_extra_account","number","description","is_locked","login","name",
                  "firstname","salutation","degree","birthday","sex","db_status","password");
    $this->info['company_id'] = $ist[0]['company_id'];
    $this->info['userID']     = "OGo".$this->info['company_id'];
    foreach($arr as $attr){
      if($attr == "number"){
        $add_user[$attr]  = $this->info['userID'];
      }else{
        $add_user[$attr]  = $this->info[$attr];
      }
    }
    $QUERY[] = gen_syntax($add_user,"person","EDIT",$ist);


    /*
       Entry for staff table
     */
    $arr = array("staff_id","company_id","description","login","is_team","is_account","db_status");
    $this->info['staff_id'] = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_staff[$attr] = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_staff,"staff","EDIT",$ist);


    /* 
       Create entries for company nfo 
     */
    $arr = array("company_info_id","company_id","db_status");
    $this->info['company_info_id']    = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_company_info[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_info,"company_info","EDIT",$ist);


    /* 
       Create entries for company value 
     */
    $QUERY[] = "DELETE FROM company_value WHERE company_id=".$ist[0]['company_id']." AND attribute='mail1';";
    $arr = array("db_status","value_string","attribute","company_id","company_value_id");
    $this->info['attribute']        = "email1";
    $this->info['company_value_id'] = $this->ogo->gen_id();
    foreach($arr as $attr){
      $add_company_value[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_value,"company_value","ADD",false);


    /* 
       address entries 
     */
    $QUERY[] = "DELETE FROM address WHERE company_id=".$ist[0]['company_id'].";";
    $this->info['company_id'] = $ist[0]['company_id'];
    $arr = array("company_id","address_id","street","zip","country","zipcity","state","db_status","name1","type");
    foreach(array("private","mailing","location") as  $type){
      $this->info['type']       = $type;
      $this->info['address_id'] = $this->ogo->gen_id();
      foreach($arr as $attr){
        $add_address[$attr]  = $this->info[$attr];
      } 
      $QUERY[] = gen_syntax($add_address,"address","ADD",false);
    }


    /*
       telephone entries 
     */
    $QUERY[] = "DELETE FROM telephone WHERE company_id=".$ist[0]['company_id'].";";
    $this->info['company_id'] = $ist[0]['company_id'];
    $arr = array("telephone_id","object_version","company_id","number","type","db_status");
    foreach(array("01_tel","02_tel","03_tel_funk","05_tel_private","10_fax","fax_private") as $type){
      $this->info['type']           = $type;
      $this->info['telephone_id']   = $this->ogo->gen_id();
      foreach($arr as $attr){
        $add_telephone[$attr]  = $this->info[$attr];
      }

      $QUERY[] = gen_syntax($add_telephone,"telephone","ADD",false);
    }


    /*
       company_assignment entries (Location Team) 
     */
    $this->info['old_company_id'] = $this->info['company_id'];
    
    /* First remove location team */
    $QUERY[] = "DELETE FROM company_assignment WHERE (sub_company_id=".$ist[0]['company_id'].") AND
      company_id in (SELECT company_id FROM team WHERE is_location_team=1);";

    $this->info['sub_company_id']       = $ist[0]['company_id'];
    $this->info['company_assignment_id']= $this->ogo->gen_id();
    $this->info['company_id']           = $this->info['LocationTeamID'];
    $arr = array("company_assignment_id","company_id","sub_company_id","db_status");
    foreach($arr as $attr){
      $add_company_assignment[$attr]  = $this->info[$attr];
    }
    $QUERY[] = gen_syntax($add_company_assignment,"company_assignment","ADD",false);


    /*
       company_assignment entries (Teams) 
     */
    /* First remove location team */
    $QUERY[] = "DELETE FROM company_assignment 
                WHERE 
                    (sub_company_id=".$ist[0]['company_id'].") 
                  AND
                    company_id IN 
                      (SELECT company_id FROM team  WHERE  (is_team=1) AND company_id NOT IN 
                         (SELECT company_id FROM team WHERE is_location_team=1));";

    $arr = array("company_assignment_id","company_id","sub_company_id","db_status");

    if(is_array($this->info['TeamIDis'])){
      foreach($this->info['TeamIDis'] as $TeamID){
        $this->info['company_id']           = $TeamID;
        $this->info['sub_company_id']       = $ist[0]['company_id'];
        $this->info['company_assignment_id']= $this->ogo->gen_id();
        $add_company_assignment = array();
        foreach($arr as $attr){
          $add_company_assignment[$attr]  = $this->info[$attr];
        }    
        $QUERY[] = gen_syntax($add_company_assignment,"company_assignment","ADD",false);
      }
    }
    $remove_all = false;

    foreach($QUERY as $q ){
      if(!$this->ogo-> Query($q)){
        print $q;
        $remove_all = true;
        break;
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
