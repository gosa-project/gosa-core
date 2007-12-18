<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">

   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
   <table summary="">
    <tr>
     <td><label for="mail">{t}Primary address{/t}</label>{$must}</td>
     <td>
{if $multiple_support}
	<input id="mail" name="dummy1" size=35 maxlength=65 value="{t}Multiple edit{/t}" disabled>
{else}
{render acl=$mailACL}
	<input id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
{/render}
{/if}
     </td>
    </tr>
    <tr>
     <td><label for="gosaMailServer">{t}Server{/t}</label></td>
     <td>
{render acl=$gosaMailServerACL checkbox=$multiple_support checked=$use_gosaMailServer}
      <select size="1" id="gosaMailServer" name="gosaMailServer" title="{t}Specify the mail server where the user will be hosted on{/t}">
	{html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
	<option disabled>&nbsp;</option>
      </select>
{/render}
     </td>
    </tr>
{if !$multiple_support}
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
{/if}
    <tr>
     <td><label for="gosaMailQuota">{t}Quota size{/t}</label></td>
     <td>
{render acl=$gosaMailQuotaACL checkbox=$multiple_support checked=$use_gosaMailQuota}
      <input id="gosaMailQuota" name="gosaMailQuota" size="6" align="middle" maxlength="60" 
		value="{$gosaMailQuota}"> MB
{/render}
     </td>
    </tr>
	</table>
  </td>

{if !$multiple_support}

  <td style="border-left:1px solid #A0A0A0;vertical-align:top;">
	&nbsp;
  </td>
	<td>	
   <h2><img class="center" alt="" align="middle" src="images/alternatemail.png" /><label for="alternates_list"> {t}Alternative addresses{/t}</label></h2>
{render acl=$gosaMailAlternateAddressACL}
   <select id="alternates_list" style="width:100%;height:100px;" name="alternates_list[]" size="15" multiple 
	title="{t}List of alternative mail addresses{/t}">
	{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
	<option disabled>&nbsp;</option>
{/render}
   </select>
   <br />
{render acl=$gosaMailAlternateAddressACL}
   <input name="alternate_address" size="30" align="middle" maxlength="65" value="">
{/render}
{render acl=$gosaMailAlternateAddressACL}
   <input type=submit value="{t}Add{/t}" name="add_alternate">
{/render}
{render acl=$gosaMailAlternateAddressACL}
   <input type=submit value="{t}Delete{/t}" name="delete_alternate">
{/render}
  </td>

{/if}

 </tr>
</table>

{if $multiple_support}

<p class="seperator">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/envelope.png" />&nbsp;{t}Mail options{/t}</h2>

{else}

<!-- SIEVE -->
<p class="seperator">&nbsp;</p>
<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td>
{render acl=$gosaMailDeliveryModeCACL}
   <input class="center" type=checkbox name="own_script" value="1" {$own_script} 
		onClick="	
			changeState('sieveManagement');
			changeState('drop_own_mails');
			changeState('use_vacation');
			changeState('use_spam_filter');
			changeState('use_mailsize_limit');
			changeState('import_vacation');
			changeState('vacation_template');
//			changeState('forward_address');
//			changeState('add_forwarder');
//			changeState('add_local_forwarder');
//			changeState('delete_forwarder');
			changeState('only_local');
			changeState('gosaVacationMessage');
			changeState('gosaSpamSortLevel');
			changeState('gosaSpamMailbox');
			changeState('gosaMailMaxSize');
			changeStates();

		"

> {t}Use custom sieve script{/t} <b>({t}disables all Mail options!{/t})</b>
{/render}
  </td>
 </tr>
 <tr>
  <td>
{render acl=$sieveManagementACL}	
   <input {if $own_script == ""} disabled {/if} id='sieveManagement' type='submit' name='sieveManagement' value='{t}Sieve Management{/t}'>
{/render}
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>
{/if}

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr style="padding-bottom:0px;">
  <td style="width:50%">
{render acl=$gosaMailDeliveryModeIACL checkbox=$multiple_support checked=$use_drop_own_mails}
   <input {if $own_script != ""} disabled {/if} class="center" id='drop_own_mails' type=checkbox name="drop_own_mails" value="1" {$drop_own_mails} title="{t}Select if you want to forward mails without getting own copies of them{/t}"> {t}No delivery to own mailbox{/t}
{/render}

<br>
{render acl=$gosaMailDeliveryModeVACL checkbox=$multiple_support checked=$use_use_vacation}
 <input type=checkbox name="use_vacation" value="1" {$use_vacation} id="use_vacation" {if $own_script != ""} disabled {/if}
    title="{t}Select to automatically response with the vacation message defined below{/t}" class="center"
onclick="changeState('day'); changeState('month'); changeState('year'); changeState('sday'); changeState('smonth'); changeState('syear');
"> {t}Activate vacation message{/t}
{/render}

   <br>

    <table>
         <tr>
          <td>
    {t}from{/t}
          </td>
          <td>
{render acl=$gosaVacationMessageACL}
    <select {if $own_script != ""} disabled {/if} name=day id="day" onChange="createResult(this.form,this.form.gosaVacationStart);" {$rangeEnabled}>
        {html_options values=$days output=$days selected=$start_day}
    </select>
{/render}
{render acl=$gosaVacationMessageACL}
    <select  {if $own_script != ""} disabled {/if} name=month id="month" onChange="populate(this.form,this.form.gosaVacationStart);" {$rangeEnabled}>
        {html_options options=$months selected=$start_month}
    </select>
{/render}
{render acl=$gosaVacationMessageACL}
    <select  {if $own_script != ""} disabled {/if} name=year id="year" onChange="populate(this.form,this.form.gosaVacationStart);" {$rangeEnabled}>
        {html_options values=$years output=$years selected=$start_year}
    </select>
{/render}
    <input type="hidden" name="gosaVacationStart" value="{$gosaVacationStart}">
          </td>
         </tr>
         <tr>
          <td>
    {t}till{/t}
          </td>
          <td>
{render acl=$gosaVacationMessageACL}
    <select  {if $own_script != ""} disabled {/if} name=sday id="sday" onChange="createResult2(this.form,this.form.gosaVacationStop);" {$rangeEnabled}>
        {html_options values=$days output=$days selected=$end_day}
    </select>
{/render}
{render acl=$gosaVacationMessageACL}
    <select  {if $own_script != ""} disabled {/if} name=smonth id="smonth" onChange="populate2(this.form,this.form.gosaVacationStop);" {$rangeEnabled}>
        {html_options options=$months selected=$end_month}
    </select>
{/render}
{render acl=$gosaVacationMessageACL}
    <select {if $own_script != ""} disabled {/if}  name=syear id="syear" onChange="populate2(this.form,this.form.gosaVacationStop);" {$rangeEnabled}>
        {html_options values=$years output=$years selected=$end_year}
    </select>
{/render}
    <input type="hidden" name="gosaVacationStop" value="{$gosaVacationStop}">
          </td>
         </tr>
        </table>


   <td rowspan=2 style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
{render acl=$gosaMailDeliveryModeSACL checkbox=$multiple_support checked=$use_use_spam_filter}
   <input {if $own_script != ""} disabled {/if} id='use_spam_filter' type=checkbox name="use_spam_filter" value="1" {$use_spam_filter} title="{t}Select if you want to filter this mails through spamassassin{/t}" class="center">
{/render}
 <label for="gosaSpamSortLevel">{t}Move mails tagged with spam level greater than{/t}</label>
	
{render acl=$gosaSpamSortLevelACL checkbox=$multiple_support checked=$use_gosaSpamSortLevel}
   <select {if $own_script != ""} disabled {/if} id="gosaSpamSortLevel" size="1" name="gosaSpamSortLevel" title="{t}Choose spam level - smaller values are more sensitive{/t}">
        {html_options values=$spamlevel output=$spamlevel selected=$gosaSpamSortLevel}
   </select>
{/render}
   <label for="gosaSpamMailbox">{t}to folder{/t}</label>
{render acl=$gosaSpamMailboxACL checkbox=$multiple_support checked=$use_gosaSpamMailbox}
   <select {if $own_script != ""} disabled {/if} size="1" id="gosaSpamMailbox" name="gosaSpamMailbox">
        	{html_options values=$spambox output=$spambox selected=$gosaSpamMailbox}
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailDeliveryModeRACL checkbox=$multiple_support checked=$use_use_mailsize_limit}
   <input {if $own_script != ""} disabled {/if} id='use_mailsize_limit' type=checkbox name="use_mailsize_limit" value="1" {$use_mailsize_limit} class="center">
{/render}
	<label for="gosaMailMaxSize">{t}Reject mails bigger than{/t}</label> 
{render acl=$gosaMailMaxSizeACL checkbox=$multiple_support checked=$use_gosaMailMaxSize}
   <input {if $own_script != ""} disabled {/if} id="gosaMailMaxSize" name="gosaMailMaxSize" size="6" align="middle" maxlength="30" value="{$gosaMailMaxSize}" class="center"> {t}MB{/t}
{/render}
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top; width:45%">
   <p style="margin-bottom:0px;">
    <b><label for="gosaVacationMessage">{t}Vacation message{/t}</label></b>
   </p>
{render acl=$gosaVacationMessageACL checkbox=$multiple_support checked=$use_gosaVacationMessage}
   <textarea {if $own_script != ""} disabled {/if} id="gosaVacationMessage" style="width:99%; height:100px;" name="gosaVacationMessage" rows="4" cols="512">{$gosaVacationMessage}</textarea>
{/render}
   <br>

   {if $show_templates eq "true"}
{render acl=$gosaVacationMessageACL}
   <select id='vacation_template' name="vacation_template" {if $own_script != ""} disabled {/if}>
        	{html_options options=$vacationtemplates selected=$template}
		<option disabled>&nbsp;</option>
   </select>
{/render}
{render acl=$gosaVacationMessageACL}
   <input {if $own_script != ""} disabled {/if} id='import_vacation' type="submit" value="{t}Import{/t}" name="import_vacation">
{/render}
   {/if}
  </td>
  <td>
   <p style="margin-bottom:0px;">
    <b><label for="forwarder_list">{t}Forward messages to{/t}</label></b>
   </p>

{if $multiple_support}
<input type="checkbox" name="use_gosaMailForwardingAddress" onclick="changeState('gosaMailForwardingAddress');" 
	class="center" {if $use_gosaMailForwardingAddress} checked {/if}>   
{/if}

{render acl=$gosaMailForwardingAddressACL}
   <select {if $use_gosaMailForwardingAddress} checked {/if}
		id="gosaMailForwardingAddress" style="width:100%; height:100px;" name="forwarder_list[]" size=15 multiple>
			{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress selected=$template}        
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailForwardingAddressACL}
   <input id='forward_address' name="forward_address" size=20 align="middle" maxlength=65 value="">
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input id='add_forwarder' type="submit" value="{t}Add{/t}" name="add_forwarder" >&nbsp;
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input id='add_local_forwarder' type="submit" value="{t}Add local{/t}" name="add_local_forwarder" >&nbsp;
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input id='delete_forwarder' type="submit" value="{t}Delete{/t}" name="delete_forwarder">
{/render}
  </td>
 </tr>
</table>
<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/false.png" />&nbsp;{t}Advanced mail options{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <tr>
  <td>
{render acl=$gosaMailDeliveryModeLACL checkbox=$multiple_support checked=$use_only_local}
   <input {if $own_script != ""} disabled {/if} id='only_local' type=checkbox name="only_local" value="1" {$only_local} title="{t}Select if user can only send and receive inside his own domain{/t}" class="center">
{/render}
   {t}User is only allowed to send and receive local mails{/t}
  </td>
 </tr>
</table>

<input type="hidden" name="mailTab" value="mailTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">

	{literal}
	function changeStates()
	{

		if(document.getElementById('use_vacation').checked){
			changeState('day');
			changeState('month');
			changeState('year');
			changeState('sday');
			changeState('smonth');
			changeState('syear');
		}else{
			changeSubselectState('use_vacation','day');
			changeSubselectState('use_vacation','month');
			changeSubselectState('use_vacation','year');
			changeSubselectState('use_vacation','sday');
			changeSubselectState('use_vacation','smonth');
			changeSubselectState('use_vacation','syear');
		}
	}

	{/literal}

<!-- // First input field on page
	focus_field('mail');
  -->
</script>
