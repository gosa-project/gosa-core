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


            $descriptorSpec = array(0 => array("pipe", "r"),
                    1 => array('pipe', 'w'),
                    2 => array('pipe', 'w'));
            $process = proc_open($this->command, $descriptorSpec, $pipes);
            $txOff = 0; $txLen = strlen($stdin);

            $stdout = ''; $stdoutDone = FALSE;
            $stderr = ''; $stderrDone = FALSE;
            stream_set_blocking($pipes[0], 0); // Make stdin/stdout/stderr non-blocking
            stream_set_blocking($pipes[1], 0);
            stream_set_blocking($pipes[2], 0);
            if ($txLen == 0) fclose($pipes[0]);
            while (TRUE) {
                $rx = array(); // The program's stdout/stderr
                if (!$stdoutDone) $rx[] = $pipes[1];
                if (!$stderrDone) $rx[] = $pipes[2];
                foreach ($rx as $r) {
                    if ($r == $pipes[1]) {
                        $stdout .= fread($pipes[1], 8192);
                        if (feof($pipes[1])) { fclose($pipes[1]); $stdoutDone = TRUE; }
                    } else if ($r == $pipes[2]) {
                        $stderr .= fread($pipes[2], 8192);
                        if (feof($pipes[2])) { fclose($pipes[2]); $stderrDone = TRUE; }
                    }
                }
                if (!is_resource($process)) break;
                if ($txOff >= $txLen && $stdoutDone && $stderrDone) break;
            }
            $code = proc_close($process);

            if(!empty($stdout)) $stdout = "<pre>{$stdout}</pre>";
            if(!empty($stdout)) $stdout = "<pre>{$stdout}</pre>";
            $output = "
                <table summary='"._("Results")."'>
                <tr><td><b>Result:</b></td><td>$stdout</td></tr>
                <tr><td><b>Error:</b></td><td>$stdout</td></tr>
                <tr><td><b>Return code:</b></td><td>$code</td></tr>
                </table>";
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
