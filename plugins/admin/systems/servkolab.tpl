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
			{t}POP3 service{/t}
		</td>
	</tr>
	<tr>
        <td>
            <input name="cyrus_pop3s" value="1" type="checkbox" {$cyrus_pop3sCheck}>
        </td>
		<td>
			{t}POP3/SSL service{/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_imap" value="1" type="checkbox" {$cyrus_imapCheck}>
        </td>
        <td>
			{t}IMAP service{/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_imaps" value="1" type="checkbox" {$cyrus_imapsCheck}>
        </td>
        <td>
			{t}IMAP/SSL service{/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="cyrus_sieve" value="1" type="checkbox" {$cyrus_sieveCheck}>
        </td>
        <td>
			{t}Sieve service{/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="proftpd_ftp" value="1" type="checkbox" {$proftpd_ftpCheck}>
        </td>	
        <td>
			{t}FTP FreeBusy service (legacy, not interoperable with Kolab2 FreeBusy){/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="apache_http" value="1" type="checkbox" {$apache_httpCheck}>
        </td>
        <td>
			{t}HTTP FreeBusy service (legacy){/t}
        </td>
    </tr>
    <tr>
        <td>
            <input name="postfix_enable_virus_scan" value="1" type="checkbox" {$postfix_enable_virus_scanCheck}>
        </td>
        <td>
			{t}Amavis email scanning (virus/spam){/t}
		</td>
	</tr>
   </table>

   <p class="seperator">&nbsp;</p>
   <br>

   <table>
    <tr>
		<td colspan="2"> 
			<b>{t}Quota settings{/t}</b>
		</td>
	</tr>
	<tr>
		<td>
			<input name="cyrus_quotawarn" value="{$cyrus_quotawarn}" type="text" maxlength="3" size="4">%
		</td>
		<td>
			Percentage to warn users
		</td>
	</tr>
   </table>
  
  </td>
  <td style="vertical-align:top" width="50%">

   <table>
    <tr>
        <td colspan="2">
            <b>{t}Free/Busy settings{/t}</b>
        </td>
    </tr>
    <tr>
        <td>
            <input name="apache_allow_unauthenticated_fb" value="1" type="checkbox" {$apache_allow_unauthenticated_fbCheck}> {t}Allow unauthenticated downloading of Free/Busy information{/t}
        </td>
	 </tr>
	 <tr>
        <td>
        	<input name="kolabFreeBusyFuture" value="{$kolabFreeBusyFuture}" type="text" maxlength="3" size="4">
			When creating free/busy lists, include data from x days in the past
		</td>
     </tr>
   </table>

<p class="seperator">&nbsp;</p>
<br>
   <table>
    <tr>
        <td>
        	<b>{t}SMTP privileged networks{/t}</b>
		</td>
    </tr>
    <tr>
	  <td>
	    {t}Hosts/networks allowed to relay{/t}&nbsp;
        <input name="postfix_mynetworks" size="35" maxlength="120" value="{$postfix_mynetworks}" type="text">
	</td>
     </tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td>
        	<b>{t}SMTP smarthost/relayhost{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input name="RelayMxSupport" value="1" type="checkbox" {$RelayMxSupportCheck}>
			{t}Enable MX lookup for relayhost{/t}
		</td>
     </tr>
	<tr>	
		<td>
		    {t}Host used to relay mails{/t}&nbsp;
			<input name="postfix_relayhost" size="35" maxlength="120" value="{$postfix_relayhost}" type="text">
		</td>
	</tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table>
    <tr>
        <td>
        	<b>{t}Accept Internet Mail{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input name="postfix_allow_unauthenticated" value="1" type="checkbox" {$postfix_allow_unauthenticatedCheck}>
        	{t}Accept mail from other domains over non-authenticated SMTP{/t}
		</td>
     </tr>
   </table>


  </td>
 </tr>
</table>
