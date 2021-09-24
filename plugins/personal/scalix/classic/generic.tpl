<table style='width:100%; ' summary="{t}SCALIX settings{/t}">

 <tr>
  <td style='width:50%; '>

   <h3>{t}Generic{/t}
</h3>
<!-- Hide user specific attributes when in group mode. -->
   <table summary="{t}Mail settings{/t}" >
    <tr>
     <td><label for="scalixMailnode">{t}SCALIX mail node{/t}</label>{$must}</td>
     <td>
{render acl=$scalixMailnodeACL}
		<input type='text' id="scalixMailnode" name="scalixMailnode" size=35 maxlength=65 value="{$scalixMailnode}">
{/render}
	 </td>
    </tr>
{if !$scalixGroup}
    <tr>
     <td><label for="scalixMailboxClass">{t}SCALIX mailbox class{/t}</label></td>
     <td>
{render acl=$scalixMailboxClassACL}
      <select size="1" id="scalixMailboxClass" name="scalixMailboxClass"  
			title="{t}Limited users can not make use of the group calendar functionality in SCALIX{/t}">
		    {html_options values=$mailboxClasses output=$mailboxClasses selected=$scalixMailboxClass}
      </select>
{/render}
     </td>
    </tr>


    <tr>
     <td><label for="scalixServerLanguage">{t}SCALIX server language{/t}</label></td>
     <td>
{render acl=$scalixServerLanguageACL}
      <select size="1" id="scalixServerLanguage" name="scalixServerLanguage" 
			title="{t}Message catalog language for client{/t}">
		    {html_options values=$serverLanguages output=$serverLanguages selected=$scalixServerLanguage}
      </select>
{/render}
     </td>
    </tr>
{/if} 
   </table>
  
{if !$scalixGroup}
   <hr>
   
   <table summary="{t}Settings{/t}" >
    <tr>
     <td>
{render acl=$scalixAdministratorACL}
	  <input type=checkbox name="scalixAdministrator" value="1" {$scalixAdministrator}
	   title="{t}Select for administrator capabilities{/t}"> {t}SCALIX Administrator{/t}
{/render}
	  <br>
{render acl=$scalixMailboxAdministratorACL}
	  <input type=checkbox name="scalixMailboxAdministrator" value="1" {$scalixMailboxAdministrator}
	   title="{t}Select for mailbox administrator capabilities{/t}"> {t}SCALIX Mailbox Administrator{/t}
{/render}
	  <br>
{render acl=$scalixHideUserEntryACL}
	  <input type=checkbox name="scalixHideUserEntry" value="1" {$scalixHideUserEntry}
	   title="{t}Hide user entry from address book{/t}"> {t}Hide this user entry in SCALIX{/t}
{/render}
	  <br>
   </table>
   
   <hr>
   
   <table summary="{t}Settings{/t}" >
    <tr>
     <td><label for="scalixLimitMailboxSize">{t}Limit mailbox size{/t}</label></td>
     <td>
{render acl=$scalixLimitMailboxSizeACL}
		<input type='text' id="scalixLimitMailboxSize" name="scalixLimitMailboxSize" size=5 maxlength=10 value="{$scalixLimitMailboxSize}">&nbsp;{t}MB{/t}
{/render}
	 </td>
    </tr>
    <tr>
     <td >
{render acl=$scalixLimitOutboundMailACL}
	  <input type=checkbox name="scalixLimitOutboundMail" value="1" {$scalixLimitOutboundMail}
	   title="{t}As sanction on mailbox quota overuse, stop user from sending mail{/t}"> {t}Limit Outbound Mail{/t}
{/render}
	  <br>
{render acl=$scalixLimitInboundMailACL}
	  <input type=checkbox name="scalixLimitInboundMail" value="1" {$scalixLimitInboundMail}
	   title="{t}As sanction on mailbox quota overuse, stop user from receiving mail{/t}"> {t}Limit Inbound Mail{/t}
{/render}
	  <br>
{render acl=$scalixLimitNotifyUserACL}
	  <input type=checkbox name="scalixLimitNotifyUser" value="1" {$scalixLimitNotifyUser}
	   title="{t}As sanction on mailbox quota overuse, notify the user by email{/t}"> {t}Notify User{/t}
{/render}
	  <br>
     </td>
    </tr>
   </table>
{/if}
  </td>

  <td class='left-border'>

   &nbsp;
  </td>

  <td>

   <h3>
<label for="emails_list"> {t}SCALIX email addresses{/t}</label></h3>
{render acl=$scalixEmailAddressACL}
   <select id="emails_list" style="width:100%;height:100px;" name="emails_list[]" size="15"
		 multiple title="{t}List of SCALIX email addresses{/t}" >
            {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br />
{render acl=$scalixEmailAddressACL}
   <input type='text' name="email_address" size="30" align="middle" maxlength="65" value="">
{/render}
{render acl=$scalixEmailAddressACL}
   <button type='submit' name='add_email'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$scalixEmailAddressACL}
   <button type='submit' name='delete_email'>{msgPool type=delButton}</button>

{/render}
  </td>
 </tr>
</table>
<input type="hidden" name="scalixTab" value="scalixTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('scalixMailnode');
  -->
</script>
