<table summary="{t}Phone numbers{/t}" style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
   <h2><img alt="" align="middle" src="images/banana.png"> {t}Phone numbers{/t}</h2>
      <select style="width:380px; height:120px;" name="phonenumber_list[]" size=6 multiple {$telephoneNumberACL}>
       {html_options options=$phoneNumbers}
		<option disabled>&nbsp;</option>
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

  <td>
   <h2><img alt="" align="middle" src="images/hardware.png"> {t}Telephone hardware{/t}</h2>

   <table summary="{t}Telephone{/t}" style="width:100%" border=0>
    <tr>
     <td>{t}Telephone{/t}&nbsp;
      {$hardware_list}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

   <table summary="">
	<tr>
    <td>
     <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Forward calls to{/t}</h2>

     <table summary="{t}Phone number{/t}">
      <tr> 
       <td>{t}Phone number{/t}</td><td>{t}Timeout (s){/t}</td><td>&nbsp;</td>
      </tr>
      {$forwarder_list}
     </table>
    </td>
   </table>
  </td>

 </tr>
 <tr>
	<td style="vertical-align:top; border-top:1px solid #A0A0A0; width:50%" colspan="2">&nbsp;</td>
 </tr>
 <tr style="vertical-align:top; border-top:1px solid #A0A0A0;">
	<td  style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
		<br>
		<table summary="{t}Phone macro{/t}">
			<tr>
				<td colspan=2><img alt="" src="images/select_macro.png" align="middle">&nbsp;{t}Phone macro{/t}
					 <select name="macro" onchange="document.mainform.submit()" {$telephoneNumberACL}>
					   {html_options options=$macros selected=$macro}
				<option disabled>&nbsp;</option>
					 </select>
					 <input type="submit" name="refresh" value="{t}refresh{/t}" {$goFonMacroACL}>
				</td>
			</tr>
		</table>
		<br>
		{$macrotab}
	</td>
	<td valign="top" style="vertical-align:top; width:50%">
		<table summary="">
			<tr>
				<td>
					{t}Phone PIN{/t}
				</td>
				<td>
					<input type="password" name="goFonPIN" value="{$goFonPIN}">
				</td>
			</tr>
		</table>
	</td>
 </tr>
</table>
<input type="hidden" name="phoneTab" value="phoneTab">
