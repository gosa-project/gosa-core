<div class='default'>
	<p>
		<b>{t}Create your configuration file{/t}</b>
	</p>
	<p>	
		{$msg2}
	</p>

      {if $webgroup == ""}
{t}Depending on the user name your web server is running on:{/t}
<code>
<pre> chown root:www-data {$CONFIG_DIR}/{$CONFIG_FILE}
 chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}

or

 chown root:apache {$CONFIG_DIR}/{$CONFIG_FILE}
 chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
</pre>
{else}
<pre>
 chown root:{$webgroup} {$CONFIG_DIR}/{$CONFIG_FILE}
 chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
</pre>
</code>
{/if} 
	<p>	
		<button class="btn-small gonicus-color" type='submit' name='getconf'>{t}Download configuration{/t}</button>

	</p>
		{if $err_msg != ""}
			<hr>
			<p>{t}Status: {/t}<b class="red-text">{$err_msg}</b></p>
		{/if}

</div>
<input type='hidden' value='1' name='step8_posted'>
