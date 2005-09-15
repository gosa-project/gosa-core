<table summary="" style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;" width="50%">

   <input type=checkbox name="goShareServer" value="1" {$goShareServer} 
   	onchange="changeState('goExportEntry');
			changeState('NewNfsEditId');
			changeState('DelNfsEntId');
			changeState('NewNfsAddId');"> 

		<b>{t}Nfs Export{/t}</b>
   <table summary="">
    <tr>
     <td>
		<select style="width:350px;" id="goExportEntry" name="goExportEntry" {$goExportEntryACL} {$goShareServerState} size=4 multiple >
    		{html_options values=$goExportEntry output=$goExportEntryKeys}
			<option disabled>&nbsp;</option>
	    </select>
	<br>	
		<input type="submit" 	value="{t}Edit{/t}" 	name="NewNfsEdit" {$goShareServer} {$goExportEntryACL}  id="NewNfsEditId"> 
		<input type="submit" 	value="{t}Add{/t}"	name="NewNfsAdd"  {$goShareServer} {$goExportEntryACL}	id="NewNfsAddId"> 
		<input type="submit" 	value="{t}Delete{/t}"  	name="DelNfsEnt"  {$goShareServer} {$goExportEntryACL}	id="DelNfsEntId">
	</td>
	</tr>
   </table>
   
   <p class="seperator">&nbsp;</p>
	
	<br>

   <input type=checkbox name="goNtpServer" value="1" {$goNtpServer} {$goNtpServerACL}
            onchange="changeState('goTimeEntry');
            changeState('NewNTPExportId');
            changeState('DelNTPEntId');
            changeState('NewNTPAddId');">

        <b>{t}Time Service{/t}</b>
   <table summary="">
    <tr>
     <td>
        <select style="width:350px;" id="goTimeEntry" name="goTimeSource[]" {$goNtpServerState} {$goTimeSourceACL}  size=4 multiple>
            {html_options values=$goTimeSource output=$goTimeSource}
			<option disabled>&nbsp;</option>
        </select>
    <br>
        <input type="text" name="NewNTPExport" {$goNtpServerState} id="NewNTPExportId">
        <input type="submit"    value="{t}Add{/t}"      name="NewNTPAdd"    {$goNtpServerState} {$goTimeSourceACL} id="NewNTPAddId">
        <input type="submit"    value="{t}Delete{/t}"   name="DelNTPEnt"    {$goNtpServerState} {$goTimeSourceACL} id="DelNTPEntId">
    </td>
	</tr>
   </table>



  </td>
  <td width="50%">

  <input type=checkbox name="goLdapServer" value="1" {$goLdapServer} {$goLdapServerACL}
            onchange="changeState('goLdapBaseId');">

     <b>{t}LDAP Service{/t}</b>

		<input type="text"      value="{$goLdapBase}"     {$goLdapBaseACL}    name="goLdapBase" {$goLdapServerState} id="goLdapBaseId">

	<p class="seperator">&nbsp;</p>
    <br>

	 <input type=checkbox name="goTerminalServer" value="1" {$goTerminalServer} {$goTerminalServerACL}
            onchange="changeState('FontPathId');
            		  changeState('Temporariid');">

     <b>{t}Terminal Service{/t}</b>

  <table summary="">
	<tr>
	 <td colspan=2>
	  <input type="checkbox" value="true" name="goXdmcpIsEnabled" {$goTerminalServerState} {$goXdmcpIsEnabledACL} id="Temporariid" {$goXdmcpIsEnabled}> {t}Temporary disable login{/t}</td>
    </tr>
    <tr>
	 <td>{t}Font path{/t}</td>
	 <td> <input type="text"      value="{$goFontPath}"         name="goFontPath" {$goTerminalServerState} {$goXdmcpIsEnabledACL}  id="FontPathId"></td>
	</tr>
   </table>



	
   	<p class="seperator">&nbsp;</p>
    <br>

     <input type=checkbox name="goSyslogServer" value="1" {$goSyslogServer} {$goSyslogServerACL}>

     <b>{t}Syslog Service{/t}</b>

   	<p class="seperator">&nbsp;</p>
    <br>
	

     <input type=checkbox name="goCupsServer" value="1" {$goCupsServer} {$goCupsServerACL}>

     <b>{t}Print Service{/t}</b>
	</td>	
 </tr>
</table>

<input type="hidden" name="servicetab">
