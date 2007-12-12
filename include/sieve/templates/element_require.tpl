<table class='sieve_require_container'>
	{foreach from=$LastError item=val key=key}
		<tr>
			<td colspan=4>
				<div class='sieve_error_msgs'>{$LastError[$key]}</div>
			</td>
		</tr>

	{/foreach}
	<tr>
		<td style=''>
			<b>{t}Require{/t}</b>
		</td>
	</tr>
	<tr>
		<td style='padding-left:20px;;'>
			<input type='text'  name='require_{$ID}' class='sieve_require_input' value='{$Require}'>
		</td>
	</tr>
</table>

