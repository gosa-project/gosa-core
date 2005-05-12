<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <input type=checkbox name="must_change_password" value="1" {$must_change_password} {$must_change_passwordACL}>
   {t}User must change password on first login{/t}
   <br>
   <input type=checkbox name="use_shadowMin" value="1" {$use_shadowMin} {$shadowMinACL}>
   {$shadowmins}
   <br>
   <input type=checkbox name="use_shadowMax" value="1" {$use_shadowMax} {$shadowMaxACL}>
   {$shadowmaxs}
   <br>
  </td>
  <td style="border-left:1px solid #A0A0A0">
    &nbsp;
  </td>
  <td style="vertical-align:top">
   <input type=checkbox name="use_shadowExpire" value="1" {$use_shadowExpire} {$shadowExpireACL}>
   {t}Password expires on{/t}
   <select name=day onChange="createResult(this.form,this.form.shadowExpire);" {$shadowExpireACL}>
    {html_options values=$days output=$days selected=$day}
   </select>
   <select name=month onChange="populate(this.form,this.form.shadowExpire);" {$shadowExpireACL}>
    {html_options options=$months selected=$month}
   </select>
   <select name=year onChange="populate(this.form,this.form.shadowExpire);" {$shadowExpireACL}>
    {html_options values=$years output=$years selected=$year}
   </select>
   <input type="hidden" name="shadowExpire" value="{$shadowExpire}">
   <br>
   <input type=checkbox name="use_shadowInactive" value="1" {$use_shadowInactive} {$shadowInactiveACL}>
   {$shadowinactives}
   <br>
   <input type=checkbox name="use_shadowWarning" value="1" {$use_shadowWarning} {$shadowWarningACL}>
   {$shadowwarnings}
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!--
  // Populate expiery dialog
  populate(document.mainform,document.mainform.shadowExpire);
  -->
</script>
