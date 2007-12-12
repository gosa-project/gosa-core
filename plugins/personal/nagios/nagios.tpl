<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2>
    <img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Nagios Account{/t}</h2>
   <table summary="">
    <tr>
     <td><label for="NagiosAlias">{t}Alias{/t}</label>{$must}</td>
     <td>
{render acl=$NagiosAliasACL}
      <input id="NagiosAlias" name="NagiosAlias" size=25 maxlength=65 value="{$NagiosAlias}">
{/render}
     </td>
    </tr>

    <tr>
     <td><label for="NagiosMail">{t}Mail address{/t}</label>{$must}</td>
     <td>
{render acl=$NagiosMailACL}
      <input id="NagiosMail" name="NagiosMail" size=25 maxlength=65 value="{$NagiosMail}">
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="HostNotificationPeriod">{t}Host notification period{/t}</label>{$must}
     </td>
     <td>
{render acl=$HostNotificationPeriodACL}
      <select name="HostNotificationPeriod" id="HostNotificationPeriod">
       {html_options options=$HostNotificationPeriodValues values=$HostNotificationPeriodValues selected=$HostNotificationPeriod }
      </select>
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="ServiceNotificationPeriod">{t}Service notification period{/t}</label>{$must}
     </td>
     <td>
{render acl=$ServiceNotificationPeriodACL}
      <select name="ServiceNotificationPeriod" id="ServiceNotificationPeriod">
       {html_options options=$ServiceNotificationPeriodValues values=$ServiceNotificationPeriodValues selected=$ServiceNotificationPeriod}
      </select>
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="ServiceNotificationOptions">{t}Service notification options{/t}</label>{$must}
     </td>
     <td>
{render acl=$ServiceNotificationOptionsACL}
      <select name="ServiceNotificationOptions" id="ServiceNotificationOptions">
       {html_options options=$ServiceNotificationOptionsValues values=$ServiceNotificationOptionsValues selected=$ServiceNotificationOptions}
      </select>
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="HostNotificationOptions">{t}Host notification options{/t}</label>{$must}
     </td>
     <td>
{render acl=$HostNotificationOptionsACL}
      <select name="HostNotificationOptions" id="HostNotificationOptions">
       {html_options options=$HostNotificationOptionsValues values=$HostNotificationOptionsValues selected=$HostNotificationOptions }
      </select>
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="NagiosPager">{t}Pager{/t}</label>
     </td>
     <td>
{render acl=$NagiosPagerACL}
      <input id="NagiosPager" name="NagiosPager" size=25 maxlength=65 value="{$NagiosPager}">
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="ServiceNotificationCommands">{t}Service notification commands{/t}</label>
     </td>
     <td>
{render acl=$ServiceNotificationCommandsACL}
      <input id="ServiceNotificationCommands" disabled name="ServiceNotificationCommands" size=25 maxlength=65  value="{$ServiceNotificationCommands}">
{/render}
     </td>
    </tr>
    <tr>
     <td>
      <label for="HostNotificationCommands">{t}Host notification commands{/t}</label>
     </td>
     <td>
{render acl=$HostNotificationCommandsACL}
      <input id="HostNotificationCommands" disabled name="HostNotificationCommands" size=25 maxlength=65  value="{$HostNotificationCommands}">
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="width:100%; vertical-align:top;">
   <h2> 
    <img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Nagios authentification{/t}
   </h2>
   <table summary="">
    <tr>
     <td>
{render acl=$AuthorizedSystemInformationACL}
      <input type="checkbox" name="AuthorizedSystemInformation" value="1" {$AuthorizedSystemInformationCHK}>{t}view system informations{/t}
{/render}
     </td>
    </tr>
 
    <tr>
     <td>
{render acl=$AuthorizedConfigurationInformationACL}
      <input type="checkbox" name="AuthorizedConfigurationInformation" value="1" 
       {$AuthorizedConfigurationInformationCHK}>{t}view configuration information{/t}
{/render}
     </td>
    </tr>
 	
    <tr>
     <td>
{render acl=$AuthorizedSystemCommandsACL}
      <input type="checkbox" name="AuthorizedSystemCommands" value="1" 
       {$AuthorizedSystemCommandsCHK}>{t}trigger system commands{/t}
{/render}
     </td>
    </tr>
 	
    <tr>
     <td>
{render acl=$AuthorizedAllServicesACL}
      <input type="checkbox" name="AuthorizedAllServices" value="1" 
       {$AuthorizedAllServicesCHK}>{t}view all services{/t}
{/render}
     </td>
    </tr>
 	
    <tr>
     <td>
{render acl=$AuthorizedAllHostsACL}
      <input type="checkbox" name="AuthorizedAllHosts" value="1" 
       {$AuthorizedAllHostsCHK}>{t}view all hosts{/t}
{/render}
     </td> 
    </tr>
 	
    <tr>
     <td>
{render acl=$AuthorizedAllServiceCommandsACL}
      <input type="checkbox" name="AuthorizedAllServiceCommands" value="1" 
       {$AuthorizedAllServiceCommandsCHK}>{t}trigger all service commands{/t}
{/render}
     </td>
    </tr>
 	
    <tr>
     <td>
{render acl=$AuthorizedAllHostCommandsACL}
      <input type="checkbox" name="AuthorizedAllHostCommands" value="1" 
       {$AuthorizedAllHostCommandsCHK}>{t}trigger all host commands{/t}
{/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<input type="hidden" name="nagiosTab" value="nagiosTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('NagiosAlias');
  -->
</script>

