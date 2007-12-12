<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">

 <!-- Headline container -->
 <tr>
   <td style="width:50%; vertical-align:top;">
     <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>

     <table summary="">
       <tr>
         <td><label for="facsimileTelephoneNumber">{t}Fax{/t}</label>{$must}</td>
         <td>

{render acl=$facsimileTelephoneNumberACL}
           <input name="facsimileTelephoneNumber" id="facsimileTelephoneNumber" size=20 maxlength=65
		value="{$facsimileTelephoneNumber}" title="{t}Fax number for GOfax to trigger on{/t}">
{/render}

         </td>
       </tr>
       <tr>
         <td><label for="goFaxLanguage">{t}Language{/t}</label></td>
	 <td>

{render acl=$goFaxLanguageACL}
           <select size="1" name="goFaxLanguage" id="goFaxLanguage" 
		title="{t}Specify the GOfax communication language for fax to mail gateway{/t}">
			{html_options options=$languages selected=$goFaxLanguage}
           </select>
{/render}

         </td>
       </tr>
       <tr>
         <td><label for="goFaxFormat">{t}Delivery format{/t}</label></td>
         <td>

{render acl=$goFaxFormatACL}
           <select id="goFaxFormat" size="1" name="goFaxFormat" title="{t}Specify delivery format for fax to mail gateway{/t}">
	    {html_options values=$formats output=$formats selected=$goFaxFormat}
           </select>
{/render}
         </td>
       </tr>
     </table>
     
   </td>
   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   <td style="vertical-align:top; width:100%">
     <h2><img class="center" alt="" align="middle" src="images/printer.png" />&nbsp;{t}Delivery methods{/t}</h2>

{render acl=$goFaxIsEnabledACL}
     <input type=checkbox name="goFaxIsEnabled" value="1" {$goFaxIsEnabled}>
{/render}
     {t}Temporary disable fax usage{/t}<br>

     {if $has_mailaccount eq "false"}
{render acl=$faxtomailACL}
     <input type=checkbox name="faxtomail" value="1" {$faxtomail}>
{/render}
      <label for="mail">{t}Deliver fax as mail to{/t}</label>&nbsp;
{render acl=$faxtomailACL}
      <input name="mail" id="mail" size=25 maxlength=65 value="{$mail}">
{/render}
     {else}
{render acl=$faxtomailACL}
     <input type=checkbox name="faxtomail" value="1" {$faxtomail}>
{/render}
      {t}Deliver fax as mail{/t}
     {/if}
     <br>

{render acl=$faxtoprinterACL}
     <input type=checkbox name="faxtoprinter" value="1" {$faxtoprinter}>
{/render}
     {t}Deliver fax to printer{/t}&nbsp;
{render acl=$faxtoprinterACL}
     <select size="1" name="goFaxPrinter">
      {html_options options=$printers selected=$goFaxPrinter}
		<option disabled>&nbsp;</option>
     </select>
{/render}
   </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td style="width:50%; border-right:1px solid #A0A0A0">
    <h2><img class="center" alt="" align="middle" src="images/fax_small.png">&nbsp;{t}Alternate fax numbers{/t}</h2>
{render acl=$facsimileAlternateTelephoneNumberACL}
    <select style="width:100%" name="alternate_list[]" size="10" multiple>
			{html_options values=$facsimileAlternateTelephoneNumber output=$facsimileAlternateTelephoneNumber}
			<option disabled>&nbsp;</option>
    </select>
{/render}
    <br>
{render acl=$facsimileAlternateTelephoneNumberACL}
    <input name="forward_address" size=20 align="middle" maxlength=65 value="">
{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <input type=submit value="{t}Add{/t}" name="add_alternate">&nbsp;
{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <input type=submit value="{t}Add local{/t}" name="add_local_alternate" >&nbsp;
{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <input type=submit value="{t}Delete{/t}" name="delete_alternate">
{/render}
   </td>
   <td style="vertical-align:top; width:50%">
      <h2><img class="center" alt="" align="middle" src="images/false.png" />&nbsp;{t}Blocklists{/t}</h2>
      <table summary="" style="width:100%">
        <tr>
          <td>{t}Blocklists for incoming fax{/t}</td>
          <td>
{render acl=$goFaxRBlocklistACL}
            <input type=submit name="edit_incoming" value="{t}Edit{/t}">
{/render}
          </td>
        </tr>
        <tr>
          <td>{t}Blocklists for outgoing fax{/t}</td>
          <td>
{render acl=$goFaxSBlocklistACL}
            <input type=submit name="edit_outgoing" value="{t}Edit{/t}">
{/render}
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>

<input type="hidden" name="faxTab" value="faxTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('facsimileTelephoneNumber');
  -->
</script>
