<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">

 <!-- Headline container -->
 <tr>
  <td colspan="2" style="width:50%; vertical-align:top;">
   <h2><p class="center"><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</p></h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td><label for="smbHome">{t}Samba home{/t}</label></td>
     <td>
      <input id="smbHome" name="smbHome" size=30 maxlength=60 {$smbHomeACL} value="{$smbHome}">
      <select size="1" name="homeDrive" {$homeDriveACL}>
       <option disabled>&nbsp;</option>
       {html_options values=$drives output=$drives selected=$homeDrive}
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
     <td><label for="scriptPath">{t}Script path{/t}</label></td>
     <td>
      <input id="scriptPath" name="scriptPath" size=35 maxlength=60 {$scriptPathACL} value="{$scriptPath}">
     </td>
    </tr>
    <tr>
     <td><label for="profilePath">{t}Profile path{/t}</label></td>
     <td>
      <input id="profilePath" name="profilePath" size=35 maxlength=60 {$profilePathACL} value="{$profilePath}">
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><p class="center"><img class="center" alt="" align="middle" src="images/certs.png" />&nbsp;{t}Access options{/t}</p></h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <tr>
  <td style="width:50%;">
   <input id="allow_pwchange" type=checkbox name="allow_pwchange" value="1" {$flagsP} {$allow_pwchangeACL}>
   <label for="allow_pwchange">{t}Allow user to change password from client{/t}</label>
   <br>
   <input id="no_password_required" type=checkbox name="no_password_required" value="1" {$flagsN}  {$no_password_requiredACL}>
   <label for="no_password_required">{t}Login from windows client requires no password{/t}</label>
   <br>
   <input id="temporary_disable" type=checkbox name="temporary_disable" value="1" {$flagsD} {$temporary_disableACL}>
   <label for="temporary_disable">{t}Temporary disable samba account{/t}</label>
   <br>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top; width:50%">
   <input type=checkbox name="password_expires" value="1" {$flagsC} {$password_expiresACL}>
   <label for="day">{t}Password expires on{/t}</label>
   <select id="day" name=day onChange="createResult(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options values=$days output=$days selected=$day}
   </select>
   <select name=month onChange="populate(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options options=$months selected=$month}
   </select>
   <select name=year onChange="populate(this.form,this.form.sambaPwdMustChange);" {$sambaPwdMustChangeACL}>
    {html_options values=$years output=$years selected=$year}
   </select>
   <input type="hidden" name="sambaPwdMustChange" value="{$sambaPwdMustChange}">
  </td>
 </tr>
</table>

<input type="hidden" name="sambaTab" value="sambaTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- 
  // Populate expiery dialog
  populate(document.mainform,document.mainform.sambaPwdMustChange);
  -->
</script>
