<?php
/**
 * Smarty Internal Plugin Config File Compiler
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * This is the config file compiler class. It calls the lexer and parser to
 * perform the compiling.
 *
 * @package Smarty
 * @subpackage Config
 * @author Uwe Tews
 */

/**
 * Main config file compiler class
 *
 * @package Smarty
 * @subpackage Config
 */
<<<<<<< HEAD
class Smarty_Internal_Config_File_Compiler {
=======
class Smarty_Internal_Config_File_Compiler
{
    /**
     * Lexer class name
     *
     * @var string
     */
    public $lexer_class;

    /**
     * Parser class name
     *
     * @var string
     */
    public $parser_class;
>>>>>>> gosa-core_v2.8

    /**
     * Lexer object
     *
     * @var object
     */
    public $lex;

    /**
     * Parser object
     *
     * @var object
     */
    public $parser;

    /**
     * Smarty object
     *
     * @var Smarty object
     */
    public $smarty;

    /**
     * Smarty object
     *
<<<<<<< HEAD
     * @var Smarty_Internal_Config object
     */
    public $config;
=======
     * @var Smarty_Internal_Template object
     */
    public $template;
>>>>>>> gosa-core_v2.8

    /**
     * Compiled config data sections and variables
     *
     * @var array
     */
    public $config_data = array();

