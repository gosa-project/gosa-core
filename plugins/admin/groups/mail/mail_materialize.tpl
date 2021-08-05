<h6></h6>

<div class="row">
	<div class="col s6">
		<div class="card">
			<div class="card-content">
				<span class="card-title">{t}Generic{/t}</span>
				<div class="input-field">
					{if !$isModifyableMail && $initially_was_account}
					<input type='text' disabled size=30 value="{$mail}">
					{else}
					{if $domainSelectionEnabled}
					{render acl=$mailACL}
					<input id="mail" name="mail" value="{$mail}" {if $mailEqualsCN} disabled {/if}>
					{/render}
					@<select name='MailDomain' size=1>
						{html_options values=$MailDomains output=$MailDomains selected=$MailDomain}
					</select>
					{else}
					{if $mailEqualsCN}
					<input type='text' disabled name='dummy' value='{$mail}'>
					@<input type='text' value="{$MailDomain}" name="MailDomain">
					{else}
					{render acl=$mailACL}
					<input type='text' id="mail" name="mail" value="{$mail}">
					{/render}
					{/if}
					{/if}
					<label for="mail">{t}Primary address{/t}{$must}</label>
					{/if}
				</div>

				<div class="input-field">
					{if !$isModifyableServer && $initially_was_account}
					<input type='text' disabled size=30 value="{$gosaMailServer}">
					{else}
					{render acl=$gosaMailServerACL}
					<select size="1" id="gosaMailServer" name="gosaMailServer"
						title="{t}Specify the mail server where the user will be hosted on{/t}">
						{html_options values=$MailServers output=$MailServers selected=$gosaMailServer}
						<option disabled>&nbsp;</option>
					</select>
					{/render}
					<label>{t}Server{/t}</label>
					{/if}
				</div>

				{if $quotaEnabled}
				<div class="row">
					<div class="col s6">
						{t}Quota usage{/t}
					</div>
					<div class="col s6">
						{$quotaUsage}
					</div>
				</div>

				<div class="input-field">
					{render acl=$gosaMailQuotaACL}
					<input id="gosaMailQuota" name="gosaMailQuota" value="{$gosaMailQuota}" type='text'>
					{/render}
					<label for="gosaMailQuota">{t}Quota size{/t}</label>
					<span class="helper-text">MB</span>
				</div>
				{/if}

				{if $folderTypesEnabled && !$multiple_support}
				<div class="row valign-wrapper">
					<div class="col s6">
						{t}Folder type{/t}
						<div class="input-field inline">
							<select id="FolderTypeCAT" name="FolderTypeCAT" onChange="document.mainform.submit();"
								size=1>
								{foreach from=$AvailableFolderTypes.CAT item=item key=key}
								<option {if $key==$FolderType.CAT} selected {/if} value="{$key}">{$item}</option>
								{/foreach}
							</select>
						</div>
					</div>
					<div class="col s5">
						<div class="input-field">
							<select id="FolderTypeSUB_CAT" name="FolderTypeSUB_CAT"
								onChange="document.mainform.submit();" size=1>
								{foreach from=$AvailableFolderTypes.SUB_CAT item=item key=key}
								{if $key == $FolderType.CAT}
								{foreach from=$item item=item2 key=key2}
								<option {if $key2==$FolderType.SUB_CAT} selected {/if} value='{$key2}'>{$item2}</option>
								{/foreach}
								{/if}
								{/foreach}
							</select>
						</div>
					</div>
					<div class="col s1">
						{image path="<i class='material-icons'>refresh</i>"}
					</div>
				</div>
				{/if}
			</div>
		</div>
	</div>

	<div class="col s6">
		<div class="card">
			<div class="card-content">
				<span class="card-title">{t}Alternative addresses{/t}</span>
				{if !$multiple_support}
				{render acl=$gosaMailAlternateAddressACL}
				<div class="input-field">
					<select name="alternates_list[]" multiple title="{t}List of alternative mail addresses{/t}">
						{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
						<option disabled>&nbsp;</option>
					</select>
				</div>
				{/render}
				{render acl=$gosaMailAlternateAddressACL}
				<div class="input-field">
					<input type='text' name="alternate_address" value="">
				</div>
				{/render}
				<div class="card-action">
					{render acl=$gosaMailAlternateAddressACL}
					<button class="btn-small gonicus-color" type='submit' name='add_alternate'>{msgPool
						type=addButton}</button>
					{/render}
					{render acl=$gosaMailAlternateAddressACL}
					<button class="btn-small gonicus-color" type='submit' name='delete_alternate'>{msgPool
						type=delButton}</button>
					{/render}
				</div>
				{/if}
			</div>
		</div>
	</div>
</div>

<div class="row">
	{if !$multiple_support}
	<div class="col s6">
		<div class="card">
			<div class="card-content">
				<span class="card-title">{t}IMAP shared folders{/t}</span>
				<input type='hidden' name='mail_acls_posted' value='1'>
				{foreach from=$folder_acls item=item key=user}
				<div class="input-field">
					{render acl=$aclACL}
					<select size="1" name="acl_value_{$item.post_name}">
						{html_options options=$AclTypes selected=$item.acl}
						<option disabled>&nbsp;</option>
					</select>
					{if $user == "__anyone__"}
					<label for="default_permissions">{t}Default permission{/t}</label>
					{elseif $user == "__member__"}
					<label for="member_permissions">{t}Member permission{/t}</label>
					{else}
					<input type='text' name='acl_user_{$item.post_name}' value='{$user}'>
					{/if}
				</div>
				{if !($user == "__anyone__" || $user == "__member__")}
				<button class="btn-small gonicus-color" type='submit' name='remove_acl_user_{$item.post_name}'>{msgPool
					type=delButton}</button>
				{/if}
				{/render}
				{if $user == "__member__"}
				{if $show_effective_memeber}
				<button class="btn-small gonicus-color" type='submit' name='show_effective_memeber'>{t}Hide{/t}</button>
				{else}
				<button class="btn-small gonicus-color" type='submit' name='show_effective_memeber'>{t}Show{/t}</button>
				{/if}
				{/if}
				{if $user == "__member__" && $show_effective_memeber}
				{foreach from=$Effective item=i key=k}
				<i>{$k}</i>
				{/foreach}
				{/if}
				{/foreach}
			</div>
		</div>
	</div>
	{/if}

	<div class="col s6">
		<div class="card">
			<div class="card-content">
				<span class="card-title">{t}Forward messages to non group members{/t}</span>
				{render acl=$gosaMailForwardingAddressACL}
				<div class="input-field">
					<select style="width:100%;" name="forwarder_list[]" size=10 multiple>
						{if $multiple_support}
						{foreach from=$Forward_all item=item key=key}
						<option value="{$item}">{$item}&nbsp;({t}Used in all groups{/t})</option>
						{/foreach}
						{foreach from=$Forward_some item=item key=key}
						<option value="{$item}" style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;'>
							{$item}&nbsp;({t}Not used in all groups{/t})</option>
						{/foreach}
						{else}
						{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
						<option disabled>&nbsp;</option>
						{/if}
					</select>
					{/render}
				</div>
				<div class="input-field">
					{render acl=$gosaMailForwardingAddressACL}
					<input type='text' name="forward_address" value="">
					{/render}
				</div>
				<div class="card-action">
					{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small gonicus-color" type='submit' name='add_forwarder'>
						{msgPool type=addButton}
					</button>
					{/render}
					{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small gonicus-color" type='submit' name='add_local_forwarder'>
						{t}Add local{/t}
					</button>
					{/render}
					{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small gonicus-color" type='submit' name='delete_forwarder'>
						{msgPool type=delButton}
					</button>
					{/render}
				</div>
			</div>
		</div>
	</div>
</div>

<div class="row">
	<div class="col s12">
		<div class="card">
			<div class="card-content">
				<span class="card-title">{t}Advanced mail options{/t}</span>
				<div class="input-field">
					{render acl=$gosaMailDeliveryModeIACL}
					<input type="checkbox" name="only_local" value="1" {$only_local}
						title="{t}Select if user can only send and receive inside his own domain{/t}">
					{/render}
					<span>{t}User is only allowed to send and receive local mails{/t}</span>
				</div>
			</div>
		</div>
	</div>
</div>
<input type="hidden" name='mailedit' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
	focus_field('mail');
</script>

{if $multiple_support}
<input type="hidden" name="multiple_mail_group_posted" value="1">
{/if}