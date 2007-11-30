<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
 <td colspan=3>
<h2 style='margin:0px; padding:0px;'><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
 </td>
</tr>
<tr>
<td style="vertical-align:top">
	<table summary="">
	<tr>
	<td>
	<LABEL for="cn">
	{t}Conference name{/t}
	</LABEL>
	{$must}
	</td>
	<td>
	<input id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of conference to create{/t}" {$cnACL}>
	</td>
	</tr>
	<tr>
	<td><LABEL for="goFonConferenceOption_D">{t}Type{/t}</LABEL>{$must}</td>
	<td>
	<select id="goFonConferenceOption_D" size="1" name="goFonConferenceOption_D" title="{t}Choose subtree to place conference in{/t}" {$goFonConferenceOption_DACL}> 
	{html_options options=$goFonConferenceOptions selected=$goFonConferenceOption_D}
	</select>
	</td>
	</tr>
	<tr>
	<td colspan="2"><br></td>
	</tr>
	<tr>
	<td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	<td>
	<select id="base" size="1" name="base" title="{t}Choose subtree to place conference in{/t}" {$baseACL}> 
	{html_options options=$bases selected=$base}
	</select>
        {if $baseACL == ""}
            <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
        {else}
            <img src="images/folder_gray.png" class="center" title="{t}Select a base{/t}">
        {/if}
	</td>
	</tr>
	<tr>
	<td><LABEL for="goFonHomeServer">{t}Home server{/t}</LABEL>{$must}</td>
	<td>
		<select name='goFonHomeServer'>
		 {html_options options=$goFonHomeServers selected=$goFonHomeServer}
                </select>
	</td>
	</tr>
	 <tr>
	   <td>{t}Language{/t}
	   </td>
	   <td>
		 <select name="language" title="{t}Select language{/t}">
		   {html_options options=$languages selected=$language}
		 </select>
	   </td>
	 </tr>
	</table>

</td>
<td style="border-left:1px solid #A0A0A0">
     &nbsp;
</td>
<td width="50%">
	<table summary="">
	<tr>
	<td>
	<LABEL for="description">
	{t}Description{/t}
	</LABEL>
	</td>
	<td>
	<input id="description" name="description" size=40 maxlength=80 {$descriptionACL} 
	value="{$description}" title="{t}Descriptive text for department{/t}">
	</td>
	</tr>
	<tr>
	<td>
	{t}Lifetime (in days){/t}
	</td>
	<td>
	<input name="goFonConferenceOptionLifetime" size=15 value="{$goFonConferenceOptionLifetime}" {$goFonConferenceOptionLifetimeACL}>
	</td>
	</tr>
	<tr>
	<td>
	{t}Phone number{/t}
	{$must}
	</td>
	<td>
	<input name="telephoneNumber" value="{$telephoneNumber}" size=15 {$telephoneNumberACL}>
	</td>
	</tr>
	</table>

</td>
</tr>
<tr>
<td colspan=3 style='width:100%;'>
	<p class='seperator'>&nbsp;</p>
</td>
</tr>
<tr>
 <td colspan=3>
<h2 style='margin:0px; padding:0px;'><img class="center" alt="" align="middle" src="images/select_ogroup.png"> {t}Options{/t}</h2>
 </td>
</tr>
<tr>
<td>

	<table summary="">
    <tr>
	<td>
		<input type="checkbox" onclick="changeState('goFonPIN');" name="goFonConferenceOption_P" 
					value="P" {$goFonConferenceOption_PCHK} {$goFonConferenceOption_PACL}> 
	</td><td>
		{t}Preset PIN{/t}
	</td>
	</tr>
	<tr>
	<td>
	 &nbsp;
	</td>
	<td>
		{t}PIN{/t} <input type='password' name="goFonPIN" {$goFonPINACL} id="goFonPIN" value="{$goFonPIN}" title="{$goFonPIN}">
	</td>
	</tr>
    <tr>
	<td colspan=2>
		<input type="checkbox" onclick="changeState('goFonConferenceOptionFormat');" name="goFonConferenceOption_r" value="r" {$goFonConferenceOption_rCHK} {$goFonConferenceOption_rACL}> 
    	{t}Record conference{/t}
	</td>
	</tr>
	<tr>
	<td>
	&nbsp;
	</td>
	<td>
	{t}Sound file format{/t}&nbsp;
	<select id="goFonConferenceOptionFormat" size="1" name="goFonConferenceOptionFormat" {$goFonConferenceOptionFormatACL} title="{t}Choose subtree to place department in{/t}"> 
	{html_options options=$goFonConferenceOptionFormats selected=$goFonConferenceOptionFormat}
	</select>
	</td>
	</tr>
	</table>

</td>
  <td style="border-left:1px solid #A0A0A0">
     &nbsp;
	   </td>
<td>
	<table summary="">
	<tr>
	<td>
		<input type="checkbox" name="goFonConferenceOption_M" value="M" {$goFonConferenceOption_MCHK} {$goFonConferenceOption_MACL}> 
		{t}Play music on hold{/t}
	</td>
	</tr>
	<tr>
	<td>
		<input type="checkbox" name="goFonConferenceOption_s" value="s" {$goFonConferenceOption_sCHK} {$goFonConferenceOption_sACL}> 
		{t}Activate session menu{/t}
	</td>
	</tr>
	<tr>
	<td>
		<input type="checkbox" name="goFonConferenceOption_i" value="i" {$goFonConferenceOption_iCHK} {$goFonConferenceOption_iACL}> 
		{t}Announce users joining or leaving the conference{/t}
	</td>
	</tr>
	<tr>
	<td>
		<input type="checkbox" name="goFonConferenceOption_c" value="c" {$goFonConferenceOption_cCHK} {$goFonConferenceOption_cACL}> 
		{t}Count users{/t}
	</td>
	</tr>
	</table>

</td>
</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
