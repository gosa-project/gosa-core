<?php

class gotoLpdEnable extends plugin
{
    public $parent;

    protected $DevID = 0;
    private $data = array();
    private $default_entry = array();

    protected $s_Type = "U";
    protected $s_Device = "/dev/usb/lp";
    protected $i_Port = 9100;   
    protected $s_Options = "";
    protected $s_WriteOnly = "Y";

    protected $s_Speed = 9600;
    protected $s_FlowControl = "S";
    protected $s_Parity = "E";
    protected $i_Bit = 5;

    /* Option predefinition */
    private $a_Types = array();
    private $a_FlowControls = array(); //  
    private $a_Parities = array();    //  
    private $a_Speeds = array();      // 
    private $a_Bits = array();        //
    private $a_Devices = array();
    public    $attributes = array("s_Type","s_Device","i_Port","s_Options","s_WriteOnly",
            "s_Speed","s_FlowControl","s_Parity","i_Bit");

    /*! \brief  Create gotoLpdEnable dialog, allows to edit printer settings.
      @param  Object  $config  The GOsa configuration object.
      @param  Array   $data    The values for attribute 'gotoLpdEnable'
      e.g.     array("U:/dev/usb/lp0:9100:9600:S:E:0:Y:options",
      "U:/dev/usb/lp1:9101:9600:S:E:5:Y:", ...)
      @param  Object  $parent  A reference to the parent plugin-object. 
      @return Object  gotoLpdEnable (plugin) 
     */
    public function __construct($config,$data,&$parent)
    {
        plugin::__construct($config,NULL);
        $this->parent = $parent;
        $this->DevID = 0;

        /* Predefine selectable values 
         */ 
        $this->a_Types = array(
                "U" => _("USB"),
                "P" => _("Parallel port"),
                "S" => _("Serial"));
        $this->a_FlowControl = array(
                "S" => _("Software"),
                "H" => _("Hardware"));
        $this->a_Parities = array(
                "E"  => _("Even"),
                "O"  => _("Odd"),
                "N"  => _("None"));
        $this->a_Speeds = array(
                "4800"  => "4.800&nbsp;"._("bit/s"),
                "9600"  => "9.600&nbsp;"._("bit/s"),
                "19200" => "19.200&nbsp;"._("bit/s"),
                "38400" => "38.400&nbsp;"._("bit/s"),
                "57600" => "57.600&nbsp;"._("bit/s"),
                "115200"=> "115.200&nbsp;"._("bit/s"),
                "230400"=> "230.400&nbsp;"._("bit/s"),
                "460800"=> "460.800&nbsp;"._("bit/s"));
        $this->a_Bits = array(
                5,6,7,8);
        $this->a_Devices = array(
                "U" => "/dev/usb/lp",
                "P" => "/dev/lp",
                "S" => "/dev/ttyS");

        /* Create default entry 
         */
        foreach($this->attributes as $attr){
            $this->default_entry[$attr] = $this->$attr;
        }

        /* Load current settings 
         */
        if(count($data) == 0){
            $this->is_account = FALSE;
        }else{
            foreach($data as $dat){
                if(substr_count($dat,":") < 8) continue;
                list($s_Type,$s_Device,$i_Port,$s_Speed,$s_FlowControl,$s_Parity,$i_Bit,$s_WriteOnly,$s_Options) = explode(":",$dat);
                $entry = array();
                foreach($this->attributes as $attr){
                    $entry[$attr] = $$attr;
                }
                $this->data[] = $entry;
            }

            /* Set first entry values 
               From "$this->data[0]" to "$this->"
             */
            if(count($this->data)){
                foreach($this->attributes as $attr){
                    $this->$attr = $this->data[$this->DevID][$attr];
                }
                $this->is_account = TRUE;
            }      
        }
        $this->initially_was_account = $this->is_account;
    }


