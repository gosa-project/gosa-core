<?php

class My_Scanner extends Scanner 
{
	function tokenize(&$script)
	{
		$pos = 0;
		$line = 1;
		$script_length = mb_strlen($script);

		while ($pos < $script_length)
		{
			foreach ($this->token_match_ as $class => $regex)
			{
				if (preg_match('/^'. $regex .'/', mb_substr($script, $pos), $match))
				{
					$length = mb_strlen($match[0]);

					if ($class != 'whitespace')
					{
						array_push($this->tokens_, array(
							'class' => $class,
							'text'  => chop(mb_substr($script, $pos, $length)),
							'line'  => $line,
						));
					}
					if ($class == 'unknown')
					{
						return;
					}

					$pos += $length;
					$line += mb_substr_count($match[0], "\n");
					break;
				}
			}
		}
		array_push($this->tokens_, array(
			'class' => 'script-end',
			'text'  => 'script-end',
			'line'  => $line,
		));
	}

	var $commentFn_ = null;
	var $tokenPos_ = 0;
	var $tokens_ = array();
	var $token_match_ = array (
		'left-bracket'   =>  '\[',
		'right-bracket'  =>  '\]',
		'block-start'    =>  '\{',
		'block-end'      =>  '\}',
		'left-parant'    =>  '\(',
		'right-parant'   =>  '\)',
		'comma'          =>  ',',
		'semicolon'      =>  ';',
		'whitespace'     =>  '[ \r\n\t]+',
		'tag'            =>  ':[[:alpha:]_][[:alnum:]_]*(?=\b)',
		'quoted-string'  =>  '"(?:\\[\\"]|[^\x00"])*"',
		'number'         =>  '[[:digit:]]+(?:[KMG])?(?=\b)',
		'comment'        =>  '(?:\/\*(?:[^\*]|\*(?=[^\/]))*\*\/|#[^\r\n]*\r?\n)',
#		'multi-line'     =>  'text:[ \t]*(?:#[^\r\n]*)?\r?\n(\.[^\r\n]+\r?\n|[^\.]*\r?\n)*\.\r?\n',
		'multi-line'     =>  'text:[^;]*',
		'identifier'     =>  '[[:alpha:]_][[:alnum:]_]*(?=\b)',
		'unknown token'  =>  '[^ \r\n\t]+'
	);
}

?>
