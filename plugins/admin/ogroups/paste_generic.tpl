<table width='100%' summary="{t}Paste object group{/t}">
	<tr>
		<td width='120'>
			<LABEL for="cn">{t}Group name{/t}</LABEL>{$must}
		</td>
		<td>
			<input type='text' id='cn' name='cn' value='{$cn}' size='40' title='{t}Please enter the new object group name{/t}'> 
		</td>
	</tr>
</table>



<input type='checkbox' value='1' name='copyMembers' {if $copyMembers} checked {/if} id='copyMembers'>
<LABEL for='copyMembers'>
&nbsp;{t}Copy members. Be careful with this option, systems can only inherit from a single object group!{/t}
</LABEL>
<script language="JavaScript" type="text/javascript">
	focus_field('cn');
</script>
