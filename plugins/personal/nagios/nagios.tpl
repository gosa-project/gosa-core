

<h2><input type="checkbox" name="nagiosContact" value="B" {$nagiosContact}>{t}Nagios Contact{/t}</h2>
<table summary="" style="width:90%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

<tr><td>nagios alias</td><td><input  style="width:280px" type="textbox" name="NagiosAlias"  {$NagiosAliasACL} value="{$NagiosAlias}"></td></tr>

<tr><td>nagios email</td><td><input style="width:280px" type="textbox" name="NagiosMail"  {$NagiosMailACL} value="{$NagiosMail}"></td></tr>

<tr><td>nagios pager</td><td><input style="width:280px" type="textbox" name="NagiosPager"  {$NagiosPagerACL} value="{$NagiosPager}"></td></tr>
<tr><td colspan=2><hr></td><td></tr>
<tr><td>service_notification_period</td><td><input style="width:280px" type="textbox" name="ServiceNotificationPeriod"  {$ServiceNotificationPeriodACL} value="{$ServiceNotificationPeriod}"></td>

<td>host_notification_options</td><td><input style="width:280px" type="textbox" name="HostNotificationOptions" {$HostNotificationOptionsACL} value="{$HostNotificationOptions}"></td></tr>

<tr><td>service_notification_options</td><td><input style="width:280px" type="textbox" name="ServiceNotificationOptions" {$ServiceNotificationOptions} value="{$ServiceNotificationPeriod}"></td>

<td>host_notification_period</td><td><input style="width:280px" type="textbox" name="HostNotificationPeriod"  {$HostNotificationPeriodACL} value="{$HostNotificationPeriod}"></td></tr>

<tr><td>service_notification_commands</td><td><input style="width:280px" type="textbox" name="ServiceNotificationCommands" {$ServiceNotificationCommands} value="{$ServiceNotificationCommands}"></td>

<td>host_notification_commands</td><td><input style="width:280px" type="textbox" name="HostNotificationCommands" {$HostNotificationCommandsACL} value="{$HostNotificationCommands}"></td></tr>

</table>

<hr>
<h2><input type="checkbox" name="nagiosAuth" value="B "{$nagiosAuth} >{t}Nagios authorisation{/t}</h2>

<input type="checkbox" name="AuthorizedSystemInformation" {$AuthorizedSystemInformationACL} value="{$AuthorizedSystemInformation}" {$AuthorizedSystemInformation}>authorized_for_system_information<br />

<input type="checkbox" name="AuthorizedConfigurationInformation" {$AuthorizedConfigurationInformationACL} value="{$AuthorizedConfigurationInformation}" {$AuthorizedConfigurationInformation}>authorized_for_configuration_information<br />

<input type="checkbox" name="AuthorizedSystemCommands" {$AuthorizedSystemCommandsACL} value="{$AuthorizedSystemCommands}" {$AuthorizedSystemCommands}>authorized_for_system_commands<br />

<input type="checkbox" name="AuthorizedAllServices" {$AuthorizedAllServicesACL} value="{$AuthorizedAllServices}" {$AuthorizedAllServices}>authorized_for_all_services<br />

<input type="checkbox" name="AuthorizedAllHosts" {$AuthorizedAllHostsACL} value="{$AuthorizedAllHosts}" {$AuthorizedAllHosts}>authorized_for_all_hosts<br />

<input type="checkbox" name="AuthorizedAllServiceCommands" {$AuthorizedAllServiceCommandsACL} value="{$AuthorizedAllServiceCommands}" {$AuthorizedAllServiceCommands}>authorized_for_all_service_commands<br />

<input type="checkbox" name="AuthorizedAllHostCommands" {$AuthorizedAllHostCommandsACL} value="{$AuthorizedAllHostCommands}" {$AuthorizedAllHostCommands}>authorized_for_all_host_commands<br />

