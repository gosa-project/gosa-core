<?php
/**
* Smarty Internal Plugin Configfilelexer
*
* This is the lexer to break the config file source into tokens
<<<<<<< HEAD
=======
 *
>>>>>>> gosa-core_v2.8
* @package Smarty
* @subpackage Config
* @author Uwe Tews
*/
<<<<<<< HEAD
/**
* Smarty Internal Plugin Configfilelexer
*/
class Smarty_Internal_Configfilelexer
{

    public $data;
    public $counter;
    public $token;
    public $value;
    public $node;
    public $line;
    private $state = 1;
    public $smarty_token_names = array (		// Text for parser error messages
   				);


    function __construct($data, $smarty)
    {
        // set instance object
        self::instance($this);
        $this->data = $data . "\n"; //now all lines are \n-terminated
        $this->counter = 0;
        $this->line = 1;
        $this->smarty = $smarty;
        $this->mbstring_overload = ini_get('mbstring.func_overload') & 2;
    }
    public static function &instance($new_instance = null)
    {
        static $instance = null;
        if (isset($new_instance) && is_object($new_instance))
            $instance = $new_instance;
        return $instance;
    }



    private $_yy_state = 1;
    private $_yy_stack = array();

    function yylex()
=======

/**
 * Smarty_Internal_Configfilelexer
 *
 * This is the config file lexer.
 * It is generated from the smarty_internal_configfilelexer.plex file
 *
 * @package    Smarty
 * @subpackage Compiler
 * @author     Uwe Tews
 */
class Smarty_Internal_Configfilelexer
{
    const START              = 1;
    const VALUE              = 2;
    const NAKED_STRING_VALUE = 3;
    const COMMENT            = 4;
    const SECTION            = 5;
    const TRIPPLE            = 6;

    /**
     * Source
     *
     * @var string
     */
    public $data;

    /**
     * Source length
     *
     * @var int
     */
    public $dataLength = null;

    /**
     * byte counter
     *
     * @var int
     */
    public $counter;

    /**
     * token number
     *
     * @var int
     */
    public $token;

    /**
     * token value
     *
     * @var string
     */
    public $value;

    /**
     * current line
     *
     * @var int
     */
    public $line;

    /**
     * state number
     *
     * @var int
     */
    public $state = 1;

    /**
     * Smarty object
     *
     * @var Smarty
     */
    public $smarty = null;

    /**
     * trace file
     *
     * @var resource
     */
    public $yyTraceFILE;

    /**
     * trace prompt
     *
     * @var string
     */
    public $yyTracePrompt;

    /**
     * state names
     *
     * @var array
     */
    public $state_name = array(
        1 => 'START', 2 => 'VALUE', 3 => 'NAKED_STRING_VALUE', 4 => 'COMMENT', 5 => 'SECTION', 6 => 'TRIPPLE'
    );

    /**
     * token names
     *
     * @var array
     */
    public $smarty_token_names = array(        // Text for parser error messages
    );

    /**
     * compiler object
     *
     * @var Smarty_Internal_Config_File_Compiler
     */
    private $compiler = null;

    /**
     * copy of config_booleanize
     *
     * @var bool
     */
    private $configBooleanize = false;

    /**
     * storage for assembled token patterns
     *
     * @var string
     */
    private $yy_global_pattern1 = null;

    private $yy_global_pattern2 = null;

    private $yy_global_pattern3 = null;

    private $yy_global_pattern4 = null;

    private $yy_global_pattern5 = null;

    private $yy_global_pattern6 = null;

    private $_yy_state          = 1;

    private $_yy_stack          = array();

    /**
     * constructor
     *
     * @param   string                             $data template source
     * @param Smarty_Internal_Config_File_Compiler $compiler
     */
    public function __construct($data, Smarty_Internal_Config_File_Compiler $compiler)
    {
        $this->data = $data . "\n"; //now all lines are \n-terminated
        $this->dataLength = strlen($data);
        $this->counter = 0;
        if (preg_match('/^\xEF\xBB\xBF/', $this->data, $match)) {
            $this->counter += strlen($match[ 0 ]);
        }
        $this->line = 1;
        $this->compiler = $compiler;
        $this->smarty = $compiler->smarty;
        $this->configBooleanize = $this->smarty->config_booleanize;
    }

    public function replace($input)
    {
        return $input;
    } // end function

