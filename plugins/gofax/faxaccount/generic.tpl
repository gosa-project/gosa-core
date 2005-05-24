<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
   <td style="width:50%; vertical-align:top;">
     <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>

     <table>
       <tr>
         <td>{t}Fax{/t}{$must}</td>
         <td>
           <input name="facsimileTelephoneNumber" size=20 maxlength=60 {$facsimileTelephoneNumberACL} value="{$facsimileTelephoneNumber}" title="{t}Fax number for GOfax to trigger on{/t}">
         </td>
       </tr>
       <tr>
         <td>{t}Language{/t}</td>
	 <td>
           <select size="1" name="goFaxLanguage" {$goFaxLanguageACL} title="{t}Specify the GOfax communication language for fax to mail gateway{/t}">
	    {html_options values=$languages output=$languages selected=$goFaxLanguage}
           </select>
         </td>
       </tr>
       <tr>
         <td>{t}Delivery format{/t}</td>
         <td>
           <select size="1" name="goFaxFormat" {$goFaxFormatACL} title="{t}Specify delivery format for fax to mail gateway{/t}">
	    {html_options values=$formats output=$formats selected=$goFaxFormat}
           </select>
         </td>
       </tr>
     </table>
     
   </td>
   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   <td style="vertical-align:top; width:100%">
     <h2><img alt="" align="middle" src="images/printer.png"> {t}Delivery methods{/t}</h2>

     <input type=checkbox name="goFaxIsEnabled" value="1" {$goFaxIsEnabled} {$goFaxIsEnabledACL}>
     {t}Temporary disable fax usage{/t}<br>

     {if $has_mailaccount eq "false"}
     <input type=checkbox name="fax_to_mail" value="1" {$fax_to_mail} {$goFaxDeliveryModeACL}>
      {t}Deliver fax as mail to{/t}&nbsp;
      <input name="mail" size=25 maxlength=60 {$goFaxDeliveryModeACL} value="{$mail}">
     {else}
     <input type=checkbox name="fax_to_mail" value="1" {$fax_to_mail} {$goFaxDeliveryModeACL}>
      {t}Deliver fax as mail{/t}
     {/if}
     <br>

     <input type=checkbox name="fax_to_printer" value="1" {$fax_to_printer} {$goFaxDeliveryModeACL}>
     {t}Deliver fax to printer{/t}&nbsp;
     <select size="1" name="goFaxPrinter" {$goFaxPrinterACL}>
      {html_options options=$printers selected=$goFaxPrinter}
     </select>

   </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td style="width:50%;">
    <h2><img alt="" align="middle" src="images/fax_small.png"> {t}Alternate fax numbers{/t}</h2>
     <select style="width:350px; height:100px;" name="alternate_list[]" size=15 multiple {$facsimileAlternateTelephoneNumberACL}>
      	{if $SELECT_facsimileAlternateTelephoneNumber==true}
			{html_options values=$facsimileAlternateTelephoneNumber output=$facsimileAlternateTelephoneNumber}
		{else}
			<option>
        {/if}
     </select><br>
     <input name="forward_address" size=20 align="middle" maxlength=60 {$facsimileAlternateTelephoneNumberACL} value="">
     <input type=submit value="{t}Add{/t}" name="add_alternate" {$facsimileAlternateTelephoneNumberACL}>&nbsp;
     <input type=submit value="{t}Add local{/t}" name="add_local_alternate" {$facsimileAlternateTelephoneNumberACL}>&nbsp;
     <input type=submit value="{t}Delete{/t}" name="delete_alternate" {$facsimileAlternateTelephoneNumberACL}>
    </td>

    <td style="border-left:1px solid #A0A0A0">
     &nbsp;
    </td>
    
    <td style="vertical-align:top; width:100%">
      <h2><img alt="" align="middle" src="images/false.png"> {t}Blocklists{/t}</h2>
      <table>
        <tr>
          <td>{t}Blocklists for incoming fax{/t}</td>
          <td>
            <input type=submit name="edit_incoming" {$goFaxRBlockgroupsACL} value="{t}Edit{/t}">
          </td>
        </tr>
        <tr>
          <td>{t}Blocklists for outgoing fax{/t}</td>
          <td>
            <input type=submit name="edit_outgoing" {$goFaxSBlockgroupsACL} value="{t}Edit{/t}">
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
  document.mainform.facsimileTelephoneNumber.focus();
  -->
</script>
