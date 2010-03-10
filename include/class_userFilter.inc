<?php

class userFilter extends plugin 
{
  protected $filters = array();
  protected $availableCategories = array();
  
  public $objectclasses = array('gosaProperties');
  public $attributes = array('gosaUserDefinedFilter');
  public $gosaUserDefinedFilter = array();

  private $listing = NULL;
  private $fixedFilter = NULL;

  /*! \brief    Returns true if we are able to read and write userFilters 
   *            (schema has to be present, gosaProperties)
   */
  static function userFilteringAvailable()
  {
    if(!session::is_set('userFilter::userFilteringAvailable')){
      global $config;
      $ldap = $config->get_ldap_link();
      $ocs = $ldap->get_objectclasses();
      session::set('userFilter::userFilteringAvailable', isset($ocs['gosaProperties']));
    }
    return(session::get('userFilter::userFilteringAvailable'));
  }
  
 
  /*! \brief  Initiates the filter editing dialog.
   */ 
  function __construct($config, $listing)
  {
    // Initialize this plugin with the users dn to gather user defined filters.
    $ui = get_userinfo();
    plugin::plugin($config, $ui->dn);
    $this->listing = &$listing;
    $filter= $this->listing->getFilter();
    $this->fixedFilter = $filter->getFixedFilters();

    // Keep list of currently managed categories.
    $this->availableCategories = array_unique($this->listing->categories);

    // Load list of filters
    if(isset($this->attrs['gosaUserDefinedFilter'])){
      for($i=0; $i< $this->attrs['gosaUserDefinedFilter']['count']; $i++){
        $tmp = userFilter::explodeFilterString($this->attrs['gosaUserDefinedFilter'][$i]);
        if(isset($tmp['name'])){
          $this->filters[$tmp['name']]= $tmp; 
        }
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

  
  /*! \brief    Parses a filter string into an array.
   */
  static function explodeFilterString($filter)
  {
    list($parent,$categories, $name, $description, $filter, $flags) = split(";", $filter);

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
        'parent' => $parent, 
        'name' => $name, 
        'categories' => $categories,
        'description' => base64_decode($description),
        'filter' => base64_decode($filter),
        'flags' => $flags);

    return($tmp);
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
            $filter['parent'],
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
        $this->dialog=new userFilterEditor($this->filters[$key], $this->availableCategories, $this->fixedFilter);
      }
    }

    // Act on new requests
    if(isset($_POST['addFilter'])){
      $this->dialog=new userFilterEditor(array(), $this->availableCategories, $this->fixedFilter);
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


  /*! \brief    Returns user defined filter for a given list of categories,
   *             if no categories were specified all enabled filters will be returned.
   */
  static function getFilter($category=array())
  {
    global $config;
    $ldap=$config->get_ldap_link();
    $ui = get_userinfo();
    $ldap->cd($config->current['BASE']);
    $ldap->search("(&(objectClass=gosaProperties)(gosaUserDefinedFilter=*))",array('gosaUserDefinedFilter'));
    $filter = array();
    while($attrs = $ldap->fetch()){
      for($i=0; $i < $attrs['gosaUserDefinedFilter']['count']; $i++){
        $tmp = userFilter::explodeFilterString($attrs['gosaUserDefinedFilter'][$i]);
        if(!isset($tmp['name'])) continue;
          
        // Remove line breaks from the filter, which may were added for better reading. 
        $c = preg_split('/\n/',$tmp['filter']);

        foreach($c as $key => $str) $c[$key] = trim($str);
        $tmp['filter'] = implode($c);
 
        // The filter is visible if it is shared or if is one of our own creations.
        //  ... and enabled.
        $visible = in_array('enable', $tmp['flags']) && 
          ($attrs['dn'] == $ui->dn || in_array('share', $tmp['flags']));
         
        // Convert filter encoding
        $tmp['filter'] = mb_convert_encoding($tmp['filter'], 'UTF-8');
 
        // Add filter if it matches the category list
        if($visible && (count($category) == 0 || array_intersect($category, $tmp['categories']))){ 
          $filter[$tmp['name']] = $tmp;
        }
      }
    }
    return($filter);
  }


  /*! \brief    Write user-filter modifications back to the ldap.  
   */
  function save()
  {
    // Build up new list of filters 
    $attrs = array();
    foreach($this->filters as $filter){
      $tmp = $filter['parent'].";";
      $tmp.= implode(',', $filter['categories']).";";
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
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }
  }  

  
  /*! \brief    Do not save any posted values here.
   */
  function save_object(){}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
