<?php

class printerPPDSelectionDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");

  var $list                 =array();
  var $header               =array();
  var $current_del_id       = "";
  var $Vendor               = "";

  function printerPPDSelectionDialog (&$config, $dn= NULL,$list=false,$headers=false,$ppd=false)
  {
    plugin::plugin ($config, $dn);
    $this->list       = $list;
    $this->header     = $headers;
    $this->depselect  = $this->config->current['BASE'];
    
    if(!session::is_set('printerPPDSelectionDialog')){
      session::set("printerPPDSelectionDialog",array('regex'=> "*"));
    }  

    /* Order the manufacturers index */
    ksort($this->header);
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
    $s_action = "none";
 
    $printerPPDSelectionDialog = session::get('printerPPDSelectionDialog');
    $regex = $printerPPDSelectionDialog['regex'];

    if(isset($_GET['search'])){
      $regex = $_GET['search']."*";
    }
  
    if(isset($_POST['regex'])){
      $regex= $_POST['regex'];
    }
    
    $regex = str_replace("**","*",$regex);
    $printerPPDSelectionDialog['regex'] = $regex;
    session::set('printerPPDSelectionDialog',$printerPPDSelectionDialog);
 

    /* Delete requested, check if everything is ok and display confirmation dialog */
    if(isset($_GET['act']) && $_GET['act'] == "del"){

      /* Get id and check if id is valid */
      $id_to_del = base64_decode($_GET['id']);
      $found = "";
      foreach($this->list[$this->Vendor] as $key => $data){
        if($data['link'] == $id_to_del){
          $found = $key;
          break;
        }
      }
      if(!empty($found)){
        $this->current_del_id = $id_to_del;

        $smarty->assign("warning", sprintf(_("You're about to delete the ppd file '%s' at '%s'."), $found,$id_to_del));
        return($smarty->fetch(get_template_path('remove_ppd.tpl', TRUE)));

      }else{
        print_red(sprintf(_("Could not found specified ppd file '%s'."),$id_to_del));
      }
    }

    /* Deletion confirmed */
    if(isset($_POST['delete_ppd_confirm']) && !empty($this->current_del_id)){

      /* check if file is removeable */
      if(is_writeable($this->current_del_id)){
        $is_ok = @unlink($this->current_del_id);

        /* Update ppd list */
        if($is_ok){
          foreach($this->list as $vendor => $ppds){
            foreach($ppds as $ppd => $data){
              if($data['link'] == $this->current_del_id){
                unset($this->list[$vendor][$ppd]);
              }
            }
          }
        }
      }else{
        $is_ok = false;
      }

      if(!$is_ok){
        print_red(_("Something went wrong while trying to remove the ppd file from server, possibly we have no write access."));
      }
    }


    /** Added **/
    $list = array();
    foreach($this->list as $cat => $ppds){
      foreach($ppds as $ppd){
        if(preg_match("/^".str_replace("*",".*",$regex)."/i",$ppd['ppd'])){
          if(is_readable($ppd['link'])){
            $list[$ppd['link']] = $ppd;
          }
        }
      }
    }
 
    if((isset($_GET['act']))&&($_GET['act']=="open"))  {
      
      if((!empty($_GET['id'])) && (!isset( $this->header[base64_decode($_GET['id'])]))){
        print_red(sprintf(_("Selected vendor '%s' does not exists in our list of ppds."),base64_decode($_GET['id'])));      
        $this->Vendor = ""; 
      }else{
        $this->Vendor = base64_decode($_GET['id']);
      }
    }

    $div = new divSelectBox("printerPPDSelectionDialog");
    $div ->SetHeight(450);
    $div ->SetSummary(_("Printer ppd selection."));
    
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>
                  <img src='images/folder.png' class='center' border=0>&nbsp;%s
                 </a>";
    $uselink  = "<a href='?plug=".$_GET['plug']."&amp;act=use&amp;id=%s'>%s</a>";
    $dellink  = "<a href='?plug=".$_GET['plug']."&amp;act=del&amp;id=%s'>
                  <img src='images/edittrash.png' alt='"._("delete")."' border=0>
                 </a>"; 

    if(empty($this->Vendor)){
      foreach($this-> header as $key => $entry){
        $div ->AddEntry (array(
              array("string"=>sprintf($linkopen,base64_encode($key),$key),"attach"=>"style='border-right:0px;'")
              ));
      }
    }else{
      $div ->AddEntry (array(
            array("string"=>sprintf($linkopen,"",".. ["._("back")."]"),"attach"=>"style='border-right:0px;'")
            ));
      foreach($list as $key => $ppd){
        if(preg_match("/^".$this->Vendor."/",$ppd['ppd'])){

          if(is_writeable($ppd['link'])){
            $del_str = sprintf($dellink,base64_encode($key));
          }else{
            $del_str = "";
          }

          $div ->AddEntry (array(
                                  array("string"=>sprintf($uselink,base64_encode($key),$ppd['ppd'])),
                                  array("string"=>$del_str,"attach"=>"style='border-right:0px;'")

                ));
        }
      }
    }

    $smarty->assign("List",         $div -> DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("launchimage",  get_template_path('images/small_filter.png'));
    $smarty->assign("tree_image",   get_template_path('images/tree.png'));
    $smarty->assign("alphabet",     generate_alphabet());
    $smarty->assign("apply",        apply_filter());
    $smarty->assign("regex",        $regex);
  
    $display.= $smarty->fetch(get_template_path('printerPPDSelectionDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function save_object()
  {
  } 

  function check(){
  }

  /* Save to LDAP */
  function save()
  {
    return $this->selectedPPD;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
