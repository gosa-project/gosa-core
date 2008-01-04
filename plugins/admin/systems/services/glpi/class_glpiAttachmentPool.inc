<?php

class glpiAttachmentPool extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account = TRUE;
  var $attributes     = array("name","comment","mime","filename","date_mod");
  var $objectclasses  = array("whatever");

  var $parent;
  var $edit = false;
  var $entry = false;
  var $date_mod ="";
  var $name     ="";
  var $comment  ="";
  var $mime     ="";
  var $filename ="";  

  var $Selected = array();

  var $delAttach= "";

  function glpiAttachmentPool (&$config, $dn= NULL,$used=NULL)
  {
    plugin::plugin ($config, $dn);
    if(!isset($_SESSION['GlpiAttachmentFilter'])){
      session::set('GlpiAttachmentFilter',array("filter"=>"*"));
    }

    /* Assign used attributes */
    if($used !== NULL){
      $this->Selected = $used;
    }
  }

  function execute()
  {
    plugin::execute();
    $attach     = $this->parent->handle->getAttachments();
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display    = "";
    $only_once  = true; 

    /* walk through all posted objects */
    foreach($_POST as $name => $value){

      /* Open dialog to create a new entry */
      if(preg_match("/new_attach/",$name)){
        $this->edit = true;
        $this->entry=array();

        /* Set default values */
        foreach($this->attributes as $attr) {
          $this->$attr = "";
          $this->entry[$attr]="";
        }
      }
  
      /* Remove attach*/ 
      if((preg_match("/^delAttach_/",$name))&&($only_once)){
        $only_once = false;
        $str = preg_replace("/^delAttach_/","",$name);
        $str = base64_decode(preg_replace("/_.*$/","",$str));

        /* remove attach from db */
        $this->delAttach = $str;
        $smarty->assign("warning", sprintf(_("You're about to delete the glpi attachment component '%s'."), $attach[$str]['name']));
        return($smarty->fetch(get_template_path('remove_glpi.tpl', TRUE)));
      }
 
      /* Start editing entry */ 
      if((preg_match("/^editAttach_/",$name))&&($only_once)){
        $only_once = false;
        $str = preg_replace("/^editAttach_/","",$name);
        $str = base64_decode(preg_replace("/_.*$/","",$str));
        
        /* Check if we have an attachment with given ID */ 
        foreach($attach as $att ){
          if($att['ID'] == $str ){

            /* Entry was found, show dialog */          
            $this->edit = true;
            $this->entry  = $att;       

            foreach($att as $name => $value){
              $this->$name = $value;
            } 
          } 
        }
      }
    }

    if((isset($_POST['delete_glpi_confirm']))&&(isset($attach[$this->delAttach]))){
      if($this->parent->handle->is_attachmentUsed($this->delAttach)){
        $tmp = $this->parent->handle->is_attachmentUsed($this->delAttach);
        
        $names = "";
        foreach($tmp as $name){
          $names .= ", ".$name;
        }
        $names = preg_replace("/^, /","",$names);
        $names = trim($names);
        if(count($tmp) == 3){
          $names .= " ...";
        }
        print_red(sprintf(_("You can't delete this attachment, it is still in use by these system(s) '%s'"),$names));

      }else{
        $this->parent->handle->deleteAttachment($this->delAttach);
        @unlink(CONFIG_DIR."/glpi/".$this->filename);
        $attach     = $this->parent->handle->getAttachments();
      }
    }

    /* Someone tries to upload a file */
    if(($this->edit == true)&&(isset($_POST['upload']))){
      if(!isset($_FILES['filename'])){
        print_red(_("There is no valid file uploaded."));
      }else{
        $FILE = $_FILES['filename'];  
        if(!isset($FILE['name'])){
          print_red(_("There is no valid file uploaded."));
        }else{
          if($FILE['error']!=0) {
            print_red(_("Upload wasn't successfull."));
          }else{
            if(!is_dir(CONFIG_DIR."/glpi/")){
              print_red(sprintf(_("Missing directory '%s/glpi/' to store glpi uploads."),CONFIG_DIR));
            }else{
              $filen = CONFIG_DIR."/glpi/".$FILE['name'];
              if(file_exists($filen)){
                print_red(_("There is already a file with the same name uploaded."));
              }else{
                $fh = fopen($filen,"w");
                if(!$fh){
                  print_red(sprintf(_("Can't create file '%s'."),$filen));
                }else{
                  $str = file_get_contents($FILE['tmp_name']);
                  fwrite($fh,$str,strlen($str));
                  fclose($fh);
                  $this->mime     = $FILE['type'];   
                  $this->filename = $FILE['name'];   
                }
              } // File already exists
            }
          } // Check if any error occurred
        } // check if valid filename was uploaded
      } // ende check if file was uploaded 
    }// upload post

    /* save attachment*/
    if(($this->edit == true)&&(isset($_POST['SaveAttachment']))){
      if(count($this->check())==0){
        $this->date_mod = date("Y-m-d H:i:s");
        $this->save_entry();
        $this->edit= false;
        $this->entry = array();
        $attach = $this->parent->handle->getAttachments();
      }else{
        foreach($this->check() as $msg){
          print_red($msg);
        }
      }
    }

    /* Abort editing/adding  attachment*/
    if(($this->edit == true)&&(isset($_POST['CancelAttachment']))){
      $this->edit  = false;
      $this->entry = array();
    }

    /* If edit == true, we have to show a dialog to edit or add an attach 
     */
    if($this->edit == true){
      foreach($this->attributes as $attr){
        $smarty->assign($attr,htmlentities(utf8_decode($this->$attr)));
      }
      if(!empty($this->filename)){
        if(is_readable(CONFIG_DIR."/glpi/".$this->filename)){
          $status =_("File is available.");
        }else{
          $status =_("File is not readable, possibly the file is missing.");
        }
      }else{
        $status = _("Currently no file uploaded.");
      }
      $smarty->assign("status",$status); 
      return($smarty->fetch(get_template_path('glpiAttachmentEdit.tpl', TRUE)));
    }

    /* Create list with checkboxes to select / deselect some attachents */
    $divlist = new divlist("Attachment");  
    $divlist->SetPluginMode();
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("Name")),
          array("string" => _("Mime"),"attach"=>"style='width:200px;'"),
          array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));

    $divlist->SetSummary(_("This table displays all available attachments."));
    $divlist->SetEntriesPerPage(0);

    $editdel = "<input type='image' name='editAttach_%s' src='images/edit.png'>";
    $editdel.= "<input type='image' name='delAttach_%s'  src='images/edittrash.png'>";

    /* Add all given attachments to divlist 
     */ 
    foreach($attach as $entry){

      /* Create display name*/
      if((empty($entry['name']))||(empty($entry['comment']))){

        /* In glpi it is possible to add entries without name 
           so i've added this block 
         */ 
        if(empty($entry['name'])){
          $str1 = "<i>"._("empty")."</i>";
        }else{ 
          $str1 = $entry['name'];
        }

        if(!empty($entry['comment'])){
          $str1.= " [".$entry['comment']."]";
        }
      }else{
        /* Both attributes given */
        $str1 = $entry['name']." [".$entry['comment']."]";
      }

      $edit = str_replace("%s",base64_encode($entry['ID']),$editdel);
      $str2 = $entry['mime']."&nbsp;";      

      $chkbox = "<input type='hidden' name='wasOnPage_%s'>".
        "<input type='checkbox' name='useMe_%s' value='%s' %CHECKED%>";

      if(in_array($entry['ID'],$this->Selected)){
        $chkbox = preg_replace("/%CHECKED%/"," checked ",$chkbox);
      }else {
        $chkbox = preg_replace("/%CHECKED%/"," ",$chkbox);
      }
      $chkbox = preg_replace("/%s/",$entry['ID'],$chkbox);
      $divlist->AddEntry(array(
            array("string" => $chkbox, 
              "attach" => "style='text-align:center;width:20px;'"),
            array("string"=> $str1),
            array("string"=> $str2,"attach"=>"style='width:200px;'"),
            array("string"=> $edit ,"attach" => "style='width:60px;border-right:0px;text-align:right;'")
            ));

    }

    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      "   <input class='center' type='image' 
      src='images/fai_new_template.png' align='middle' title='"._("Create new attachment")."' 
      name='new_attach' alt='"._("New Attachment"). "'>&nbsp;".
      "</div>";

    $smarty->assign("attachments",      $divlist->DrawList());
    $smarty->assign("attachmenthead",   $listhead);
    $smarty->assign("search_image",     get_template_path('images/search.png'));
    $smarty->assign("searchu_image",    get_template_path('images/search_user.png'));
    $smarty->assign("tree_image",       get_template_path('images/tree.png'));
    $smarty->assign("infoimage",        get_template_path('images/info_small.png'));
    $smarty->assign("launchimage",      get_template_path('images/small_filter.png'));
    $smarty->assign("apply",            apply_filter());
    $smarty->assign("alphabet",         generate_alphabet());
    $smarty->assign("attachment_regex", $_SESSION['GlpiAttachmentFilter']['filter']);

    $display.= $smarty->fetch(get_template_path('glpiAttachmentPool.tpl', TRUE));
    return($display);
  }

  function save()
  {
    return($this->Selected);
  }

  /* Adds new or saves edited entries */
  function save_entry()
  {
    if($this->edit){
      $tmp = array();
      foreach($this->attributes as $attr){
        $tmp[$attr] = $this->$attr;
      } 
      $id = -1;
      if(isset($this->entry['ID'])){
        $id = $this->entry['ID'];
      }
      $this->parent->handle->saveAttachments($tmp,$id);
    }
  }

  function save_object()
  {
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }

    /* Checkboxes are only posted, if they are checked
     * To check if it was deselected, we check if wasOnPage 
     * was posted with given name, so we can deselect this entry
     */  

    foreach($_POST as $name => $value){
      if(preg_match("/wasOnPage_/",$name)){
        $id=preg_replace("/wasOnPage_/","",$name);
        if(isset($_POST["useMe_".$id])){
          $this->Selected[$id]=$id;
        }else{
          if(isset($this->Selected[$id])){
            unset($this->Selected[$id]);
          }
        }
      }
    }
  }

  /* Simple check */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if($this->edit){
    
      /* check if given name is already in use */
      $att = $this->parent->handle->getAttachments();
      $ok = true;
      $this->name=trim($this->name);

      foreach($att as $val){
        if(!isset($this->entry['ID'])){
          if($val['name'] == $this->name){
            $ok = false;
          }
        }else{
          if(($val['name'] == $this->name)&&($this->entry['ID'] != $val['ID'])){
            $ok = false;
          }
        }
      }
      if(!$ok){
        $message[] = _("This name is already in use.");
      }
      if(empty($this->name)){
        $message[] = _("Please specify a valid name for this attachment.");
      }
    }
    return($message);
  }

}// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
