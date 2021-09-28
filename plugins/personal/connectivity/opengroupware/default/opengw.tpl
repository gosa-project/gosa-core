<div class="opengw-wrapper">
	<div class="row">
		<label>
			<input type="checkbox" value="1" name="is_account" {$is_account} {$opengwAccountACL} onClick="document.mainform.submit();">
			<span><h2>OpenGroupware.org</h2></span>
		</label>

		<div class="col s12 xl6">
			{render acl=$LocationTeamACL}
			<div class="input-field">
				<select size="1" id="LocationTeam" name="LocationTeam" {if $OGWstate!=""} disabled {/if}>
					{html_options values=$validLocationTeams output=$validLocationTeam selected=$LocationTeam}
				</select>
				<label>{t}Location team{/t}</label>
			</div>
			{/render}

			{render acl=$TemplateUserACL}
			<div class="input-field">
				<select size="1" id="TemplateUser" name="TemplateUser" {if $OGWstate!=""} disabled {/if}>
					{html_options values=$validTemplateUsers output=$validTemplateUser selected=$TemplateUser}
				</select>
				<label>{t}Template user{/t}</label>
			</div>
			{/render}

			{render acl=$LockedACL}
			<label>
				<input type="checkbox" value="1" name="is_locked" {$is_lockedCHK} {if $OGWstate!=""} disabled {/if}>
				<span>{t}Locked{/t}</span>				   
			</label>
			{/render}
		</div>

		<div class="col s12 xl6">
			{t}Teams{/t}
			{render acl=$TeamsACL}
			{$validTeams}	
			{/render}
		</div>
	</div>
</div>
