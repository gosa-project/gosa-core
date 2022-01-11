<?php
/**
 * Smarty Internal Plugin Compile Eval
<<<<<<< HEAD
 *
=======
>>>>>>> gosa-core_v2.8
 * Compiles the {eval} tag.
 *
 * @package Smarty
 * @subpackage Compiler
 * @author Uwe Tews
 */

/**
 * Smarty Internal Plugin Compile Eval Class
 *
 * @package Smarty
 * @subpackage Compiler
 */
<<<<<<< HEAD
class Smarty_Internal_Compile_Eval extends Smarty_Internal_CompileBase {

=======
class Smarty_Internal_Compile_Eval extends Smarty_Internal_CompileBase
{
>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $required_attributes = array('var');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $optional_attributes = array('assign');
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
    /**
     * Attribute definition: Overwrites base class.
     *
     * @var array
     * @see Smarty_Internal_CompileBase
     */
    public $shorttag_order = array('var','assign');

    /**
     * Compiles code for the {eval} tag
     *
     * @param array  $args     array with attributes from parser
     * @param object $compiler compiler object
<<<<<<< HEAD
=======
     *
>>>>>>> gosa-core_v2.8
     * @return string compiled code
     */
    public function compile($args, $compiler)
    {
<<<<<<< HEAD
        $this->required_attributes = array('var');
        $this->optional_attributes = array('assign');
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if (isset($_attr['assign'])) {
              // output will be stored in a smarty variable instead of beind displayed
            $_assign = $_attr['assign'];
        }

        // create template object
        $_output = "\$_template = new {$compiler->smarty->template_class}('eval:'.".$_attr['var'].", \$_smarty_tpl->smarty, \$_smarty_tpl);";
=======
        // check and get attributes
        $_attr = $this->getAttributes($compiler, $args);
        if (isset($_attr[ 'assign' ])) {
            // output will be stored in a smarty variable instead of being displayed
            $_assign = $_attr[ 'assign' ];
        }
        // create template object
        $_output =
            "\$_template = new {$compiler->smarty->template_class}('eval:'.{$_attr[ 'var' ]}, \$_smarty_tpl->smarty, \$_smarty_tpl);";
>>>>>>> gosa-core_v2.8
        //was there an assign attribute?
        if (isset($_assign)) {
            $_output .= "\$_smarty_tpl->assign($_assign,\$_template->fetch());";
        } else {
<<<<<<< HEAD
            $_output .= "echo \$_template->fetch();";
        }
        return "<?php $_output ?>";
    }

}

?>
=======
            $_output .= 'echo $_template->fetch();';
        }
        return "<?php $_output ?>";
    }
}
>>>>>>> gosa-core_v2.8
