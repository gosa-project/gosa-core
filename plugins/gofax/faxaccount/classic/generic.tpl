<table style='width:100%; ' summary="{t}Fax account{/t}">


 <!-- Headline container -->
 <tr>
   <td style='width:50%; '>

     <h3>{t}Generic{/t}</h3>

     <table summary="{t}Generic settings{/t}">
       <tr>
         <td><label for="facsimileTelephoneNumber">{t}Fax{/t}</label>{$must}</td>
         <td>
{if $multiple_support}
	<input type='text' name="dummy1" value="{t}Multiple edit{/t}" disabled id="facsimileTelephoneNumber">
{else}
{render acl=$facsimileTelephoneNumberACL}
           <input name="facsimileTelephoneNumber" id="facsimileTelephoneNumber" size=20 maxlength=65 type='text'
		value="{$facsimileTelephoneNumber}" title="{t}Fax number for GOfax to trigger on{/t}">
{/render}
{/if}

         </td>
       </tr>
       <tr>
         <td><label for="goFaxLanguage">{t}Language{/t}</label></td>
	 <td>

{render acl=$goFaxLanguageACL checkbox=$multiple_support checked=$use_goFaxLanguage}
           <select size="1" name="goFaxLanguage" id="goFaxLanguage" 
		title="{t}Specify the GOfax communication language for FAX to mail gateway{/t}">
			{html_options options=$languages selected=$goFaxLanguage}
           </select>
{/render}

         </td>
       </tr>
       <tr>
         <td><label for="goFaxFormat">{t}Delivery format{/t}</label></td>
         <td>

{render acl=$goFaxFormatACL checkbox=$multiple_support checked=$use_goFaxFormat}
           <select id="goFaxFormat" size="1" name="goFaxFormat" title="{t}Specify delivery format for FAX to mail gateway{/t}">
	    {html_options values=$formats output=$formats selected=$goFaxFormat}
           </select>
{/render}
         </td>
       </tr>
     </table>
     
   </td>
   <td class='left-border'>

    &nbsp;
   </td>
   <td style='width:100%'>

     <h3>{t}Delivery methods{/t}</h3>

{render acl=$goFaxIsEnabledACL checkbox=$multiple_support checked=$use_goFaxIsEnabled}
     <input type=checkbox name="goFaxIsEnabled" value="1" {$goFaxIsEnabled} class="center">
{/render}
     {t}Temporary disable FAX usage{/t}<br>

     {if $has_mailaccount eq "false"}
{render acl=$faxtomailACL checkbox=$multiple_support checked=$use_faxtomail}
     <input type=checkbox name="faxtomail" value="1" {$faxtomail} class="center">
{/render}
      <label for="mail">{t}Deliver FAX as mail to{/t}</label>&nbsp;
{render acl=$faxtomailACL checkbox=$multiple_support checked=$use_mail}
      <input type='text' name="mail" id="mail" size=25 maxlength=65 value="{$mail}" class="center">
{/render}
     {else}
{render acl=$faxtomailACL checkbox=$multiple_support checked=$use_faxtomail}
     <input type=checkbox name="faxtomail" value="1" {$faxtomail} class="center">
{/render}
      {t}Deliver FAX as mail{/t}
     {/if}
     <br>

{render acl=$faxtoprinterACL checkbox=$multiple_support checked=$use_faxtoprinter}
     <input type=checkbox name="faxtoprinter" value="1" {$faxtoprinter} class="center">
{/render}
     {t}Deliver FAX to printer{/t}&nbsp;
{render acl=$faxtoprinterACL checkbox=$multiple_support checked=$use_goFaxPrinter}
     <select size="1" name="goFaxPrinterSelected">
      {html_options options=$printers selected=$goFaxPrinter}
		<option disabled>&nbsp;</option>
     </select>
{/render}
   </td>
 </tr>
</table>

<hr>

<table style='width:100%; ' summary="{t}Alternative numbers{/t}">

  <tr>
    <td style='width:50%; ' class='right-border'>


	{if !$multiple_support}

    <h3>{t}Alternate FAX numbers{/t}</h3>
{render acl=$facsimileAlternateTelephoneNumberACL}
    <select style="width:100%" name="alternate_list[]" size="10" multiple>
			{html_options values=$facsimileAlternateTelephoneNumber output=$facsimileAlternateTelephoneNumber}
			<option disabled>&nbsp;</option>
    </select>
{/render}
    <br>
{render acl=$facsimileAlternateTelephoneNumberACL}
    <input type='text' name="forward_address" size=20 align="middle" maxlength=65 value="">
{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <button type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <button type='submit' name='add_local_alternate'>{t}Add local{/t}</button>&nbsp;

{/render}
{render acl=$facsimileAlternateTelephoneNumberACL}
    <button type='submit' name='delete_alternate'>{msgPool type=delButton}</button>

{/render}
	{/if}
   </td>
   <td style='width:50%'>

      <h3>{t}Blacklists{/t}</h3>
      <table summary="{t}Blacklists{/t}" style="width:100%">
        <tr>
          <td>{t}Blacklists for incoming FAX{/t}</td>
          <td>
{render acl=$goFaxRBlocklistACL checkbox=$multiple_support checked=$use_edit_incoming}
            <button type='submit' name='edit_incoming'>{t}Edit{/t}</button>

{/render}
          </td>
        </tr>
        <tr>
          <td>{t}Blacklists for outgoing FAX{/t}</td>
          <td>
{render acl=$goFaxSBlocklistACL checkbox=$multiple_support checked=$use_edit_outgoing}
            <button type='submit' name='edit_outgoing'>{t}Edit{/t}</button>

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
