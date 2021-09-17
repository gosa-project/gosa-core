<table style='width:100%; ' summary="{t}Mail settings{/t}">

 <tr>
  <td style='width:50%; '>

   {if !$multiple_support}
   <h3>{t}Generic{/t}</h3>
   <table summary="{t}Mail address settings{/t}">
    <tr>
     <td><label for="mail">{t}Mail address{/t}</label>{$must}</td>
     <td>
      {render acl=$mailACL}
      <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
      {/render}
     </td>
    </tr>
   </table>
   {else}
   <h3>{t}Generic{/t}</h3>
   <table summary="{t}Mail address settings{/t}">
    <tr>
    <td colspan="2"><i>{t}Please specify a mail address pattern (e.g., &#123;%cn&#124;@your-domain.com) or leave empty.{/t}</i>
    </td>
    </tr>
    <tr><td>&nbsp;</td></tr>
    <tr>
     <td><label for="mail">{t}Mail address pattern{/t}</label>{$must}</td>
     <td>
      {render acl=$mailACL}
      <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
      {/render}
     </td>
    </tr>
   </table>
   {/if}

<!-- Alternate addresses -->
{if !$multiple_support}
  <td style='padding-left:2px;'>

   <h3>
	{t}Alternative addresses{/t}
   </h3>

{render acl=$gosaMailAlternateAddressACL}
   <select style="width:100%;" name="alternates_list[]" size=10 multiple
	title="{t}List of alternative mail addresses{/t}">
    {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
	<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailAlternateAddressACL}
   <input type='text' name="alternate_address" size="30" align=middle maxlength="60" value="">
{/render}

{render acl=$gosaMailAlternateAddressACL}
   <button type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;

{/render}

{render acl=$gosaMailAlternateAddressACL}
   <button type='submit' name='delete_alternate'>{msgPool type=delButton}</button>

{/render}
  </td>
{/if}
 </tr>
</table>

<hr>

<h3>{t}Advanced mail options{/t}</h3>
<table style='width:100%; ' summary="{t}Advanced mail options{/t}">

  <tr>
  <td style='width:50%'>

   <h3>{t}Forward messages to non group members{/t}</h3>

{render acl=$gosaMailForwardingAddressACL}
   <select style="width:100%;" name="forwarder_list[]" size=10 multiple>

	{if $multiple_support}

		{foreach from=$Forward_all item=item key=key}
			<option value="{$item}">{$item}&nbsp;({t}Used in all groups{/t})</option>
		{/foreach}
		{foreach from=$Forward_some item=item key=key}
			<option value="{$item}" style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;'>{$item}&nbsp;({t}Not used in all groups{/t})</option>
		{/foreach}
	{else}
    {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
	<option disabled>&nbsp;</option>
	{/if}
   </select>
{/render}

   <br>

{render acl=$gosaMailForwardingAddressACL}
   <input type='text' name="forward_address" size=20 align=middle maxlength=65 value="">
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <button type='submit' name='add_forwarder'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$gosaMailForwardingAddressACL}
   <button type='submit' name='delete_forwarder'>{msgPool type=delButton}</button>

{/render}

  </td>
 </tr>
</table>
<input type="hidden" name='mailedit' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('mail');
  -->
</script>
{if $multiple_support}
	<input type="hidden" name="multiple_mail_group_posted" value="1">
{/if}
