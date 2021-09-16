<table width='100%' class='sieve_comment_container' summary="{t}Sieve comment{/t}">  
	<tr>
		<td>
			<b>{t}Comment{/t}</b>
		</td>
		<td style='text-align: right;'>
			{if $Small}
				<button type='submit' name='toggle_small_{$ID}'>{t}Edit{/t}</button> 	

			{else}
				<button type='submit' name='toggle_small_{$ID}'>{msgPool type=cancelButton}</button> 	

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
