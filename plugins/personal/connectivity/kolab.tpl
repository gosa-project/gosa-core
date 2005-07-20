<h2>{t}Kolab account{/t}</h2>

{if $is_account ne 'true'}
	{t}The kolab account is currently disabled. It's features can be adjusted if you add a mail account.{/t}
{else}

<table summary="" style="width:100%">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <b><LABEL for="delegate_list">{t}Delegations{/t}"</LABEL></b><br>
   <select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple>
    {html_options values=$kolabDelegate output=$kolabDelegate}
	<option disabled>&nbsp;</option>
   </select>
   <br>
   <input name="delegate_address" size=30 align=middle maxlength=60
        {$kolabDelegateACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_delegation"
        {$kolabDelegateACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_delegation"
        {$kolabDelegateACL}>
   <p>
    <b>{t}Mail size{/t}</b><br>
     &nbsp;<input type="checkbox" name="unrestrictedMailSize" value="1" {$unrestrictedMailSizeACL} {$unrestrictedMailSizeState}> {t}No mail size restriction for this account{/t}
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
   <td><input id="calFBURL" name="calFBURL" size=30 maxlength=60 {$calFBURLACL} value="{$calFBURL}"></td>
  </tr>
  <tr>
  <td><LABEL for="kolabFreeBusyFuture">{t}Future{/t}</LABEL></td>
   <td><input id="kolabFreeBusyFuture" name="kolabFreeBusyFuture" size=5 maxlength=6 {$kolabFreeBusyFutureACL} value="{$kolabFreeBusyFuture}"> {t}days{/t}</td>
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
