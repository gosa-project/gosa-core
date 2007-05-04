<?php

/* String used to indent the different code blocks */
define("SIEVE_INDENT_TAB","  ");


/* This class is inherited from the original 'Parser'
 *  class written by Heiko Hund
 */
class My_Parser extends Parser 
{
	var $parent = NULL;
	var $registeredExtensions_ =array();

	function My_Parser($parent)
	{
		$this->registeredExtensions_ = array();		
		$this->parent = $parent;
	}

	function execute()
	{
		$ret = $this->dumpParseTree();
		return($ret);
	}
	

	/* Check if there are errors, collect them and return them */
	function check()
	{
		return($this->tree_->check());
	}
	

	/* Initiate parser, but use some other 
     *  classes, that are rewritten.
     */
	function parse($script) 
	{
		$script = preg_replace("/^###GOSA/","",$script);

		$this->registeredExtensions_ = array();
        $this->status_text = "incomplete";
        $this->script_ = $script;
        $this->tree_ = new My_Tree(@Scanner::scriptStart(),$this);
        $this->tree_->setDumpFunc(array(&$this, 'dumpToken_'));
        $this->scanner_ = new My_Scanner($this->script_);
        $this->scanner_->setCommentFunc(array($this, 'comment_'));

        if ($this->commands_($this->tree_->getRoot()) &&
            $this->scanner_->nextTokenIs('script-end'))
        {
   			$this->scanner_->nextToken(); 
            return $this->success_('success');
        }

        return $this->status_;
	}

	
	function get_sieve_script()
	{
		return("###GOSA\n".$this->tree_->get_sieve_script());
	}		

	
	function save_object()
	{
		$this->tree_->save_object();
	}


	function dumpParseTree()
	{
		return $this->tree_->execute();
	}
}
?>
