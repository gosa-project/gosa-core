<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
<!-- Hide user specific attributes when in group mode. -->
   <table summary="">
    <tr>
     <td><label for="scalixMailnode">{t}Scalix mailnode{/t}</label>{$must}</td>
     <td><input id="scalixMailnode" name="scalixMailnode" size=35 maxlength=65 {$scalixMailnodeACL} value="{$scalixMailnode}"></td>
    </tr>
{if !$scalixGroup}
    <tr>
     <td><label for="scalixMailboxClass">{t}Scalix mailbox class{/t}</label></td>
     <td>
      <select size="1" id="scalixMailboxClass" name="scalixMailboxClass" {$scalixMailboxClassACL} 
			title="{t}Limited users can not make use of the group calendar functionality in Scalix{/t}">
		    {html_options values=$mailboxClasses output=$mailboxClasses selected=$scalixMailboxClass}
      </select>
     </td>
    </tr>


    <tr>
     <td><label for="scalixServerLanguage">{t}Scalix server language{/t}</label></td>
     <td>
      <select size="1" id="scalixServerLanguage" name="scalixServerLanguage" {$scalixServerLanguageACL} 
			title="{t}Message catalog language for client.{/t}">
		    {html_options values=$serverLanguages output=$serverLanguages selected=$scalixServerLanguage}
      </select>
     </td>
    </tr>
{/if}
   </table>
{if !$scalixGroup}   
   <p class="seperator">&nbsp;</p>
   
   <table summary="">
    <tr>
     <td>
	  <input type=checkbox name="scalixAdministrator" value="1" {$scalixAdministrator}
	   {$scalixAdministratorACL} title="{t}Select for admin capability.{/t}"> {t}Scalix Administrator{/t}
	  <br>
	  <input type=checkbox name="scalixMailboxAdministrator" value="1" {$scalixMailboxAdministrator}
	   {$scalixMailboxAdministratorACL} title="{t}Select for mailbox admin capability.{/t}"> {t}Scalix Mailbox Administrator{/t}
	  <br>
	  <input type=checkbox name="scalixHideUserEntry" value="1" {$scalixHideUserEntry}
	   {$scalixHideUserEntryACL} title="{t}Hide user entry from addressbook.{/t}"> {t}Hide this user entry in Scalix{/t}
	  <br>
   </table>
   
   <p class="seperator">&nbsp;</p>
   
   <table summary="">
    <tr>
     <td><label for="scalixLimitMailboxSize">{t}Limit mailbox size{/t}</label></td>
     <td><input id="scalixLimitMailboxSize" name="scalixLimitMailboxSize" size=5 maxlength=10 {$scalixLimitMailboxSizeACL} value="{$scalixLimitMailboxSize}">&nbsp;{t}MB{/t}</td>
    </tr>
    <tr>
     <td >
	  <input type=checkbox name="scalixLimitOutboundMail" value="1" {$scalixLimitOutboundMail}
	   {$scalixLimitOutboundMailACL} title="{t}As sanction on mailbox quota overuse, stop user from sending mail.{/t}"> {t}Limit Outbound Mail{/t}
	  <br>
	  <input type=checkbox name="scalixLimitInboundMail" value="1" {$scalixLimitInboundMail}
	   {$scalixLimitInboundMailACL} title="{t}As sanction on mailbox quota overuse, stop user from receiving mail.{/t}"> {t}Limit Inbound Mail{/t}
	  <br>
	  <input type=checkbox name="scalixLimitNotifyUser" value="1" {$scalixLimitNotifyUser}
	   {$scalixLimitNotifyUserACL} title="{t}As sanction on mailbox quota overuse, notify the user by eMail.{/t}"> {t}Notify User{/t}
	  <br>
     </td>
    </tr>
   </table>
{/if}
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/envelope.png" /><label for="emails_list"> {t}Scalix email addresses{/t}</label></h2>
   <select id="emails_list" style="width:100%;height:100px;" name="emails_list[]" size="15"
		 multiple title="{t}List of scalix email addresses{/t}" {$scalixEmailAddressACL}>
            {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
			<option disabled>&nbsp;</option>
   </select>
   <br />
   <input name="email_address" size="30" align="middle" maxlength="65" {$scalixEmailAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_email"
	{$scalixEmailAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_email"
	{$scalixEmailAddressACL}>
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
