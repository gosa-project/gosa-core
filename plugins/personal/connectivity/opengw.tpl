<h2><input type="checkbox" value="1" name="is_account" {$is_account} {$opengwAccountACL} onClick="document.mainform.submit();">&nbsp;{t}Opengroupware{/t}</h2>
<table width="100%" summary="">
	<tr>
		<td width="50%">
			<table summary="">
				<tr>
					<td>
						{t}Location team{/t} &nbsp;
					</td>
					<td>
						<select size="1" id="LocationTeam" name="LocationTeam"
							{if $LocationTeamACL!="" || $OGWstate!=""}
								disabled 
							{/if}>
							{html_options values=$validLocationTeams output=$validLocationTeam selected=$LocationTeam}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						{t}Template user{/t} &nbsp;
					</td>
					<td>
						<select size="1" id="TemplateUser" name="TemplateUser"
							{if $TemplateUserACL!="" || $OGWstate!=""}
								disabled 
							{/if}>
							{html_options values=$validTemplateUsers output=$validTemplateUser selected=$TemplateUser}
						</select>
					</td>
				</tr>	
				<tr>
					<td valign="top">
						{t}Locked{/t} &nbsp; 
					</td>
					<td valign="top">
						<input type="checkbox" value="1" name="is_locked" {$is_lockedCHK}  
						   {if $is_lockedACL!="" || $OGWstate!=""}
                                disabled
                           {/if}>
					</td>
				</tr>
				<tr>
					<td valign="top">
						<input type="checkbox" name="SetPassword" value="1" {$OGWPasswordCHK} onClick="changeState('OGWPassword');" 
						   {if $OGWstate!=""}
                                disabled
                           {/if}>
						{t}Set password{/t} &nbsp; 
					</td>
					<td valign="top">
						<input type="text" name="OGWPassword" value="{$OGWPassword}" id="OGWPassword" 
						   {if $OGWPasswordACL!="" || $OGWstate!=""}
                                disabled
                           {/if}>
					</td>
				</tr>
			</table>
		</td>
		<td valign="top" style="border-left: 1px solid rgb(160, 160, 160);">
			<table summary="">
				<tr>
					<td valign="top">
						{t}Teams{/t} &nbsp; 
					</td>
					<td valign="top">
						{$validTeams}	
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
