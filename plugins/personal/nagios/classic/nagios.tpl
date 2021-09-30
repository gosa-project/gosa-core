<table style='width:100%; ' summary="{t}Nagios Account{/t}">

 <tr>
  <td style='width:50%; '>


   <h3>{t}Nagios Account{/t}</h3>
   <table summary="{t}Nagios Account{/t}">
    <tr>
     <td><label for="NagiosAlias">{t}Alias{/t}</label>{$must}</td>
     <td>
{render acl=$NagiosAliasACL}
      <input type='text' id="NagiosAlias" name="NagiosAlias" size=25 maxlength=65 value="{$NagiosAlias}">
{/render}
     </td>
    </tr>

    <tr>
     <td><label for="NagiosMail">{t}Mail address{/t}</label>{$must}</td>
     <td>
{render acl=$NagiosMailACL}
      <input type='text' id="NagiosMail" name="NagiosMail" size=25 maxlength=65 value="{$NagiosMail}">
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="HostNotificationPeriod">{t}Host notification period{/t}</label>{$must}
     </td>
     <td>
{render acl=$HostNotificationPeriodACL}
      <select name="HostNotificationPeriod" id="HostNotificationPeriod" size=1>
       {html_options options=$HostNotificationPeriodValues values=$HostNotificationPeriodValues selected=$HostNotificationPeriod}
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
      <select name="ServiceNotificationPeriod" id="ServiceNotificationPeriod" size=1>
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
      <select name="ServiceNotificationOptions" id="ServiceNotificationOptions" size=1>
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
      <select name="HostNotificationOptions" id="HostNotificationOptions" size=1>
       {html_options options=$HostNotificationOptionsValues values=$HostNotificationOptionsValues selected=$HostNotificationOptions}
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
      <input type='text' id="NagiosPager" name="NagiosPager" size=25 maxlength=65 value="{$NagiosPager}">
{/render}
     </td>
    </tr>

    <tr>
     <td>
      <label for="ServiceNotificationCommands">{t}Service notification commands{/t}</label>
     </td>
     <td>
{render acl=$ServiceNotificationCommandsACL}
      <input type='text' id="ServiceNotificationCommands" disabled name="ServiceNotificationCommands" size=25 maxlength=65  value="{$ServiceNotificationCommands}">
{/render}
     </td>
    </tr>
    <tr>
     <td>
      <label for="HostNotificationCommands">{t}Host notification commands{/t}</label>
     </td>
     <td>
{render acl=$HostNotificationCommandsACL}
      <input type='text' id="HostNotificationCommands" disabled name="HostNotificationCommands" size=25 maxlength=65  value="{$HostNotificationCommands}">
{/render}
     </td>
    </tr>
   </table>

  </td>
  <td class='left-border'>

   &nbsp;
  </td>
  <td style='width:100%; '>


   <h3>&nbsp;{t}Nagios authentication{/t}</h3>
   <table summary="{t}Nagios account{/t}">
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

