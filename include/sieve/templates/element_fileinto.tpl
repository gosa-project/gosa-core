<table class='sieve_fileinto_container'>
	<tr>
		<td colspan=2>
			<b>{t}Move mail into folder{/t}</b>
		</td>
		<td style='text-align:right;'>
			{if $User_Mode}
				<input type='submit' name='user_mode_{$ID}' value='{t}Select from list{/t}'>
			{else}
				<input type='submit' name='user_mode_{$ID}' value='{t}Manual selection{/t}'>
			{/if}
		</td>
	</tr>
	<tr>
		<td style='width:20px;'>
		</td>
		<td>
			{t}Folder{/t}
			{if $User_Mode}
				<input class='sieve_fileinto_input' type='text' value="{$Selected}" name='fileinto_{$ID}'>
			{else}
				<select name='fileinto_{$ID}' class='sieve_fileinto_input'>
					{html_options values=$Boxes output=$Boxes selected=$Selected}
				</select>
			{/if}
		</td>
		<td >
		</td>
	</tr>
</table>
