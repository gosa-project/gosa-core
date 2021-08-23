<div class="group-mail-wrapper">
	<div class="row">
		<div class="col s12 xl6">
			<h3>{t}Generic{/t}</h3>
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

						<label>{t}Server{/t}</label>
					{/render}
				{/if}
			</div>

      		<div class="contingent">
				{if $quotaEnabled}
            		<div class="list">
						{t}Quota usage{/t}
						{$quotaUsage}
					</div>

					<div class="input-field">
						{render acl=$gosaMailQuotaACL}
						<input id="gosaMailQuota" name="gosaMailQuota" value="{$gosaMailQuota}" type='text'>
						{/render}
						<label for="gosaMailQuota">{t}Quota size{/t}</label>
						<span class="helper-text">MB</span>
					</div>
				{/if}
			</div>

			{if $folderTypesEnabled && !$multiple_support}
				<div class="folder-setting">
					<div class="input-field inline">
						<label>{t}Folder type{/t}</label>
						<select id="FolderTypeCAT" name="FolderTypeCAT" onChange="document.mainform.submit();" size=1>
							{foreach from=$AvailableFolderTypes.CAT item=item key=key}
								<option {if $key==$FolderType.CAT} selected {/if} value="{$key}">{$item}</option>
							{/foreach}
						</select>
					</div>
					<div class="input-field">
						<select id="FolderTypeSUB_CAT" name="FolderTypeSUB_CAT" onChange="document.mainform.submit();"
							size=1>
							{foreach from=$AvailableFolderTypes.SUB_CAT item=item key=key}
								{if $key == $FolderType.CAT}
									{foreach from=$item item=item2 key=key2}
										<option {if $key2==$FolderType.SUB_CAT} selected {/if} value='{$key2}'>{$item2}</option>
									{/foreach}
								{/if}
							{/foreach}
						</select>
					</div>
					<div class="action">
						{image path="<i class='material-icons'>refresh</i>"}
					</div>
				</div>
			{/if}
		</div>

		<div class="col s12 xl6">
			{if !$multiple_support}
				<h3>{t}Alternative addresses{/t}</h3>

				{render acl=$gosaMailAlternateAddressACL}
					<div class="input-field">
						<select name="alternates_list[]" multiple title="{t}List of alternative mail addresses{/t}">
							{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
							<option disabled>&nbsp;</option>
						</select>
					</div>
				{/render}

				{render acl=$gosaMailAlternateAddressACL}
					<div class="input-field add">
						<input type='text' name="alternate_address" value="">

						{render acl=$gosaMailAlternateAddressACL}
							<button class="btn-small" type='submit' name='add_alternate'>{msgPool type=addButton}</button>
						{/render}

						{render acl=$gosaMailAlternateAddressACL}
							<button class="btn-small" type='submit' name='delete_alternate'>{msgPool type=delButton}</button>
						{/render}
					</div>
				{/render}
			{/if}
		</div>
	</div>

	<div class="row">
		{if !$multiple_support}
			<div class="col s12 xl6">
				<h3>{t}IMAP shared folders{/t}</h3>

				<input type='hidden' name='mail_acls_posted' value='1'>

				{foreach from=$folder_acls item=item key=user}
					{render acl=$aclACL}
						<div class="input-field">
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
							<button class="btn-small" type='submit' name='remove_acl_user_{$item.post_name}'>{msgPool type=delButton}</button>
						{/if}
					{/render}

					{if $user == "__member__"}
						{if $show_effective_memeber}
							<button class="btn-small" type='submit' name='show_effective_memeber'>{t}Hide{/t}</button>
						{else}
							<button class="btn-small" type='submit' name='show_effective_memeber'>{t}Show{/t}</button>
						{/if}
					{/if}

					{if $user == "__member__" && $show_effective_memeber}
						{foreach from=$Effective item=i key=k}
							<i>{$k}</i>
						{/foreach}
					{/if}
				{/foreach}
			</div>
		{/if}

		<div class="col s12 xl6">
			<h3>{t}Forward messages to non group members{/t}</h3>

			<div class="input-field">
				{render acl=$gosaMailForwardingAddressACL}
					<select name="forwarder_list[]" size=10 multiple>
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

			<div class="input-field add">
				{render acl=$gosaMailForwardingAddressACL}
					<input type='text' name="forward_address" value="">
				{/render}

				{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small" type='submit' name='add_forwarder'>
						{msgPool type=addButton}
					</button>
				{/render}

				{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small" type='submit' name='add_local_forwarder'>
						{t}Add local{/t}
					</button>
				{/render}

				{render acl=$gosaMailForwardingAddressACL}
					<button class="btn-small" type='submit' name='delete_forwarder'>
						{msgPool type=delButton}
					</button>
				{/render}
			</div>
		</div>
	</div>

	<div class="row">
		<div class="col s12">
			<h3 class="card-title">{t}Advanced mail options{/t}</h3>

			<div class="input-field">
				{render acl=$gosaMailDeliveryModeIACL}
					<input type="checkbox" name="only_local" value="1" {$only_local} title="{t}Select if user can only send and receive inside his own domain{/t}">
				{/render}

				<span>{t}User is only allowed to send and receive local mails{/t}</span>
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
</div>
