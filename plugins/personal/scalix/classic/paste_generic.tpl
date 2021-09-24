<table summary="{t}SCALIX paste{/t}">
	<tr>
  <td>

   <h3>
<label for="emails_list"> {t}SCALIX email addresses{/t}</label></h3>
   <select id="emails_list" style="width:100%;height:100px;" name="emails_list[]" size="15"
		 multiple title="{t}List of SCALIX email addresses{/t}" >
            {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
			<option disabled>&nbsp;</option>
   </select>
   <br />
   <input type='text' name="email_address" size="30" align="middle" maxlength="65" value="">
   <button type='submit' name='add_email'>{msgPool type=addButton}</button>&nbsp;

   <button type='submit' name='delete_email'>{msgPool type=delButton}</button>

  </td>
 </tr>
</table>

<input type="hidden" name="scalixTab" value="scalixTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.scalixMailnode.focus();
  -->
</script>
