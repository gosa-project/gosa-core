<?php


class commandVerifier
{
    private $property = NULL;
    private $config = NULL;

    private $command = "";
    
    function __construct($config, $property)
    {
        $this->config = &$config;
        $this->property = &$property;
        $this->command  = $this->property->getValue(TRUE);
    }

    function execute()
    {
        $smarty = get_smarty();      
        
        $output= "";

        if(isset($_POST['execute'])){
            exec ($this->command, $ret,$code);
            $ret = implode($ret,'\n');
            $output = "code: {$code}\nreturn: {$ret}";
        }

  
        $smarty->assign('value', htmlentities($this->command,ENT_COMPAT,'UTF-8'));
        $smarty->assign('output', $output);
        return($smarty->fetch(get_template_path('commandVerifier.tpl', 'TRUE')));
    }

    function save_object()
    {
        if(isset($_POST['command'])) $this->command = get_post('command');
    }

    function save()
    {
        $this->property->setValue($this->command);
    }
}


?>
