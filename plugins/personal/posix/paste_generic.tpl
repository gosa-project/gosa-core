<table width='100%'>
<tr>
  <td style='vertical-align:top;'>
	<h2>{t}Posix settings{/t}</h2>
	<table>
	  <tr>
		<td>
		  <label for="homeDirectory">{t}Home directory{/t}</label>
		</td>
		<td>
		  <input id="homeDirectory" name="homeDirectory" size=40 maxlength=120 value="{$homeDirectory}">
		</td>
	  </tr>
	  <tr>
		<td>
		  <input id="force_ids" type=checkbox name="force_ids" value="1" ".$force_ids."}
					 onclick="changeState('uidNumber');changeState('gidNumber');">
		</td>
		<td>
		  <label for="force_ids">
			{t}Force UID/GID{/t}
		  </label>
		</td>
			<td style="width:20px;"></td>
			<td>
				<label for="uidNumber">{t}UID{/t}</label>
			</td>
			<td>
				<input id="uidNumber" name="uidNumber" size=5 maxlength=5 {$forceMode} value="{$uidNumber}">
			</td>
		</tr>
		<tr>
			<td colspan="3"></td>
			<td>
				<label for="gidNumber">{t}GID{/t}</label>
			</td>
			<td>
				<input id="gidNumber" name="gidNumber" size=5 maxlength=5 {$forceMode} value="{$gidNumber}">
			</td>
		</tr>
<!--
		<tr>
			<td>
			"._("Group membership")."
			</td>
			<td>
			  <input type='submit' name='editGroupMembership' value='"._("Edit")."'>
			</td>
		</tr>
		-->
	</table>
  </td>
</tr>
</table>

