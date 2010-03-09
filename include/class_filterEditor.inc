<?php

class filterEditor extends plugin 
{
  protected $fitlers = array();
  protected $availableCategories = array();
  
  public $objectClass = array('gosaProperties');
  public $attributes = array('gosaUserDefinedFilter');
  public $gosaUserDefinedFilter = array();
  
  
  function __construct($config, $categories)
  {

    // Initialize this plugin with the users dn to gather user defined filters.
    $ui = get_userinfo();
    plugin::plugin($config, $ui->dn);

    // Keep list of currently managed categories.
    $this->availableCategories = array_unique($categories);
    $this->availableCategories[] = 'systems';
    $this->availableCategories[] = 'phones';
    $this->availableCategories[] = 'printer';
    $this->availableCategories[] = 'component';

    // Load list of filters
    if(isset($this->attrs['gosaUserDefinedFilter'])){
      for($i=0; $i< $this->attrs['gosaUserDefinedFilter']['count']; $i++){
        list($categories, $name, $description, $filter, $flags) = split(";", $this->attrs['gosaUserDefinedFilter'][$i]);
        $tmp = array(
            'name' => $name, 
            'categories' => split(',', $categories),
            'description' => base64_decode($description),
            'filter' => base64_decode($filter),
            'flags' => split(',',$flags));
        $this->filters[$name] = $tmp; 
      }
    }

    $this->filterWidget= new sortableListing();
    $this->filterWidget->setDeleteable(true);
    $this->filterWidget->setEditable(true);
    $this->filterWidget->setWidth("100%");
    $this->filterWidget->setHeight("70px");
    $this->filterWidget->setColspecs(array('100px', '200px', '100px', '70px','150px'));
    $this->filterWidget->setAcl($ui->get_permissions($ui->dn,'users/user','gosaUserDefinedFilter'));
    $this->filterWidget->setListData($this->filters, $this->convertFilterList());
  }

  function convertFilterList()
  { 
    $data = array();
    foreach($this->filters as $name => $filter){
      $data[$name] = array('data' =>
          array(
            $filter['name'],
            $filter['description'],
            implode(", ",$filter['categories']),
            implode(", ",$filter['flags'])));
    }
    return($data); 
  }

  function execute()
  {
    plugin::execute();

    // Close edit dialog
    if(isset($_POST['cancelFilterSettings'])){
      $this->dialog = NULL;
    }

    // Close edit dialog
    if(isset($_POST['saveFilterSettings']) && $this->dialog instanceOf filterEditEntry){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        msg_dialog::display_checks($msgs);
      }else{
        $orig_name = $this->dialog->getOriginalName();
        $new_name = $this->dialog->getCurrentName();

        // Remove old entry
        if($orig_name != "") unset($this->filters[$orig_name]);

        $this->filters[$new_name] = $this->dialog->save();
        $this->dialog = NULL;
        $this->filterWidget->setListData($this->filters, $this->convertFilterList());
        $this->filterWidget->update();
      }
    }

    // Act on edit requests 
    $this->filterWidget->save_object();
    $action = $this->filterWidget->getAction();
    if($action['action'] == 'edit' && count($action['targets']) == 1){
      $key= $this->filterWidget->getKey($action['targets'][0]);
      if(isset($this->filters[$key])){
        $this->dialog=new filterEditEntry($this->filters[$key], $this->availableCategories);
      }
    }

    // Act add requests
    if(isset($_POST['addFilter'])){
      $this->dialog=new filterEditEntry(array(), $this->availableCategories);
    }

    // Act on remove requests 
    $action = $this->filterWidget->getAction();
    if($action['action'] == 'delete' && count($action['targets']) == 1){
      $key= $this->filterWidget->getKey($action['targets'][0]);
      if(isset($this->filters[$key])){
        unset($this->filters[$key]);
        $this->filterWidget->update();
      }
    }

    // Display edit dialog
    if($this->dialog instanceOf filterEditEntry){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    $smarty = get_smarty();
    $smarty->assign("list", $this->filterWidget->render());
    return($smarty->fetch(get_template_path('filterEditor.tpl', FALSE)));
  }

  function enabled()
  {
    return(count($this->_parent->categories) != 0);
  }

  function save_object()
  {
  
  }

  function save()
  {
    $attrs = array();
    foreach($this->filters as $filter){
      $tmp = implode(',', $filter['categories']).";";
      $tmp.= $filter['name'].";";
      $tmp.= base64_encode($filter['description']).";";
      $tmp.= base64_encode($filter['filter']).";";
      $tmp.= implode(',', $filter['flags']);
      $attrs[] = $tmp;
    }
    $this->gosaUserDefinedFilter = $attrs;
    plugin::save();

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    
    new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MODIFY, get_class()));
    }
  }  
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
