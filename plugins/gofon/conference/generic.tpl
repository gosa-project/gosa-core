<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
<td style="vertical-align:top;">
<h2>
<img alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}
</h2>
     
	<table summary="">
	<tr>
	<td>
	<LABEL for="cn">
	{t}Name of Conference{/t}
	</LABEL>
	{$must}
	</td>
	<td>
	<input id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of subtree to create{/t}">
	</td>
	</tr>
	<tr>
	<td>
	<LABEL for="description">
	{t}Description{/t}
	</LABEL>
	{$must}
	</td>
	<td>
	<input id="description" name="description" size=25 maxlength=60 {$descriptionACL} 
	value="{$description}" title="{t}Descriptive text for department{/t}">
	</td>
	</tr>
	<tr>
	<td colspan="2"><br></td>
	</tr>
	<tr>
	<td><LABEL for="goFonConferenceOption_D">{t}Type{/t}</LABEL>{$must}</td>
	<td>
	<select id="goFonConferenceOption_D" size="1" name="goFonConferenceOption_D" title="{t}Choose subtree to place department in{/t}"> 
	{html_options options=$goFonConferenceOptions selected=$goFonConferenceOption_D}
	</select>
	</td>
	</tr>
	<tr>
	<td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	<td>
	<select id="base" size="1" name="base" title="{t}Choose subtree to place department in{/t}"> 
	{html_options options=$bases selected=$base}
	</select>
	</td>
	</tr>
	</table>

</td>
<td>
<h2><img alt="" align="middle" src="images/house.png"> {t}Location{/t}</h2>

	<table summary="">
	<tr>
	<td>
	{t}Lifetime in days{/t}
	</td>
	<td>
	<input name="goFonConferenceOptionLifetime" value="{$goFonConferenceOptionLifetime}" {$goFonConferenceOptionLifetimeACL}>
	</td>
	</tr>
	<tr>
	<td>
	{t}Number{/t}
	</td>
	<td>
	<input name="telephoneNumber" value="{$telephoneNumber}" {$telephoneNumberACL}>
	</td>
	</tr>
	</table>

</td>
</tr>
<tr>
<td colspan=2 style='width:100%;'>
	<p class='seperator'>&nbsp;</p>
</td>
</tr>
<tr>
<td>
<h2><img alt="" align="middle" src="images/house.png"> {t}Options{/t}</h2>

	<table summary="">
    <tr>
	<td colspan=2>
		<input type="checkbox" name="goFonConferenceOption_P" value="P" {$goFonConferenceOption_PCHK} {$goFonConferenceOption_PACL}> 
		{t}preset PIN{/t}
	</td>
	</tr>
	<tr>
	<td>
	&nbsp;	{t}PIN{/t}
	</td>
	<td>
		<input type='text' name="goFonPIN" {$goFonPINACL} value="{$goFonPIN}">
	</td>
	</tr>
	<tr>
	<td colspan=2>
	<br><br>
	</td>
	</tr>
    <tr>
	<td colspan=2>
		<input type="checkbox" name="goFonConferenceOption_r" value="r" {$goFonConferenceOption_rCHK} {$goFonConferenceOption_rACL}> 
    	{t}Record Conference{/t}
	</td>
	</tr>
	<tr>
	<td>
	&nbsp;	{t}Format{/t}
	</td>
	<td>
	<select id="goFonConferenceOptionFormat" size="1" name="goFonConferenceOptionFormat" title="{t}Choose subtree to place department in{/t}"> 
	{html_options options=$goFonConferenceOptionFormats selected=$goFonConferenceOptionFormat}
	</select>
	</td>
	</tr>
	</table>

</td>
<td>
	<table summary="">
	<tr>
	<td>
		{t}Play music on hold{/t}
	</td>
	<td>
		<input type="checkbox" name="goFonConferenceOption_M" value="M" {$goFonConferenceOption_MCHK} {$goFonConferenceOption_MACL}> 
	</td>
	</tr>
	<tr>
	<td>
		{t}Activate menu{/t}
	</td>
	<td>
		<input type="checkbox" name="goFonConferenceOption_s" value="s" {$goFonConferenceOption_sCHK} {$goFonConferenceOption_sACL}> 
	</td>
	</tr>
	<tr>
	<td>
		{t}Announce new or leaving user{/t}
	</td>
	<td>
		<input type="checkbox" name="goFonConferenceOption_i" value="i" {$goFonConferenceOption_iCHK} {$goFonConferenceOption_iACL}> 
	</td>
	</tr>
	<tr>
	<td>
		{t}Count user{/t}
	</td>
	<td>
		<input type="checkbox" name="goFonConferenceOption_c" value="c" {$goFonConferenceOption_cCHK} {$goFonConferenceOption_cACL}> 
	</td>
	</tr>
	</table>

</td>
</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.ou.focus();
  -->
</script>