    public function PrintTrace()
    {
        $this->yyTraceFILE = fopen('php://output', 'w');
        $this->yyTracePrompt = '<br>';
    }

    public function yylex()
>>>>>>> gosa-core_v2.8
    {
        return $this->{'yylex' . $this->_yy_state}();
    }

<<<<<<< HEAD
    function yypushstate($state)
    {
        array_push($this->_yy_stack, $this->_yy_state);
        $this->_yy_state = $state;
    }

    function yypopstate()
    {
        $this->_yy_state = array_pop($this->_yy_stack);
    }

    function yybegin($state)
    {
        $this->_yy_state = $state;
    }




    function yylex1()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 0,
              4 => 0,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(#|;)|\G(\\[)|\G(\\])|\G(=)|\G([ \t\r]+)|\G(\n)|\G([0-9]*[a-zA-Z_]\\w*)|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state START');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r1_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======
    public function yypushstate($state)
    {
        if ($this->yyTraceFILE) {
            fprintf(
                $this->yyTraceFILE,
                "%sState push %s\n",
                $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state
            );
        }
        array_push($this->_yy_stack, $this->_yy_state);
        $this->_yy_state = $state;
        if ($this->yyTraceFILE) {
            fprintf(
                $this->yyTraceFILE,
                "%snew State %s\n",
                $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state
            );
        }
    }

    public function yypopstate()
    {
        if ($this->yyTraceFILE) {
            fprintf(
                $this->yyTraceFILE,
                "%sState pop %s\n",
                $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state
            );
        }
        $this->_yy_state = array_pop($this->_yy_stack);
        if ($this->yyTraceFILE) {
            fprintf(
                $this->yyTraceFILE,
                "%snew State %s\n",
                $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state
            );
        }
    }

    public function yybegin($state)
    {
        $this->_yy_state = $state;
        if ($this->yyTraceFILE) {
            fprintf(
                $this->yyTraceFILE,
                "%sState set %s\n",
                $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state
            );
        }
    }

    public function yylex1()
    {
        if (!isset($this->yy_global_pattern1)) {
            $this->yy_global_pattern1 =
                $this->replace("/\G(#|;)|\G(\\[)|\G(\\])|\G(=)|\G([ \t\r]+)|\G(\n)|\G([0-9]*[a-zA-Z_]\\w*)|\G([\S\s])/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern1, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state START');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r1_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const START = 1;
    function yy_r1_1($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_COMMENTSTART;
    $this->yypushstate(self::COMMENT);
    }
    function yy_r1_2($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_OPENB;
    $this->yypushstate(self::SECTION);
    }
    function yy_r1_3($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_CLOSEB;
    }
    function yy_r1_4($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_EQUAL;
    $this->yypushstate(self::VALUE);
    }
    function yy_r1_5($yy_subpatterns)
    {

    return false;
    }
    function yy_r1_6($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_NEWLINE;
    }
    function yy_r1_7($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_ID;
    }
    function yy_r1_8($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_OTHER;
    }



    function yylex2()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 0,
              4 => 0,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
              9 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G([ \t\r]+)|\G(\\d+\\.\\d+(?=[ \t\r]*[\n#;]))|\G(\\d+(?=[ \t\r]*[\n#;]))|\G(\"\"\")|\G('[^'\\\\]*(?:\\\\.[^'\\\\]*)*'(?=[ \t\r]*[\n#;]))|\G(\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"(?=[ \t\r]*[\n#;]))|\G([a-zA-Z]+(?=[ \t\r]*[\n#;]))|\G([^\n]+?(?=[ \t\r]*\n))|\G(\n)/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state VALUE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r2_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======
    }

    public function yy_r1_1()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_COMMENTSTART;
        $this->yypushstate(self::COMMENT);
    }

    public function yy_r1_2()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_OPENB;
        $this->yypushstate(self::SECTION);
    }

    public function yy_r1_3()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_CLOSEB;
    }

    public function yy_r1_4()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_EQUAL;
        $this->yypushstate(self::VALUE);
    } // end function

    public function yy_r1_5()
    {
        return false;
    }

    public function yy_r1_6()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_NEWLINE;
    }

    public function yy_r1_7()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_ID;
    }

    public function yy_r1_8()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_OTHER;
    }

