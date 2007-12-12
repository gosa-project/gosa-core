<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
   <table summary="">
    <tr>
     <td><label for="scalixMailnode">{t}Scalix mailnode{/t}</label>{$must}</td>
     <td>
{render acl=$scalixMailnodeACL}
		<input id="scalixMailnode" name="scalixMailnode" size=35 maxlength=65 value="{$scalixMailnode}">
{/render}
	 </td>
    </tr>
    <tr>
     <td><label for="scalixMailboxClass">{t}Salix mailbox class{/t}</label></td>
     <td>
{render acl=$scalixMailboxClassACL}
      <select size="1" id="scalixMailboxClass" name="scalixMailboxClass"  
			title="{t}Limited users con not make use of the group calendar functionality in Scalix{/t}">
		    {html_options values=$mailboxClasses output=$mailboxClasses selected=$scalixMailboxClass}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="scalixServerLanguage">{t}Scalix server language{/t}</label></td>
     <td>
{render acl=$scalixServerLanguageACL}
      <select size="1" id="scalixServerLanguage" name="scalixServerLanguage" 
			title="{t}Message catalog language for client.{/t}">
		    {html_options values=$serverLanguages output=$serverLanguages selected=$scalixServerLanguage}
      </select>
{/render}
     </td>
    </tr>
   </table>
   
   <p class="seperator">&nbsp;</p>
   
   <table summary="">
    <tr>
     <td>
{render acl=$scalixAdministratorACL}
	  <input type=checkbox name="scalixAdministrator" value="1" {$scalixAdministrator}
	   title="{t}Select for admin capability.{/t}"> {t}Scalix Administrator{/t}
{/render}
	  <br>
{render acl=$scalixMailboxAdministratorACL}
	  <input type=checkbox name="scalixMailboxAdministrator" value="1" {$scalixMailboxAdministrator}
	   title="{t}Select for mailbox admin capability.{/t}"> {t}Scalix Mailbox Administrator{/t}
{/render}
	  <br>
{render acl=$scalixHideUserEntryACL}
	  <input type=checkbox name="scalixHideUserEntry" value="1" {$scalixHideUserEntry}
	   title="{t}Hide user entry from addressbook.{/t}"> {t}Hide this user entry in Scalix{/t}
{/render}
	  <br>
   </table>
   
   <p class="seperator">&nbsp;</p>
   
   <table summary="">
    <tr>
     <td><label for="scalixLimitMailboxSize">{t}Limit mailbox size{/t}</label></td>
     <td>
{render acl=$scalixLimitMailboxSizeACL}
		<input id="scalixLimitMailboxSize" name="scalixLimitMailboxSize" size=5 maxlength=10 value="{$scalixLimitMailboxSize}">&nbsp;{t}MB{/t}
{/render}
	 </td>
    </tr>
    <tr>
     <td >
{render acl=$scalixLimitOutboundMailACL}
	  <input type=checkbox name="scalixLimitOutboundMail" value="1" {$scalixLimitOutboundMail}
	   title="{t}As sanction on mailbox quota overuse, stop user from sending mail.{/t}"> {t}Limit Outbound Mail{/t}
{/render}
	  <br>
{render acl=$scalixLimitInboundMailACL}
	  <input type=checkbox name="scalixLimitInboundMail" value="1" {$scalixLimitInboundMail}
	   title="{t}As sanction on mailbox quota overuse, stop user from receiving mail.{/t}"> {t}Limit Inbound Mail{/t}
{/render}
	  <br>
{render acl=$scalixLimitNotifyUserACL}
	  <input type=checkbox name="scalixLimitNotifyUser" value="1" {$scalixLimitNotifyUser}
	   title="{t}As sanction on mailbox quota overuse, notify the user by eMail.{/t}"> {t}Notify User{/t}
{/render}
	  <br>
     </td>
    </tr>
   </table>
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/envelope.png" /><label for="emails_list"> {t}Scalix email addresses{/t}</label></h2>
{render acl=$scalixEmailAddressACL}
   <select id="emails_list" style="width:100%;height:100px;" name="emails_list[]" size="15"
		 multiple title="{t}List of scalix email addresses{/t}" >
            {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br />
{render acl=$scalixEmailAddressACL}
   <input name="email_address" size="30" align="middle" maxlength="65" value="">
{/render}
{render acl=$scalixEmailAddressACL}
   <input type=submit value="{t}Add{/t}" name="add_email">&nbsp;
{/render}
{render acl=$scalixEmailAddressACL}
   <input type=submit value="{t}Delete{/t}" name="delete_email">
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
