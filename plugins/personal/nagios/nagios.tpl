<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Nagios Account{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="NagiosAlias">{t}alias{/t}</LABEL>{$must}</td>
     <td><input id="NagiosAlias" name="NagiosAlias" size=25 maxlength=65 {$NagiosAliasACL} value="{$NagiosAlias}"></td>
    </tr>

    <tr>
     <td><LABEL for="NagiosEmail">{t}email{/t}</LABEL>{$must}</td>
     <td><input id="NagiosEmail" name="NagiosEmail" size=25 maxlength=65 {$NagiosEmailACL} value="{$NagiosEmail}"></td>
    </tr>

    <tr>
     <td><LABEL for="NagiosPager">{t}pager{/t}</LABEL></td>
     <td><input id="NagiosPager" name="NagiosPager" size=25 maxlength=65 {$NagiosPagerACL} value="{$NagiosPager}"></td>
    </tr>

    <tr>
     <td><LABEL for="ServiceNotificationPeriod">{t}Service Notification Period{/t}</LABEL>{$must}</td>
     <td><input id="ServiceNotificationPeriod" name="ServiceNotificationPeriod" size=25 maxlength=65 {$ServiceNotificationPeriodACL} value="{$ServiceNotificationPeriod}"></td>
    </tr>

    <tr>
     <td><LABEL for="ServiceNotificationOptions">{t}Service Notification Options{/t}</LABEL>{$must}</td>
     <td><input id="ServiceNotificationOptions" name="ServiceNotificationOptions" size=25 maxlength=65 {$ServiceNotificationOptionsACL} value="{$ServiceNotificationOptions}"></td>
    </tr>

    <tr>
     <td><LABEL for="ServiceNotificationCommands">{t}Service Notification Commands{/t}</LABEL>{$must}</td>
     <td><input id="ServiceNotificationCommands" name="ServiceNotificationCommands" size=25 maxlength=65 {$ServiceNotificationCommandsACL} value="{$ServiceNotificationCommands}"></td>
    </tr>

    <tr>
     <td><LABEL for="HostNotificationPeriod">{t}Host Notification Period{/t}</LABEL>{$must}</td>
     <td><input id="HostNotificationPeriod" name="HostNotificationPeriod" size=25 maxlength=65 {$HostNotificationPeriodACL} value="{$HostNotificationPeriod}"></td>
    </tr>

    <tr>
     <td><LABEL for="HostNotificationOptions">{t}Host Notification Options{/t}</LABEL>{$must}</td>
     <td><input id="HostNotificationOptions" name="HostNotificationOptions" size=25 maxlength=65 {$HostNotificationOptionsACL} value="{$HostNotificationOptions}"></td>
    </tr>

    <tr>
     <td><LABEL for="HostNotificationCommands">{t}Host Notification Period{/t}</LABEL>{$must}</td>
     <td><input id="HostNotificationCommands" name="HostNotificationCommands" size=25 maxlength=65 {$HostNotificationCommandsACL} value="{$HostNotificationCommands}"></td>
    </tr>
	</table>
  

</table>

<input type="hidden" name="nagiosTab" value="nagiosTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.mail.focus();
  -->
</script>