    public function yylex2()
    {
        if (!isset($this->yy_global_pattern2)) {
            $this->yy_global_pattern2 =
                $this->replace("/\G([ \t\r]+)|\G(\\d+\\.\\d+(?=[ \t\r]*[\n#;]))|\G(\\d+(?=[ \t\r]*[\n#;]))|\G(\"\"\")|\G('[^'\\\\]*(?:\\\\.[^'\\\\]*)*'(?=[ \t\r]*[\n#;]))|\G(\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"(?=[ \t\r]*[\n#;]))|\G([a-zA-Z]+(?=[ \t\r]*[\n#;]))|\G([^\n]+?(?=[ \t\r]*\n))|\G(\n)/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern2, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state VALUE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r2_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const VALUE = 2;
    function yy_r2_1($yy_subpatterns)
    {

    return false;
    }
    function yy_r2_2($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_FLOAT;
    $this->yypopstate();
    }
    function yy_r2_3($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_INT;
    $this->yypopstate();
    }
    function yy_r2_4($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_TRIPPLE_QUOTES;
    $this->yypushstate(self::TRIPPLE);
    }
    function yy_r2_5($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_SINGLE_QUOTED_STRING;
    $this->yypopstate();
    }
    function yy_r2_6($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_DOUBLE_QUOTED_STRING;
    $this->yypopstate();
    }
    function yy_r2_7($yy_subpatterns)
    {

    if (!$this->smarty->config_booleanize || !in_array(strtolower($this->value), Array("true", "false", "on", "off", "yes", "no")) ) {
=======
    }

    public function yy_r2_1()
    {
        return false;
    }

    public function yy_r2_2()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_FLOAT;
        $this->yypopstate();
    }

    public function yy_r2_3()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_INT;
        $this->yypopstate();
    }

    public function yy_r2_4()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_TRIPPLE_QUOTES;
        $this->yypushstate(self::TRIPPLE);
    }

    public function yy_r2_5()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_SINGLE_QUOTED_STRING;
        $this->yypopstate();
    }

    public function yy_r2_6()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_DOUBLE_QUOTED_STRING;
        $this->yypopstate();
    } // end function

    public function yy_r2_7()
    {
        if (!$this->configBooleanize ||
            !in_array(strtolower($this->value), array('true', 'false', 'on', 'off', 'yes', 'no'))) {
>>>>>>> gosa-core_v2.8
        $this->yypopstate();
        $this->yypushstate(self::NAKED_STRING_VALUE);
        return true; //reprocess in new state
    } else {
        $this->token = Smarty_Internal_Configfileparser::TPC_BOOL;
        $this->yypopstate();
    }
    }
<<<<<<< HEAD
    function yy_r2_8($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
    $this->yypopstate();
    }
    function yy_r2_9($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
    $this->value = "";
    $this->yypopstate();
    }



    function yylex3()
    {
        $tokenMap = array (
              1 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G([^\n]+?(?=[ \t\r]*\n))/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state NAKED_STRING_VALUE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r3_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======

    public function yy_r2_8()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
        $this->yypopstate();
    }

    public function yy_r2_9()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
        $this->value = '';
        $this->yypopstate();
    } // end function

    public function yylex3()
    {
        if (!isset($this->yy_global_pattern3)) {
            $this->yy_global_pattern3 = $this->replace("/\G([^\n]+?(?=[ \t\r]*\n))/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern3, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state NAKED_STRING_VALUE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r3_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const NAKED_STRING_VALUE = 3;
    function yy_r3_1($yy_subpatterns)
    {

=======
    }

    public function yy_r3_1()
    {
>>>>>>> gosa-core_v2.8
    $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
    $this->yypopstate();
    }

<<<<<<< HEAD


    function yylex4()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G([ \t\r]+)|\G([^\n]+?(?=[ \t\r]*\n))|\G(\n)/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state COMMENT');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r4_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======
    public function yylex4()
    {
        if (!isset($this->yy_global_pattern4)) {
            $this->yy_global_pattern4 = $this->replace("/\G([ \t\r]+)|\G([^\n]+?(?=[ \t\r]*\n))|\G(\n)/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern4, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state COMMENT');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r4_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const COMMENT = 4;
    function yy_r4_1($yy_subpatterns)
    {

    return false;
    }
    function yy_r4_2($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
    }
    function yy_r4_3($yy_subpatterns)
    {

=======
    }

    public function yy_r4_1()
    {
        return false;
    }

    public function yy_r4_2()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_NAKED_STRING;
    } // end function

    public function yy_r4_3()
    {
>>>>>>> gosa-core_v2.8
    $this->token = Smarty_Internal_Configfileparser::TPC_NEWLINE;
    $this->yypopstate();
    }

<<<<<<< HEAD


    function yylex5()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(\\.)|\G(.*?(?=[\.=[\]\r\n]))/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state SECTION');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r5_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======
    public function yylex5()
    {
        if (!isset($this->yy_global_pattern5)) {
            $this->yy_global_pattern5 = $this->replace("/\G(\\.)|\G(.*?(?=[\.=[\]\r\n]))/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern5, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state SECTION');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r5_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const SECTION = 5;
    function yy_r5_1($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_DOT;
    }
    function yy_r5_2($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Configfileparser::TPC_SECTION;
    $this->yypopstate();
    }


    function yylex6()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(\"\"\"(?=[ \t\r]*[\n#;]))|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state TRIPPLE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r6_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
=======
    }

    public function yy_r5_1()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_DOT;
    }

    public function yy_r5_2()
    {
        $this->token = Smarty_Internal_Configfileparser::TPC_SECTION;
        $this->yypopstate();
    } // end function

    public function yylex6()
    {
        if (!isset($this->yy_global_pattern6)) {
            $this->yy_global_pattern6 = $this->replace("/\G(\"\"\"(?=[ \t\r]*[\n#;]))|\G([\S\s])/isS");
        }
        if (!isset($this->dataLength)) {
            $this->dataLength = strlen($this->data);
        }
        if ($this->counter >= $this->dataLength) {
            return false; // end of input
        }
        do {
            if (preg_match($this->yy_global_pattern6, $this->data, $yymatches, 0, $this->counter)) {
                if (!isset($yymatches[ 0 ][ 1 ])) {
                    $yymatches = preg_grep("/(.|\s)+/", $yymatches);
                } else {
                    $yymatches = array_filter($yymatches);
                }
                if (empty($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                                        ' an empty string.  Input "' . substr(
                                            $this->data,
                                            $this->counter,
                                            5
                                        ) . '... state TRIPPLE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r6_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
>>>>>>> gosa-core_v2.8
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
<<<<<<< HEAD
                    $this->counter += ($this->mbstring_overload ? mb_strlen($this->value,'latin1'): strlen($this->value));
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
=======
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
>>>>>>> gosa-core_v2.8
                        return false; // end of input
                    }
                    // skip this token
                    continue;
<<<<<<< HEAD
                }            } else {
=======
                }
            } else {
>>>>>>> gosa-core_v2.8
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->counter]);
            }
            break;
        } while (true);
<<<<<<< HEAD

    } // end function


    const TRIPPLE = 6;
    function yy_r6_1($yy_subpatterns)
    {

=======
    }

    public function yy_r6_1()
    {
>>>>>>> gosa-core_v2.8
    $this->token = Smarty_Internal_Configfileparser::TPC_TRIPPLE_QUOTES_END;
    $this->yypopstate();
    $this->yypushstate(self::START);
    }
<<<<<<< HEAD
    function yy_r6_2($yy_subpatterns)
    {

  if ($this->mbstring_overload) {
    $to = mb_strlen($this->data,'latin1');
  } else {
    $to = strlen($this->data);
  }
=======

    public function yy_r6_2()
    {
        $to = strlen($this->data);
>>>>>>> gosa-core_v2.8
  preg_match("/\"\"\"[ \t\r]*[\n#;]/",$this->data,$match,PREG_OFFSET_CAPTURE,$this->counter);
  if (isset($match[0][1])) {
    $to = $match[0][1];
  } else {
<<<<<<< HEAD
    $this->compiler->trigger_template_error ("missing or misspelled literal closing tag");
  }
  if ($this->mbstring_overload) {
    $this->value = mb_substr($this->data,$this->counter,$to-$this->counter,'latin1');
  } else {
    $this->value = substr($this->data,$this->counter,$to-$this->counter);
  }
  $this->token = Smarty_Internal_Configfileparser::TPC_TRIPPLE_TEXT;
    }


}
?>
=======
            $this->compiler->trigger_config_file_error('missing or misspelled literal closing tag');
        }
        $this->value = substr($this->data, $this->counter, $to - $this->counter);
        $this->token = Smarty_Internal_Configfileparser::TPC_TRIPPLE_TEXT;
    }
}
>>>>>>> gosa-core_v2.8
