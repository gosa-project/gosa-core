<table width='100%' class='sieve_reject_container'  summary="{t}Sieve: reject{/t}">

{foreach from=$LastError item=val key=key}
        <tr>
            <td colspan=4>
                <div class='sieve_error_msgs'>{$LastError[$key]}</div>

            </td>
        </tr>

    {/foreach}
	<tr>
		<td>
			<b>{t}Reject mail{/t}</b>
			&nbsp;
			{if $Multiline}
<!--				{t}This is a multi-line text element{/t}-->
			{else}
<!--				{t}This is stored as single string{/t}-->
			{/if}
		</td>
	</tr>
	<tr>
		<td class='sieve_reject_input'>
			<textarea name='reject_message_{$ID}' class='sieve_reject_input'>{$Message}</textarea>
		</td>
	</tr>
</table>
