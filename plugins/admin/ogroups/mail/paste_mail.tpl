<table width='100%' summary="{t}Paste group mail settings{/t}">
        <tr>
                <td width='120'>
			<LABEL for="mail">{t}Mail{/t}</LABEL>{$must}
		</td>
		<td>
			<input type='text' id='main' name='mail' value='{$mail}' size='40' title='{t}Please enter a mail address{/t}'> 
		</td>
	</tr>
</table>

<script language="JavaScript" type="text/javascript">
	focus_field('mail');
</script>
