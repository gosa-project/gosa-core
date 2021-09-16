<table summary="{t}Kolab service{/t}" style="width:100%">
 <tr>
  <td style='width:50%' class='right-border'>

   <h3>{t}Generic{/t}</h3>

   <table summary="{t}Generic settings{/t}" width="100%">
    <tr>
     <td>
      {t}Postfix mydomain{/t}&nbsp;{$must}
     </td>
     <td>
      {render acl=$postfixmydomainACL}
      <input type="text" name="postfix_mydomain" value="{$postfix_mydomain}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>
      {t}Cyrus administrators{/t}
     </td>
     <td>
      {render acl=$cyrusadminsACL}
      <input type="text" name="cyrus_admins" value="{$cyrus_admins}">
      {/render}
     </td>
    </tr>
    <tr>
     <td colspan="2">
      {t}Mail domains{/t}&nbsp;({t}Postfix mydestination{/t})&nbsp;{$must}
      {render acl=$postfixmydestinationACL}
      {$mdDiv}
      {/render}
      {render acl=$postfixmydestinationACL}
      <input size="30" type='text' name='new_domain_name' value=''>
      {/render}
      {render acl=$postfixmydestinationACL}
      <button type='submit' name='add_domain_name'>
      {msgPool type=addButton}</button>
      {/render}
     </td>
    </tr>
   </table>
   	
   <hr>

   <h3>{t}Services{/t}</h3>
   <table summary="{t}Service settings{/t}">
    <tr>
     <td>
      {render acl=$cyruspop3ACL}
      <input id="cyrus_pop3" name="cyrus_pop3" value="1" type="checkbox" {$cyrus_pop3Check}>
      {/render}
     </td>
     <td>
      <label for="cyrus_pop3">
      {t}POP3 service{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$cyruspop3sACL}
      <input id="cyrus_pop3s" name="cyrus_pop3s" value="1" type="checkbox" {$cyrus_pop3sCheck}>
      {/render}
     </td>
     <td>
      <label for="cyrus_pop3s">
      {t}POP3/SSL service{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$cyrusimapACL}
      <input id="cyrus_imap" name="cyrus_imap" value="1" type="checkbox" {$cyrus_imapCheck}>
      {/render}
     </td>
     <td>
      <label for="cyrus_imap">
      {t}IMAP service{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$cyrusimapsACL}
      <input id="cyrus_imaps" name="cyrus_imaps" value="1" type="checkbox" {$cyrus_imapsCheck}>
      {/render}
     </td>
     <td>
      <label for="cyrus_imaps">
      {t}IMAP/SSL service{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$cyrussieveACL}
      <input id="cyrus_sieve" name="cyrus_sieve" value="1" type="checkbox" {$cyrus_sieveCheck}>
      {/render}
     </td>
     <td>
      <label for="cyrus_sieve">
      {t}Sieve service{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$proftpdftpACL}
      <input id="proftpd_ftp" name="proftpd_ftp" value="1" type="checkbox" {$proftpd_ftpCheck}>
      {/render}
     </td>
     	
     <td>
      <label for="proftpd_ftp">
      {t}FTP FreeBusy service (legacy, not interoperable with Kolab2 FreeBusy){/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$apachehttpACL}
      <input id="apache_http" name="apache_http" value="1" type="checkbox" {$apache_httpCheck}>
      {/render}
     </td>
     <td>
      <label for="apache_http">
      {t}HTTP FreeBusy service (legacy){/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$postfixenablevirusscanACL}
      <input id="postfix_enable_virus_scan" name="postfix_enable_virus_scan" value="1" type="checkbox" {$postfix_enable_virus_scanCheck}>
      {/render}
     </td>
     <td>
      <label for="postfix_enable_virus_scan">
      {t}Amavis email scanning (virus/SPAM){/t}</label>
     </td>
    </tr>
   </table>

   <hr>

   <h3>{t}Quota settings{/t}</h3>
   <table summary="{t}Quota settings{/t}">
    <tr>
     <td>
      {render acl=$cyrusquotawarnACL}
      {$quotastr}
      {/render}
     </td>
    </tr>
   </table>

  </td>
  <td>

   <h3>{t}Free/Busy settings{/t}</h3>
   <table summary="{t}Free/Busy settings{/t}">
    <tr>
     <td>
      {render acl=$apacheallowunauthenticatedfbACL}
      <input name="apache_allow_unauthenticated_fb" value="1" type="checkbox" {$apache_allow_unauthenticated_fbCheck}>
       {t}Allow unauthenticated downloading of Free/Busy information{/t}
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

   <hr>

   <h3>{t}SMTP privileged networks{/t}</h3>
      <label for="postfix_mynetworks">
      {t}Hosts/networks allowed to relay{/t}</label>
      {render acl=$postfixmynetworksACL}
          {$networkDiv}
      {/render}
      {render acl=$postfixmynetworksACL}
          <input size="30" type='text' name='new_network_name' value=''>
      {/render}
      {render acl=$postfixmynetworksACL}
        <button type='submit' name='add_network_name'>{msgPool type=addButton}</button>
      {/render}

   <hr>

   <h3>{t}SMTP smart host/relay host{/t}</h3>
   <table summary="{t}SMTP smart host/relay host{/t}">
    <tr>
     <td>
      {render acl=$postfixrelayhostACL}
      <input id="RelayMxSupport" name="RelayMxSupport" value="1" type="checkbox" {$RelayMxSupportCheck}>
      {/render}
      <label for="RelayMxSupport">
      {t}Enable MX lookup for relay host{/t}</label>
     </td>
    </tr>
    <tr>
     	
     <td>
      <label for="postfix_relayhost">
      {t}Host used to relay mails{/t}</label>
      &nbsp;
      {render acl=$postfixrelayhostACL}
      <input id="postfix_relayhost" name="postfix_relayhost" size="35" maxlength="120" value="{$postfix_relayhost}" type="text">
      {/render}
     </td>
    </tr>
   </table>

   <hr>


   <h3>{t}Accept Internet Mail{/t}</h3>
   <table summary="{t}Accept Internet Mail{/t}">
    <tr>
     <td>
      {render acl=$postfixallowunauthenticatedACL}
      <input id="postfix_allow_unauthenticated" name="postfix_allow_unauthenticated" value="1" type="checkbox" {$postfix_allow_unauthenticatedCheck}>
      {/render}
      <label for="postfix_allow_unauthenticated">
      {t}Accept mail from other domains over non-authenticated SMTP{/t}</label>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<input type="hidden" name="kolabtab">
<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
