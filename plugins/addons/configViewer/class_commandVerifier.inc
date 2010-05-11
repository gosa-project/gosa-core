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
            $descriptorspec = array(0 => array("pipe", "w"),1 => array("pipe", "w"), 2 => array("pipe", "w"));
            $process = proc_open($this->command, $descriptorspec, $pipes);
            if (is_resource($process)) {
                $send= stream_get_contents($pipes[0]);
                $res = trim(htmlentities(stream_get_contents($pipes[1]),ENT_COMPAT,'UTF-8'));
                $err = trim(htmlentities(stream_get_contents($pipes[2]),ENT_COMPAT,'UTF-8'));
                fclose($pipes[0]);
                fclose($pipes[1]);
                fclose($pipes[2]);
                $code = proc_close($process);

                if(!empty($err)) $err = "<pre>{$err}</pre>";
                if(!empty($res)) $res = "<pre>{$res}</pre>";

                $output = "
                    <table summary='"._("Results")."'>
                    <tr><td><b>Result:</b></td><td>$res</td></tr>
                    <tr><td><b>Error:</b></td><td>$err</td></tr>
                    <tr><td><b>Return code:</b></td><td>$code</td></tr>
                    </table>";
            }
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
