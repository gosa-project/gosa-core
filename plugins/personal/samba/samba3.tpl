<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
  <td colspan=2 style="width:50%; vertical-align:top;">
   <h2><img align="center" src="images/rightarrow.png"> {t}Generic{/t}</h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top;">
   <table>
    <tr>
     <td>{t}Home directory{/t}</td>
     <td>
      <input name="sambaHomePath" size=30 maxlength=60 {$sambaHomePathACL} value="{$sambaHomePath}">
      <select size="1" name="sambaHomeDrive" {$sambaHomeDriveACL}>
       <option></option>
       {html_options values=$drives output=$drives selected=$sambaHomeDrive}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Domain{/t}</td>
     <td>
      <select size="1" name="sambaDomainName" {$sambaDomainNameACL}>
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
   <table>
    <tr>
     <td>{t}Script path{/t}</td>
     <td>
      <input name="sambaLogonScript" size=35 maxlength=60 {$sambaLogonScriptACL} value="{$sambaLogonScript}">
     </td>
    </tr>
    <tr>
     <td>{t}Profile path{/t}</td>
     <td>
      <input name="sambaProfilePath" size=35 maxlength=60 {$sambaProfilePathACL} value="{$sambaProfilePath}">
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img align="center" src="images/display.png"> {t}Terminal Server{/t}</h2>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="vertical-align:top;width:50%">
   <input type=checkbox name="tslogin" value="1" {$tslogin} {$tsloginACL}>
   <i>{t}Allow login on terminal server{/t}</i>
   <table>
    <tr>
     <td>{t}Home directory{/t}</td>
     <td>
      <input name="CtxWFHomeDir" size=30 maxlength=60 {$CtxWFHomeDirACL} value="{$CtxWFHomeDir}">
      <select size="1" name="CtxWFHomeDirDrive" {$CtxWFHomeDirDriveACL}>
       <option></option>
       {html_options values=$drives output=$drives selected=$CtxWFHomeDirDrive}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Profile path{/t}</td>
     <td>
      <input name="CtxWFProfilePath" size=35 maxlength=60 {$CtxWFProfilePathACL} value="{$CtxWFProfilePath}">
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <input type=checkbox name="inherit" value="1" {$inherit} {$inheritACL} onclick="changeState('CtxInitialProgram'); changeState('CtxWorkDirectory')">
   <i>{t}Inherit client config{/t}</i>
   <table>
    <tr>
     <td>{t}Initial program{/t}</td>
     <td>
      <input name="CtxInitialProgram" id="CtxInitialProgram" size=35 maxlength=60 {$CtxInitialProgramACL} value="{$CtxInitialProgram}" {$CtxInitialProgramMode}>
     </td>
    </tr>
    <tr>
     <td>{t}Working directory{/t}</td>
     <td>
      <input name="CtxWorkDirectory" id="CtxWorkDirectory" size=35 maxlength=60 {$CtxWorkDirectoryACL} value="{$CtxWorkDirectory}" {$CtxInitialProgramMode}>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="vertical-align:top;">
   <i>{t}Timeout settings (in minutes){/t}</i>
   <table>
    <tr>
     <td>
      <input type=checkbox name="CtxMaxConnectionTimeF" value="1" {$CtxMaxConnectionTimeF} {$CtxMaxConnectionTimeACL} onclick="changeState('CtxMaxConnectionTime')">
      {t}Connection{/t}
     </td>
     <td>
      <input name="CtxMaxConnectionTime" id="CtxMaxConnectionTime" size=5 maxlength=5 {$CtxMaxConnectionTimeACL} value="{$CtxMaxConnectionTime}" {$CtxMaxConnectionTimeMode}>
     </td>
    </tr>
    <tr>
     <td>
      <input type=checkbox name="CtxMaxDisconnectionTimeF" value="1" {$CtxMaxDisconnectionTimeF} {$CtxMaxDisconnectionTimeACL} onclick="changeState('CtxMaxDisconnectionTime')">
      {t}Disconnection{/t}
     </td>
     <td>
      <input name="CtxMaxDisconnectionTime" id="CtxMaxDisconnectionTime" size=5 maxlength=5 {$CtxMaxDisconnectionTimeACL} value="{$CtxMaxDisconnectionTime}" {$CtxMaxDisconnectionTimeMode}>
     </td>
    </tr>
    <tr>
     <td>
      <input type=checkbox name="CtxMaxIdleTimeF" value="1" {$CtxMaxIdleTimeF} {$CtxMaxIdleTimeACL} onclick="changeState('CtxMaxIdleTime')">
      {t}IDLE{/t}
     </td>
     <td>
      <input name="CtxMaxIdleTime" id="CtxMaxIdleTime" size=5 maxlength=5 {$CtxMaxIdleTimeACL} value="{$CtxMaxIdleTime}" {$CtxMaxIdleTimeMode}>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">

   <i>{t}Client devices{/t}</i>
   <table>
    <tr>
     <td>
      <input type=checkbox name="connectclientdrives" value="1" {$connectclientdrives} {$connectclientdrivesACL}>
      {t}Connect client drives at logon{/t}
     </td>
    </tr>
    <tr>
     <td>
      <input type=checkbox name="connectclientprinters" value="1" {$connectclientprinters} {$connectclientprintersACL}>
      {t}Connect client printers at logon{/t}
     </td>
    </tr>
    <tr>
     <td>
      <input type=checkbox name="defaultprinter" value="1" {$defaultprinter} {$defaultprinterACL}>
      {t}Default to main client printer{/t}
     </td>
    </tr>
   </table>

  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <i>{t}Miscellaneous{/t}</i>
   <table>
    <tr>
     <td>
      {t}Shadowing{/t}
     </td>
     <td>
      <select size="1" name="shadow" {$shadowACL}>
       {html_options options=$shadow selected=$shadowmode}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}On broken or timed out{/t}</td>
     <td>
      <select size="1" name="brokenconn" {$brokenconnACL}>
       {html_options options=$brokenconn selected=$brokenconnmode}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Reconnect if disconnected{/t}</td>
     <td>
      <select size="1" name="reconn" {$reconnACL}>
       {html_options options=$reconn selected=$reconnmode}
      </select>
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img align="center" src="images/certs.png"> {t}Access options{/t}</h2>