    /**
<<<<<<< HEAD
     * Initialize compiler
     *
     * @param Smarty $smarty base instance
     */
    public function __construct($smarty)
    {
        $this->smarty = $smarty;
=======
     * compiled config data must always be written
     *
     * @var bool
     */
    public $write_compiled_code = true;

    /**
     * Initialize compiler
     *
     * @param string $lexer_class  class name
     * @param string $parser_class class name
     * @param Smarty $smarty       global instance
     */
    public function __construct($lexer_class, $parser_class, Smarty $smarty)
    {
        $this->smarty = $smarty;
        // get required plugins
        $this->lexer_class = $lexer_class;
        $this->parser_class = $parser_class;
        $this->smarty = $smarty;
>>>>>>> gosa-core_v2.8
        $this->config_data['sections'] = array();
        $this->config_data['vars'] = array();
    }

    /**
<<<<<<< HEAD
     * Method to compile a Smarty template.
     *
     * @param  Smarty_Internal_Config $config config object
     * @return bool true if compiling succeeded, false if it failed
     */
    public function compileSource(Smarty_Internal_Config $config)
    {
        /* here is where the compiling takes place. Smarty
          tags in the templates are replaces with PHP code,
          then written to compiled files. */
        $this->config = $config;
        // get config file source
        $_content = $config->source->content . "\n";
        // on empty template just return
        if ($_content == '') {
            return true;
        }
        // init the lexer/parser to compile the config file
        $lex = new Smarty_Internal_Configfilelexer($_content, $this->smarty);
        $parser = new Smarty_Internal_Configfileparser($lex, $this);
        if ($this->smarty->_parserdebug) $parser->PrintTrace();
        // get tokens from lexer and parse them
        while ($lex->yylex()) {
            if ($this->smarty->_parserdebug) echo "<br>Parsing  {$parser->yyTokenName[$lex->token]} Token {$lex->value} Line {$lex->line} \n";
            $parser->doParse($lex->token, $lex->value);
        }
        // finish parsing process
        $parser->doParse(0, 0);
        $config->compiled_config = '<?php $_config_vars = ' . var_export($this->config_data, true) . '; ?>';
=======
     * Method to compile Smarty config source.
     *
     * @param Smarty_Internal_Template $template
     *
     * @return bool true if compiling succeeded, false if it failed
     * @throws \SmartyException
     */
    public function compileTemplate(Smarty_Internal_Template $template)
    {
        $this->template = $template;
        $this->template->compiled->file_dependency[ $this->template->source->uid ] =
            array(
                $this->template->source->filepath,
                $this->template->source->getTimeStamp(),
                $this->template->source->type
            );
        if ($this->smarty->debugging) {
            if (!isset($this->smarty->_debug)) {
                $this->smarty->_debug = new Smarty_Internal_Debug();
            }
            $this->smarty->_debug->start_compile($this->template);
        }
        // init the lexer/parser to compile the config file
        /* @var Smarty_Internal_ConfigFileLexer $this->lex */
        $this->lex = new $this->lexer_class(
            str_replace(
                array(
                    "\r\n",
                    "\r"
                ),
                "\n",
                $template->source->getContent()
            ) . "\n",
            $this
        );
        /* @var Smarty_Internal_ConfigFileParser $this->parser */
        $this->parser = new $this->parser_class($this->lex, $this);
        if (function_exists('mb_internal_encoding')
            && function_exists('ini_get')
            && ((int)ini_get('mbstring.func_overload')) & 2
        ) {
            $mbEncoding = mb_internal_encoding();
            mb_internal_encoding('ASCII');
        } else {
            $mbEncoding = null;
        }
        if ($this->smarty->_parserdebug) {
            $this->parser->PrintTrace();
        }
        // get tokens from lexer and parse them
        while ($this->lex->yylex()) {
            if ($this->smarty->_parserdebug) {
                echo "<br>Parsing  {$this->parser->yyTokenName[$this->lex->token]} Token {$this->lex->value} Line {$this->lex->line} \n";
            }
            $this->parser->doParse($this->lex->token, $this->lex->value);
        }
        // finish parsing process
        $this->parser->doParse(0, 0);
        if ($mbEncoding) {
            mb_internal_encoding($mbEncoding);
        }
        if ($this->smarty->debugging) {
            $this->smarty->_debug->end_compile($this->template);
        }
        // template header code
        $template_header =
            "<?php /* Smarty version " . Smarty::SMARTY_VERSION . ", created on " . strftime("%Y-%m-%d %H:%M:%S") .
            "\n";
        $template_header .= "         compiled from '{$this->template->source->filepath}' */ ?>\n";
        $code = '<?php $_smarty_tpl->smarty->ext->configLoad->_loadConfigVars($_smarty_tpl, ' .
                var_export($this->config_data, true) . '); ?>';
        return $template_header . $this->template->smarty->ext->_codeFrame->create($this->template, $code);
>>>>>>> gosa-core_v2.8
    }

    /**
     * display compiler error messages without dying
<<<<<<< HEAD
     *
     * If parameter $args is empty it is a parser detected syntax error.
     * In this case the parser is called to obtain information about exspected tokens.
     *
     * If parameter $args contains a string this is used as error message
     *
     * @param string $args individual error message or null
     */
    public function trigger_config_file_error($args = null)
    {
        $this->lex = Smarty_Internal_Configfilelexer::instance();
        $this->parser = Smarty_Internal_Configfileparser::instance();
        // get template source line which has error
=======
     * If parameter $args is empty it is a parser detected syntax error.
     * In this case the parser is called to obtain information about expected tokens.
     * If parameter $args contains a string this is used as error message
     *
     * @param string $args individual error message or null
     *
     * @throws SmartyCompilerException
     */
    public function trigger_config_file_error($args = null)
    {
        // get config source line which has error
>>>>>>> gosa-core_v2.8
        $line = $this->lex->line;
        if (isset($args)) {
            // $line--;
        }
        $match = preg_split("/\n/", $this->lex->data);
<<<<<<< HEAD
        $error_text = "Syntax error in config file '{$this->config->source->filepath}' on line {$line} '{$match[$line-1]}' ";
=======
        $error_text =
            "Syntax error in config file '{$this->template->source->filepath}' on line {$line} '{$match[$line - 1]}' ";
>>>>>>> gosa-core_v2.8
        if (isset($args)) {
            // individual error message
            $error_text .= $args;
        } else {
<<<<<<< HEAD
            // exspected token from parser
=======
            // expected token from parser
>>>>>>> gosa-core_v2.8
            foreach ($this->parser->yy_get_expected_tokens($this->parser->yymajor) as $token) {
                $exp_token = $this->parser->yyTokenName[$token];
                if (isset($this->lex->smarty_token_names[$exp_token])) {
                    // token type from lexer
                    $expect[] = '"' . $this->lex->smarty_token_names[$exp_token] . '"';
                } else {
                    // otherwise internal token name
                    $expect[] = $this->parser->yyTokenName[$token];
                }
            }
            // output parser error message
            $error_text .= ' - Unexpected "' . $this->lex->value . '", expected one of: ' . implode(' , ', $expect);
        }
        throw new SmartyCompilerException($error_text);
    }
<<<<<<< HEAD

}

?>
=======
}
>>>>>>> gosa-core_v2.8
