<table style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;" width="50%">

   <table>
    <tr>
	 <td colspan="2"><b>{t}Services{/t}</b></td>
	</tr>
	<tr>
		<td>
			<input name="cyrus-pop3" value="1" type="checkbox" >
		</td>
		<td>
			POP3 Service  	
		</td>
	</tr>
	<tr>
        <td>
            <input name="cyrus-pop3s" value="1" type="checkbox" >
        </td>
		<td>
			POP3/SSL service (TCP port 995) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus-imap" value="1" type="checkbox" >
        </td>
        <td>
			IMAP Service 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus-imaps" value="1" type="checkbox" >
        </td>
        <td>
			IMAP/SSL Service (TCP port 993) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus-sieve" value="1" type="checkbox" >
        </td>
        <td>
			Sieve service (TCP port 2000) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="proftpd-ftp" value="1" type="checkbox" >
        </td>
        <td>
			FTP FreeBusy Service (Legacy, not interoperable with Kolab2 FreeBusy) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox" >
        </td>
        <td>
			HTTP FreeBusy Service (Legacy) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox" >
        </td>
        <td>
			Amavis Email Scanning (Virus/Spam)
		</td>
	</tr>
   </table>




<p class="seperator">&nbsp;</p>
<br>

	
   <table>
	<tr>
		<td colspan="2" >
			<b>{t}Kolab Hosts{/t}</b>
		</td>
	</tr>
    <tr>
     <td>
        <select style="width:350px;" id="goExportEntry" name="goExportEntry" {$goNfsServerState} size=6 multiple {$trustmodeACL} {$trusthide}>
            {html_options values=$goExportEntry output=$goExportEntry}
        </select>
    <br>
        <input type="text"      name="NewNfsExport" {$goNfsServerState} id="NewNfsExportId">
        <input type="submit"    value="{t}Add{/t}"      name="NewNfsAdd"    {$goNfsServerState} id="NewNfsAddId">
        <input type="submit"    value="{t}Delete{/t}"   name="DelNfsEnt"    {$goNfsServerState} id="DelNfsEntId">
    </td>
	</tr>
	</table>	


  </td>
  <td style="vertical-align:top" width="50%">


   <table>
    <tr>
		<td colspan="2"> 
			<b>{t}Quota settings{/t}</b>
		</td>
	</tr>
	<tr>
		<td>
			<input name="" value="" type="text" maxlength="2" size="3">%
		</td>
		<td>
			Percentage to warn users
		</td>
	</tr>
   </table>
  

<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td colspan="2">
            <b>{t}Free/Busy settings{/t}</b>
        </td>
    </tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox">
        </td>
        <td>
            Allow unauthenticated downloading of Free/Busy information 
        </td>
	 </tr>
	 <tr>
        <td>
        	<input name="" value="" type="text" maxlength="2" size="3">
		</td>
        <td>
			When creating free/busy lists, include data from x days in the past
		</td>
     </tr>

   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td colspan="2">
        	<b>{t}Privileged Networks{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input name="" value="" type="text">
        </td>
        <td>
			Networks allowed to relay and send mail through unauthenticated SMTP connections to the Kolab server (comma separated networks in x.x.x.x/y format)
		</td>
     </tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td colspan="2">
        	<b>{t}SMTP "smarthost/relayhost"{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox" >
        </td>
        <td>
        	Smarthost to use to send outgoing mail (host.domain.tld). Leave empty for no relayhost.
			Enable MX lookup for relayhost (if in doubt, leave it off)
		</td>
     </tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td colspan="2">
        	<b>{t}Accept Internet Mai{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox" >
        </td>
        <td>
        	Accept mail from other domains over non-authenticated SMTP. This must be enabled if you want to use the Kolab server to receive mail from other internet domains.
		</td>
     </tr>
   </table>







  </td>
 </tr>
</table>
