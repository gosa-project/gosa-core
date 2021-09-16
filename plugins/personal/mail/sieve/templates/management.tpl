<h3>{t}List of sieve scripts{/t}</h3>
<!--
{if $uattrib_empty}
		
	<font color='red'><b>{t}Connection to the sieve server could not be established, the authentication attribute is empty.{/t}</b></font><br>
	{t}Please verify that the attributes UID and mail are not empty and try again.{/t}
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


<button type='submit' name='create_new_script'>{msgPool type='addButton'}</button>
<div class="plugin-actions">
 <button type=submit name="sieve_finish">{msgPool type=saveButton}</button>
 <button type=submit name="sieve_cancel">{msgPool type=cancelButton}</button>
</div>
