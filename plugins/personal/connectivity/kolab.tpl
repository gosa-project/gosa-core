
{if $mail_account ne 'true'}
	<h2>{t}Kolab account{/t}</h2>
	{t}The kolab account is currently disabled. It's features can be adjusted if you add a mail account.{/t}
{else}
<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if} 
	onClick="
	changeState('delegate_list');
	changeState('delegate_address');
	changeState('add_delegation');
	changeState('delete_delegation');
	changeState('unrestrictedMailSize');
	changeState('calFBURL');
	changeState('kolabFreeBusyFuture');
	changeState('kolabInvitationPolicy');
	{$changeState}"
	{$kolabAccountACL}
>
<b>{t}Kolab account{/t}</b>
<br>
<br>
<table summary="" style="width:100%">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <b><LABEL for="delegate_list">{t}Delegations{/t}</LABEL></b><br>
   <select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple  {if !$kolabState} disabled {/if} >
    {html_options values=$kolabDelegate output=$kolabDelegate}
	<option disabled>&nbsp;</option>
   </select>
   <br>
   <input name="delegate_address" size=30 align=middle maxlength=60 {if !$kolabState} disabled {/if}
        {$kolabDelegateACL} value="" id="delegate_address">
   <input type=submit value="{t}Add{/t}" name="add_delegation" {if !$kolabState} disabled {/if}
        {$kolabDelegateACL} id="add_delegation">&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_delegation" {if !$kolabState} disabled {/if}
        {$kolabDelegateACL} id="delete_delegation">
   <p>
    <b>{t}Mail size{/t}</b><br>
     &nbsp;<input type="checkbox" id='unrestrictedMailSize' name="unrestrictedMailSize" value="1" {if !$kolabState} disabled {/if}
	{$unrestrictedMailSizeACL} {$unrestrictedMailSizeState}> {t}No mail size restriction for this account{/t}
    </p>
  </td>
  <td rowspan=2 style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">

 <b>{t}Free Busy information{/t}</b><br>
 <table summary="">
  <tr>
   <td><LABEL for="calFBURL">{t}URL{/t}</LABEL></td>
   <td><input id="calFBURL" name="calFBURL" size=30 maxlength=60 {$calFBURLACL} value="{$calFBURL}" {if !$kolabState} disabled {/if}></td>
  </tr>
  <tr>
  <td><LABEL for="kolabFreeBusyFuture">{t}Future{/t}</LABEL></td>
   <td><input id="kolabFreeBusyFuture" name="kolabFreeBusyFuture" size=5 maxlength=6 i {if !$kolabState} disabled {/if}{$kolabFreeBusyFutureACL} value="{$kolabFreeBusyFuture}"> {t}days{/t}</td>
  </tr>
 </table>


 <b>{t}Invitation policy{/t}</b><br>
 <table summary="">
   {$invitation}
 </table>

  </td>
 </tr>
</table>

{/if}
