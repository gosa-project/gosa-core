<table summary="{t}Phone numbers{/t}" style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
   <h2><p class="center"><img class="center" alt="" align="middle" src="images/banana.png" />&nbsp;{t}Phone numbers{/t}</p></h2>
      <select style="width:100%;" name="phonenumber_list[]" size=7 multiple {$telephoneNumberACL}>
       {html_options options=$phoneNumbers}
		<option disabled>
			&nbsp;
			</option>
      </select>
      <br>
      <input name="phonenumber" size=20 align=middle maxlength=60 {$telephoneNumberACL} value="">
      <input type="submit" value="{t}Add{/t}" name="add_phonenumber"
	{$telephoneNumberACL}>&nbsp;
      <input type="submit" value="{t}Delete{/t}" name="delete_phonenumber"
        {$telephoneNumberACL}>
  </td>

  <td style="vertical-align:top; width:50%;">
<table summary="" style="width:100%" border=0>
 <tr>

  <td tyle="vertical-align:top;">
   <h2><p class="center"><img class="center" alt="" align="middle" src="images/hardware.png" />&nbsp;{t}Telephone hardware{/t}</p></h2>

   <table summary="{t}Telephone{/t}" border=0>
    <tr>
     <td>{t}Telephone{/t}</td>
     <td>{$hardware_list}</td>
    </tr>
			<tr>
				<td>
					<label for="goFonVoicemailPIN">{t}Voicemail PIN{/t}{$must}</label>
				</td>
				<td>
					<input type="password" id="goFonVoicemailPIN" name="goFonVoicemailPIN" value="{$goFonVoicemailPIN}" {$goFonVoicemailPINACL}>
			</td>
			</tr>
			<tr>
				<td>
					<label for="goFonPIN">{t}Phone PIN{/t}{$must}</label>
				</td>
				<td>
					<input type="password" id="goFonPIN" name="goFonPIN" value="{$goFonPIN}" {$goFonPINACL}>
			</td>
			</tr>
   </table>
  </td>
 </tr>
</table>
  </td>

 </tr>
 <tr>
	<td style="vertical-align:top; border-top:1px solid #A0A0A0; width:50%" colspan="2">&nbsp;</td>
 </tr>
 <tr style="vertical-align:top; border-top:1px solid #A0A0A0;">
	<td colspan=2 style="vertical-align:top;width:50%">
		<h2><p class="center"><img class="center" alt="" src="images/select_macro.png" align="middle" />&nbsp;{t}Phone macro{/t}</p></h2>
		<table summary="{t}Phone macro{/t}">
			<tr>
				<td colspan=2>
					 <select name="macro" onchange="document.mainform.submit()" {$goFonMacroACL}>
					   {html_options options=$macros selected=$macro}
				<option disabled>&nbsp;</option>
					 </select>
					 <input type="submit" name="refresh" value="{t}Refresh{/t}" {$goFonMacroACL}>
				</td>
			</tr>
		</table>
		<br>
		{$macrotab}
	</td>
 </tr>
</table>
<input type="hidden" name="phoneTab" value="phoneTab">


<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.phonenumber.focus();
  -->
</script>
