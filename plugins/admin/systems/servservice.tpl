{t}Useful lheadline here.{/t}
<p class="seperator">
&nbsp;
</p>
<table style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;" width="50%">

   <input type=checkbox name="goNfsServer" value="1" {$goNfsServer} {$goNfsServerACL}  
			onchange="changeState('goExportEntry');
			changeState('NewNfsExportId');
			changeState('DelNfsEntId');
			changeState('NewNfsAddId');"> 

		<b>{t}Nfs Export{/t}</b>
   <table>
    <tr>
     <td>{t}NFS Exports{/t}</td>
     <td>
		<select style="width:350px;" id="goExportEntry" name="goExportEntry" {$goNfsServerState} size=4 multiple {$trustmodeACL} {$trusthide}>
    		{html_options values=$goExportEntry output=$goExportEntry}
	    </select>
	<br>	
		<input type="text" 		value="enabled" 		name="NewNfsExport" {$goNfsServerState} id="NewNfsExportId"> 
		<input type="submit" 	value="{t}Add{/t}"	 	name="NewNfsAdd" 	{$goNfsServerState} id="NewNfsAddId"> 
		<input type="submit" 	value="{t}Delete{/t}"  	name="DelNfsEnt" 	{$goNfsServerState} id="DelNfsEntId">
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
   <table>
    <tr>
     <td>{t}NFS Exports{/t}</td>
     <td>
        <select style="width:350px;" id="goTimeEntry" name="goTimeSource" {$goNtpServerState} size=4 multiple {$trustmodeACL} {$trusthide}>
            {html_options values=$goTimeSource output=$goTimeSource}
        </select>
    <br>
        <input type="text"      value="enabled"         name="NewNTPExport" {$goNtpServerState} id="NewNTPExportId">
        <input type="submit"    value="{t}Add{/t}"      name="NewNTPAdd"    {$goNtpServerState} id="NewNTPAddId">
        <input type="submit"    value="{t}Delete{/t}"   name="DelNTPEnt"    {$goNtpServerState} id="DelNTPEntId">
    </td>
	</tr>
   </table>



  </td>
  <td width="50%">

  <input type=checkbox name="goLdapServer" value="1" {$goLdapServer} {$goLdapServerACL}
            onchange="changeState('goLdapBaseId');">

     <b>{t}LDAP Service{/t}</b>

		<input type="text"      value="enabled"         name="goLdapBase" {$goLdapServerState} id="goLdapBaseId">

	<p class="seperator">&nbsp;</p>
    <br>

	 <input type=checkbox name="goTerminalServer" value="1" {$goTerminalServer} {$goTerminalServerACL}
            onchange="changeState('FontPathId');
            		  changeState('Temporariid');">

     <b>{t}Terminal Service{/t}</b>

  <table>
    <tr>
	 <td> Font path </td>
	 <td> <input type="text"      value="enabled"         name="goFontPath" {$goTerminalServerState} id="FontPathId"></td>
	</tr>
	<tr>
	 <td>Temporary disable login </td>
	 <td><input type="checkbox"  value="1"         		name="goXdmcpIsEnabled" {$goTerminalServerState} id="Temporariid"> </td>
    </tr>
   </table>



	
   	<p class="seperator">&nbsp;</p>
    <br>

     <input type=checkbox name="goSyslogServer" value="1" {$goSyslogServer} {$goSyslogServerACL}>

     <b>{t}Syslog Service{/t}</b>

   	<p class="seperator">&nbsp;</p>
    <br>
	

     <input type=checkbox name="goCupsServer" value="1" {$goCupsServer} {$goCupsServerACL}>

     <b>{t} Print Service{/t}</b>
	</td>	
 </tr>
</table>
