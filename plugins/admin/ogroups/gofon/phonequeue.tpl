<p style='padding-left:7px;'>
 <img class='center' src='images/lamp.png' alt='!'>&nbsp;<b>{t}Only users with the same asterisk home server will be included to this queue.{/t}</b>
</p>

<table summary="{t}Queue Settings{/t}" style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="4">
<tr>
<td valign='top'>
		<h2><img class="center" alt="" src="images/select_phone.png" align="middle">&nbsp;{t}Phone numbers{/t}</h2>
		<table summary="">
		<tr>
		<td colspan=2>
		  <table summary="{t}Generic queue Settings{/t}">
		   <tr>
		    <td>
{render acl=$telephoneNumberACL}
			<select style="width:300px; height:60px;" name="goFonQueueNumber_List" size=6>
			{html_options options=$telephoneNumber}
			<option disabled>&nbsp;</option>
			</select>
{/render}
		</td>
		<td style="vertical-align:center">
{render acl=$telephoneNumberACL}
			<input type="submit" value="{t}Up{/t}" 		name="up_phonenumber"><br>
{/render}
{render acl=$telephoneNumberACL}
			<input type="submit" value="{t}Down{/t}" 	name="down_phonenumber">
{/render}
		</td>
		</tr>
		<tr>
		 <td colspan=2>
{render acl=$telephoneNumberACL}
			<input name="phonenumber" size=20 align=middle maxlength=60 value="">
{/render}
{render acl=$telephoneNumberACL}
			<input type="submit" value="{t}Add{/t}" 	name="add_phonenumber" >&nbsp;
{/render}
{render acl=$telephoneNumberACL}
			<input type="submit" value="{t}Delete{/t}" 	name="delete_phonenumber" >
{/render}
	     </td>
		</tr>
		</table>
		</tr>
		<tr>
		  <td colspan=2><h2><img class="center" alt="" src="images/select_ogroup.png" align="middle">&nbsp;{t}Options{/t}</h2></td>
		</tr>
                <tr>
                <td><LABEL for="goFonHomeServer">{t}Home server{/t}</LABEL>{$must}</td>
                <td>
{render acl=$goFonHomeServerACL}
                        <select name='goFonHomeServer'>
                         {html_options options=$goFonHomeServers selected=$goFonHomeServer}
                        </select>
{/render}
                </td>
                </tr>

		<tr>
		<td>
			{t}Language{/t}	
		</td>
		<td>
{render acl=$goFonQueueLanguageACL}
			<select name="goFonQueueLanguage">
			{html_options options=$goFonQueueLanguageOptions selected=$goFonQueueLanguage}
			<option disabled>&nbsp;</option>
			</select>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Timeout{/t}
		</td>
		<td>
{render acl=$goFonTimeOutACL}
			<input name='goFonTimeOut' value='{$goFonTimeOut}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Retry{/t}
		</td>
		<td>
{render acl=$goFonQueueRetryACL}
			<input name='goFonQueueRetry' value='{$goFonQueueRetry}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Strategy{/t}	
		</td>
		<td>
{render acl=$goFonQueueStrategyACL}
			<select name="goFonQueueStrategy">
            {html_options options=$goFonQueueStrategyOptions selected=$goFonQueueStrategy}
            <option disabled>&nbsp;</option>
            </select>
{/render}
	
		</td>
		</tr>
		<tr>
		<td>
			{t}Max queue length{/t}
		</td>
		<td>
{render acl=$goFonMaxLenACL}
			<input name='goFonMaxLen' value='{$goFonMaxLen}'>	
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Announce frequency{/t}
		</td>
		<td>
{render acl=$goFonAnnounceFrequencyACL}
			<input name='goFonAnnounceFrequency' value='{$goFonAnnounceFrequency}'>
{/render}
			{t}(in seconds){/t}	
		</td>
		</tr>
		</table>
