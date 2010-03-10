<?php

class userFilter extends plugin 
{
  protected $fitlers = array();
  protected $availableCategories = array();
  
  public $objectClass = array('gosaProperties');
  public $attributes = array('gosaUserDefinedFilter');
  public $gosaUserDefinedFilter = array();
  
 
  /*! \brief  Initiates the filter editing dialog.
   */ 
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
   
        // Ensure that we no empty category in our category list.
        if(empty($categories)){
          $categories = array();
        }else{
          $categories = split(',', $categories);
        }

        // Ensure that we no empty entry in out flags list.
        if(empty($flags)){
          $flags = array();
        }else{
          $flags = split(',', $flags);
        }

        // build up filter entry.
        $tmp = array(
            'name' => $name, 
            'categories' => $categories,
            'description' => base64_decode($description),
            'filter' => base64_decode($filter),
            'flags' => $flags);


        $this->filters[$name] = $tmp; 
      }
    }

    // Create the filter list
    $this->filterWidget= new sortableListing();
    $this->filterWidget->setDeleteable(true);
    $this->filterWidget->setEditable(true);
    $this->filterWidget->setWidth("100%");
    $this->filterWidget->setHeight("270px");
    $this->filterWidget->setColspecs(array('100px', '200px', '100px', '70px','150px'));
    $this->filterWidget->setAcl($ui->get_permissions($ui->dn,'users/user','gosaUserDefinedFilter'));
    $this->filterWidget->setListData($this->filters, $this->convertFilterList());
  }


  /*! \brief    Converts the list of filters ($this->filters) into data which is useable
   *             for the sortableList object ($this->filterWidget).
   *  @return   Array   An array containg data useable for sortableLists ($this->filterWidget)
   */
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


  /*! \brief    Display the user-filter overview as HTML content.
   *  @return   string    HTML-content showing the user-filter editing dialog.
   */
  function execute()
  {
    plugin::execute();

    // Cancel filter modifications (edit dialog)
    if(isset($_POST['cancelFilterSettings'])){
      $this->dialog = NULL;
    }

    // Save modified filter entries (edit dialog)
    if(isset($_POST['saveFilterSettings']) && $this->dialog instanceOf userFilterEditor){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        msg_dialog::displayChecks($msgs);
      }else{
        $orig_name = $this->dialog->getOriginalName();
        $new_name = $this->dialog->getCurrentName();

        // The object was renamed and
        if($orig_name != $new_name && isset($this->filters[$new_name])){
          $msgs = array(msgPool::duplicated(_("Name")));
          msg_dialog::displayChecks($msgs);
        }else{

          // Remove old entry if filter was renamed
          if($orig_name != "" && isset($this->filters[$orig_name])){
            unset($this->filters[$orig_name]);
          }
          
          // Now append the new filter object.
          $this->filters[$new_name] = $this->dialog->save();
          $this->dialog = NULL;
          $this->filterWidget->setListData($this->filters, $this->convertFilterList());
          $this->filterWidget->update();
        }
      }
    }

    // Act on edit requests 
    $this->filterWidget->save_object();
    $action = $this->filterWidget->getAction();
    if($action['action'] == 'edit' && count($action['targets']) == 1){
      $key= $this->filterWidget->getKey($action['targets'][0]);
      if(isset($this->filters[$key])){
        $this->dialog=new userFilterEditor($this->filters[$key], $this->availableCategories);
      }
    }

    // Act on new requests
    if(isset($_POST['addFilter'])){
      $this->dialog=new userFilterEditor(array(), $this->availableCategories);
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
    if($this->dialog instanceOf userFilterEditor){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    $smarty = get_smarty();
    $smarty->assign("list", $this->filterWidget->render());
    return($smarty->fetch(get_template_path('userFilter.tpl', FALSE)));
  }


  /*! \brief    Write user-filter modifications back to the ldap.  
   */
  function save()
  {
    // Build up new list of filters 
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

  
  /*! \brief    Do not save any posted values here.
   */
  function save_object(){}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