<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <input type=checkbox name="allow_pwchange" value="1" {$flagsP} {$allow_pwchangeACL}>
   {t}Allow user to change password from client{/t}
   <br>
   <input type=checkbox name="no_password_required" value="1" {$flagsN}  {$no_password_requiredACL}>
   {t}Login from windows client requires no password{/t}
   <br>
   <input type=checkbox name="temporary_disable" value="1" {$flagsD} {$temporary_disableACL}>
   {t}Lock samba account{/t}
   <br>
   <input type=checkbox name="password_expires" value="1" {$flagsC} {$password_expiresACL}>
   {t}Password expires on{/t}
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
   <input type=checkbox name="logon_time_set" value="1" {$flagsT} {$logon_time_setACL}>
   {t}Limit Logon Time{/t}
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
   <input type=checkbox name="logoff_time_set" value="1" {$flagsO} {$logoff_time_setACL}>
   {t}Limit Logoff Time{/t}
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
   <input type=checkbox name="kickoff_time_set" value="1" {$flagsK} {$kickoff_time_setACL}>
   {t}Account expires after{/t}
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
  </td>	
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   {t}Allow connection from these workstations only{/t}
   <br>
   <select style="width:350px;" name="workstation_list[]" size=10 multiple {$sambaUserWorkstationACL}>
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
  // First input field on page
  document.mainform.sambaHomePath.focus();

  // Populate expiery dialog
  populate(document.mainform,document.mainform.sambaPwdMustChange);

  // Populate samba Time dialogs
  populate_sambaLogonTime(document.mainform,document.mainform.sambaLogonTime);
  populate_sambaLogoffTime(document.mainform,document.mainform.sambaLogoffTime);
  populate_sambaKickoffTime(document.mainform,document.mainform.sambaKickoffTime);
  -->
</script>
