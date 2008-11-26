<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">

 <!-- Headline container -->
 <tr>
  <td colspan="2" style="width:50%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png"/>&nbsp;{t}Generic{/t}</h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td><label for="sambaHomePath">{t}Home directory{/t}</label></td>
     <td>
      <input id="sambaHomePath" name="sambaHomePath" size=30 maxlength=60 {$sambaHomePathACL} value="{$sambaHomePath}">
      <select size="1" name="sambaHomeDrive" {$sambaHomeDriveACL}>
       {html_options values=$drives output=$drives selected=$sambaHomeDrive}
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="sambaDomainName">{t}Domain{/t}</label></td>
     <td>
      <select id="sambaDomainName" size="1" name="sambaDomainName" {$sambaDomainNameACL}>
       {html_options values=$domains output=$domains selected=$sambaDomainName}
      </select>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td><label for="">{t}Script path{/t}</label></td>
     <td>
      <input id="sambaLogonScript" name="sambaLogonScript" size=35 maxlength=60 {$sambaLogonScriptACL} value="{$sambaLogonScript}">
     </td>
    </tr>
    <tr>
     <td><label for="">{t}Profile path{/t}</label></td>
     <td>
      <input id="sambaProfilePath" name="sambaProfilePath" size=35 maxlength=60 {$sambaProfilePathACL} value="{$sambaProfilePath}">
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/display.png" />&nbsp;{t}Terminal Server{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="vertical-align:top;width:50%">
   <input type=checkbox name="tslogin" id="tslogin" value="1" {$tslogin} {$tsloginACL}
   	onclick="
		
		{if $connectclientdrivesACL==""}	changeState('CtxWFHomeDir');		{/if}
		{if $CtxWFHomeDirDriveACL==""} 		changeState('CtxWFHomeDirDrive');	{/if}
		{if $CtxWFProfilePathACL==""} 		changeState('CtxWFProfilePath'); 	{/if}

		{if $inheritACL==""}				
		changeState('inherit');				
		changeTripleSelectState_2nd_neg('tslogin','inherit','CtxInitialProgram');
		changeTripleSelectState_2nd_neg('tslogin','inherit','CtxWorkDirectory');
		{/if}
		
		{if $CtxMaxConnectionTimeACL==""}      	changeState('CtxMaxConnectionTimeF'); 	{/if}
		{if $CtxMaxDisconnectionTimeACL==""}   	changeState('CtxMaxDisconnectionTimeF');{/if}
		{if $CtxMaxIdleTimeACL==""}      		changeState('CtxMaxIdleTimeF');			{/if}
	
		
		changeTripleSelectState('tslogin','CtxMaxConnectionTimeF','CtxMaxConnectionTime');
		changeTripleSelectState('tslogin','CtxMaxDisconnectionTimeF','CtxMaxDisconnectionTime');
		changeTripleSelectState('tslogin','CtxMaxIdleTimeF','CtxMaxIdleTime');
	
		{if $connectclientdrivesACL==""}	changeState('connectclientdrives');		{/if}
		{if $connectclientprintersACL==""}	changeState('connectclientprinters');	{/if}
		{if $defaultprinterACL==""}			changeState('defaultprinter');			{/if}
		
		{if $shadowACL == "" }				changeState('shadow'); 				{/if}
		{if $brokenconnACL == "" }          changeState('brokenconn');			{/if}
		{if $reconnACL == "" }              changeState('reconn');				{/if}
	">
   <i>{t}Allow login on terminal server{/t}</i>
   <table summary="">
    <tr>
     <td><label for="CtxWFHomeDir">{t}Home directory{/t}</label></td>
     <td>
      <input id="CtxWFHomeDir" name="CtxWFHomeDir" size=30 maxlength=60 {$CtxWFHomeDirACL} value="{$CtxWFHomeDir}" {$tsloginstate}>
      <select size="1" id="CtxWFHomeDirDrive" name="CtxWFHomeDirDrive" {$CtxWFHomeDirDriveACL} {$tsloginstate}>
       {html_options values=$drives output=$drives selected=$CtxWFHomeDirDrive}
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="CtxWFProfilePath">{t}Profile path{/t}</label></td>
     <td>
      <input id="CtxWFProfilePath" name="CtxWFProfilePath" size=35 maxlength=60 {$CtxWFProfilePathACL} value="{$CtxWFProfilePath}" {$tsloginstate}>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <input type=checkbox id="inherit" name="inherit" {$inherit} {$inheritACL} 
	onclick="{if $CtxInitialProgramACL ==""}changeState('CtxInitialProgram');{/if}
			 {if $CtxWorkDirectoryACL ==""}changeState('CtxWorkDirectory');{/if}" {$tsloginstate}>
   <i>{t}Inherit client config{/t}</i>
   <table summary="">
    <tr>
     <td><label for="CtxInitialProgram">{t}Initial program{/t}</label></td>
     <td>
      <input id="CtxInitialProgram" name="CtxInitialProgram" size=35 maxlength=60 {if $CtxInitialProgramACL==""}{$CtxInitialProgramMode}{/if} value="{$CtxInitialProgram}" 
		{if $CtxInitialProgramACL!="" || $tsloginstate!="" || $inheritstate!=""}
			disabled 
		{/if}
		{$tsloginstate}
		>
     </td>
    </tr>
    <tr>
     <td><label for="CtxWorkDirectory">{t}Working directory{/t}</label></td>
     <td>
      <input id="CtxWorkDirectory" name="CtxWorkDirectory" size=35 maxlength=60 
		{if $CtxWorkDirectoryACL==""}
			{$CtxInitialProgramMode}
		{/if} value="{$CtxWorkDirectory}" 
		{if $CtxInitialProgramACL=="disabled" || $tsloginstate=="disabled" || $inheritstate=="disabled"}
			disabled 
		{/if}
		{$CtxWorkDirectoryACL}
		>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="vertical-align:top;">
   <i>{t}Timeout settings (in minutes){/t}</i>
   <table summary="">
    <tr>
     <td>
      <input id="CtxMaxConnectionTimeF" type=checkbox name="CtxMaxConnectionTimeF" value="1" {$CtxMaxConnectionTimeF} {$CtxMaxConnectionTimeACL} onclick="changeSubselectState('CtxMaxConnectionTimeF','CtxMaxConnectionTime')" {$tsloginstate}>
      <label for="CtxMaxConnectionTimeF">{t}Connection{/t}</label>
     </td>
     <td>
      <input name="CtxMaxConnectionTime" id="CtxMaxConnectionTime" size=5 maxlength=5 {if $CtxMaxConnectionTimeACL==""}{$CtxMaxConnectionTimeMode}{/if} value="{$CtxMaxConnectionTime}" {$CtxMaxConnectionTimeACL} {$tsloginstate}>
     </td>
    </tr>
    <tr>
     <td>
      <input id="CtxMaxDisconnectionTimeF" type=checkbox name="CtxMaxDisconnectionTimeF" value="1" {$CtxMaxDisconnectionTimeF} {$CtxMaxDisconnectionTimeACL} onclick="changeState('CtxMaxDisconnectionTime')" {$tsloginstate}>
      <label for="CtxMaxDisconnectionTimeF">{t}Disconnection{/t}</label>
     </td>
     <td>
      <input name="CtxMaxDisconnectionTime" id="CtxMaxDisconnectionTime" size=5 maxlength=5 {if $CtxMaxDisconnectionTimeACL ==""}{$CtxMaxDisconnectionTimeMode}{/if} value="{$CtxMaxDisconnectionTime}" {$CtxMaxDisconnectionTimeACL} {$tsloginstate}>
     </td>
    </tr>
    <tr>
     <td>
      <input id="CtxMaxIdleTimeF" type=checkbox name="CtxMaxIdleTimeF" value="1" {$CtxMaxIdleTimeF} {$CtxMaxIdleTimeACL} onclick="changeState('CtxMaxIdleTime')" {$tsloginstate}>
      <label for="CtxMaxIdleTimeF">{t}IDLE{/t}</label>
     </td>
     <td>
      <input name="CtxMaxIdleTime" id="CtxMaxIdleTime" size=5 maxlength=5 {if $CtxMaxIdleTimeACL==""}{$CtxMaxIdleTimeMode}{/if} value="{$CtxMaxIdleTime}" {$CtxMaxIdleTimeACL} {$tsloginstate}>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">

   <i>{t}Client devices{/t}</i>
   <table summary="">
    <tr>
     <td>
      <input id="connectclientdrives" type=checkbox name="connectclientdrives" value="1" {$connectclientdrives} {$connectclientdrivesACL} {$tsloginstate}>
      <label for="connectclientdrives">{t}Connect client drives at logon{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      <input id="connectclientprinters" type=checkbox name="connectclientprinters" value="1" {$connectclientprinters} {$connectclientprintersACL} {$tsloginstate}>
      <label for="connectclientprinters">{t}Connect client printers at logon{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      <input id="defaultprinter" type=checkbox name="defaultprinter" value="1" {$defaultprinter} {$defaultprinterACL} {$tsloginstate}>
      <label for="defaultprinter">{t}Default to main client printer{/t}</label>
     </td>
    </tr>
   </table>

  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;width:50%">
   <i>{t}Miscellaneous{/t}</i>
   <table summary="">
    <tr>
     <td>
      <label for="shadow">{t}Shadowing{/t}</label>
     </td>
     <td>
      <select id="shadow" size="1" name="shadow" {$shadowACL} {$tsloginstate}>
       {html_options options=$shadow selected=$shadowmode}
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="brokenconn">{t}On broken or timed out{/t}</label></td>
     <td>
      <select id="brokenconn" size="1" name="brokenconn" {$brokenconnACL} {$tsloginstate}>
       {html_options options=$brokenconn selected=$brokenconnmode}
      </select>
     </td>
    </tr>
    <tr>
     <td><label for="reconn">{t}Reconnect if disconnected{/t}</label></td>
     <td>
      <select id="reconn" size="1" name="reconn" {$reconnACL} {$tsloginstate}>
       {html_options options=$reconn selected=$reconnmode}
      </select>
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/certs.png" />&nbsp;{t}Access options{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <input id="allow_pwchange" type=checkbox name="allow_pwchange" value="1" {$flagsP} {$allow_pwchangeACL}>
   <label for="allow_pwchange">{t}Allow user to change password from client{/t}</label>
   <br>
   <input id="no_password_required" type=checkbox name="no_password_required" value="1" {$flagsN}  {$no_password_requiredACL}>
   <label for="no_password_required">{t}Login from windows client requires no password{/t}</label>
   <br>
   <input id="temporary_disable" type=checkbox name="temporary_disable" value="1" {$flagsD} {$temporary_disableACL}>
   <label for="temporary_disable">{t}Lock samba account{/t}</label>
   <br>
   <input id="password_expires" type=checkbox name="password_expires" value="1" {$flagsC} {$sambaPwdMustChangeACL}>
   <label for="password_expires">{t}Password expires on{/t}</label>
   <select name=day onChange="createResult(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options values=$days output=$days selected=$day}
   </select>
   <select name=month onChange="populate(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options options=$months selected=$month}
   </select>
   <select name=year onChange="populate(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options values=$years output=$years selected=$year}
   </select>
   <br>
   <input id="logon_time_set" type=checkbox name="logon_time_set" value="1" {$flagsT} {$sambaLogonTimeACL}>
   <label for="logon_time_set">{t}Limit Logon Time{/t}</label>
   <select name=sambaLogonTime_day onChange="createResult_sambaLogonTime(this.form,this.form.sambaLogonTime);" {$sambaLogonTimeACL}>
   {html_options values=$sambaLogonTime_days output=$sambaLogonTime_days selected=$sambaLogonTime_day}
   </select>
   <select name=sambaLogonTime_month onChange="populate_sambaLogonTime(this.form,this.form.sambaLogonTime);" {$sambaLogonTimeACL}>
   {html_options options=$sambaLogonTime_months output=$sambaLogonTime_months selected=$sambaLogonTime_month}
   </select>
   <select name=sambaLogonTime_year onChange="populate_sambaLogonTime(this.form,this.form.sambaLogonTime);" {$sambaLogonTimeACL}>
   {html_options values=$sambaLogonTime_years output=$sambaLogonTime_years selected=$sambaLogonTime_year}
   </select>
   <br>
   <input id="logoff_time_set" type=checkbox name="logoff_time_set" value="1" {$flagsO} {$sambaLogoffTimeACL}>
   <label for="logoff_time_set">{t}Limit Logoff Time{/t}</label>
   <select name=sambaLogoffTime_day onChange="createResult_sambaLogoffTime(this.form,this.form.sambaLogoffTime);" {$sambaLogoffTimeACL}>
   {html_options values=$sambaLogoffTime_days output=$sambaLogoffTime_days selected=$sambaLogoffTime_day}
   </select>
   <select name=sambaLogoffTime_month onChange="populate_sambaLogoffTime(this.form,this.form.sambaLogoffTime);" {$sambaLogoffTimeACL}>
   {html_options options=$sambaLogoffTime_months output=$sambaLogoffTime_months selected=$sambaLogoffTime_month}
   </select>
   <select name=sambaLogoffTime_year onChange="populate_sambaLogoffTime(this.form,this.form.sambaLogoffTime);" {$sambaLogoffTimeACL}>
   {html_options values=$sambaLogoffTime_years output=$sambaLogoffTime_years selected=$sambaLogoffTime_year}
   </select>
   <br>
   
   <input id="kickoff_time_set" type=checkbox name="kickoff_time_set" value="1" {$flagsK} {$sambaKickoffTimeACL}>
   <label for="kickoff_time_set">{t}Account expires after{/t}</label>
   <select name=sambaKickoffTime_day onChange="createResult_sambaKickoffTime(this.form,this.form.sambaKickoffTime);" {$sambaKickoffTimeACL}>
   {html_options values=$sambaKickoffTime_days output=$sambaKickoffTime_days selected=$sambaKickoffTime_day}
   </select>
   <select name=sambaKickoffTime_month onChange="populate_sambaKickoffTime(this.form,this.form.sambaKickoffTime);" {$sambaKickoffTimeACL}>
   {html_options options=$sambaKickoffTime_months output=$sambaKickoffTime_months selected=$sambaKickoffTime_month}
   </select>
   <select name=sambaKickoffTime_year onChange="populate_sambaKickoffTime(this.form,this.form.sambaKickoffTime);" {$sambaKickoffTimeACL}>
   {html_options values=$sambaKickoffTime_years output=$sambaKickoffTime_years selected=$sambaKickoffTime_year}
   </select>
   <br>
   <input type="hidden" name="sambaPwdMustChange" value="{$sambaPwdMustChange}">
   <input type="hidden" name="sambaLogonTime" value="{$sambaLogonTime}">
   <input type="hidden" name="sambaLogoffTime" value="{$sambaLogoffTime}">
   <input type="hidden" name="sambaKickoffTime" value="{$sambaKickoffTime}">
   <br>
   {t}Samba logon times{/t}&nbsp;<input type='submit' name='SetSambaLogonHours' value='{t}Edit settings...{/t}' {$sambaLogonHoursACL}>
  </td>	
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <label for="workstation_list">{t}Allow connection from these workstations only{/t}</label>
   <br>
   <select id="workstation_list" style="width:100%;" name="workstation_list[]" size=10 multiple {$sambaUserWorkstationACL}>
    {html_options values=$workstations output=$workstations}
   </select>
   <br>
   <input type="submit" value="{t}Add{/t}" name="add_ws"
        {$sambaUserWorkstationACL}>&nbsp;
   <input type="submit" value="{t}Delete{/t}" name="delete_ws"
        {$sambaUserWorkstationACL}>
  </td>
 </tr>
</table>

<input type="hidden" name="sambaTab" value="sambaTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- 
  // Populate expiery dialog
  populate(document.mainform,document.mainform.sambaPwdMustChange);

  // Populate samba Time dialogs
  populate_sambaLogonTime(document.mainform,document.mainform.sambaLogonTime);
  populate_sambaLogoffTime(document.mainform,document.mainform.sambaLogoffTime);
  populate_sambaKickoffTime(document.mainform,document.mainform.sambaKickoffTime);
  -->
</script>
