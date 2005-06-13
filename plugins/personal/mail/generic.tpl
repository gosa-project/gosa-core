<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
   <table>
    <tr>
     <td>{t}Primary address{/t}{$must}</td>
     <td><input name="mail" size=25 maxlength=65 {$mailACL} value="{$mail}"></td>
    </tr>
    <tr>
     <td>{t}Server{/t}</td>
     <td>
      <select size="1" name="gosaMailServer" {$gosaMailServerACL} title="{t}Specify the mail server where the user will be hosted on{/t}">
		    {html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
			<option disabled></option>
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <br>
      {t}Quota usage{/t}
     </td>
     <td>
      <br>
      {if $quotadefined eq "true"}
      	{$quotausage}
      {else}
      {t}not defined{/t}
      {/if}
     </td>
    </tr>
    <tr>
     <td>{t}Quota size{/t}</td>
     <td>
      <input name="gosaMailQuota" size="6" align="middle" maxlength="30" {$gosaMailQuotaACL}
		value="{$gosaMailQuota}"> KB
     </td>
    </tr>
   </table>
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img alt="" align="middle" src="images/alternatemail.png"> {t}Alternative addresses{/t}</h2>
   <select style="width:350px; height:100px;" name="alternates_list[]" size=15
	 multiple title="{t}List of alternative mail addresses{/t}">
            {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
			<option disabled></option>
   </select>
   <br>
   <input name="alternate_address" size=30 align="middle" maxlength="65" {$gosaMailAlternateAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_alternate"
	{$gosaMailAlternateAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_alternate"
	{$gosaMailAlternateAddressACL}>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img alt="" align="middle" src="images/envelope.png"> {t}Mail options{/t}</h2>
<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr style="padding-bottom:0px;">
  <td style="width:45%">
   <input type=checkbox name="drop_own_mails" value="1" {$drop_own_mails}
	{$gosaMailDeliveryModeACL} title="{t}Select if you want to forward mails without getting own copies of them{/t}"> {t}No delivery to own mailbox{/t}
   <br>
   <input type=checkbox name="use_vacation" value="1" {$use_vacation}
	{$gosaVacationMessageACL} title="{t}Select to automatically response with the vacation message defined below{/t}"> {t}Activate vacation message{/t}
  </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <input type=checkbox name="use_spam_filter" value="1" {$use_spam_filter}
	{$gosaSpamSortLevelACL} title="{t}Select if you want to filter this mails through spamassassin{/t}"> {t}Move mails tagged with spam level greater than{/t}
   <select size="1" name="gosaSpamSortLevel" {$gosaSpamSortLevelACL} title="{t}Choose spam level - smaller values are more sensitive{/t}">
        {html_options values=$spamlevel output=$spamlevel selected=$gosaSpamSortLevel}
   </select>
   {t}to folder{/t}
   <select size="1" name="gosaSpamMailbox" {$gosaSpamMailboxACL}>
        	{html_options values=$spambox output=$spambox selected=$gosaSpamMailbox}
			<option disabled></option>
   </select>
   <br>
   <input type=checkbox name="use_mailsize_limit" value="1" {$use_mailsize_limit}
	{$gosaMailMaxSizeACL}> {t}Reject mails bigger than{/t} 
   <input name="gosaMailMaxSize" size=6 align="middle" maxlength="30" {$gosaMailMaxSizeACL}
	value="{$gosaMailMaxSize}"> {t}MB{/t}
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top; width:45%">
   <p style="margin-bottom:0px;">
    <b>{t}Vacation message{/t}</b>
   </p>
   <textarea style="width:350px; height:100px;" name="gosaVacationMessage" rows="4" cols="512"
	{$gosaVacationMessageACL}>{$gosaVacationMessage}</textarea>
   <br>
   {if $show_templates eq "true"}
   <select name="vacation_template">
        	{html_options options=$vacationtemplates selected=$template}
			<option disabled></option>
	{html_options options=$vacationtemplates selected=$template}
   </select>
   <input type="submit" value="{t}Import{/t}" name="import_vacation"
	{$gosaVacationMessageACL}>
   {/if}
  </td>
  <td>
   <p style="margin-bottom:0px;">
    <b>{t}Forward messages to{/t}</b>
   </p>
   <select style="width:350px; height:100px;" name="forwarder_list[]" size=15 multiple>
			{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress selected=$template}        
			<option disabled></option>
   </select>
   <br>
   <input name="forward_address" size=20 align="middle" maxlength=65
	{$gosaMailForwardingAddressACL} value="">
   <input type="submit" value="{t}Add{/t}" name="add_forwarder"
	{$gosaMailForwardingAddressACL}>&nbsp;
   <input type="submit" value="{t}Add local{/t}" name="add_local_forwarder"
	{$gosaMailForwardingAddressACL}>&nbsp;
   <input type="submit" value="{t}Delete{/t}" name="delete_forwarder"
	{$gosaMailForwardingAddressACL}>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img alt="" align="middle" src="images/false.png"> {t}Advanced mail options{/t}</h2>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td>
   <input type=checkbox name="only_local" value="1" {$only_local} {$only_localACL} title="{t}Select if user can only send and receive inside his own domain{/t}">
   {t}User is only allowed to send and receive local mails{/t}
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td>
   <input type=checkbox name="own_script" value="1" {$own_script} {$custom_sieveACL}>
   {t}Use custom sieve script{/t} <b>({t}disables all Mail options!{/t})</b>
  </td>
 </tr>
</table>

<input type="hidden" name="mailTab" value="mailTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.mail.focus();
  -->
</script>
