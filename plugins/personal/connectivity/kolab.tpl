
{if !$mail_account}
	<h2>{t}Kolab account{/t}</h2>
	{t}The kolab account is currently disabled. It's features can be adjusted if you add a mail account.{/t}
{else}



<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if} 
	{if (!$is_account && $is_createable) || ($is_account && $is_removeable)}
	{else}
		disabled 	
	{/if}
	onClick="
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


<b>{t}Kolab account{/t}</b>
<br>
<br>
<table summary="" style="width:100%">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <b><LABEL for="delegate_list">{t}Delegations{/t}</LABEL></b><br>
{render acl=$kolabDelegateACL}
   <select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple  {if !$kolabState}disabled{/if} >
    {html_options values=$kolabDelegate output=$kolabDelegate}
	<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$kolabDelegateACL}
   <input name="delegate_address" size=30 align=middle maxlength=60 {if !$kolabState} disabled {/if} value="" id="delegate_address">
{/render}
{render acl=$kolabDelegateACL}
   <input type=submit value="{t}Add{/t}" name="add_delegation" {if !$kolabState} disabled {/if} id="add_delegation">&nbsp;
{/render}
{render acl=$kolabDelegateACL}
   <input type=submit value="{t}Delete{/t}" name="delete_delegation" {if !$kolabState} disabled {/if} id="delete_delegation">
{/render}
   <p>
    <b>{t}Mail size{/t}</b><br>
{render acl=$unrestrictedMailSizeACL}
     &nbsp;<input type="checkbox" id='unrestrictedMailSize' name="unrestrictedMailSize" value="1" {if !$kolabState} disabled {/if}
	{$unrestrictedMailSizeState}> {t}No mail size restriction for this account{/t}
{/render}
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
   <td>
{render acl=$calFBURLACL}
   <input id="calFBURL" name="calFBURL" size=30 maxlength=60 value="{$calFBURL}" {if !$kolabState} disabled {/if}>
{/render}
   </td>
  </tr>
  <tr>
  <td><LABEL for="kolabFreeBusyFuture">{t}Future{/t}</LABEL></td>
  <td>
{render acl=$kolabFreeBusyFutureACL}
   <input id="kolabFreeBusyFuture" name="kolabFreeBusyFuture" size=5 maxlength=6 {if !$kolabState} disabled {/if} value="{$kolabFreeBusyFuture}"> 
	{t}days{/t}
{/render}
   </td>
  </tr>
 </table>


 <b>{t}Invitation policy{/t}</b><br>
{render acl=$kolabInvitationPolicyACL}
 <table summary="">
   {$invitation}
 </table>
{/render}

  </td>
 </tr>
</table>

{/if}
