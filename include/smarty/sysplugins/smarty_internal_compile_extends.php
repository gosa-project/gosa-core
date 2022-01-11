<?php
<<<<<<< HEAD

/**
* Smarty Internal Plugin Compile extend
*
=======
/**
 * Smarty Internal Plugin Compile extend
>>>>>>> gosa-core_v2.8
* Compiles the {extends} tag
*
* @package Smarty
* @subpackage Compiler
* @author Uwe Tews
*/

/**
* Smarty Internal Plugin Compile extend Class
*
* @package Smarty
* @subpackage Compiler
*/
<<<<<<< HEAD
class Smarty_Internal_Compile_Extends extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Extends extends Smarty_Internal_Compile_Shared_Inheritance
{
>>>>>>> gosa-core_v2.8
    /**
    * Attribute definition: Overwrites base class.
    *
    * @var array
    * @see Smarty_Internal_CompileBase
    */
    public $required_attributes = array('file');
<<<<<<< HEAD
    /**
    * Attribute definition: Overwrites base class.
    *
    * @var array
    * @see Smarty_Internal_CompileBase
    */
    public $shorttag_order = array('file');
    /**
    * mbstring.overload flag
    *
    * @var int
    */
    public $mbstring_overload = 0;

    /**
    * Compiles code for the {extends} tag
    *
    * @param array  $args     array with attributes from parser
    * @param object $compiler compiler object
    * @return string compiled code
    */
    public function compile($args, $compiler)
    {
        static $_is_stringy = array('string' => true, 'eval' => true);
        $this->_rdl = preg_quote($compiler->smarty->right_delimiter);
        $this->_ldl = preg_quote($compiler->smarty->left_delimiter);
        if (!$compiler->smarty->auto_literal) {
            $al = '\s*';
        } else {
            $al = '';
        }
        $filepath = $compiler->template->source->filepath;
        $this->mbstring_overload = ini_get('mbstring.func_overload') & 2;
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if ($_attr['nocache'] === true) {
            $compiler->trigger_template_error('nocache option not allowed', $compiler->lex->taglineno);
        }

        $_smarty_tpl = $compiler->template;
        $include_file = null;
        if (strpos($_attr['file'], '$_tmp') !== false) {
            $compiler->trigger_template_error('illegal value for file attribute', $compiler->lex->taglineno);
        }
        eval('$include_file = ' . $_attr['file'] . ';');
        // create template object
        $_template = new $compiler->smarty->template_class($include_file, $compiler->smarty, $compiler->template);
        // save file dependency
        if (isset($_is_stringy[$_template->source->type])) {
            $template_sha1 = sha1($include_file);
        } else {
            $template_sha1 = sha1($_template->source->filepath);
        }
        if (isset($compiler->template->properties['file_dependency'][$template_sha1])) {
            $compiler->trigger_template_error("illegal recursive call of \"{$include_file}\"", $compiler->lex->line - 1);
        }
        $compiler->template->properties['file_dependency'][$template_sha1] = array($_template->source->filepath, $_template->source->timestamp, $_template->source->type);
        $_content = ($this->mbstring_overload ? mb_substr($compiler->lex->data, $compiler->lex->counter - 1, 20000000, 'latin1') : substr($compiler->lex->data, $compiler->lex->counter - 1));
        if (preg_match_all("!({$this->_ldl}{$al}block\s(.+?)\s*{$this->_rdl})!", $_content, $s) !=
        preg_match_all("!({$this->_ldl}{$al}/block\s*{$this->_rdl})!", $_content, $c)) {
            $compiler->trigger_template_error('unmatched {block} {/block} pairs');
        }
        preg_match_all("!{$this->_ldl}{$al}block\s(.+?)\s*{$this->_rdl}|{$this->_ldl}{$al}/block\s*{$this->_rdl}|{$this->_ldl}\*([\S\s]*?)\*{$this->_rdl}!", $_content, $_result, PREG_OFFSET_CAPTURE);
        $_result_count = count($_result[0]);
        $_start = 0;
        while ($_start+1 < $_result_count) {
            $_end = 0;
            $_level = 1;
            if (($this->mbstring_overload ? mb_substr($_result[0][$_start][0],0,mb_strlen($compiler->smarty->left_delimiter,'latin1')+1, 'latin1') : substr($_result[0][$_start][0],0,strlen($compiler->smarty->left_delimiter)+1)) == $compiler->smarty->left_delimiter.'*') {
                $_start++;
                continue;
            }
            while ($_level != 0) {
                $_end++;
                if (($this->mbstring_overload ? mb_substr($_result[0][$_start + $_end][0],0,mb_strlen($compiler->smarty->left_delimiter,'latin1')+1, 'latin1') : substr($_result[0][$_start + $_end][0],0,strlen($compiler->smarty->left_delimiter)+1)) == $compiler->smarty->left_delimiter.'*') {
                    continue;
                }
                if (!strpos($_result[0][$_start + $_end][0], '/')) {
                    $_level++;
                } else {
                    $_level--;
                }
            }
            $_block_content = str_replace($compiler->smarty->left_delimiter . '$smarty.block.parent' . $compiler->smarty->right_delimiter, '%%%%SMARTY_PARENT%%%%',
            ($this->mbstring_overload ? mb_substr($_content, $_result[0][$_start][1] + mb_strlen($_result[0][$_start][0], 'latin1'), $_result[0][$_start + $_end][1] - $_result[0][$_start][1] - + mb_strlen($_result[0][$_start][0], 'latin1'), 'latin1') : substr($_content, $_result[0][$_start][1] + strlen($_result[0][$_start][0]), $_result[0][$_start + $_end][1] - $_result[0][$_start][1] - + strlen($_result[0][$_start][0]))));
            Smarty_Internal_Compile_Block::saveBlockData($_block_content, $_result[0][$_start][0], $compiler->template, $filepath);
            $_start = $_start + $_end + 1;
        }
        if ($_template->source->type == 'extends') {
            $_template->block_data = $compiler->template->block_data;
        }
        $compiler->template->source->content = $_template->source->content;
        if ($_template->source->type == 'extends') {
            $compiler->template->block_data = $_template->block_data;
            foreach ($_template->source->components as $key => $component) {
                $compiler->template->properties['file_dependency'][$key] = array($component->filepath, $component->timestamp, $component->type);
            }
        }
        $compiler->template->source->filepath = $_template->source->filepath;
        $compiler->abort_and_recompile = true;
        return '';
    }

}

