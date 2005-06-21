<?php
class macro extends plugin
{
  /* CLI vars */
  var $cli_summary= "Handling of GOsa's application object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* application attributes */
  var $cn= "";
  var $description= "";
  var $base= "";
  var $displayName = "";
  var $goFonMacroParameter =array();
  var $goFonMacroContent;
  var $goFonMacroVisible=0;

  /* attribute list for save action */
  var $attributes= array("cn","base", "description","displayName","goFonMacroParameter","goFonMacroContent","goFonMacroVisible");
  var $objectclasses= array("top", "goFonMacro");

  function macro ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $tmp = array();  // temporary Var 
    $tmp2 = array(); // temporary Var ...
    $tmp3 = "";
    $ldap= $config->get_ldap_link();

    $this->dn = $dn;

    /* This is always an account */
    $this->is_account= TRUE;

    /* Edit or new one ?*/
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
    } else {
      $this->base= dn2base($this->dn);
    }

    /* initialising macro parameter */
    unset($this->attrs['goFonMacroParameter']['count']);

    /* Set Parameters, or a new array if ther are no parameters */
    if(isset($this->attrs['goFonMacroParameter'])){
      $this->goFonMacroParameter = $this->attrs['goFonMacroParameter'];
    }else{
      $this->goFonMacroParameter =array();
    }

    /* Create an array for parameters if not given yet */
    if(!is_array($this->goFonMacroParameter)){
      $tmp3 = $this->goFonMacroParameter;
      $this->goFonMacroParameter =array();	
      if(!empty($tmp3)) {
        $this->goFonMacroParameter[] = $tmp3;
      }
    }

    /* Create a better sorted array */
    foreach($this->goFonMacroParameter as $para)    {
      $tmp = split("!",$para);
      $tmp2[$tmp[0]]['name'] 		= $tmp[1];  
      $tmp2[$tmp[0]]['type'] 		= $tmp[2];  
      $tmp2[$tmp[0]]['default'] 	= $tmp[3];  
    }

    /* Assign this array */
    $this->goFonMacroParameter = $tmp2;	

  }

  function execute()
  {
    /* Variables */
    $vars 	= "";
    $tmp  	= array();
    $number = 0; 

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no application.")."</b>";
      return ($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign all vars to Smarty */
    foreach($this->attributes as $ar){
      $smarty->assign($ar, $this->$ar);
    }

    /* Add an empty Parameter */
    if(isset($_POST['addvar'])){
      if(!is_array($this->goFonMacroParameter)){
        $vars = $this->goFonMacroParameter;
        $this->goFonMacroParameter = array();
        $this->goFonMacroParameter[]= $vars;
      }
      $number= count($this->goFonMacroParameter);
      $number++;
      $this->goFonMacroParameter[]=array("name"=>"new","type"=>"string","default"=>"test");
    }

    /* read out post data, and assign it to the parameters */
    /* And or delete */
    foreach($_POST as $name=>$value){	

      /* Test if there is a variable begining with "del" */
      if(preg_match("/del/",$name)){

        /* Extract entry id to delete */
        $nr = str_replace("del","",$name)	;

        /* unset entry */			
        unset($this->goFonMacroParameter[$nr]);

      }elseif(preg_match("/number/",$name)){
        /* Set Post vars */
        $key = $_POST[$name];
        $this->goFonMacroParameter[$key]['name']		=$_POST["varname".$key];		
        $this->goFonMacroParameter[$key]['type']		=$_POST["vartype".$key];		
        $this->goFonMacroParameter[$key]['default']		=$_POST["default".$key];		
      }
    } 

    /*generate Table which shows als parameters */
    foreach($this->goFonMacroParameter as $key=>$para)	{

      if($para['type']=="string") {
        $sel1 = "selected";
        $sel2 = "";
      }else{
        $sel1 = "";
        $sel2 = "selected";
      }

      $vars .="<tr>
        <td>".$key."<input name=\"number".$key."\" value='".$key."' type='hidden'> </td>
        <td><input name='varname".$key."'   value='".$para['name']."'></td>
        <td>
        <select name='vartype".$key."'>
        <option name='vartype".$key."' value='string' ".$sel1.">string</option>
        <option name='vartype".$key."' value='combo'   ".$sel2.">combo</option>
        <option name='vartype".$key."' value='bool'   ".$sel2.">bool</option>
        </select>
        </td>
        <td><input name='default".$key."'   value='".$para['default']."'></td>
        <td><input name='del".$key."' value='delete' type='submit'></td>
        </tr>";


    }

    /* Display add renew button */
    $vars .="<tr>	
      <td colspan=5>
      <input name='addvar' value="._("Add")." type='submit'>
      <input name='actvar' value="._("Refresh")." type='submit'>
      </td>
      </tr>";
    /* Checkboxes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("vars", $vars);

    if($this->goFonMacroVisible){
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else{
      $smarty->assign("goFonMacroVisibleChecked","");
    }

    if(isset($_POST['goFonMacroVisible'])) {
      $this->goFonMacroVisible= 1 ;
      $smarty->assign("goFonMacroVisibleChecked"," checked ");
    }else  {
      if(isset($_POST['cn'])){
        $this->goFonMacroVisible= 0 ;
        $smarty->assign("goFonMacroVisibleChecked","");
      }
    }


    /* Show main page */
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->rmDir($this->dn);

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['cn'])){
      plugin::save_object();

    }
  }


  /* Check values */
  function check()
  {
    $message = array();
    
    foreach($this->attributes as $attr){
      if(chkacl($this->acl,$attr)){
        $str =  sprintf( _("Insufficient permissions, can't change attribute '%s' in goFonMacro"),$attr) ;
        return(array($str));
        }
      }

    if(($this->cn != strtolower($this->cn))||(empty($this->cn))){
      $message[] = "The attribute 'cn' must be lowercase and a least one char.";
    }
    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Post checks */
    $this->execute();

    plugin::save();

    $this->attrs['goFonMacroParameter']=array();

    foreach($this->goFonMacroParameter as $key=>$fonpara){
      $this->attrs['goFonMacroParameter'][]=$key."!".$fonpara['name']."!".$fonpara['type']."!".$fonpara['default'];
    }

    unset($this->attrs['base']);

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn);
    $a= $ldap->fetch();

    if (count($a)){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
      $this->handle_post_events("modify");
    } else {
      if(count($this->attrs['goFonMacroParameter']==0)){
        unset($this->attrs['goFonMacroParameter']);
      }		

      $ldap->cd($this->dn);
      $ldap->create_missing_trees( $this->dn);
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
    show_ldap_error($ldap->get_error());
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
