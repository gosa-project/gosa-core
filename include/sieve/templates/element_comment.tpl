<table class='sieve_comment_container'> 
	<tr>
		<td>
			<b>{t}Comment{/t}</b>
		</td>
		<td style='text-align: right;'>
			{if $Small}
				<input type='submit' name='toggle_small_{$ID}' value='{t}Edit{/t}'> 	
			{else}
				<input type='submit' name='toggle_small_{$ID}' value='{t}Cancel{/t}'> 	
			{/if}
		</td>
	</tr>
	<tr>
		<td style='padding-left:20px;' colspan=2>
		{if $Small}
			{$Comment}
		{else}
			<textarea  name='comment_{$ID}' class='sieve_comment_area'>{$Comment}</textarea>
		{/if}
		</td>
	</tr>
</table>
