<table summary="{t}Queue Settings{/t}" style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="4">
<tr>
<td valign='top'>
		<h2><img alt="" src="images/select_phone.png" align="middle">&nbsp;{t}Phone numbers{/t}</h2>
		<table summary="">
		<tr>
		<td colspan=2>
		  <table summary="{t}Generic queue Settings{/t}">
		   <tr>
		    <td>
			<select style="width:300px; height:60px;" name="goFonQueueNumber_List" size=6 {$telephoneNumberACL}>
			{html_options options=$telephoneNumber}
			<option disabled>&nbsp;</option>
			</select>
		</td>
		<td style="vertical-align:center">
			<input type="submit" value="{t}Up{/t}" 		name="up_phonenumber" 		{$telephoneNumberACL}><br>
			<input type="submit" value="{t}Down{/t}" 	name="down_phonenumber" 	{$telephoneNumberACL}>
		</td>
		</tr>
		<tr>
		 <td colspan=2>
			<input name="phonenumber" size=20 align=middle maxlength=60 			{$telephoneNumberACL} value="">
			<input type="submit" value="{t}Add{/t}" 	name="add_phonenumber" 		{$telephoneNumberACL}>&nbsp;
			<input type="submit" value="{t}Delete{/t}" 	name="delete_phonenumber" 	{$telephoneNumberACL}>
	     </td>
		</tr>
		</table>
		</tr>
		<tr>
		  <td colspan=2><h2><img alt="" src="images/select_phone.png" align="middle">&nbsp;{t}Options{/t}</h2></td>
		</tr>
		<tr>
		<td>
			{t}Language{/t}	
		</td>
		<td>
			<select name="goFonQueueLanguage" {$goFonQueueLanguageACL}>
			{html_options options=$goFonQueueLanguageOptions selected=$goFonQueueLanguage}
			<option disabled>&nbsp;</option>
			</select>
		</td>
		</tr>
		<tr>
		<td>
			{t}Timeout{/t}
		</td>
		<td>
			<input name='goFonTimeOut' value='{$goFonTimeOut}' {$goFonTimeOutACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Retry{/t}
		</td>
		<td>
			<input name='goFonQueueRetry' value='{$goFonQueueRetry}' {$goFonQueueRetryACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Strategy{/t}	
		</td>
		<td>
			<select name="goFonQueueStrategy" {$goFonQueueStrategyACL}>
            {html_options options=$goFonQueueStrategyOptions selected=$goFonQueueStrategy}
            <option disabled>&nbsp;</option>
            </select>
	
		</td>
		</tr>
		<tr>
		<td>
			{t}Max queue length{/t}
		</td>
		<td>
			<input name='goFonMaxLen' value='{$goFonMaxLen}' {$goFonMaxLenACL}>	
		</td>
		</tr>
		<tr>
		<td>
			{t}Announce frequency{/t}
		</td>
		<td>
			<input name='goFonAnnounceFrequency' value='{$goFonAnnounceFrequency}' {$goFonAnnounceFrequencyACL}>
			{t}(in seconds){/t}	
		</td>
		</tr>
		</table>
</td>
<td valign='top' style="border-left: 1px solid rgb(160, 160, 160);">
	<h2>
    <img alt="" src="images/select_phone.png" align="middle">
    {t}Queue sound file setup{/t}
    </h2>
	<table summary="{t}Generic queue Settings{/t}">
		<!--<tr>
		<td>
			{t}Use music on hold instead of ringing{/t}
		</td>
		<td>
			<input type="checkbox" name='goFonMusiconHold' value='1' {$goFonMusiconHoldCHK} {$goFonMusiconHoldACL}>
		</td>
		</tr>
		-->
		<tr>
		<td>
			{t}Music on hold{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonMusiconHold' value='{$goFonMusiconHold}' {$goFonMusiconHoldACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Welcome sound file{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonWelcomeMusic' value='{$goFonWelcomeMusic}' {$goFonWelcomeMusicACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Announce message{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueAnnounce' value='{$goFonQueueAnnounce}' {$goFonQueueAnnounceACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Sound file for 'You are next ...'{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueYouAreNext' value='{$goFonQueueYouAreNext}' {$goFonQueueYouAreNextACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}'There are ...'{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueThereAre' value='{$goFonQueueThereAre}' {$goFonQueueThereAreACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}'... calls waiting'{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueCallsWaiting' value='{$goFonQueueCallsWaiting}' {goFonQueueCallsWaitingACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}'Thank you' message{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueThankYou' value='{$goFonQueueThankYou}' {$goFonQueueThankYouACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}'minutes' sound file{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueMinutes' value='{$goFonQueueMinutes}' {$goFonQueueMinutesACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}'seconds' sound file{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueSeconds' value='{$goFonQueueSeconds}' {$goFonQueueSecondsACL}>
		</td>
		</tr>
		<tr>
		<td>
			{t}Hold sound file{/t}
		</td>
		<td>
			<input type="text" style='width:250px;' name='goFonQueueReportHold' value='{$goFonQueueReportHold}' {$goFonQueueReportHoldACL}>
		</td>
		</tr>

		</table>
</td>
</tr>
<tr>
	<td colspan=2>
		<p class="seperator">
	</td>
</tr>
<tr>
<td>
		<h2><img alt="" src="images/select_phone.png" align="middle">&nbsp;{t}Phone attributes {/t}</h2>
		<table summary="" style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="4">
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonQueueAnnounceHoldtime' value='yes'  {$goFonQueueAnnounceHoldtimeCHK} {$goFonQueueAnnounceHoldtimeACL}>
            {t}Announce holdtime{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonDialOption_t' value='t'  {$goFonDialOption_tCHK} {$goFonDialOption_tACL}>
            {t}Allow the called user to transfer his call{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonDialOption_T' value='T' {$goFonDialOption_TCHK} {$goFonDialOption_TACL}>
            {t}Allows calling user to transfer call{/t}
        </td>
        </table>

</td>
<td valign='top' style="border-left: 1px solid rgb(160, 160, 160);">
		<h2><img alt="" src="images/select_phone.png" align="middle">&nbsp;{t}Additional phone attributes {/t}</h2>
	 	<table summary=""  style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="4">
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonDialOption_h' value='h' {$goFonDialOption_hCHK} {$goFonDialOption_hACL}>
            {t}Allow the called to hangup by pressing *{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonDialOption_H' value='H' {$goFonDialOption_HCHK} {$goFonDialOption_HACL}>
            {t}Allows calling to hangup by pressing *{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
            <input type="checkbox" name='goFonDialOption_r' value='r' {$goFonDialOption_rCHK} {$goFonDialOption_rACL}>
            {t}Ring instead of playing background music{/t}
        </td>
        </tr>
        </table>
</td>
</tr>

</table>
			
