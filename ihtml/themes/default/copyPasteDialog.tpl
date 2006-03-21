<h2>{t}Copy &amp; paste wizard{/t}</h2>

{t}This dialog assists you with GOsas copy and paste functionality.{/t}<br>
{t}Some objects require unique attributes, as example the often used attribute cn should be unique within the complete ldap database in order to exclude a confounding.{/t}
<br><br>
{t}If a paste requires such unique attributes, the wizard will display them below.{/t}
<br><br>
<br>
{if $Complete == false}
	<div class="pluginfo">
	{$objectDN}
	</div>
	{$AttributesToFix}
	<p class='seperator'>&nbsp;</p>
	<br>
	<div style='text-align:right;width:100%;'>
		<input type='submit' name='PerformCopyPaste' value='{t}Save{/t}'>&nbsp;
		<input type='submit' name='AbortCopyPaste' value='{t}Cancel{/t}'>
	</div>
{else}
	<h2>{t}Operation complete{/t}</h2>
	<div style='text-align:right;width:100%;'>
		<input type='submit' name='Back' value='{t}Finish{/t}'>&nbsp;
	</div>
{/if}
