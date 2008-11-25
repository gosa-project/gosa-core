<input type="hidden" name="mailedit" value="1">
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
  <td style="width:50%; border-right:1px solid #A0A0A0; vertical-align:top">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="mail">{t}Primary address{/t}</LABEL>{$must}</td>
     <td><input id="mail" name="mail" size="25" maxlength="65" {$mailACL} value="{$mail}" title="{t}Primary mail address for this shared folder{/t}"></td>
    </tr>
    <tr>
     <td><LABEL for="gosaMailServer">{t}Server{/t}</LABEL></td>
     <td>
      <select size="1" name="gosaMailServer" {$gosaMailServerACL} title="{t}Select mail server to place user on{/t}">
       {html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
	   <option disabled>&nbsp;</option>
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <br>
     	<LABEL for="gosaMailQuota"> {t}Quota usage{/t}</LABEL>
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
     <td><input id="gosaMailQuota" name="gosaMailQuota" size="6" align=middle maxlength="30" {$gosaMailQuotaACL} value="{$gosaMailQuota}"> MB</td>
    </tr>
	{if $kolab}
	<tr>
		<td>
			{t}Folder type{/t}
		</td>
		<td>
			<select name="kolabFolderType_Type" onChange="document.mainform.submit();">
				{html_options options=$kolabFolderType_Types selected=$kolabFolderType_Type}
			</select>
			<select name="kolabFolderType_SubType" onChange="document.mainform.submit();">
				{html_options options=$kolabFolderType_SubTypes selected=$kolabFolderType_SubType}
			</select>
			{if !$JS}
				<input type='image' src='images/list_reload.png' class='center' alt='{t}Reload{/t}'>
			{/if}
		</td>
	</tr>
	{/if}
   </table>
     
  </td>
  <td style="vertical-align:top;padding-left:2px;">
   <h2><img class="center" alt="" align="middle" src="images/alternatemail.png"> {t}Alternative addresses{/t}</h2>
   <select style="width:100%;" name="alternates_list[]" size=10 multiple title="{t}List of alternative mail addresses{/t}">
    {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
	<option disabled>&nbsp;</option>
   </select>
   <br>
   <input name="alternate_address" size="30" align=middle maxlength="60" {$gosaMailAlternateAddressACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_alternate" {$gosaMailAlternateAddressACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_alternate" {$gosaMailAlternateAddressACL}>
  </td>
 </tr>

</table>

<p class="seperator">&nbsp;</p>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="vertical-align:top;width:50%; border-right:1px solid #A0A0A0">
   <h2><img class="center" alt="" align="middle" src="images/members.png"> {t}IMAP shared folders{/t}</h2>

   <table summary="" cellpadding=0 border=0>
    <tr>
     <td><LABEL for="default_permissions">{t}Default permission{/t}</LABEL></td>
     <td>
      <select size="1" id="default_permissions" name="default_permissions" {$permissionsACL}>
       {html_options options=$perms selected=$default_permissions}
	   <option disabled>&nbsp;</option>
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="member_permissions">{t}Member permission{/t}</LABEL></td>
     <td>
      <select id="member_permissions" size="1" name="member_permissions" {$permissionsACL}>
       {html_options options=$perms selected=$member_permissions}
      </select>
     </td>
    </tr>
	{$plusattributes}
   </table>
<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/false.png" />&nbsp;{t}Advanced mail options{/t}</h2> 
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="2" border="0">
 <tr>
  <td>
   <input type="checkbox" name="only_local" value="1" {$only_local} {$only_localACL} 
       title="{t}Select if user can only send and receive inside his own domain{/t}">
       {t}User is only allowed to send and receive local mails{/t}
  </td> 
 </tr>
 </table> 
      
  </td>
  <td style="vertical-align:top;width:50%">
   <h2><img class="center" alt="" align="middle" src="images/envelope.png"> {t}Forward messages to non group members{/t}</h2>
   <select style="width:100%;" name="forwarder_list[]" size=10 multiple>
    {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
	<option disabled>&nbsp;</option>
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
	focus_field('mail');
  -->
</script>
