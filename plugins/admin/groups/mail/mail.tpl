<table style='width:100%; ' summary="{t}Mail settings{/t}">

 <tr>
  <td style='width:50%; '>

   <h3>{t}Generic{/t}</h3>
   <table summary="{t}Address and mail server settings{/t}">
    <tr>
     <td><label for="mail">{t}Primary address{/t}</label>{$must}</td>
     <td>
	 {if !$isModifyableMail && $initially_was_account}
		<input type='text' disabled size=30 value="{$mail}">
	 {else}
		 {if $domainSelectionEnabled}
			{render acl=$mailACL}
				<input id="mail" name="mail" size=20 maxlength=65 value="{$mail}"
					{if $mailEqualsCN} disabled {/if}
				>
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
				<input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
			{/render}
			{/if}
		{/if}
    {/if}
     </td>
    </tr>
    <tr>
     <td><label for="gosaMailServer">{t}Server{/t}</label></td>
     <td>
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
     {/if}
     </td>
    </tr>
    <tr>
     <td>&nbsp;
     </td>
    </tr>
{if $quotaEnabled}
    <tr>
     <td>{t}Quota usage{/t}</td>
     <td>{$quotaUsage}</td>
    </tr>
    <tr>
     <td><label for="gosaMailQuota">{t}Quota size{/t}</label></td>
     <td>
{render acl=$gosaMailQuotaACL}
      <input id="gosaMailQuota" name="gosaMailQuota" size="6" align="middle" maxlength="60"
        value="{$gosaMailQuota}" type='text'> MB
            {/render}
     </td>
    </tr>
{/if}

{if $folderTypesEnabled && !$multiple_support}
	<tr>
		<td>
			{t}Folder type{/t}
		</td>
		<td>
			<select id="FolderTypeCAT" name="FolderTypeCAT" onChange="document.mainform.submit();" size=1>
				{foreach from=$AvailableFolderTypes.CAT item=item key=key}
					<option {if $key == $FolderType.CAT} selected {/if} value="{$key}">{$item}</option>
				{/foreach}
			</select>
			<select id="FolderTypeSUB_CAT" name="FolderTypeSUB_CAT" onChange="document.mainform.submit();" size=1>
				{foreach from=$AvailableFolderTypes.SUB_CAT item=item key=key}
                    {if $key == $FolderType.CAT} 
						{foreach from=$item item=item2 key=key2}
							<option {if $key2 == $FolderType.SUB_CAT} selected {/if}
								value='{$key2}'>{$item2}</option>
						{/foreach}
					{/if}
				{/foreach}
			</select>
			{image path="images/lists/reload.png"}

		</td>
	</tr>
	{/if}
   </table>
  </td>

<!-- Alternate addresses -->
{if !$multiple_support}
  <td style='padding-left:2px;'>

   <h3>
	{t}Alternative addresses{/t}
   </h3>

{render acl=$gosaMailAlternateAddressACL}
   <select style="width:100%;" name="alternates_list[]" size=10 multiple 
	title="{t}List of alternative mail addresses{/t}">
    {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
	<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailAlternateAddressACL}
   <input type='text' name="alternate_address" size="30" align=middle maxlength="60" value="">
{/render}

{render acl=$gosaMailAlternateAddressACL}
   <button type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;

{/render}

{render acl=$gosaMailAlternateAddressACL}
   <button type='submit' name='delete_alternate'>{msgPool type=delButton}</button>

{/render}
  </td>
{/if}
 </tr>
</table>

<hr>

{if !$multiple_support}
<table style='width:100%; ' summary="{t}Mail folder configuration{/t}">

 <tr>
  <td style='width:50%; ' class='right-border'>

   <h3>
		{t}IMAP shared folders{/t}
   </h3>
   <input type='hidden' name='mail_acls_posted' value='1'>
   <table summary="{t}Folder permissions{/t}" cellpadding=0 border=0>
	{foreach from=$folder_acls item=item key=user}
		<tr>
		{if $user == "__anyone__"}
     		<td><LABEL for="default_permissions">{t}Default permission{/t}</LABEL></td>
		{elseif $user == "__member__"}
     		<td><LABEL for="member_permissions">{t}Member permission{/t}</LABEL></td>
		{else}
     		<td>
				<input type='text' name='acl_user_{$item.post_name}' value='{$user}'>
			</td>
		{/if}
		 <td>
{render acl=$aclACL}
		  <select size="1" name="acl_value_{$item.post_name}">
		   {html_options options=$AclTypes selected=$item.acl}
		   <option disabled>&nbsp;</option>
		  </select>
			{if !($user == "__anyone__" || $user == "__member__")}
		  		<button type='submit' name='remove_acl_user_{$item.post_name}'>{msgPool type=delButton}</button>

			{/if}
{/render}
			{if $user == "__member__"}
				{if $show_effective_memeber}
					<button type='submit' name='show_effective_memeber'>{t}Hide{/t}</button> 

				{else}
					<button type='submit' name='show_effective_memeber'>{t}Show{/t}</button> 

				{/if}
			{/if}
		 </td>
		</tr>
		{if $user == "__member__" && $show_effective_memeber}
			{foreach from=$Effective item=i key=k}
				<tr><td>&nbsp;&nbsp;<i>{$k}</i></td></tr>
			{/foreach}
		{/if}
	{/foreach}

		
		<tr>
			<td colspan="1"></td>
			<td><button type='submit' name='add_acl_user'>{msgPool type=addButton}</button>

		</tr>
   </table>
{/if}      

<hr>

<h3>{t}Advanced mail options{/t}</h3> 
<table style='width:100%; ' summary="{t}Advanced mail options{/t}">

 <tr>
  <td>
{render acl=$gosaMailDeliveryModeIACL}
   <input type="checkbox" name="only_local" value="1" {$only_local} 
	title="{t}Select if user can only send and receive inside his own domain{/t}">
{/render}
	{t}User is only allowed to send and receive local mails{/t}
  </td> 
 </tr>
 </table> 

  </td>
  <td style='width:50%'>

   <h3>{t}Forward messages to non group members{/t}</h3>

{render acl=$gosaMailForwardingAddressACL}
   <select style="width:100%;" name="forwarder_list[]" size=10 multiple>

	{if $multiple_support}

		{foreach from=$Forward_all item=item key=key}
			<option value="{$item}">{$item}&nbsp;({t}Used in all groups{/t})</option>
		{/foreach}
		{foreach from=$Forward_some item=item key=key}
			<option value="{$item}" style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;'>{$item}&nbsp;({t}Not used in all groups{/t})</option>
		{/foreach}
	{else}
    {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
	<option disabled>&nbsp;</option>
	{/if}
   </select>
{/render}

   <br>

{render acl=$gosaMailForwardingAddressACL}
   <input type='text' name="forward_address" size=20 align=middle maxlength=65 value="">
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <button type='submit' name='add_forwarder'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$gosaMailForwardingAddressACL}
   <button type='submit' name='add_local_forwarder'>{t}Add local{/t}</button>&nbsp;

{/render}
{render acl=$gosaMailForwardingAddressACL}
   <button type='submit' name='delete_forwarder'>{msgPool type=delButton}</button>

{/render}

  </td>
 </tr>
</table>
<input type="hidden" name='mailedit' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('mail');
  -->
</script>
{if $multiple_support}
	<input type="hidden" name="multiple_mail_group_posted" value="1">
{/if}
