<h2>{t}Copy &amp; paste wizard{/t}</h2>

{$message} Some values need to be unique in the complete directory while some combinations make no sense. GOsa shows the relevant attributes. Please maintain the values below to fullfill the policies.
<br>
<p class='seperator'>&nbsp;</p>
<br>
{if $Complete == false}
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
