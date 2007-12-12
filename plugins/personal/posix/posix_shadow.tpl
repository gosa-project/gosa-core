<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">

<!-- must_change_password  -->
{render acl=$mustchangepasswordACL}
   <input type=checkbox name="mustchangepassword" value="1" {$mustchangepassword}>
{/render}
   {t}User must change password on first login{/t}
   <br>

<!-- shadowMin -->
{render acl=$shadowMinACL}
   <input type=checkbox name="use_shadowMin" value="1" {$use_shadowMin}>
{/render}
{render acl=$shadowMinACL}
   {$shadowmins}
{/render}
   <br>

<!-- use_shadowMax -->
{render acl=$shadowMaxACL}
   <input type=checkbox name="use_shadowMax" value="1" {$use_shadowMax}>
{/render}
{render acl=$shadowMaxACL}
   {$shadowmaxs}
{/render}
   <br>

<!-- use_shadowExpire -->
{render acl=$shadowExpireACL}
   <input type=checkbox name="use_shadowExpire" value="1" {$use_shadowExpire}>
{/render}
   {t}Password expires on{/t}
{render acl=$shadowExpireACL}
   <select name=day onChange="createResult(this.form,this.form.shadowExpire);">
    {html_options values=$days output=$days selected=$day}
   </select>
{/render}
{render acl=$shadowExpireACL}
   <select name=month onChange="populate(this.form,this.form.shadowExpire);">
    {html_options options=$months selected=$month}
   </select>
{/render}
{render acl=$shadowExpireACL}
   <select name=year onChange="populate(this.form,this.form.shadowExpire);">
    {html_options values=$years output=$years selected=$year}
   </select>
   <input type="hidden" name="shadowExpire" value="{$shadowExpire}">
{/render}
   <br>

<!-- shadowInactive -->
{render acl=$shadowInactiveACL}
   <input type=checkbox name="use_shadowInactive" value="1" {$use_shadowInactive}>
{/render}
{render acl=$shadowInactiveACL}
   {$shadowinactives}
{/render}
   <br>

<!-- use_shadowWarning -->
{render acl=$shadowWarningACL}
   <input type=checkbox name="use_shadowWarning" value="1" {$use_shadowWarning}>
{/render}
{render acl=$shadowWarningACL}
   {$shadowwarnings}
{/render}
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
