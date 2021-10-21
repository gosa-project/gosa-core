<div class="kolab-wrapper">
	{if $mail_account}
		<div class="row">
			<div class="col s12">
				<h3>{t}Kolab account{/t}</h3>
				<p>{t}The Kolab account is currently disabled. It's features can be adjusted if you add a mail account.{/t}</p>
			</div>
		</div>
	{else}
		<div class="row">
			<div class="col s12">
				{if $multiple_support}
					<div class="multi-check">
						<label>
							<p>
								<input type="checkbox" name="use_kolabState" value="1" {if $use_kolabState} checked {/if} onClick="changeState('kolabState');">
								<span></span>
							</p>
						</label>

						<label>
							<p>
								<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if} {if  !$use_kolabState} disabled {/if}>
								<span>{t}Kolab account{/t}</span>
							</p>
						</label>
					</div>
				{else}
					<label>
						<p>
							<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if} {if (!$is_account && $is_createable)
								|| ($is_account && $is_removeable)} {else} disabled {/if} onClick="
								{if $kolabDelegate_W}
								changeState('delegate_list');
								changeState('delegate_address');
								changeState('add_delegation');
								changeState('delete_delegation');
								{/if}

								{if $unrestrictedMailSize_W}
								changeState('unrestrictedMailSize');
								{/if}

								{if $calFBURL_W}
								changeState('calFBURL');
								{/if}

								{if $kolabFreeBusyFuture_W}
								changeState('kolabFreeBusyFuture');
								{/if}

								{if $kolabInvitationPolicy_W}
								{$changeState}
								{/if}">
							<span>{t}Kolab account{/t}</span>
						</p>
					</label>
				{/if}
			</div>
		</div>

    	<div class="row kolab-account">
			<div class="col s12 xl6">
				<h3>{t}Delegations{/t}</h3>

				{if $multiple_support}
					<label>
						<p>
							<input type="checkbox" name="use_kolabDelegate" {if $use_kolabDelegate} checked {/if}
								class="center" onClick="changeState('delegate_list');">
							<span></span>
						</p>
					</label>

					<div class="input-field">
						<select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple {if !$use_kolabDelegate}disabled{/if}>
							{html_options values=$kolabDelegate output=$kolabDelegate}
						</select>
					</div>

					<div class="input-field add">
						<input type='text' name="delegate_address" size=30 align=middle maxlength=60 value="" id="delegate_address">
						<button class="btn-small" type='submit' name='add_delegation' id="add_delegation">{msgPool type=addButton}</button>
						<button class="btn-small" type='submit' name='delete_delegation' id="delete_delegation">{msgPool type=delButton}</button>
					</div>
				{else}
					{render acl=$kolabDelegateACL}
						<div class="input-field">
							<select id="delegate_list" name="delegate_list[]" multiple {if !$kolabState}disabled{/if}>
								{html_options values=$kolabDelegate output=$kolabDelegate}
							</select>
						</div>
					{/render}

					{render acl=$kolabDelegateACL}
						<div class="input-field add">
							<input type='text' name="delegate_address" {if !$kolabState} disabled {/if} value="" id="delegate_address">
							<button class="btn-small" type='submit' name='add_delegation' id="add_delegation" {if !$kolabState} disabled {/if}>{msgPool type=addButton}</button>
							<button class="btn-small" type='submit' name='delete_delegation' id="delete_delegation" {if !$kolabState} disabled {/if}>{msgPool type=delButton}</button>
						</div>
					{/render}
				{/if}

				<h3>{t}Mail size{/t}</h3>

				{render acl=$unrestrictedMailSizeACL checkbox=$multiple_support checked=$use_unrestrictedMailSize}
					<label>
						<p>
							<input class="center" type="checkbox" id='unrestrictedMailSize' name="unrestrictedMailSize" value="1" {if !$kolabState && !$multiple_support} disabled {/if} {$unrestrictedMailSizeState}>
							<span>{t}No mail size restriction for this account{/t}</span>
						</p>
					</label>
				{/render}
			</div>

			<div class="col s12 xl6">
				<h3>{t}Free Busy information{/t}</h3>

				{render acl=$calFBURLACL checkbox=$multiple_support checked=$use_calFBURL}
					<div class="input-field">
						<input type='text' id="calFBURL" name="calFBURL" value="{$calFBURL}" {if !$kolabState && !$multiple_support} disabled {/if}>
						<label for="calFBURL">{t}URL{/t}</label>
					</div>
				{/render}

				{render acl=$kolabFreeBusyFutureACL checkbox=$multiple_support checked=$use_kolabFreeBusyFuture}
					<div class="input-field">
						<input type='text' id="kolabFreeBusyFuture" name="kolabFreeBusyFuture" size=5 maxlength=6 {if !$kolabState && !$multiple_support} disabled {/if} value="{$kolabFreeBusyFuture}">
						<label for="kolabFreeBusyFuture">{t}Future{/t}&nbsp;({t}days{/t})</label>
					</div>
				{/render}

				<h3>{t}Invitation policy{/t}</h3>

				{if $multiple_support}
					<label>
						<p>
							<input type="checkbox" name="use_kolabInvitationPolicy" {if $use_kolabInvitationPolicy} checked {/if} value="1" class="center">
							<span></span>
						</p>
					</label>
				{/if}

				{render acl=$kolabInvitationPolicyACL}
					{$invitation}
				{/render}
			</div>
		</div>
	{/if}
</div>