</td>
<td valign='top' style="border-left: 1px solid rgb(160, 160, 160);">
	<h2>
    <img class="center" alt="" src="images/sound.png" align="middle">
    {t}Queue sound setup{/t}
    </h2>
	<table summary="{t}Generic queue Settings{/t}">
		<!--<tr>
		<td>
			{t}Use music on hold instead of ringing{/t}
		</td>
		<td>
{render acl=goFonMusiconHoldACL}
			<input type="checkbox" name='goFonMusiconHold' value='1' {$goFonMusiconHoldCHK}>
{/render}
		</td>
		</tr>
		-->
		<tr>
		<td>
			{t}Music on hold{/t}
		</td>
		<td>
{render acl=$goFonMusiconHoldACL}
			<input type="text" style='width:250px;' name='goFonMusiconHold' value='{$goFonMusiconHold}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Welcome sound file{/t}
		</td>
		<td>
{render acl=$goFonWelcomeMusicACL}
			<input type="text" style='width:250px;' name='goFonWelcomeMusic' value='{$goFonWelcomeMusic}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Announce message{/t}
		</td>
		<td>
{render acl=$goFonQueueAnnounceACL}
			<input type="text" style='width:250px;' name='goFonQueueAnnounce' value='{$goFonQueueAnnounce}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Sound file for 'You are next ...'{/t}
		</td>
		<td>
{render acl=$goFonQueueYouAreNextACL}
			<input type="text" style='width:250px;' name='goFonQueueYouAreNext' value='{$goFonQueueYouAreNext}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}'There are ...'{/t}
		</td>
		<td>
{render acl=$goFonQueueThereAreACL}
			<input type="text" style='width:250px;' name='goFonQueueThereAre' value='{$goFonQueueThereAre}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}'... calls waiting'{/t}
		</td>
		<td>
{render acl=$goFonQueueCallsWaitingACL}
			<input type="text" style='width:250px;' name='goFonQueueCallsWaiting' value='{$goFonQueueCallsWaiting}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}'Thank you' message{/t}
		</td>
		<td>
{render acl=$goFonQueueThankYouACL}
			<input type="text" style='width:250px;' name='goFonQueueThankYou' value='{$goFonQueueThankYou}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}'minutes' sound file{/t}
		</td>
		<td>
{render acl=$goFonQueueMinutesACL}
			<input type="text" style='width:250px;' name='goFonQueueMinutes' value='{$goFonQueueMinutes}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}'seconds' sound file{/t}
		</td>
		<td>
{render acl=$goFonQueueSecondsACL}
			<input type="text" style='width:250px;' name='goFonQueueSeconds' value='{$goFonQueueSeconds}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Hold sound file{/t}
		</td>
		<td>
{render acl=$goFonQueueReportHoldACL}
			<input type="text" style='width:250px;' name='goFonQueueReportHold' value='{$goFonQueueReportHold}'>
{/render}
		</td>
		</tr>
		<tr>
		<td>
			{t}Less Than sound file{/t}
		</td>
		<td>
{render acl=$goFonQueueLessThanACL}
			<input type="text" style='width:250px;' name='goFonQueueLessThan' value='{$goFonQueueLessThan}'>
{/render}
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
<td colspan=2><h2 style='margin-bottom:0px;'><img class="center" alt="" src="images/select_ogroup.png" align="middle">&nbsp;{t}Phone attributes {/t}</h2></td>
</tr>
<tr>
<td>
		<table summary="" style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="0">
        <tr>
        <td colspan=2>
{render acl=$goFonQueueAnnounceHoldtimeACL}
            <input type="checkbox" name='goFonQueueAnnounceHoldtime' value='yes'  {$goFonQueueAnnounceHoldtimeCHK}>
{/render}
            {t}Announce holdtime{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
{render acl=$goFonDialOptiontACL}
            <input type="checkbox" name='goFonDialOptiont' value='t'  {$goFonDialOptiontCHK}>
{/render}
            {t}Allow the called user to transfer his call{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
{render acl=$goFonDialOptionTACL}
            <input type="checkbox" name='goFonDialOptionT' value='T' {$goFonDialOptionTCHK}>
{/render}
            {t}Allows calling user to transfer call{/t}
        </td>
        </table>

</td>
<td valign='top' style="border-left: 1px solid rgb(160, 160, 160);">
	 	<table summary=""  style="width: 100%; vertical-align: top; text-align: left;" border="0" cellpadding="0">
        <tr>
        <td colspan=2>
{render acl=$goFonDialOptionhACL}
            <input type="checkbox" name='goFonDialOptionh' value='h' {$goFonDialOptionhCHK}>
{/render}
            {t}Allow the called to hangup by pressing *{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
{render acl=$goFonDialOptionHACL}
            <input type="checkbox" name='goFonDialOptionH' value='H' {$goFonDialOptionHCHK}>
{/render}
            {t}Allows calling to hangup by pressing *{/t}
        </td>
        </tr>
        <tr>
        <td colspan=2>
{render acl=$goFonDialOptionrACL}
            <input type="checkbox" name='goFonDialOptionr' value='r' {$goFonDialOptionrCHK}>
{/render}
            {t}Ring instead of playing background music{/t}
        </td>
        </tr>
        </table>
</td>
</tr>

</table>
