<table>
	<tr>
  <td style="vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/envelope.png" /><label for="emails_list"> {t}Scalix email addresses{/t}</label></h2>
   <select id="emails_list" style="width:100%;height:100px;" name="emails_list[]" size="15"
		 multiple title="{t}List of scalix email addresses{/t}" >
            {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
			<option disabled>&nbsp;</option>
   </select>
   <br />
   <input name="email_address" size="30" align="middle" maxlength="65" value="">
   <input type=submit value="{t}Add{/t}" name="add_email">&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_email">
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
