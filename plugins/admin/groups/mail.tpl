<input type="hidden" name="mailedit" value="1">
<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
   <table>
    <tr>
     <td>{t}Primary address{/t}{$must}</td>
     <td><input name="mail" size=25 maxlength=65 {$mailACL} value="{$mail}" title="{t}Primary mail address for this shared folder{/t}"></td>
    </tr>
    <tr>
     <td>{t}Server{/t}</td>
     <td>
      <select size="1" name="gosaMailServer" {$gosaMailServerACL} title="{t}Select mail server to place user on{/t}">
       {html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
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
      <img alt="" src="progress.php?x=100&amp;y=16&amp;p={$quotausage}" align="middle">
      {else}
      {t}not defined{/t}
      {/if}
     </td>
    </tr>
    <tr>
     <td>{t}Quota size{/t}</td>
     <td><input name="gosaMailQuota" size=6 align=middle maxlength=30 {$gosaMailQuota} value="{$gosaMailQuota}"> KB</td>
    </tr>
   </table>
     
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top">
   <h2><img alt="" align="middle" src="images/alternatemail.png"> {t}Alternative addresses{/t}</h2>
   <select style="width:350px; height:100px;" name="alternates_list[]" size=15 multiple title="{t}List of alternative mail addresses{/t}">
    {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
   </select>
   <br>
   <input name="alternate_address" size=30 align=middle maxlength=30 {$gosaMailAlternateAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_alternate" {$gosaMailAlternateAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_alternate" {$gosaMailAlternateAddressACL}>
  </td>
 </tr>

</table>

<p class="seperator">&nbsp;</p>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/members.png"> {t}IMAP shared folders{/t}</h2>

   <table cellpadding=0 border=0>
    <tr>
     <td>{t}Default permission{/t}</td>
     <td>
      <select size="1" name="default_permissions" {$permissionsACL}>
       {html_options options=$perms selected=$default_permissions}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Member permission{/t}</td>
     <td>
      <select size="1" name="member_permissions" {$permissionsACL}>
       {html_options options=$perms selected=$member_permissions}
      </select>
     </td>
    </tr>
	{$plusattributes}
   </table>
      
  </td>
  <td style="border-left:1px solid #A0A0A0">
	&nbsp;
  </td>
  <td style="vertical-align:top">
   <h2><img alt="" align="middle" src="images/envelope.png"> {t}Forward messages to non group members{/t}</h2>
   <select style="width:350px; height:100px;" name="forwarder_list[]" size=15 multiple>
    {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
   </select>
   <br>
   <input name="forward_address" size=20 align=middle maxlength=65 {$gosaMailForwardingAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_forwarder" {$gosaMailForwardingAddressACL}>&nbsp;
   <input type=submit value="{t}Add local{/t}" name="add_local_forwarder" {$gosaMailForwardingAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_forwarder" {$gosaMailForwardingAddressACL}>
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.mail.focus();
  -->
</script>
