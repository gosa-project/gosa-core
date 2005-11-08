<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Nagios Account{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="NagiosAlias">{t}Alias{/t}</LABEL>{$must}</td>
     <td><input id="NagiosAlias" name="NagiosAlias" size=25 maxlength=65 {$NagiosAliasACL} value="{$NagiosAlias}"></td>
    </tr>

    <tr>
     <td><LABEL for="NagiosMail">{t}Mail address{/t}</LABEL>{$must}</td>
     <td><input id="NagiosMail" name="NagiosMail" size=25 maxlength=65 {$NagiosMailACL} value="{$NagiosMail}"></td>
    </tr>

	<tr>
	<td><LABEL for="HostNotificationPeriod">{t}Host notification period{/t}</LABEL>{$must}</td>
	<td><select name="HostNotificationPeriod" id="HostNotificationPeriod" {$HostNotificationPeriodACL}>
	{html_options options=$HostNotificationPeriodValues values=$HostNotificationPeriodValues selected=$HostNotificationPeriod }
	</select>
	</td>
	</tr>

	<tr>
	<td><LABEL for="ServiceNotificationPeriod">{t}Service notification period{/t}</LABEL>{$must}</td>
	<td><select name="ServiceNotificationPeriod" id="ServiceNotificationPeriod" {$ServiceNotificationPeriodACL}>
	{html_options options=$ServiceNotificationPeriodValues values=$ServiceNotificationPeriodValues selected=$ServiceNotificationPeriod}
	</select>
	</td>
	</tr>
	<tr>
	<td><LABEL for="ServiceNotificationOptions">{t}Service notification options{/t}</LABEL>{$must}</td>
	<td><select name="ServiceNotificationOptions" id="ServiceNotificationOptions" {$ServiceNotificationOptionsACL}>
	{html_options options=$ServiceNotificationOptionsValues values=$ServiceNotificationOptionsValues selected=$ServiceNotificationOptions}
	</select>
	</td>
	</tr>


	<tr>
	<td><LABEL for="HostNotificationOptions">{t}Host notification options{/t}</LABEL>{$must}</td>
	<td><select name="HostNotificationOptions" id="HostNotificationOptions" {$HostNotificationOptionsACL}>
	{html_options options=$HostNotificationOptionsValues values=$HostNotificationOptionsValues selected=$HostNotificationOptions }
	</select>
	</td>
	</tr>
    <tr>
     <td><LABEL for="NagiosPager">{t}Pager{/t}</LABEL></td>
     <td><input id="NagiosPager" name="NagiosPager" size=25 maxlength=65 {$NagiosPagerACL} value="{$NagiosPager}"></td>
    </tr>

    <tr>
     <td><LABEL for="ServiceNotificationCommands">{t}Service notification commands{/t}</LABEL></td>
     <td><input id="ServiceNotificationCommands" disabled name="ServiceNotificationCommands" size=25 maxlength=65 {$ServiceNotificationCommandsACL} value="{$ServiceNotificationCommands}"></td>
    </tr>
    <tr>
     <td><LABEL for="HostNotificationCommands">{t}Host notification commands{/t}</LABEL></td>
     <td><input id="HostNotificationCommands" disabled name="HostNotificationCommands" size=25 maxlength=65 {$HostNotificationCommandsACL} value="{$HostNotificationCommands}"></td>
    </tr>
</table>
</td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
<td style="width:100%; vertical-align:top;">
   <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Nagios authentification{/t}</h2>
   <table summary="">
	<tr><td><input type="checkbox" name="AuthorizedSystemInformation" value="1" {$AuthorizedSystemInformationACL}  
{$AuthorizedSystemInformationCHK}>{t}view system informations{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedConfigurationInformation" {$AuthorizedConfigurationInformationACL} value="1" 
{$AuthorizedConfigurationInformationCHK}>{t}view configuration information{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedSystemCommands" {$AuthorizedSystemCommandsACL} value="1" 
{$AuthorizedSystemCommandsCHK}>{t}trigger system commands{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedAllServices" {$AuthorizedAllServicesACL} value="1" 
{$AuthorizedAllServicesCHK}>{t}view all services{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedAllHosts" {$AuthorizedAllHostsACL} value="1" 
{$AuthorizedAllHostsCHK}>{t}view all hosts{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedAllServiceCommands" {$AuthorizedAllServiceCommandsACL} value="1" {$AuthorizedAllServiceCommandsCHK}>{t}trigger all service commands{/t}</td></tr>
	
	<tr><td><input type="checkbox" name="AuthorizedAllHostCommands" {$AuthorizedAllHostCommandsACL} value="1" 
{$AuthorizedAllHostCommandsCHK}>{t}trigger all host commands{/t}</td></tr>

   </table>
</td>

</table>

<input type="hidden" name="nagiosTab" value="nagiosTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.NagiosAlias.focus();
  -->
</script>

