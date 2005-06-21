<?php
class macroParameter extends plugin
{
  /* CLI vars */
  var $cli_summary= "Handling of GOsa's application object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* application attributes */
  var $base= "";
  var $goFonMacroParameter =array();
  var $type_shortcut= array("string" => array("selected", "", ""),
                            "combo"  => array("", "selected", ""),
                            "bool"   => array("", "", "selected"));

  /* attribute list for save action */
  var $attributes= array("base","goFonMacroParameter");
  var $objectclasses= array("top", "goFonMacro");


  function macroParameter ($config, $dn= NULL)
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
        $this->goFonMacroParameter[]  = $tmp3;
      }
    }

    foreach($this->goFonMacroParameter as $para){
      $tmp = split("!",$para);
      $num = preg_replace("/[^0-9]/","",$tmp[1]);
      $tmp2[$num]['name']        = $tmp[1];
      $tmp2[$num]['type']        = $tmp[2];
      $tmp2[$num]['default']     = $tmp[3];
    }

    /* Assign this array */
    $this->goFonMacroParameter = $tmp2;
  }

  function check_paras($content,$goFonMacroParameter)
  {
    preg_match_all("/[$]\{ARG[0-9]*\}/",$content,$res,PREG_OFFSET_CAPTURE);

    $anz = count($res[0]);
    $new = array();

    foreach($res[0] as $val){
      $num = preg_replace("/[^0-9]/","",$val[0]); 
      $new[$num]['val'] = $val[0];
      $new[$num]['num'] = $num;
    }

    foreach($goFonMacroParameter as $gokey => $goval){
      foreach($new as $nkey => $nval){
        if($gokey == $nval['num']){
          $goFonMacroParameter[$gokey]['check']= true;
        }
      }
    }
    foreach($new as $key => $val){
      $goFonMacroParameter[$key]['check']= true;
      if((!isset($goFonMacroParameter[$key]['name']))||(empty($goFonMacroParameter[$key]['name']))){
        $goFonMacroParameter[$key]['name']="\${ARG".$key."}";
      }
    }  

    foreach($goFonMacroParameter as $key => $val){
      if(!isset($goFonMacroParameter[$key]['check'])){
        $goFonMacroParameter[$key]['check']= false;
      }
      if(!isset($goFonMacroParameter[$key]['default'])){
        $goFonMacroParameter[$key]['default'] = "";
      }
    }
    asort($goFonMacroParameter);
    return($goFonMacroParameter);

  }


  function execute()
  {
    /* Variables */
    $vars 	= "";
    $tmp  	= array();
    $number = 0; 

    $content = $_SESSION['macroManagment']->macrotabs->by_object['macro']->goFonMacroContent;

    if(strstr($content,"ARG")){
      $vorpos = strpos($content,"ARG");
      $rest   = substr($content,$vorpos, strlen($content));
    }    

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no phone macro.")."</b>";
      return ($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();

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


    #FIXME: Der Transfer der momentanen Variablen muss nach save_object(),
    #       damit kann in save() auf den execute() Aufruf verzichtet werden!
    
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
        $this->goFonMacroParameter[$key]['name']	 = $_POST["varname".$key];		
        $this->goFonMacroParameter[$key]['type']	 = $_POST["vartype".$key];		
        $this->goFonMacroParameter[$key]['default']= $_POST["default".$key];		
      }
    } 

    /*generate Table which shows als parameters */

    $FonParas = $this->check_paras($content,$this->goFonMacroParameter); 

    foreach($FonParas as $key=>$para)	{

      /* Select correct item of combobox */
      if(isset($para['type'])){
        list($sel1, $sel2, $sel3)= $this->type_shortcut[$para['type']];
      }else{
        list($sel1, $sel2, $sel3)= array("", "", "");
      }

      #FIXME: Der Name des Parameters ist nicht gesetzt
      #       -> Argument (ARG#)    Name [....]    Typ [...^]    Default [....]

      /* Assemble output table */
      $vars .="<tr>
        <td><input name=\"number".$key."\" value='".$key."' type='hidden'> </td>
        <td><input name='varname".$key."' type='hidden'   value='".$para['name']."'>".$para['name']."</td>
        <td>
        <select name='vartype".$key."'>
        <option name='vartype".$key."' value='string' ".$sel1.">"._("String")."</option>
        <option name='vartype".$key."' value='combo'   ".$sel2.">"._("Combobox")."</option>
        <option name='vartype".$key."' value='bool'   ".$sel3.">"._("Bool")."</option>
        </select>
        </td>
        <td><input name='default".$key."'   value='".$para['default']."'></td>
        <td>";
      if($para['check']==true) {
        $vars.=_("Ok");
        $vars.=" <td></td>";
      }else{
        $vars.="<b>"._("Unused")."</b>";
        $vars.=" <td><input name='del".$key."' value='delete' type='submit'></td>";
      }

      $vars.=" </tr>";
    }

    /* Checkboxes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("vars", $vars);

    /* Show main page */
    return($smarty->fetch (get_template_path('parameter.tpl', TRUE)));
  }


  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['phoneparameters'])){
      plugin::save_object();
    }
  }


  /* Check values */
  function check()
  {
    $message = array();

    foreach($this->attributes as $attr){
      if(chkacl($this->acl,$attr)){
        $str =  sprintf( _("Insufficient permissions, can't change attribute '%s' in goFonMacro."),$attr) ;
        return(array($str));
      }
    }
    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Post checks */

    # FIXME: das hat hier nix zu suchen...
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
