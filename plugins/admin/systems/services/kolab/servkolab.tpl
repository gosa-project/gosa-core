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
{render acl=$postfixmydomainACL}
			<input type="text" name="postfix_mydomain" value="{$postfix_mydomain}">
{/render}
		</td>
	</tr>
	<tr>
		<td>{t}Postfix mydestination{/t}
		</td>
		<td>
{render acl=$postfixmydestinationACL}
			<input type="text" name="postfix_mydestination" value="{$postfix_mydestination}">
{/render}
		</td>
	</tr>
	<tr>
		<td>{t}Cyrus admins{/t}
		</td>
		<td>
{render acl=$cyrusadminsACL}
			<input type="text" name="cyrus_admins" value="{$cyrus_admins}">
{/render}
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
{render acl=$cyruspop3ACL}
			<input id="cyrus_pop3" name="cyrus_pop3" value="1" type="checkbox" {$cyrus_pop3Check}>
{/render}
		</td>
		<td>
			<LABEL for="cyrus_pop3">{t}POP3 service{/t}</LABEL>
		</td>
	</tr>
	<tr>
        <td>
{render acl=$cyruspop3sACL}
            <input id="cyrus_pop3s" name="cyrus_pop3s" value="1" type="checkbox" {$cyrus_pop3sCheck}>
{/render}
        </td>
		<td>
			<LABEL for="cyrus_pop3s">{t}POP3/SSL service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$cyrusimapACL}
            <input id="cyrus_imap" name="cyrus_imap" value="1" type="checkbox" {$cyrus_imapCheck}>
{/render}
        </td>
        <td>
			<LABEL for="cyrus_imap">{t}IMAP service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$cyrusimapsACL}
            <input id="cyrus_imaps" name="cyrus_imaps" value="1" type="checkbox" {$cyrus_imapsCheck}>
{/render}
        </td>
        <td>
			<LABEL for="cyrus_imaps">{t}IMAP/SSL service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$cyrussieveACL}
            <input id="cyrus_sieve" name="cyrus_sieve" value="1" type="checkbox" {$cyrus_sieveCheck}>
{/render}
        </td>
        <td>
			<LABEL for="cyrus_sieve">{t}Sieve service{/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$proftpdftpACL}
            <input id="proftpd_ftp" name="proftpd_ftp" value="1" type="checkbox" {$proftpd_ftpCheck}>
{/render}
        </td>	
        <td>
			<LABEL for="proftpd_ftp">{t}FTP FreeBusy service (legacy, not interoperable with Kolab2 FreeBusy){/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$apachehttpACL}
            <input id="apache_http" name="apache_http" value="1" type="checkbox" {$apache_httpCheck}>
{/render}
        </td>
        <td>
			<LABEL for="apache_http">{t}HTTP FreeBusy service (legacy){/t}</LABEL>
        </td>
    </tr>
    <tr>
        <td>
{render acl=$postfixenablevirusscanACL}
			<input id="postfix_enable_virus_scan" name="postfix_enable_virus_scan" value="1" type="checkbox" {$postfix_enable_virus_scanCheck}>
{/render}
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
{render acl=$cyrusquotawarnACL}
			{$quotastr}
{/render}
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
{render acl=$apacheallowunauthenticatedfbACL}
            <input name="apache_allow_unauthenticated_fb" value="1" type="checkbox" {$apache_allow_unauthenticated_fbCheck}> {t}Allow unauthenticated downloading of Free/Busy information{/t}
{/render}
        </td>
	 </tr>
	 <tr>
        <td>
{render acl=$kolabFreeBusyFutureACL}
		{$fbfuture}
{/render}
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
{render acl=$postfixmynetworksACL}
        <input id="postfix_mynetworks" name="postfix_mynetworks" size="60" maxlength="220" value="{$postfix_mynetworks}" type="text">
{/render}
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
{render acl=$postfixrelayhostACL}
            <input id="RelayMxSupport" name="RelayMxSupport" value="1" type="checkbox" {$RelayMxSupportCheck}>
{/render}
			<LABEL for="RelayMxSupport">{t}Enable MX lookup for relayhost{/t}</LABEL>
		</td>
     </tr>
	<tr>	
		<td>
		    <LABEL for="postfix_relayhost">{t}Host used to relay mails{/t}</LABEL>&nbsp;
{render acl=$postfixrelayhostACL}
			<input id="postfix_relayhost" name="postfix_relayhost" size="35" maxlength="120" value="{$postfix_relayhost}" type="text">
{/render}
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
{render acl=$postfixallowunauthenticatedACL}
            <input id="postfix_allow_unauthenticated" name="postfix_allow_unauthenticated" value="1" type="checkbox" {$postfix_allow_unauthenticatedCheck}>
{/render}
        	<LABEL for="postfix_allow_unauthenticated">{t}Accept mail from other domains over non-authenticated SMTP{/t}</LABEL>
		</td>
     </tr>
   </table>


  </td>
 </tr>
</table>

<input type="hidden" name="kolabtab">

<p class="seperator">&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>

