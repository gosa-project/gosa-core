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
{render acl=$sambaHomePathACL}
      <input id="sambaHomePath" name="sambaHomePath" size=30 maxlength=60 value="{$sambaHomePath}">
{/render}
{render acl=$sambaHomeDriveACL}
      <select size="1" name="sambaHomeDrive">
       <option disabled>&nbsp;</option>
       {html_options values=$drives output=$drives selected=$sambaHomeDrive}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="sambaDomainName">{t}Domain{/t}</label></td>
     <td>
{render acl=$sambaDomainNameACL}
      <select id="sambaDomainName" size="1" name="sambaDomainName">
       {html_options values=$domains output=$domains selected=$sambaDomainName}
      </select>
{/render}
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
{render acl=$sambaLogonScriptACL}
      <input id="sambaLogonScript" name="sambaLogonScript" size=35 maxlength=60 value="{$sambaLogonScript}">
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="">{t}Profile path{/t}</label></td>
     <td>
{render acl=$sambaProfilePathACL}
      <input id="sambaProfilePath" name="sambaProfilePath" size=35 maxlength=60 value="{$sambaProfilePath}">
{/render}
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
{render acl=$AllowLoginOnTerminalServerACL}
   <input type=checkbox name="tslogin" id="tslogin" value="1" {$tslogin}
   	onclick="
		changeState('CtxWFHomeDir');
		changeState('CtxWFHomeDirDrive');
		changeState('CtxWFProfilePath');
		changeState('inherit');			
		changeTripleSelectState_2nd_neg('tslogin','inherit','CtxInitialProgram');
		changeTripleSelectState_2nd_neg('tslogin','inherit','CtxWorkDirectory');
		changeState('CtxMaxConnectionTimeF');
		changeState('CtxMaxDisconnectionTimeF');
		changeState('CtxMaxIdleTimeF');
		changeTripleSelectState('tslogin','CtxMaxConnectionTimeF','CtxMaxConnectionTime');
		changeTripleSelectState('tslogin','CtxMaxDisconnectionTimeF','CtxMaxDisconnectionTime');
		changeTripleSelectState('tslogin','CtxMaxIdleTimeF','CtxMaxIdleTime');
		changeState('connectclientdrives');
		changeState('connectclientprinters');
		changeState('defaultprinter');
		changeState('shadow');
		changeState('brokenconn');
		changeState('reconn');
	">
{/render}
   <i>{t}Allow login on terminal server{/t}</i>
   <table summary="">
    <tr>
     <td><label for="CtxWFHomeDir">{t}Home directory{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxWFHomeDir" name="CtxWFHomeDir" size=30 maxlength=60 value="{$CtxWFHomeDir}" {$tsloginstate}>
{/render}
{render acl=$AllowLoginOnTerminalServerACL}
      <select size="1" id="CtxWFHomeDirDrive" name="CtxWFHomeDirDrive"  {$tsloginstate}>
       <option disabled>&nbsp;</option>
       {html_options values=$drives output=$drives selected=$CtxWFHomeDirDrive}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="CtxWFProfilePath">{t}Profile path{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxWFProfilePath" name="CtxWFProfilePath" size=35 maxlength=60 value="{$CtxWFProfilePath}" {$tsloginstate}>
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
{render acl=$AllowLoginOnTerminalServerACL}
   <input type=checkbox id="inherit" name="inherit" {if $inheritstate} checked {/if}
	onClick="changeState('CtxInitialProgram');
 		 changeState('CtxWorkDirectory');"
	
	> 
{/render}
   <i>{t}Inherit client config{/t}</i>
   <table summary="">
    <tr>
     <td><label for="CtxInitialProgram">{t}Initial program{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxInitialProgram" name="CtxInitialProgram" size=35 maxlength=60 value="{$CtxInitialProgram}" {$inheritstate}>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="CtxWorkDirectory">{t}Working directory{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxWorkDirectory" name="CtxWorkDirectory" size=35 maxlength=60	value='{$CtxWorkDirectory}' {$inheritstate}>
{/render}
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
{render acl=$AllowLoginOnTerminalServerACL}
      <input 		id="CtxMaxConnectionTimeF" 	type=checkbox 			name="CtxMaxConnectionTimeF" 
			value="1" 			{$CtxMaxConnectionTimeF} 	
			onclick="changeState('CtxMaxConnectionTime')" {$tsloginstate}>
{/render}

      <label for="CtxMaxConnectionTimeF">{t}Connection{/t}</label>
     </td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input name="CtxMaxConnectionTime" id="CtxMaxConnectionTime" size=5 maxlength=5 value="{$CtxMaxConnectionTime}" {$tsloginstate}
			{ if !$CtxMaxConnectionTimeF } disabled  {/if}>
{/render}
     </td>
    </tr>
    <tr>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxMaxDisconnectionTimeF" type=checkbox name="CtxMaxDisconnectionTimeF" value="1" {$CtxMaxDisconnectionTimeF} onclick="changeState('CtxMaxDisconnectionTime')" {$tsloginstate}>
{/render}
      <label for="CtxMaxDisconnectionTimeF">{t}Disconnection{/t}</label>
     </td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input name="CtxMaxDisconnectionTime" id="CtxMaxDisconnectionTime" size=5 maxlength=5 value="{$CtxMaxDisconnectionTime}" {$tsloginstate}
			{ if !$CtxMaxDisconnectionTimeF } disabled  {/if}>
{/render}
     </td>
    </tr>
    <tr>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="CtxMaxIdleTimeF" type=checkbox name="CtxMaxIdleTimeF" value="1" {$CtxMaxIdleTimeF} onclick="changeState('CtxMaxIdleTime')" {$tsloginstate}>
{/render}
      <label for="CtxMaxIdleTimeF">{t}IDLE{/t}</label>
     </td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input name="CtxMaxIdleTime" id="CtxMaxIdleTime" size=5 maxlength=5 value="{$CtxMaxIdleTime}" {$tsloginstate}
			{ if !$CtxMaxIdleTimeF } disabled  {/if}>
{/render}
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
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="connectclientdrives" type=checkbox name="connectclientdrives" value="1" {$connectclientdrives} {$tsloginstate}>
{/render}
      <label for="connectclientdrives">{t}Connect client drives at logon{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="connectclientprinters" type=checkbox name="connectclientprinters" value="1" {$connectclientprinters}{$tsloginstate}>
{/render}
      <label for="connectclientprinters">{t}Connect client printers at logon{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <input id="defaultprinter" type=checkbox name="defaultprinter" value="1" {$defaultprinter}>
{/render}
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
{render acl=$AllowLoginOnTerminalServerACL}
      <select id="shadow" size="1" name="shadow" >
       {html_options options=$shadow selected=$shadowmode}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="brokenconn">{t}On broken or timed out{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <select id="brokenconn" size="1" name="brokenconn">
       {html_options options=$brokenconn selected=$brokenconnmode}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="reconn">{t}Reconnect if disconnected{/t}</label></td>
     <td>
{render acl=$AllowLoginOnTerminalServerACL}
      <select id="reconn" size="1" name="reconn">
       {html_options options=$reconn selected=$reconnmode}
      </select>
{/render}
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
{render acl=$sambaPwdCanChangeACL}
   <input id="allow_pwchange" type=checkbox name="allow_pwchange" value="1" {$flagsP}>
{/render}
   <label for="allow_pwchange">{t}Allow user to change password from client{/t}</label>
   <br>
{render acl=$sambaAcctFlagsNACL}
   <input id="no_password_required" type=checkbox name="no_password_required" value="1" {$flagsN}>
{/render}
   <label for="no_password_required">{t}Login from windows client requires no password{/t}</label>
   <br>
{render acl=$sambaAcctFlagsLACL}
   <input id="temporary_disable" type=checkbox name="temporary_disable" value="1" {$flagsD}>
{/render}
   <label for="temporary_disable">{t}Lock samba account{/t}</label>
   <br>
{render acl=$sambaPwdMustChangeACL}
   <input id="password_expires" type=checkbox name="password_expires" value="1" {$flagsC}>
{/render}
   <label for="password_expires">{t}Password expires on{/t}</label>
{render acl=$sambaPwdMustChangeACL}
   <select name=day onChange="createResult(this.form,this.form.sambaPwdMustChange);">
    {html_options values=$days output=$days selected=$day}
   </select>
{/render}
{render acl=$sambaPwdMustChangeACL}
   <select name=month onChange="populate(this.form,this.form.sambaPwdMustChange);">
    {html_options options=$months selected=$month}
   </select>
{/render}
{render acl=$sambaPwdMustChangeACL}
   <select name=year onChange="populate(this.form,this.form.sambaPwdMustChange);">
    {html_options values=$years output=$years selected=$year}
   </select>
{/render}
   <br>
{render acl=$sambaLogonTimeACL}
   <input id="logon_time_set" type=checkbox name="logon_time_set" value="1" {$flagsT}>
{/render}
   <label for="logon_time_set">{t}Limit Logon Time{/t}</label>
{render acl=$sambaLogonTimeACL}
   <select name=sambaLogonTime_day onChange="createResult_sambaLogonTime(this.form,this.form.sambaLogonTime);">
   {html_options values=$sambaLogonTime_days output=$sambaLogonTime_days selected=$sambaLogonTime_day}
   </select>
{/render}
{render acl=$sambaLogonTimeACL}
   <select name=sambaLogonTime_month onChange="populate_sambaLogonTime(this.form,this.form.sambaLogonTime);">
   {html_options options=$sambaLogonTime_months output=$sambaLogonTime_months selected=$sambaLogonTime_month}
   </select>
{/render}
{render acl=$sambaLogonTimeACL}
   <select name=sambaLogonTime_year onChange="populate_sambaLogonTime(this.form,this.form.sambaLogonTime);">
   {html_options values=$sambaLogonTime_years output=$sambaLogonTime_years selected=$sambaLogonTime_year}
   </select>
{/render}
   <br>
{render acl=$sambaLogoffTimeACL}
   <input id="logoff_time_set" type=checkbox name="logoff_time_set" value="1" {$flagsO}>
{/render}
   <label for="logoff_time_set">{t}Limit Logoff Time{/t}</label>
{render acl=$sambaLogoffTimeACL}
   <select name=sambaLogoffTime_day onChange="createResult_sambaLogoffTime(this.form,this.form.sambaLogoffTime);">
   {html_options values=$sambaLogoffTime_days output=$sambaLogoffTime_days selected=$sambaLogoffTime_day}
   </select>
{/render}
{render acl=$sambaLogoffTimeACL}
   <select name=sambaLogoffTime_month onChange="populate_sambaLogoffTime(this.form,this.form.sambaLogoffTime);">
   {html_options options=$sambaLogoffTime_months output=$sambaLogoffTime_months selected=$sambaLogoffTime_month}
   </select>
{/render}
{render acl=$sambaLogoffTimeACL}
   <select name=sambaLogoffTime_year onChange="populate_sambaLogoffTime(this.form,this.form.sambaLogoffTime);">
   {html_options values=$sambaLogoffTime_years output=$sambaLogoffTime_years selected=$sambaLogoffTime_year}
   </select>
{/render}
   <br>
   
{render acl=$sambaKickoffTimeACL}
   <input id="kickoff_time_set" type=checkbox name="kickoff_time_set" value="1" {$flagsK}>
{/render}
   <label for="kickoff_time_set">{t}Account expires after{/t}</label>
{render acl=$sambaKickoffTimeACL}
   <select name=sambaKickoffTime_day onChange="createResult_sambaKickoffTime(this.form,this.form.sambaKickoffTime);">
   {html_options values=$sambaKickoffTime_days output=$sambaKickoffTime_days selected=$sambaKickoffTime_day}
   </select>
{/render}
{render acl=$sambaKickoffTimeACL}
   <select name=sambaKickoffTime_month onChange="populate_sambaKickoffTime(this.form,this.form.sambaKickoffTime);">
   {html_options options=$sambaKickoffTime_months output=$sambaKickoffTime_months selected=$sambaKickoffTime_month}
   </select>
{/render}
{render acl=$sambaKickoffTimeACL}
   <select name=sambaKickoffTime_year onChange="populate_sambaKickoffTime(this.form,this.form.sambaKickoffTime);">
   {html_options values=$sambaKickoffTime_years output=$sambaKickoffTime_years selected=$sambaKickoffTime_year}
   </select>
{/render}
   <br>
   <input type="hidden" name="sambaPwdMustChange" value="{$sambaPwdMustChange}">
   <input type="hidden" name="sambaLogonTime" value="{$sambaLogonTime}">
   <input type="hidden" name="sambaLogoffTime" value="{$sambaLogoffTime}">
   <input type="hidden" name="sambaKickoffTime" value="{$sambaKickoffTime}">
<br>
{render acl=$sambaLogonHoursACL mode=read_active}
   {t}Samba logon times{/t}&nbsp;<input type='submit' name='SetSambaLogonHours' value='{t}Edit settings...{/t}'>
{/render}

  </td>	
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <label for="workstation_list">{t}Allow connection from these workstations only{/t}</label>
   <br>
{render acl=$sambaUserWorkstationsACL}
   <select id="workstation_list" style="width:100%;" name="workstation_list[]" size=10 multiple>
    {html_options values=$workstations output=$workstations}
   </select>
{/render}
   <br>
{render acl=$sambaUserWorkstationsACL}
   <input type="submit" value="{t}Add{/t}" name="add_ws">
{/render}
{render acl=$sambaUserWorkstationsACL}
   <input type="submit" value="{t}Delete{/t}" name="delete_ws">
{/render}
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
