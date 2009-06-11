<div class='default'>
	<p>
		<b>{t}Create your configuration file{/t}</b>
	</p>
	<p>	
		{$msg2}
	</p>
	<tt>
	chown root:{$webgroup} {$CONFIG_DIR}/{$CONFIG_FILE}<br>
	chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
	</tt>
	<p>	
		<input type='submit' name='getconf' value='{t}Download configuration{/t}'>
	</p>
		{if $err_msg != ""}
			<p class="seperator">&nbsp;</p>
			<br>
			{t}Status: {/t}
			<a style='color:red ; font-weight:bold '>{$err_msg}</a>
		{/if}

</div>
<input type='hidden' value='1' name='step8_posted'>
