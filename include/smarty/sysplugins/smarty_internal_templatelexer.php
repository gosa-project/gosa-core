<?php
<<<<<<< HEAD
/**
* Smarty Internal Plugin Templatelexer
*
* This is the lexer to break the template source into tokens
* @package Smarty
* @subpackage Compiler
* @author Uwe Tews
*/
/**
* Smarty Internal Plugin Templatelexer
*/
class Smarty_Internal_Templatelexer
{
    public $data;
    public $counter;
    public $token;
    public $value;
    public $node;
    public $line;
    public $taglineno;
    public $state = 1;
    private $heredoc_id_stack = Array();
    public $smarty_token_names = array (		// Text for parser error messages
    				'IDENTITY'	=> '===',
    				'NONEIDENTITY'	=> '!==',
    				'EQUALS'	=> '==',
    				'NOTEQUALS'	=> '!=',
    				'GREATEREQUAL' => '(>=,ge)',
    				'LESSEQUAL' => '(<=,le)',
    				'GREATERTHAN' => '(>,gt)',
    				'LESSTHAN' => '(<,lt)',
    				'MOD' => '(%,mod)',
    				'NOT'			=> '(!,not)',
    				'LAND'		=> '(&&,and)',
    				'LOR'			=> '(||,or)',
    				'LXOR'			=> 'xor',
=======
/*
 * This file is part of Smarty.
 *
 * (c) 2015 Uwe Tews
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Smarty_Internal_Templatelexer
 * This is the template file lexer.
 * It is generated from the smarty_internal_templatelexer.plex file
 *
 *
 * @author Uwe Tews <uwe.tews@googlemail.com>
 */
class Smarty_Internal_Templatelexer
{
    const TEXT               = 1;
    const TAG                = 2;
    const TAGBODY            = 3;
    const LITERAL            = 4;
    const DOUBLEQUOTEDSTRING = 5;

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
     * tag start line
     *
     * @var
     */
    public $taglineno;

    /**
     * php code type
     *
     * @var string
     */
    public $phpType = '';

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
     * compiler object
     *
     * @var Smarty_Internal_TemplateCompilerBase
     */
    public $compiler = null;

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
     * XML flag true while processing xml
     *
     * @var bool
     */
    public $is_xml = false;

    /**
     * state names
     *
     * @var array
     */
    public $state_name = array(1 => 'TEXT', 2 => 'TAG', 3 => 'TAGBODY', 4 => 'LITERAL', 5 => 'DOUBLEQUOTEDSTRING',);

    /**
     * token names
     *
     * @var array
     */
    public $smarty_token_names = array(        // Text for parser error messages
                                               'NOT'         => '(!,not)',
>>>>>>> gosa-core_v2.8
    				'OPENP'		=> '(',
    				'CLOSEP'	=> ')',
    				'OPENB'		=> '[',
    				'CLOSEB'	=> ']',
    				'PTR'			=> '->',
    				'APTR'		=> '=>',
    				'EQUAL'		=> '=',
    				'NUMBER'	=> 'number',
    				'UNIMATH'	=> '+" , "-',
    				'MATH'		=> '*" , "/" , "%',
    				'INCDEC'	=> '++" , "--',
    				'SPACE'		=> ' ',
    				'DOLLAR'	=> '$',
    				'SEMICOLON' => ';',
    				'COLON'		=> ':',
    				'DOUBLECOLON'		=> '::',
    				'AT'		=> '@',
    				'HATCH'		=> '#',
    				'QUOTE'		=> '"',
    				'BACKTICK'		=> '`',
<<<<<<< HEAD
    				'VERT'		=> '|',
    				'DOT'			=> '.',
    				'COMMA'		=> '","',
    				'ANDSYM'		=> '"&"',
    				'QMARK'		=> '"?"',
    				'ID'			=> 'identifier',
    				'TEXT'		=> 'text',
     				'FAKEPHPSTARTTAG'	=> 'Fake PHP start tag',
     				'PHPSTARTTAG'	=> 'PHP start tag',
     				'PHPENDTAG'	=> 'PHP end tag',
 						'LITERALSTART'  => 'Literal start',
 						'LITERALEND'    => 'Literal end',
    				'LDELSLASH' => 'closing tag',
    				'COMMENT' => 'comment',
    				'AS' => 'as',
    				'TO' => 'to',
    				);


    function __construct($data,$compiler)
    {
//        $this->data = preg_replace("/(\r\n|\r|\n)/", "\n", $data);
        $this->data = $data;
        $this->counter = 0;
        $this->line = 1;
        $this->smarty = $compiler->smarty;
        $this->compiler = $compiler;
        $this->ldel = preg_quote($this->smarty->left_delimiter,'/');
        $this->ldel_length = strlen($this->smarty->left_delimiter);
        $this->rdel = preg_quote($this->smarty->right_delimiter,'/');
        $this->rdel_length = strlen($this->smarty->right_delimiter);
        $this->smarty_token_names['LDEL'] =	$this->smarty->left_delimiter;
        $this->smarty_token_names['RDEL'] =	$this->smarty->right_delimiter;
        $this->mbstring_overload = ini_get('mbstring.func_overload') & 2;
     }


    private $_yy_state = 1;
    private $_yy_stack = array();

    function yylex()
=======
                                               'VERT'        => '"|" modifier',
                                               'DOT'         => '.',
                                               'COMMA'       => '","',
                                               'QMARK'       => '"?"',
                                               'ID'          => 'id, name',
                                               'TEXT'        => 'text',
                                               'LDELSLASH'   => '{/..} closing tag',
                                               'LDEL'        => '{...} Smarty tag',
                                               'COMMENT'     => 'comment',
                                               'AS'          => 'as',
                                               'TO'          => 'to',
                                               'PHP'         => '"<?php", "<%", "{php}" tag',
                                               'LOGOP'       => '"<", "==" ... logical operator',
                                               'TLOGOP'      => '"lt", "eq" ... logical operator; "is div by" ... if condition',
                                               'SCOND'       => '"is even" ... if condition',
    );

    /**
     * literal tag nesting level
     *
     * @var int
     */
    private $literal_cnt = 0;

    /**
     * preg token pattern for state TEXT
     *
     * @var string
     */
    private $yy_global_pattern1 = null;

    /**
     * preg token pattern for state TAG
     *
     * @var string
     */
    private $yy_global_pattern2 = null;

    /**
     * preg token pattern for state TAGBODY
     *
     * @var string
     */
    private $yy_global_pattern3 = null;

    /**
     * preg token pattern for state LITERAL
     *
     * @var string
     */
    private $yy_global_pattern4 = null;

    /**
     * preg token pattern for state DOUBLEQUOTEDSTRING
     *
     * @var null
     */
    private $yy_global_pattern5 = null;

    /**
     * preg token pattern for text
     *
     * @var null
     */
    private $yy_global_text = null;

    /**
     * preg token pattern for literal
     *
     * @var null
     */
    private $yy_global_literal = null;

    private $_yy_state         = 1;

    private $_yy_stack         = array();

    /**
     * constructor
     *
     * @param   string                             $source template source
     * @param Smarty_Internal_TemplateCompilerBase $compiler
     */
    public function __construct($source, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        $this->data = $source;
        $this->dataLength = strlen($this->data);
        $this->counter = 0;
        if (preg_match('/^\xEF\xBB\xBF/i', $this->data, $match)) {
            $this->counter += strlen($match[ 0 ]);
        }
        $this->line = 1;
        $this->smarty = $compiler->template->smarty;
        $this->compiler = $compiler;
        $this->compiler->initDelimiterPreg();
        $this->smarty_token_names[ 'LDEL' ] = $this->smarty->getLeftDelimiter();
        $this->smarty_token_names[ 'RDEL' ] = $this->smarty->getRightDelimiter();
    }

    /**
     * open lexer/parser trace file
     *
     */
    public function PrintTrace()
    {
        $this->yyTraceFILE = fopen('php://output', 'w');
        $this->yyTracePrompt = '<br>';
    }

    /**
     * replace placeholders with runtime preg  code
     *
     * @param string $preg
     *
     * @return string
     */
    public function replace($preg)
    {
        return $this->compiler->replaceDelimiter($preg);
    }

    /**
     * check if current value is an autoliteral left delimiter
     *
     * @return bool
     */
    public function isAutoLiteral()
    {
        return $this->smarty->getAutoLiteral() && isset($this->value[ $this->compiler->getLdelLength() ]) ?
            strpos(" \n\t\r", $this->value[ $this->compiler->getLdelLength() ]) !== false : false;
    } // end function

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
              3 => 1,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
              9 => 0,
              10 => 0,
              11 => 1,
              13 => 0,
              14 => 0,
              15 => 0,
              16 => 0,
              17 => 0,
              18 => 0,
              19 => 0,
              20 => 0,
              21 => 0,
              22 => 0,
              23 => 0,
              24 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(".$this->ldel."[$]smarty\\.block\\.child".$this->rdel.")|\G(\\{\\})|\G(".$this->ldel."\\*([\S\s]*?)\\*".$this->rdel.")|\G(".$this->ldel."strip".$this->rdel.")|\G(".$this->ldel."\\s{1,}strip\\s{1,}".$this->rdel.")|\G(".$this->ldel."\/strip".$this->rdel.")|\G(".$this->ldel."\\s{1,}\/strip\\s{1,}".$this->rdel.")|\G(".$this->ldel."\\s*literal\\s*".$this->rdel.")|\G(".$this->ldel."\\s{1,}\/)|\G(".$this->ldel."\\s*(if|elseif|else if|while)\\s+)|\G(".$this->ldel."\\s*for\\s+)|\G(".$this->ldel."\\s*foreach(?![^\s]))|\G(".$this->ldel."\\s*setfilter\\s+)|\G(".$this->ldel."\\s{1,})|\G(".$this->ldel."\/)|\G(".$this->ldel.")|\G(<\\?(?:php\\w+|=|[a-zA-Z]+)?)|\G(\\?>)|\G(".$this->rdel.")|\G(<%)|\G(%>)|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
=======
    public function yypushstate($state)
    {
        if ($this->yyTraceFILE) {
            fprintf($this->yyTraceFILE, "%sState push %s\n", $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state);
        }
        array_push($this->_yy_stack, $this->_yy_state);
        $this->_yy_state = $state;
        if ($this->yyTraceFILE) {
            fprintf($this->yyTraceFILE, "%snew State %s\n", $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state);
        }
    }

    public function yypopstate()
    {
        if ($this->yyTraceFILE) {
            fprintf($this->yyTraceFILE, "%sState pop %s\n", $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state);
        }
        $this->_yy_state = array_pop($this->_yy_stack);
        if ($this->yyTraceFILE) {
            fprintf($this->yyTraceFILE, "%snew State %s\n", $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state);
        }
    }

    public function yybegin($state)
    {
        $this->_yy_state = $state;
        if ($this->yyTraceFILE) {
            fprintf($this->yyTraceFILE, "%sState set %s\n", $this->yyTracePrompt,
                isset($this->state_name[ $this->_yy_state ]) ? $this->state_name[ $this->_yy_state ] : $this->_yy_state);
        }
    }

    public function yylex1()
    {
        if (!isset($this->yy_global_pattern1)) {
            $this->yy_global_pattern1 =
                $this->replace("/\G([{][}])|\G((SMARTYldel)SMARTYal[*])|\G((SMARTYldel)SMARTYalphp([ ].*?)?SMARTYrdel|(SMARTYldel)SMARTYal[\/]phpSMARTYrdel)|\G((SMARTYldel)SMARTYautoliteral\\s+SMARTYliteral)|\G((SMARTYldel)SMARTYalliteral\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/]literal\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal)|\G([<][?]((php\\s+|=)|\\s+)|[<][%]|[<][?]xml\\s+|[<]script\\s+language\\s*=\\s*[\"']?\\s*php\\s*[\"']?\\s*[>]|[?][>]|[%][>])|\G([\S\s])/isS");
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
>>>>>>> gosa-core_v2.8
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state TEXT');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
<<<<<<< HEAD
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


    const TEXT = 1;
    function yy_r1_1($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_SMARTYBLOCKCHILD;
    }
    function yy_r1_2($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }
    function yy_r1_3($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_COMMENT;
    }
    function yy_r1_5($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Templateparser::TP_STRIPON;
    }
    function yy_r1_6($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
    $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
    $this->token = Smarty_Internal_Templateparser::TP_STRIPON;
  }
    }
    function yy_r1_7($yy_subpatterns)
    {

    $this->token = Smarty_Internal_Templateparser::TP_STRIPOFF;
    }
    function yy_r1_8($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
    $this->token = Smarty_Internal_Templateparser::TP_STRIPOFF;
  }
    }
    function yy_r1_9($yy_subpatterns)
    {

   $this->token = Smarty_Internal_Templateparser::TP_LITERALSTART;
   $this->yypushstate(self::LITERAL);
    }
    function yy_r1_10($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_11($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELIF;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_13($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOR;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_14($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOREACH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_15($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELSETFILTER;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_16($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r1_17($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r1_18($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r1_19($yy_subpatterns)
    {

  if (in_array($this->value, Array('<?', '<?=', '<?php'))) {
    $this->token = Smarty_Internal_Templateparser::TP_PHPSTARTTAG;
  } elseif ($this->value == '<?xml') {
      $this->token = Smarty_Internal_Templateparser::TP_XMLTAG;
  } else {
    $this->token = Smarty_Internal_Templateparser::TP_FAKEPHPSTARTTAG;
    $this->value = substr($this->value, 0, 2);
  }
     }
    function yy_r1_20($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_PHPENDTAG;
    }
    function yy_r1_21($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }
    function yy_r1_22($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ASPSTARTTAG;
    }
    function yy_r1_23($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ASPENDTAG;
    }
    function yy_r1_24($yy_subpatterns)
    {

  if ($this->mbstring_overload) {
    $to = mb_strlen($this->data,'latin1');
  } else {
    $to = strlen($this->data);
  }
  preg_match("/{$this->ldel}|<\?|\?>|<%|%>/",$this->data,$match,PREG_OFFSET_CAPTURE,$this->counter);
  if (isset($match[0][1])) {
    $to = $match[0][1];
  }
  if ($this->mbstring_overload) {
    $this->value = mb_substr($this->data,$this->counter,$to-$this->counter,'latin1');
  } else {
    $this->value = substr($this->data,$this->counter,$to-$this->counter);
  }
  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }


    function yylex2()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 1,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
              9 => 0,
              10 => 0,
              11 => 0,
              12 => 0,
              13 => 0,
              14 => 0,
              15 => 0,
              16 => 0,
              17 => 0,
              18 => 0,
              19 => 0,
              20 => 1,
              22 => 1,
              24 => 1,
              26 => 0,
              27 => 0,
              28 => 0,
              29 => 0,
              30 => 0,
              31 => 0,
              32 => 0,
              33 => 0,
              34 => 0,
              35 => 0,
              36 => 0,
              37 => 0,
              38 => 0,
              39 => 0,
              40 => 0,
              41 => 0,
              42 => 0,
              43 => 3,
              47 => 0,
              48 => 0,
              49 => 0,
              50 => 0,
              51 => 0,
              52 => 0,
              53 => 0,
              54 => 0,
              55 => 1,
              57 => 1,
              59 => 0,
              60 => 0,
              61 => 0,
              62 => 0,
              63 => 0,
              64 => 0,
              65 => 0,
              66 => 0,
              67 => 0,
              68 => 0,
              69 => 0,
              70 => 0,
              71 => 0,
              72 => 0,
              73 => 0,
              74 => 0,
              75 => 0,
              76 => 0,
              77 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G('[^'\\\\]*(?:\\\\.[^'\\\\]*)*')|\G(".$this->ldel."\\s{1,}\/)|\G(".$this->ldel."\\s*(if|elseif|else if|while)\\s+)|\G(".$this->ldel."\\s*for\\s+)|\G(".$this->ldel."\\s*foreach(?![^\s]))|\G(".$this->ldel."\\s{1,})|\G(\\s{1,}".$this->rdel.")|\G(".$this->ldel."\/)|\G(".$this->ldel.")|\G(".$this->rdel.")|\G(\\s+is\\s+in\\s+)|\G(\\s+as\\s+)|\G(\\s+to\\s+)|\G(\\s+step\\s+)|\G(\\s+instanceof\\s+)|\G(\\s*===\\s*)|\G(\\s*!==\\s*)|\G(\\s*==\\s*|\\s+eq\\s+)|\G(\\s*!=\\s*|\\s*<>\\s*|\\s+(ne|neq)\\s+)|\G(\\s*>=\\s*|\\s+(ge|gte)\\s+)|\G(\\s*<=\\s*|\\s+(le|lte)\\s+)|\G(\\s*>\\s*|\\s+gt\\s+)|\G(\\s*<\\s*|\\s+lt\\s+)|\G(\\s+mod\\s+)|\G(!\\s*|not\\s+)|\G(\\s*&&\\s*|\\s*and\\s+)|\G(\\s*\\|\\|\\s*|\\s*or\\s+)|\G(\\s*xor\\s+)|\G(\\s+is\\s+odd\\s+by\\s+)|\G(\\s+is\\s+not\\s+odd\\s+by\\s+)|\G(\\s+is\\s+odd)|\G(\\s+is\\s+not\\s+odd)|\G(\\s+is\\s+even\\s+by\\s+)|\G(\\s+is\\s+not\\s+even\\s+by\\s+)|\G(\\s+is\\s+even)|\G(\\s+is\\s+not\\s+even)|\G(\\s+is\\s+div\\s+by\\s+)|\G(\\s+is\\s+not\\s+div\\s+by\\s+)|\G(\\((int(eger)?|bool(ean)?|float|double|real|string|binary|array|object)\\)\\s*)|\G(\\s*\\(\\s*)|\G(\\s*\\))|\G(\\[\\s*)|\G(\\s*\\])|\G(\\s*->\\s*)|\G(\\s*=>\\s*)|\G(\\s*=\\s*)|\G(\\+\\+|--)|\G(\\s*(\\+|-)\\s*)|\G(\\s*(\\*|\/|%)\\s*)|\G(\\$)|\G(\\s*;)|\G(::)|\G(\\s*:\\s*)|\G(@)|\G(#)|\G(\")|\G(`)|\G(\\|)|\G(\\.)|\G(\\s*,\\s*)|\G(\\s*&\\s*)|\G(\\s*\\?\\s*)|\G(0[xX][0-9a-fA-F]+)|\G(\\s+[0-9]*[a-zA-Z_][a-zA-Z0-9_\-:]*\\s*=\\s*)|\G([0-9]*[a-zA-Z_]\\w*)|\G(\\d+)|\G(\\s+)|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state SMARTY');
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
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r1_2()
    {
        $to = $this->dataLength;
        preg_match("/[*]{$this->compiler->getRdelPreg()}[\n]?/", $this->data, $match, PREG_OFFSET_CAPTURE,
            $this->counter);
        if (isset($match[ 0 ][ 1 ])) {
            $to = $match[ 0 ][ 1 ] + strlen($match[ 0 ][ 0 ]);
        } else {
            $this->compiler->trigger_template_error("missing or misspelled comment closing tag '{$this->smarty->getRightDelimiter()}'");
        }
        $this->value = substr($this->data, $this->counter, $to - $this->counter);
        return false;
    }

    public function yy_r1_4()
    {
        $this->compiler->getTagCompiler('private_php')->parsePhp($this);
    }

    public function yy_r1_8()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r1_10()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LITERALSTART;
        $this->yypushstate(self::LITERAL);
    }

    public function yy_r1_12()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LITERALEND;
        $this->yypushstate(self::LITERAL);
    } // end function

    public function yy_r1_14()
    {
        $this->yypushstate(self::TAG);
        return true;
    }

    public function yy_r1_16()
    {
        $this->compiler->getTagCompiler('private_php')->parsePhp($this);
    }

    public function yy_r1_19()
    {
        if (!isset($this->yy_global_text)) {
            $this->yy_global_text =
                $this->replace('/(SMARTYldel)SMARTYal|[<][?]((php\s+|=)|\s+)|[<][%]|[<][?]xml\s+|[<]script\s+language\s*=\s*["\']?\s*php\s*["\']?\s*[>]|[?][>]|[%][>]SMARTYliteral/isS');
        }
        $to = $this->dataLength;
        preg_match($this->yy_global_text, $this->data, $match, PREG_OFFSET_CAPTURE, $this->counter);
        if (isset($match[ 0 ][ 1 ])) {
            $to = $match[ 0 ][ 1 ];
        }
        $this->value = substr($this->data, $this->counter, $to - $this->counter);
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yylex2()
    {
        if (!isset($this->yy_global_pattern2)) {
            $this->yy_global_pattern2 =
                $this->replace("/\G((SMARTYldel)SMARTYal(if|elseif|else if|while)\\s+)|\G((SMARTYldel)SMARTYalfor\\s+)|\G((SMARTYldel)SMARTYalforeach(?![^\s]))|\G((SMARTYldel)SMARTYalsetfilter\\s+)|\G((SMARTYldel)SMARTYalmake_nocache\\s+)|\G((SMARTYldel)SMARTYal[0-9]*[a-zA-Z_]\\w*(\\s+nocache)?\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[$]smarty\\.block\\.(child|parent)\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/][0-9]*[a-zA-Z_]\\w*\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[$][0-9]*[a-zA-Z_]\\w*(\\s+nocache)?\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/])|\G((SMARTYldel)SMARTYal)/isS");
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
                                        ' an empty string.  Input "' . substr($this->data,
                            $this->counter, 5) . '... state TAG');
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


    const SMARTY = 2;
    function yy_r2_1($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_SINGLEQUOTESTRING;
    }
    function yy_r2_2($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r2_3($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELIF;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r2_5($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOR;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r2_6($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOREACH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r2_7($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r2_8($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_RDEL;
  $this->yypopstate();
    }
    function yy_r2_9($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r2_10($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r2_11($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_RDEL;
     $this->yypopstate();
    }
    function yy_r2_12($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISIN;
    }
    function yy_r2_13($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_AS;
    }
    function yy_r2_14($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TO;
    }
    function yy_r2_15($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_STEP;
    }
    function yy_r2_16($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_INSTANCEOF;
    }
    function yy_r2_17($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_IDENTITY;
    }
    function yy_r2_18($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_NONEIDENTITY;
    }
    function yy_r2_19($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_EQUALS;
    }
    function yy_r2_20($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_NOTEQUALS;
    }
    function yy_r2_22($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_GREATEREQUAL;
    }
    function yy_r2_24($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LESSEQUAL;
    }
    function yy_r2_26($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_GREATERTHAN;
    }
    function yy_r2_27($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LESSTHAN;
    }
    function yy_r2_28($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_MOD;
    }
    function yy_r2_29($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_NOT;
    }
    function yy_r2_30($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LAND;
    }
    function yy_r2_31($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LOR;
    }
    function yy_r2_32($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LXOR;
    }
    function yy_r2_33($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISODDBY;
    }
    function yy_r2_34($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISNOTODDBY;
    }
    function yy_r2_35($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISODD;
    }
    function yy_r2_36($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISNOTODD;
    }
    function yy_r2_37($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISEVENBY;
    }
    function yy_r2_38($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISNOTEVENBY;
    }
    function yy_r2_39($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISEVEN;
    }
    function yy_r2_40($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISNOTEVEN;
    }
    function yy_r2_41($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISDIVBY;
    }
    function yy_r2_42($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ISNOTDIVBY;
    }
    function yy_r2_43($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TYPECAST;
    }
    function yy_r2_47($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_OPENP;
    }
    function yy_r2_48($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_CLOSEP;
    }
    function yy_r2_49($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_OPENB;
    }
    function yy_r2_50($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_CLOSEB;
    }
    function yy_r2_51($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_PTR;
    }
    function yy_r2_52($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_APTR;
    }
    function yy_r2_53($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_EQUAL;
    }
    function yy_r2_54($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_INCDEC;
    }
    function yy_r2_55($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_UNIMATH;
    }
    function yy_r2_57($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_MATH;
    }
    function yy_r2_59($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_DOLLAR;
    }
    function yy_r2_60($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_SEMICOLON;
    }
    function yy_r2_61($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_DOUBLECOLON;
    }
    function yy_r2_62($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_COLON;
    }
    function yy_r2_63($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_AT;
    }
    function yy_r2_64($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_HATCH;
    }
    function yy_r2_65($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_QUOTE;
  $this->yypushstate(self::DOUBLEQUOTEDSTRING);
    }
    function yy_r2_66($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_BACKTICK;
  $this->yypopstate();
    }
    function yy_r2_67($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_VERT;
    }
    function yy_r2_68($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_DOT;
    }
    function yy_r2_69($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_COMMA;
    }
    function yy_r2_70($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ANDSYM;
    }
    function yy_r2_71($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_QMARK;
    }
    function yy_r2_72($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_HEX;
    }
    function yy_r2_73($yy_subpatterns)
    {

  // resolve conflicts with shorttag and right_delimiter starting with '='
  if (substr($this->data, $this->counter + strlen($this->value) - 1, $this->rdel_length) == $this->smarty->right_delimiter) {
     preg_match("/\s+/",$this->value,$match);
     $this->value = $match[0];
     $this->token = Smarty_Internal_Templateparser::TP_SPACE;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_ATTR;
  }
    }
    function yy_r2_74($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ID;
    }
    function yy_r2_75($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_INTEGER;
    }
    function yy_r2_76($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_SPACE;
    }
    function yy_r2_77($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }



    function yylex3()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 0,
              4 => 0,
              5 => 0,
              6 => 0,
              7 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(".$this->ldel."\\s*literal\\s*".$this->rdel.")|\G(".$this->ldel."\\s*\/literal\\s*".$this->rdel.")|\G(<\\?(?:php\\w+|=|[a-zA-Z]+)?)|\G(\\?>)|\G(<%)|\G(%>)|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
=======
    }

    public function yy_r2_1()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELIF;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_4()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELFOR;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_6()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELFOREACH;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_8()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELSETFILTER;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_10()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELMAKENOCACHE;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_12()
    {
        $this->yypopstate();
        $this->token = Smarty_Internal_Templateparser::TP_SIMPLETAG;
        $this->taglineno = $this->line;
    }

    public function yy_r2_15()
    {
        $this->yypopstate();
        $this->token = Smarty_Internal_Templateparser::TP_SMARTYBLOCKCHILDPARENT;
        $this->taglineno = $this->line;
    }

    public function yy_r2_18()
    {
        $this->yypopstate();
        $this->token = Smarty_Internal_Templateparser::TP_CLOSETAG;
        $this->taglineno = $this->line;
    }

    public function yy_r2_20()
    {
        if ($this->_yy_stack[ count($this->_yy_stack) - 1 ] === self::TEXT) {
            $this->yypopstate();
            $this->token = Smarty_Internal_Templateparser::TP_SIMPELOUTPUT;
            $this->taglineno = $this->line;
        } else {
            $this->value = $this->smarty->getLeftDelimiter();
            $this->token = Smarty_Internal_Templateparser::TP_LDEL;
            $this->yybegin(self::TAGBODY);
            $this->taglineno = $this->line;
        }
    } // end function

    public function yy_r2_23()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r2_25()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDEL;
        $this->yybegin(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yylex3()
    {
        if (!isset($this->yy_global_pattern3)) {
            $this->yy_global_pattern3 =
                $this->replace("/\G(\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal)|\G([\"])|\G('[^'\\\\]*(?:\\\\.[^'\\\\]*)*')|\G([$][0-9]*[a-zA-Z_]\\w*)|\G([$])|\G(\\s+is\\s+in\\s+)|\G(\\s+as\\s+)|\G(\\s+to\\s+)|\G(\\s+step\\s+)|\G(\\s+instanceof\\s+)|\G(\\s*([!=][=]{1,2}|[<][=>]?|[>][=]?|[&|]{2})\\s*)|\G(\\s+(eq|ne|neq|gt|ge|gte|lt|le|lte|mod|and|or|xor)\\s+)|\G(\\s+is\\s+(not\\s+)?(odd|even|div)\\s+by\\s+)|\G(\\s+is\\s+(not\\s+)?(odd|even))|\G([!]\\s*|not\\s+)|\G([(](int(eger)?|bool(ean)?|float|double|real|string|binary|array|object)[)]\\s*)|\G(\\s*[(]\\s*)|\G(\\s*[)])|\G(\\[\\s*)|\G(\\s*\\])|\G(\\s*[-][>]\\s*)|\G(\\s*[=][>]\\s*)|\G(\\s*[=]\\s*)|\G(([+]|[-]){2})|\G(\\s*([+]|[-])\\s*)|\G(\\s*([*]{1,2}|[%\/^&]|[<>]{2})\\s*)|\G([@])|\G(array\\s*[(]\\s*)|\G([#])|\G(\\s+[0-9]*[a-zA-Z_][a-zA-Z0-9_\-:]*\\s*[=]\\s*)|\G(([0-9]*[a-zA-Z_]\\w*)?(\\\\[0-9]*[a-zA-Z_]\\w*)+)|\G([0-9]*[a-zA-Z_]\\w*)|\G(\\d+)|\G([`])|\G([|][@]?)|\G([.])|\G(\\s*[,]\\s*)|\G(\\s*[;]\\s*)|\G([:]{2})|\G(\\s*[:]\\s*)|\G(\\s*[?]\\s*)|\G(0[xX][0-9a-fA-F]+)|\G(\\s+)|\G([\S\s])/isS");
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
                                        ' an empty string.  Input "' . substr($this->data,
                            $this->counter, 5) . '... state TAGBODY');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r3_' . $this->token}();
                if ($r === null) {
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
                    $this->counter += strlen($this->value);
                    $this->line += substr_count($this->value, "\n");
                    if ($this->counter >= $this->dataLength) {
                        return false; // end of input
                    }
                    // skip this token
                    continue;
                }
            } else {
                throw new Exception('Unexpected input at line ' . $this->line .
                                    ': ' . $this->data[ $this->counter ]);
            }
            break;
        } while (true);
    }

    public function yy_r3_1()
    {
        $this->token = Smarty_Internal_Templateparser::TP_RDEL;
        $this->yypopstate();
    }

    public function yy_r3_2()
    {
        $this->yypushstate(self::TAG);
        return true;
    }

    public function yy_r3_4()
    {
        $this->token = Smarty_Internal_Templateparser::TP_QUOTE;
        $this->yypushstate(self::DOUBLEQUOTEDSTRING);
        $this->compiler->enterDoubleQuote();
    }

    public function yy_r3_5()
    {
        $this->token = Smarty_Internal_Templateparser::TP_SINGLEQUOTESTRING;
    }

    public function yy_r3_6()
    {
        $this->token = Smarty_Internal_Templateparser::TP_DOLLARID;
    }

    public function yy_r3_7()
    {
        $this->token = Smarty_Internal_Templateparser::TP_DOLLAR;
    }

    public function yy_r3_8()
    {
        $this->token = Smarty_Internal_Templateparser::TP_ISIN;
    }

    public function yy_r3_9()
    {
        $this->token = Smarty_Internal_Templateparser::TP_AS;
    }

    public function yy_r3_10()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TO;
    }

    public function yy_r3_11()
    {
        $this->token = Smarty_Internal_Templateparser::TP_STEP;
    }

    public function yy_r3_12()
    {
        $this->token = Smarty_Internal_Templateparser::TP_INSTANCEOF;
    }

    public function yy_r3_13()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LOGOP;
    }

    public function yy_r3_15()
    {
        $this->token = Smarty_Internal_Templateparser::TP_SLOGOP;
    }

    public function yy_r3_17()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TLOGOP;
    }

    public function yy_r3_20()
    {
        $this->token = Smarty_Internal_Templateparser::TP_SINGLECOND;
    }

    public function yy_r3_23()
    {
        $this->token = Smarty_Internal_Templateparser::TP_NOT;
    }

    public function yy_r3_24()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TYPECAST;
    }

    public function yy_r3_28()
    {
        $this->token = Smarty_Internal_Templateparser::TP_OPENP;
    }

    public function yy_r3_29()
    {
        $this->token = Smarty_Internal_Templateparser::TP_CLOSEP;
    }

    public function yy_r3_30()
    {
        $this->token = Smarty_Internal_Templateparser::TP_OPENB;
    }

    public function yy_r3_31()
    {
        $this->token = Smarty_Internal_Templateparser::TP_CLOSEB;
    }

    public function yy_r3_32()
    {
        $this->token = Smarty_Internal_Templateparser::TP_PTR;
    }

    public function yy_r3_33()
    {
        $this->token = Smarty_Internal_Templateparser::TP_APTR;
    }

    public function yy_r3_34()
    {
        $this->token = Smarty_Internal_Templateparser::TP_EQUAL;
    }

    public function yy_r3_35()
    {
        $this->token = Smarty_Internal_Templateparser::TP_INCDEC;
    }

    public function yy_r3_37()
    {
        $this->token = Smarty_Internal_Templateparser::TP_UNIMATH;
    }

    public function yy_r3_39()
    {
        $this->token = Smarty_Internal_Templateparser::TP_MATH;
    }

    public function yy_r3_41()
    {
        $this->token = Smarty_Internal_Templateparser::TP_AT;
    }

    public function yy_r3_42()
    {
        $this->token = Smarty_Internal_Templateparser::TP_ARRAYOPEN;
    }

    public function yy_r3_43()
    {
        $this->token = Smarty_Internal_Templateparser::TP_HATCH;
    }

    public function yy_r3_44()
    {
        // resolve conflicts with shorttag and right_delimiter starting with '='
        if (substr($this->data, $this->counter + strlen($this->value) - 1, $this->compiler->getRdelLength()) ===
            $this->smarty->getRightDelimiter()) {
            preg_match('/\s+/', $this->value, $match);
            $this->value = $match[ 0 ];
            $this->token = Smarty_Internal_Templateparser::TP_SPACE;
        } else {
            $this->token = Smarty_Internal_Templateparser::TP_ATTR;
        }
    }

    public function yy_r3_45()
    {
        $this->token = Smarty_Internal_Templateparser::TP_NAMESPACE;
    }

    public function yy_r3_48()
    {
        $this->token = Smarty_Internal_Templateparser::TP_ID;
    }

    public function yy_r3_49()
    {
        $this->token = Smarty_Internal_Templateparser::TP_INTEGER;
    }

    public function yy_r3_50()
    {
        $this->token = Smarty_Internal_Templateparser::TP_BACKTICK;
        $this->yypopstate();
    }

    public function yy_r3_51()
    {
        $this->token = Smarty_Internal_Templateparser::TP_VERT;
    }

    public function yy_r3_52()
    {
        $this->token = Smarty_Internal_Templateparser::TP_DOT;
    }

    public function yy_r3_53()
    {
        $this->token = Smarty_Internal_Templateparser::TP_COMMA;
    }

    public function yy_r3_54()
    {
        $this->token = Smarty_Internal_Templateparser::TP_SEMICOLON;
    }

    public function yy_r3_55()
    {
        $this->token = Smarty_Internal_Templateparser::TP_DOUBLECOLON;
    }

    public function yy_r3_56()
    {
        $this->token = Smarty_Internal_Templateparser::TP_COLON;
    }

    public function yy_r3_57()
    {
        $this->token = Smarty_Internal_Templateparser::TP_QMARK;
    }

    public function yy_r3_58()
    {
        $this->token = Smarty_Internal_Templateparser::TP_HEX;
    }

    public function yy_r3_59()
    {
        $this->token = Smarty_Internal_Templateparser::TP_SPACE;
    } // end function

    public function yy_r3_60()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yylex4()
    {
        if (!isset($this->yy_global_pattern4)) {
            $this->yy_global_pattern4 =
                $this->replace("/\G((SMARTYldel)SMARTYalliteral\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/]literal\\s*SMARTYrdel)|\G([\S\s])/isS");
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
>>>>>>> gosa-core_v2.8
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state LITERAL');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
<<<<<<< HEAD
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


    const LITERAL = 3;
    function yy_r3_1($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LITERALSTART;
  $this->yypushstate(self::LITERAL);
    }
    function yy_r3_2($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LITERALEND;
  $this->yypopstate();
    }
    function yy_r3_3($yy_subpatterns)
    {

  if (in_array($this->value, Array('<?', '<?=', '<?php'))) {
    $this->token = Smarty_Internal_Templateparser::TP_PHPSTARTTAG;
   } else {
    $this->token = Smarty_Internal_Templateparser::TP_FAKEPHPSTARTTAG;
    $this->value = substr($this->value, 0, 2);
   }
    }
    function yy_r3_4($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_PHPENDTAG;
    }
    function yy_r3_5($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ASPSTARTTAG;
    }
    function yy_r3_6($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_ASPENDTAG;
    }
    function yy_r3_7($yy_subpatterns)
    {

  if ($this->mbstring_overload) {
    $to = mb_strlen($this->data,'latin1');
  } else {
    $to = strlen($this->data);
  }
  preg_match("/{$this->ldel}\/?literal{$this->rdel}|<\?|<%|\?>|%>/",$this->data,$match,PREG_OFFSET_CAPTURE,$this->counter);
=======
    }

    public function yy_r4_1()
    {
        $this->literal_cnt++;
        $this->token = Smarty_Internal_Templateparser::TP_LITERAL;
    }

    public function yy_r4_3()
    {
        if ($this->literal_cnt) {
            $this->literal_cnt--;
            $this->token = Smarty_Internal_Templateparser::TP_LITERAL;
        } else {
            $this->token = Smarty_Internal_Templateparser::TP_LITERALEND;
            $this->yypopstate();
        }
    }

    public function yy_r4_5()
    {
        if (!isset($this->yy_global_literal)) {
            $this->yy_global_literal = $this->replace('/(SMARTYldel)SMARTYal[\/]?literalSMARTYrdel/isS');
        }
        $to = $this->dataLength;
        preg_match($this->yy_global_literal, $this->data, $match, PREG_OFFSET_CAPTURE, $this->counter);
>>>>>>> gosa-core_v2.8
  if (isset($match[0][1])) {
    $to = $match[0][1];
  } else {
    $this->compiler->trigger_template_error ("missing or misspelled literal closing tag");
  }
<<<<<<< HEAD
  if ($this->mbstring_overload) {
    $this->value = mb_substr($this->data,$this->counter,$to-$this->counter,'latin1');
  } else {
    $this->value = substr($this->data,$this->counter,$to-$this->counter);
  }
  $this->token = Smarty_Internal_Templateparser::TP_LITERAL;
    }


    function yylex4()
    {
        $tokenMap = array (
              1 => 0,
              2 => 1,
              4 => 0,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
              9 => 0,
              10 => 0,
              11 => 0,
              12 => 0,
              13 => 3,
              17 => 0,
            );
        if ($this->counter >= ($this->mbstring_overload ? mb_strlen($this->data,'latin1'): strlen($this->data))) {
            return false; // end of input
        }
        $yy_global_pattern = "/\G(".$this->ldel."\\s{1,}\/)|\G(".$this->ldel."\\s*(if|elseif|else if|while)\\s+)|\G(".$this->ldel."\\s*for\\s+)|\G(".$this->ldel."\\s*foreach(?![^\s]))|\G(".$this->ldel."\\s{1,})|\G(".$this->ldel."\/)|\G(".$this->ldel.")|\G(\")|\G(`\\$)|\G(\\$[0-9]*[a-zA-Z_]\\w*)|\G(\\$)|\G(([^\"\\\\]*?)((?:\\\\.[^\"\\\\]*?)*?)(?=(".$this->ldel."|\\$|`\\$|\")))|\G([\S\s])/iS";

        do {
            if ($this->mbstring_overload ? preg_match($yy_global_pattern, mb_substr($this->data, $this->counter,2000000000,'latin1'), $yymatches) : preg_match($yy_global_pattern,$this->data, $yymatches, null, $this->counter)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
=======
        $this->value = substr($this->data, $this->counter, $to - $this->counter);
        $this->token = Smarty_Internal_Templateparser::TP_LITERAL;
    } // end function

    public function yylex5()
    {
        if (!isset($this->yy_global_pattern5)) {
            $this->yy_global_pattern5 =
                $this->replace("/\G((SMARTYldel)SMARTYautoliteral\\s+SMARTYliteral)|\G((SMARTYldel)SMARTYalliteral\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/]literal\\s*SMARTYrdel)|\G((SMARTYldel)SMARTYal[\/])|\G((SMARTYldel)SMARTYal[0-9]*[a-zA-Z_]\\w*)|\G((SMARTYldel)SMARTYal)|\G([\"])|\G([`][$])|\G([$][0-9]*[a-zA-Z_]\\w*)|\G([$])|\G(([^\"\\\\]*?)((?:\\\\.[^\"\\\\]*?)*?)(?=((SMARTYldel)SMARTYal|\\$|`\\$|\"SMARTYliteral)))|\G([\S\s])/isS");
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
>>>>>>> gosa-core_v2.8
                    throw new Exception('Error: lexing failed because a rule matched' .
                        ' an empty string.  Input "' . substr($this->data,
                        $this->counter, 5) . '... state DOUBLEQUOTEDSTRING');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
<<<<<<< HEAD
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


    const DOUBLEQUOTEDSTRING = 4;
    function yy_r4_1($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r4_2($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELIF;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r4_4($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOR;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r4_5($yy_subpatterns)
    {

  if ($this->smarty->auto_literal && trim(substr($this->value,$this->ldel_length,1)) == '') {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDELFOREACH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r4_6($yy_subpatterns)
    {

  if ($this->smarty->auto_literal) {
     $this->token = Smarty_Internal_Templateparser::TP_TEXT;
  } else {
     $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
  }
    }
    function yy_r4_7($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDELSLASH;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r4_8($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_LDEL;
     $this->yypushstate(self::SMARTY);
     $this->taglineno = $this->line;
    }
    function yy_r4_9($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_QUOTE;
  $this->yypopstate();
    }
    function yy_r4_10($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_BACKTICK;
  $this->value = substr($this->value,0,-1);
  $this->yypushstate(self::SMARTY);
  $this->taglineno = $this->line;
    }
    function yy_r4_11($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_DOLLARID;
    }
    function yy_r4_12($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }
    function yy_r4_13($yy_subpatterns)
    {

  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }
    function yy_r4_17($yy_subpatterns)
    {

  if ($this->mbstring_overload) {
    $to = mb_strlen($this->data,'latin1');
  } else {
    $to = strlen($this->data);
  }
  if ($this->mbstring_overload) {
    $this->value = mb_substr($this->data,$this->counter,$to-$this->counter,'latin1');
  } else {
    $this->value = substr($this->data,$this->counter,$to-$this->counter);
  }
  $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

}
=======
    }

    public function yy_r5_1()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r5_3()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r5_5()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r5_7()
    {
        $this->yypushstate(self::TAG);
        return true;
    }

    public function yy_r5_9()
    {
        $this->yypushstate(self::TAG);
        return true;
    }

    public function yy_r5_11()
    {
        $this->token = Smarty_Internal_Templateparser::TP_LDEL;
        $this->taglineno = $this->line;
        $this->yypushstate(self::TAGBODY);
    }

    public function yy_r5_13()
    {
        $this->token = Smarty_Internal_Templateparser::TP_QUOTE;
        $this->yypopstate();
    }

    public function yy_r5_14()
    {
        $this->token = Smarty_Internal_Templateparser::TP_BACKTICK;
        $this->value = substr($this->value, 0, -1);
        $this->yypushstate(self::TAGBODY);
        $this->taglineno = $this->line;
    }

    public function yy_r5_15()
    {
        $this->token = Smarty_Internal_Templateparser::TP_DOLLARID;
    }

    public function yy_r5_16()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r5_17()
    {
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }

    public function yy_r5_22()
    {
        $to = $this->dataLength;
        $this->value = substr($this->data, $this->counter, $to - $this->counter);
        $this->token = Smarty_Internal_Templateparser::TP_TEXT;
    }
}

     
>>>>>>> gosa-core_v2.8
