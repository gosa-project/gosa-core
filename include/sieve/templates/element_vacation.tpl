<table class='sieve_vacation_container'>

	{foreach from=$LastError item=val key=key}
		<tr>
			<td colspan=4>
				<div class='sieve_error_msgs'>{$LastError[$key]}</div>
			</td>
		</tr>
	{/foreach}
	{if $Expert}
	<tr>
		<td style='width:20%'>	
			<b>{t}Vacation Message{/t}</b>
		</td>	
        <td style='text-align:right; vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Normal view{/t}'>
        </td>
	</tr>
	<tr>
		<td >
			{t}Release interval{/t}&nbsp;
		</td>
		<td>
			<input type='text' name='vacation_release_{$ID}' value='{$Days}'>
			&nbsp;{t}days{/t}
		</td>
	</tr>
	<tr>
		<td>
			{t}Alternative sender addresses{/t}
		</td>
		<td>
    		<textarea name='vacation_receiver_{$ID}' style='width:100%;height:20px;'>{$Addresses}</textarea>
		</td>
	</tr>
	<tr>
		<td class='sieve_vacation_input' colspan=2>
		    <textarea name='vacation_reason_{$ID}' class='sieve_vacation_input'>{$Reason}</textarea>
		</td>
	</tr>
	{else}
	<tr>
		<td>	
			<b>{t}Vacation message{/t}</b>
		</td>	
		<td width='10%' style='vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Expert view{/t}'>
		</td>
	</tr>
	<tr>
		<td class='sieve_vacation_input' colspan=2>
		    <textarea name='vacation_reason_{$ID}' class='sieve_vacation_input'>{$Reason}</textarea>
		</td>
	</tr>
	{/if}
</table>
