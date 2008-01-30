<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
   <table summary="">
    <tr>
     <td><label for="mail">{t}Primary address{/t}</label>{$must}</td>
     <td><input id="mail" name="mail" size=35 maxlength=65 {$mailACL} value="{$mail}"></td>
    </tr>
    <tr>
     <td><label for="gosaMailServer">{t}Server{/t}</label></td>
     <td>
      <select size="1" id="gosaMailServer" name="gosaMailServer" {$gosaMailServerACL} 
			title="{t}Specify the mail server where the user will be hosted on{/t}">
		    {html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
			<option disabled>&nbsp;</option>
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
     <td><label for="gosaMailQuota">{t}Quota size{/t}</label></td>
     <td>
      <input id="gosaMailQuota" name="gosaMailQuota" size="6" align="middle" maxlength="60" {$gosaMailQuotaACL}
		value="{$gosaMailQuota}"> MB
     </td>
    </tr>
   </table>
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/alternatemail.png" /><label for="alternates_list"> {t}Alternative addresses{/t}</label></h2>
   <select id="alternates_list" style="width:100%;height:100px;" name="alternates_list[]" size="15"
		 multiple title="{t}List of alternative mail addresses{/t}" {$gosaMailAlternateAddressACL}>
            {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
			<option disabled>&nbsp;</option>
   </select>
   <br />
   <input name="alternate_address" size="30" align="middle" maxlength="65" {$gosaMailAlternateAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_alternate"
	{$gosaMailAlternateAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_alternate"
	{$gosaMailAlternateAddressACL}>
  </td>
 </tr>
</table>

<!-- SIEVE -->
<p class="seperator">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/envelope.png" />&nbsp;{t}Mail options{/t}</h2>
<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td>
   <input type=checkbox name="own_script" value="1" {$own_script}
		{$custom_sieveACL}
        onClick="
            changeState('sieveManagement');
            changeState('drop_own_mails');
            changeState('use_vacation');
            changeState('use_spam_filter');
            changeState('use_mailsize_limit');
            changeState('import_vacation');
            changeState('vacation_template');
//          changeState('forward_address');
//          changeState('add_forwarder');
//          changeState('add_local_forwarder');
//          changeState('delete_forwarder');
            changeState('only_local');
            changeState('gosaVacationMessage');
            changeState('gosaSpamSortLevel');
            changeState('gosaSpamMailbox');
            changeState('gosaMailMaxSize');
            changeStates();

        "

> {t}Use custom sieve script{/t} <b>({t}disables all Mail options!{/t})</b>
  </td>
 </tr>
 <tr>
  <td>
   <input {$sieveManagementACL} {if $own_script == ""} disabled {/if} id='sieveManagement' type='submit' name='sieveManagement' value='{t}Sieve Management{/t}'>
  </td>
 </tr>
</table>


<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/envelope.png" />&nbsp;{t}Mail options{/t}</h2>
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr style="padding-bottom:0px;">
  <td style="width:50%">
   <input {if $own_script != ""} disabled {/if} type=checkbox name="drop_own_mails" value="1" {$drop_own_mails} id="drop_own_mails"
	{$drop_own_mailsACL} title="{t}Select if you want to forward mails without getting own copies of them{/t}"> {t}No delivery to own mailbox{/t}
   <br>
   <input type=checkbox 
		{if $own_script != ""} disabled {/if} 
			{$use_vacationACL} 
			name="use_vacation" value="1" {$use_vacation} id="use_vacation"
			title="{t}Select to automatically response with the vacation message defined below{/t}"  

 	  	{if $gosaVacationTimerACL == ""}
			onclick="	changeState('day'); 
						changeState('month'); 
						changeState('year'); 
						changeState('sday'); 
						changeState('smonth'); 
						changeState('syear');"
		{/if}
	  >
	 {t}Activate vacation message{/t}
   <br>
   <div style="padding-left:22px;">
	<table>
         <tr>
          <td>
	{t}from{/t}
          </td>
          <td>
	<select {if $own_script != ""} disabled {/if} name=day id="day" onChange="createResult(this.form,this.form.gosaVacationStart);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options values=$days output=$days selected=$start_day}
	</select>
	<select {if $own_script != ""} disabled {/if} name=month id="month" onChange="populate(this.form,this.form.gosaVacationStart);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options options=$months selected=$start_month}
	</select>
	<select {if $own_script != ""} disabled {/if} name=year id="year" onChange="populate(this.form,this.form.gosaVacationStart);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options values=$years output=$years selected=$start_year}
	</select>
	<input type="hidden" name="gosaVacationStart" value="{$gosaVacationStart}">
          </td>
         </tr>
         <tr>
          <td>
	{t}till{/t}
          </td>
          <td>
	<select {if $own_script != ""} disabled {/if} name=sday id="sday" onChange="createResult2(this.form,this.form.gosaVacationStop);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options values=$days output=$days selected=$end_day}
	</select>
	<select {if $own_script != ""} disabled {/if} name=smonth id="smonth" onChange="populate2(this.form,this.form.gosaVacationStop);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options options=$months selected=$end_month}
	</select>
	<select {if $own_script != ""} disabled {/if} name=syear id="syear" onChange="populate2(this.form,this.form.gosaVacationStop);" {$gosaVacationTimerACL} {$rangeEnabled}>
		{html_options values=$years output=$years selected=$end_year}
	</select>
	<input type="hidden" name="gosaVacationStop" value="{$gosaVacationStop}">
          </td>
         </tr>
        </table>

   </div>
  </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <input type=checkbox {if $own_script != ""} disabled {/if} name="use_spam_filter" value="1" {$use_spam_filter} id="use_spam_filter"
	{$gosaSpamSortLevelACL} title="{t}Select if you want to filter this mails through spamassassin{/t}"> <label for="gosaSpamSortLevel">{t}Move mails tagged with spam level greater than{/t}</label>
	
   <select id="gosaSpamSortLevel" size="1" {if $own_script != ""} disabled {/if} name="gosaSpamSortLevel" {$gosaSpamSortLevelACL} title="{t}Choose spam level - smaller values are more sensitive{/t}">
        {html_options values=$spamlevel output=$spamlevel selected=$gosaSpamSortLevel}
   </select>
   <label for="gosaSpamMailbox">{t}to folder{/t}</label>
   <select size="1" id="gosaSpamMailbox" {if $own_script != ""} disabled {/if} name="gosaSpamMailbox" {$gosaSpamMailboxACL}>
        	{html_options values=$spambox output=$spambox selected=$gosaSpamMailbox}
			<option disabled>&nbsp;</option>
   </select>
   <br>
   <input type=checkbox {if $own_script != ""} disabled {/if} name="use_mailsize_limit" value="1" {$use_mailsize_limit} id="use_mailsize_limit"
	{$gosaMailMaxSizeACL}> <label for="gosaMailMaxSize">{t}Reject mails bigger than{/t}</label> 
   <input id="gosaMailMaxSize" {if $own_script != ""} disabled {/if} name="gosaMailMaxSize" size="6" align="middle" maxlength="30" {$gosaMailMaxSizeACL}
	value="{$gosaMailMaxSize}"> {t}MB{/t}
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top; width:45%">
   <p style="margin-bottom:0px;">
    <b><label for="gosaVacationMessage">{t}Vacation message{/t}</label></b>
   </p>
   <textarea id="gosaVacationMessage" style="width:99%; height:100px;" {if $own_script != ""} disabled {/if} name="gosaVacationMessage" rows="4" cols="512"
	{$gosaVacationMessageACL}>{$gosaVacationMessage}</textarea>
   <br>
   {if $show_templates eq "true"}
   <select name="vacation_template" id="vacation_template" {if $own_script != ""} disabled {/if} >
        	{html_options options=$vacationtemplates selected=$template}
		<option disabled>&nbsp;</option>
   </select>
   <input type="submit" value="{t}Import{/t}" name="import_vacation" id="import_vacation"
	{$gosaVacationMessageACL} {if $own_script != ""} disabled {/if} >
   {/if}
   <br>
  </td>
  <td>
   <p style="margin-bottom:0px;">
    <b><label for="forwarder_list">{t}Forward messages to{/t}</label></b>
   </p>
   <select id="gosaMailForwardingAddress" style="width:100%; height:100px;" name="forwarder_list[]" size=15 multiple {$gosaMailForwardingAddressACL}>
			{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress selected=$template}        
			<option disabled>&nbsp;</option>
   </select>
   <br>
   <input name="forward_address" size=20 align="middle" maxlength=65 id="forward_address"
	{$gosaMailForwardingAddressACL} value="">
   <input type="submit" value="{t}Add{/t}" name="add_forwarder" id="add_forwarder"
	{$gosaMailForwardingAddressACL}>&nbsp;
   <input type="submit" value="{t}Add local{/t}" name="add_local_forwarder" id="add_local_forwarder"
	{$gosaMailForwardingAddressACL}>&nbsp;
   <input type="submit" value="{t}Delete{/t}" name="delete_forwarder" id="delete_forwarder"
	{$gosaMailForwardingAddressACL}>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/false.png" />&nbsp;{t}Advanced mail options{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <tr>
  <td>
   <input id="only_local" type=checkbox {if $own_script != ""} disabled {/if} name="only_local" value="1" {$only_local} {$only_localACL} title="{t}Select if user can only send and receive inside his own domain{/t}">
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