?>
=======

    /**
     * Array of names of optional attribute required by tag
     * use array('_any') if there is no restriction of attributes names
     *
     * @var array
     */
    public $optional_attributes = array('extends_resource');

    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('file');

    /**
     * Compiles code for the {extends} tag extends: resource
     *
     * @param array                                 $args     array with attributes from parser
     * @param \Smarty_Internal_TemplateCompilerBase $compiler compiler object
     *
     * @return string compiled code
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    public function compile($args, Smarty_Internal_TemplateCompilerBase $compiler)
    {
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if ($_attr[ 'nocache' ] === true) {
            $compiler->trigger_template_error('nocache option not allowed', $compiler->parser->lex->line - 1);
        }
        if (strpos($_attr[ 'file' ], '$_tmp') !== false) {
            $compiler->trigger_template_error('illegal value for file attribute', $compiler->parser->lex->line - 1);
        }
        // add code to initialize inheritance
        $this->registerInit($compiler, true);
        $file = trim($_attr[ 'file' ], '\'"');
        if (strlen($file) > 8 && substr($file, 0, 8) === 'extends:') {
            // generate code for each template
            $files = array_reverse(explode('|', substr($file, 8)));
            $i = 0;
            foreach ($files as $file) {
                if ($file[ 0 ] === '"') {
                    $file = trim($file, '".');
                } else {
                    $file = "'{$file}'";
                }
                $i++;
                if ($i === count($files) && isset($_attr[ 'extends_resource' ])) {
                    $this->compileEndChild($compiler);
                }
                $this->compileInclude($compiler, $file);
            }
            if (!isset($_attr[ 'extends_resource' ])) {
                $this->compileEndChild($compiler);
            }
        } else {
            $this->compileEndChild($compiler, $_attr[ 'file' ]);
        }
        $compiler->has_code = false;
        return '';
    }

    /**
     * Add code for inheritance endChild() method to end of template
     *
     * @param \Smarty_Internal_TemplateCompilerBase $compiler
     * @param null|string                           $template optional inheritance parent template
     *
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    private function compileEndChild(Smarty_Internal_TemplateCompilerBase $compiler, $template = null)
    {
        $inlineUids = '';
        if (isset($template) && $compiler->smarty->merge_compiled_includes) {
            $code = $compiler->compileTag('include', array($template, array('scope' => 'parent')));
            if (preg_match('/([,][\s]*[\'][a-z0-9]+[\'][,][\s]*[\']content.*[\'])[)]/', $code, $match)) {
                $inlineUids = $match[ 1 ];
            }
        }
        $compiler->parser->template_postfix[] = new Smarty_Internal_ParseTree_Tag(
            $compiler->parser,
            '<?php $_smarty_tpl->inheritance->endChild($_smarty_tpl' .
            (isset($template) ?
                ", {$template}{$inlineUids}" :
                '') . ");\n?>"
        );
    }

    /**
     * Add code for including subtemplate to end of template
     *
     * @param \Smarty_Internal_TemplateCompilerBase $compiler
     * @param string                                $template subtemplate name
     *
     * @throws \SmartyCompilerException
     * @throws \SmartyException
     */
    private function compileInclude(Smarty_Internal_TemplateCompilerBase $compiler, $template)
    {
        $compiler->parser->template_postfix[] = new Smarty_Internal_ParseTree_Tag(
            $compiler->parser,
            $compiler->compileTag(
                'include',
                array(
                    $template,
                    array('scope' => 'parent')
                )
            )
        );
    }

    /**
     * Create source code for {extends} from source components array
     *
     * @param \Smarty_Internal_Template $template
     *
     * @return string
     */
    public static function extendsSourceArrayCode(Smarty_Internal_Template $template)
    {
        $resources = array();
        foreach ($template->source->components as $source) {
            $resources[] = $source->resource;
        }
        return $template->smarty->left_delimiter . 'extends file=\'extends:' . join('|', $resources) .
               '\' extends_resource=true' . $template->smarty->right_delimiter;
    }
}
>>>>>>> gosa-core_v2.8
