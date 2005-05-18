<table style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;" width="50%">

   <table>
    <tr>
	 <td colspan="2"><b>{t}Services{/t}</b></td>
	</tr>
	<tr>
		<td>
			<input name="cyrus_pop3" value="1" type="checkbox" {$cyrus_pop3Check}>
		</td>
		<td>
			POP3 Service  	
		</td>
	</tr>
	<tr>
        <td>
            <input name="cyrus_pop3s" value="1" type="checkbox" {$cyrus_pop3sCheck}>
        </td>
		<td>
			POP3/SSL service (TCP port 995) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_imap" value="1" type="checkbox" {$cyrus_imapCheck}>
        </td>
        <td>
			IMAP Service 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_imaps" value="1" type="checkbox" {$cyrus_imapsCheck}>
        </td>
        <td>
			IMAP/SSL Service (TCP port 993) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_sieve" value="1" type="checkbox" {$cyrus_sieveCheck}>
        </td>
        <td>
			Sieve service (TCP port 2000) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="proftpd_ftp" value="1" type="checkbox" {$proftpd_ftpCheck}>
        </td>	
        <td>
			FTP FreeBusy Service (Legacy, not interoperable with Kolab2 FreeBusy) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="apache_http" value="1" type="checkbox" {$apache_httpCheck}>
        </td>
        <td>
			HTTP FreeBusy Service (Legacy) 	
        </td>
    </tr>
    <tr>
        <td>
            <input name="postfix_enable_virus_scan" value="1" type="checkbox" {$postfix_enable_virus_scanCheck}>
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
        <select style="width:350px;" size="6"  name="kolabHost">
            {html_options values=$kolabHost output=$kolabHost}
        </select>
    <br>
        <input type="text"      name="NewkolabHost">
        <input type="submit"    value="{t}Add{/t}"      name="NewHost">
        <input type="submit"    value="{t}Delete{/t}"   name="DelHost">
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
			<input name="cyrus_quotawarn" value="{$cyrus_quotawarn}" type="text" maxlength="2" size="3">%
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
            <input name="apache_allow_unauthenticated_fb" value="1" type="checkbox" {$apache_allow_unauthenticated_fbCheck}>
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
            <input name="postfix_mynetworks" value="{$postfix_mynetworks}" type="text">
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
			<input name="postfix_relayhost" value="{$postfix_relayhost}" type="text">
		</td>
		<td>
			Smarthost to use to send outgoing mail (host.domain.tld). Leave empty for no relayhost.
		</td>
	</tr>
    <tr>
        <td>
            <input name="" value="1" type="checkbox" >
        </td>
        <td>
			Enable MX lookup for relayhost (if in doubt, leave it off)
		</td>
     </tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td colspan="2">
        	<b>{t}Accept Internet Mail{/t}</b>
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
