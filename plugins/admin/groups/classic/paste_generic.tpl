<h3>{t}Group settings{/t}</h3>
<table width='100%' summary="{t}Paste group settings{/t}" >
	<tr>
		<td style='width:150px;'>
			{t}Group name{/t}
		</td>
		<td>
			<input type='text' id='cn' name='cn' size='35' maxlength='60' value='{$cn}' title='{t}POSIX name of the group{/t}'>
		</td>
	</tr>
	<tr>
		<td>
			<input type=checkbox name='force_gid' value='1' {$used} title='{t}Normally IDs are auto-generated, select to specify manually{/t}' 
				onclick='changeState("gidNumber")'>
			<LABEL for='gidNumber'>{t}Force GID{/t}</LABEL>
		</td>
		<td>
			<input type='text' name='gidNumber' size=9 maxlength=9 id='gidNumber' {$dis} value='{$gidNumber}' title='{t}Forced ID number{/t}'>
		</td>
	</tr>
</table>

<script language="JavaScript" type="text/javascript">
	focus_field('cn');
</script>
