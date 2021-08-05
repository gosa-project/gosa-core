<h3>Create a new sieve script</h3>
{t}Please enter the name for the new script below. Script names must consist of lower case characters only.{/t}

<br>
<br>
<hr>
<br>
<b>{t}Script name{/t}</b> <input type='text' name='NewScriptName' value='{$NewScriptName}'>
<br>
<br>

<hr>
<div class="plugin-actions">
   <button type='submit' name='create_script_save'>{msgPool type=applyButton}</button>
   <button type='submit' name='create_script_cancel'>{msgPool type=cancelButton}</button>
</div>

<script language="JavaScript" type="text/javascript">
	<!--
	focus_field('NewScriptName');
	-->
</script>
