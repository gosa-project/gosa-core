<h2>{t}List of sieve scripts{/t}</h2>
<!--
{if $uattrib_empty}
		
	<font color='red'><b>{t}Connection to the sieve server could not be established, the authentification attribute is empty.{/t}</b></font><br>
	{t}Please verfiy that the attributes uid and mail are not empty and try again.{/t}
	<br>
	<br>

{elseif $Sieve_Error != ""}

	<font color='red'><b>{t}Connection to the sieve server could not be established.{/t}</b></font><br>
	{$Sieve_Error}
	<br>
	{t}Possibly the sieve account has not been created yet.{/t}
	<br>
	<br>
{/if}
	{t}Be careful. All your changes will be saved directly to sieve, if you use the save button below.{/t}
-->
	{$List}
	<input type='submit' name='create_new_script' value='{t}Create new script{/t}'>
	<p style="text-align:right;border-top:1px solid #999; padding-top:10px;">
		<input type=submit name="sieve_finish" style="width:80px" value="{t}Save{/t}">
		&nbsp;
		<input type=submit name="sieve_cancel" value="{t}Cancel{/t}">
	</p>