    /*! \brief  Create HTML output of this plugin.
      Depending on the current plugin status, there is a 
      'create' and a 'remove'account button displayed on top
      followed by the gotoLpdEnable options.
      @param  .
      @return String  HTML content
     */
    public function execute()
    {
        plugin::execute();

        $display = "";

        /* Set smarty variables 
         */
        $smarty = get_smarty();
        $smarty->assign("acl",$this->parent->getacl("gotoLpdEnable"));
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }
        foreach(array("DevID","a_Types","a_FlowControl","a_Parities","a_Speeds","a_Bits") as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }
        ksort($this->data);
        $smarty->assign("data_cnt",count($this->data));
        $smarty->assign("data",$this->data);
        $smarty->assign("a_DevIDs",array_keys($this->data));
        $smarty->assign("is_account",$this->is_account);
        return($display.$smarty->fetch(get_template_path("gotoLpdEnable.tpl",TRUE,dirname(__FILE__))));
    }


    /*! \brief  Checks the given informations and returns an array 
      with the error messages or an empty array if everything went fine.
      @param  .
      @return Array of Strings  Error messages.
     */
    public function check()
    {
        $messages = plugin::check();
        if(!$this->is_account){
            return($messages);
        }
        foreach($this->data as $id => $entry){
            if(!tests::is_id($entry['i_Port'])){
                $messages[] = msgPool::invalid(_("Port"));
            }
            if(preg_match("/:/",$entry['s_Options'])){
                $messages[] = msgPool::invalid(_("Options"));
            }
        }
        return($messages);
    }  


    /*! \brief  Save all ui inputs. 
      @param  .
      @return .
     */
    public function save_object()
    {
        if(!$this->parent->acl_is_writeable("gotoLpdEnable")){
            return;
        }

        if(!isset($_POST['gotoLpdEnable_entry_posted'])){
            return;
        }

        /* Handle account add/remove 
         */
        if(isset($_POST['gotoLpdEnable_enabled'])){
            $this->is_account = TRUE;
        }else{
            $this->is_account = FALSE;
        }

        /* Check if we have to propose device settings 
           (current device info is empty and the printer type has changed)
         */
        $propose_device = "";
        if(isset($_POST['s_Device']) && empty($_POST['s_Device']) && 
                isset($_POST['s_Type']) && $_POST['s_Type'] != $this->s_Type){
            $propose_device = $this->a_Devices[get_post('s_Type')].$this->DevID;
        }

        plugin::save_object();

        if(!empty($propose_device)){
            $this->s_Device = $propose_device;
        }

        /* Get checkbox changes 
         */
        if(isset($_POST['s_WriteOnly'])){
            $this->s_WriteOnly = "Y";
        }else{
            $this->s_WriteOnly = "N";
        }

        /* Write back attributes to data array ($this->data)
         */
        foreach($this->attributes as $attr){
            $this->data[$this->DevID][$attr] = $this->$attr;
        } 
    }


    /*! \brief  Creates an array containing all gotoLpdEnabled values,
      see constructor for more details.
      @param  .
      @return Array of gotoLpdEnabled Strings.
     */
    public function save()
    {
        $ret = array();
        $serial_only = array("s_Speed","s_FlowControl","s_Parity","i_Bit");
        $attrs = array("s_Type","s_Device","i_Port","s_Speed","s_FlowControl","s_Parity","i_Bit","s_WriteOnly","s_Options"); 
        foreach($this->data as $entry){
            $str = "";
            foreach($attrs as $attr){
                if(in_array_strict($attr,$serial_only) && $entry['s_Type'] != "Serial"){
                    $str .= ":";
                }else{
                    $str .= $entry[$attr].":";
                }
            }
            $ret[] = preg_replace("/:$/","",$str);
        }
        return($ret);
    }


    /*! \brief  .  
      @param  .
      @return .
     */
    public function acl_is_writeable($attr,$skip_write = FALSE)
    { 
        return(TRUE);
    }


    /*! \brief  .  
      @param  .
      @return .
     */
    public function acl_is_removeable($skip_write = FALSE)
    { 
        return(TRUE);
    }


    /*! \brief  .  
      @param  .
      @return .
     */
    public function acl_is_createable($skip_write = FALSE)
    { 
        return(TRUE);
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
