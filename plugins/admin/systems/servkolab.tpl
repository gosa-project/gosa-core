<table summary="" style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;" width="50%">
   <table summary="">
    <tr>
	 <td colspan="2"><b>{t}Generic{/t}</b></td>
	</tr>
	<tr>
		<td>{t}Postfix mydomain{/t}
		</td>
		<td>
			<input type="text" name="postfix_mydomain" value="{$postfix_mydomain}" {$postfix_mydomainACL}>
		</td>
	</tr>
	<tr>
		<td>{t}Postfix mydestination{/t}
		</td>
		<td>
			<input type="text" name="postfix_mydestination" value="{$postfix_mydestination}" {$postfix_mydestinationACL}>
		</td>
	</tr>
   </table>	
   <p class="seperator" >&nbsp;</p>
   <table summary="">
    <tr>
	 <td colspan="2"><b>{t}Services{/t}</b></td>
	</tr>
	<tr>
		<td>
			<input id="cyrus_pop3" name="cyrus_pop3" value="1" type="checkbox" {$cyrus_pop3Check} {$cyrus_pop3ACL}>
		</td>
		<td>
			<LABEL for="cyrus_pop3">{t}POP3 service{/t}</LABEL>
		</td>
	</tr>
	<tr>
        <td>
            <input id="cyrus_pop3s" name="cyrus_pop3s" value="1" type="checkbox" {$cyrus_pop3sCheck} {$cyrus_pop3sACL}>
        </td>
		<td>
			<LABEL for="cyrus_pop3s">{t}POP3/SSL service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
            <input id="cyrus_imap" name="cyrus_imap" value="1" type="checkbox" {$cyrus_imapCheck} {$cyrus_imapACL}>
        </td>
        <td>
			<LABEL for="cyrus_imap">{t}IMAP service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
            <input id="cyrus_imaps" name="cyrus_imaps" value="1" type="checkbox" {$cyrus_imapsCheck} {$cyrus_imapsACL}>
        </td>
        <td>
			<LABEL for="cyrus_imaps">{t}IMAP/SSL service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
            <input id="cyrus_sieve" name="cyrus_sieve" value="1" type="checkbox" {$cyrus_sieveCheck} {$cyrus_sieveACL}>
        </td>
        <td>
			<LABEL for="cyrus_sieve">{t}Sieve service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
            <input id="proftpd_ftp" name="proftpd_ftp" value="1" type="checkbox" {$proftpd_ftpCheck} {$proftpd_ftpACL}>
        </td>	
        <td>
			<LABEL for="proftpd_ftp">{t}FTP FreeBusy service (legacy, not interoperable with Kolab2 FreeBusy){/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
            <input id="apache_http" name="apache_http" value="1" type="checkbox" {$apache_httpCheck} {$apache_httpACL}>
        </td>
        <td>
			<LABEL for="apache_http">{t}HTTP FreeBusy service (legacy){/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
			<input id="postfix_enable_virus_scan" name="postfix_enable_virus_scan" value="1" type="checkbox" {$postfix_enable_virus_scanCheck} {$postfix_enable_virus_scanACL}>
        </td>
        <td>
			<LABEL for="postfix_enable_virus_scan">{t}Amavis email scanning (virus/spam){/t}</LABEL>
		</td>
	</tr>
   </table>

   <p class="seperator">&nbsp;</p>
   <br>

   <table summary="">
    <tr>
		<td> 
			<b>{t}Quota settings{/t}</b>
		</td>
	</tr>
	<tr>
		<td>
			{$quotastr}
		</td>
	</tr>
   </table>
  
  </td>
  <td style="vertical-align:top" width="50%">

   <table summary="">
    <tr>
        <td colspan="2">
            <b>{t}Free/Busy settings{/t}</b>
        </td>
    </tr>
    <tr>
        <td>
            <input name="apache_allow_unauthenticated_fb" value="1" type="checkbox" {$apache_allow_unauthenticated_fbCheck} {$apache_allow_unauthenticated_fbACL}> {t}Allow unauthenticated downloading of Free/Busy information{/t}
        </td>
	 </tr>
	 <tr>
        <td>
		{$fbfuture}
	</td>
     </tr>
   </table>

<p class="seperator">&nbsp;</p>
<br>
   <table summary="">
    <tr>
        <td>
        	<b>{t}SMTP privileged networks{/t}</b>
		</td>
    </tr>
    <tr>
	  <td>
	    <LABEL for="postfix_mynetworks">{t}Hosts/networks allowed to relay{/t}</LABEL><br>
        <input id="postfix_mynetworks" name="postfix_mynetworks" size="60" maxlength="220" value="{$postfix_mynetworks}" {$postfix_mynetworksACL} type="text">
		<br>
		( {t}Enter multiple values, seperated with{/t} , )
	</td>
     </tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table summary="">
    <tr>
        <td>
        	<b>{t}SMTP smarthost/relayhost{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input id="RelayMxSupport" name="RelayMxSupport" value="1" type="checkbox" {$RelayMxSupportCheck} {$postfix_relayhostACL}>
			<LABEL for="RelayMxSupport">{t}Enable MX lookup for relayhost{/t}</LABEL>
		</td>
     </tr>
	<tr>	
		<td>
		    <LABEL for="postfix_relayhost">{t}Host used to relay mails{/t}</LABEL>&nbsp;
			<input id="postfix_relayhost" name="postfix_relayhost" size="35" maxlength="120" value="{$postfix_relayhost}" {$postfix_relayhostACL} type="text">
		</td>
	</tr>
   </table>


<p class="seperator">&nbsp;</p>
<br>


   <table summary="">
    <tr>
        <td>
        	<b>{t}Accept Internet Mail{/t}</b>
		</td>
    </tr>
    <tr>
        <td>
            <input id="postfix_allow_unauthenticated" name="postfix_allow_unauthenticated" value="1" type="checkbox" {$postfix_allow_unauthenticatedCheck} {$postfix_allow_unauthenticatedACL}>
        	<LABEL for="postfix_allow_unauthenticated">{t}Accept mail from other domains over non-authenticated SMTP{/t}</LABEL>
		</td>
     </tr>
   </table>


  </td>
 </tr>
</table>

<input type="hidden" name="kolabtab">
