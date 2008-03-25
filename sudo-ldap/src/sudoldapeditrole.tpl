<h2>{t}sudo-ldap role{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td> {t}Role name (cn){/t}{$must} </td>
		<td> <input type="text" name="roleName" value="{$roleName}"> </td>
	</tr>
	<tr>
		<td> {t}sudoUser{/t}{$must} </td>
		<td> <input type="text" name="sudoUser" value="{$sudoUser}"> </td>
	</tr>
	<tr>
		<td> {t}sudoHost{/t}{$must} </td>
		<td> <input type="text" name="sudoHost" value="{$sudoHost}"> </td>
	</tr>
	<tr>
		<td> {t}sudoCommand{/t}{$must} </td>
		<td> <input type="text" name="sudoCommand" value="{$sudoCommand}"> </td>
	</tr>
	<tr>
		<td> {t}sudoRunAs{/t}{$must} </td>
		<td> <input type="text" name="sudoRunAs" value="{$sudoRunAs}"> </td>
	</tr>
	<tr>
		<td> {t}sudoOption{/t}{$must} </td>
		<td> <input type="text" name="sudoOption" value="{$sudoOption}"> </td>
	</tr>
</table>
<div style="text-align:right;" align="right">
	<p>
		<input type="submit" name="SaveRoleChanges" value="{t}Save{/t}">
		<input type="submit" name="CancelRoleChanges" value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('roleName');
  -->
</script>
