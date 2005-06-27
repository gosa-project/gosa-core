<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
  <td colspan=2 style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td>{t}Samba home{/t}</td>
     <td>
      <input name="smbHome" size=30 maxlength=60 {$smbHomeACL} value="{$smbHome}">
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
     <td>{t}Script path{/t}</td>
     <td>
      <input name="scriptPath" size=35 maxlength=60 {$scriptPathACL} value="{$scriptPath}">
     </td>
    </tr>
    <tr>
     <td>{t}Profile path{/t}</td>
     <td>
      <input name="profilePath" size=35 maxlength=60 {$profilePathACL} value="{$profilePath}">
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img  alt="" align="middle" src="images/certs.png"> {t}Access options{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="width:50%;">
   <input type=checkbox name="allow_pwchange" value="1" {$flagsP} {$allow_pwchangeACL}>
   {t}Allow user to change password from client{/t}
   <br>
   <input type=checkbox name="no_password_required" value="1" {$flagsN}  {$no_password_requiredACL}>
   {t}Login from windows client requires no password{/t}
   <br>
   <input type=checkbox name="temporary_disable" value="1" {$flagsD} {$temporary_disableACL}>
   {t}Temporary disable samba account{/t}
   <br>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top; width:50%">
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
   <input type="hidden" name="sambaPwdMustChange" value="{$sambaPwdMustChange}">
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
  -->
</script>
