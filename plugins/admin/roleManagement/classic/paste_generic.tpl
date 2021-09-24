<table width='100%' summary="{t}Paste role{/t}">
	<tr>
		<td width='120'>
			<LABEL for="cn">{t}Role name{/t}</LABEL>{$must}
		</td>
		<td>
			<input type='text' id='cn' name='cn' value='{$cn}' size='40' title='{t}Please enter the new object role name{/t}'> 
		</td>
	</tr>
</table>

<script language="JavaScript" type="text/javascript">
	focus_field('cn');
</script>
