<table summary="" style="width:100%">
 <tr>
 
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
  
   <input type=checkbox name="goKrbServer" value="1" {$goKrbServer} {$goKrbServerACL} onclick="changeState('goKrbAdmin'); changeState('goKrbPassword'); changeState('goKrbRealm');"> <b>{t}Kerberos kadmin access{/t}</b>
  
   <table summary="">
    <tr>
	 <td>{t}Kerberos Realm{/t}</td>
	 <td><input name="goKrbRealm" id="goKrbRealm" size=30 maxlength=60 {$goKrbRealmACL} value="{$goKrbRealm}" {$goKrbServerState}></td>
	</tr>
    <tr>
	 <td>{t}Admin user{/t}</td>
	 <td><input name="goKrbAdmin" id="goKrbAdmin" size=30 maxlength=60 {$goKrbAdminACL} value="{$goKrbAdmin}" {$goKrbServerState}></td>
	</tr>
    <tr>
	 <td>{t}Password{/t}</td>
	 <td><input type=password name="goKrbPassword" id="goKrbPassword" size=30 maxlength=60 {$goKrbPasswordACL} value="{$goKrbPassword}" {$goKrbServerState}></td>
	</tr>
   </table>

   <p class="seperator">&nbsp;</p>
   <br>

   <input type=checkbox name="goFaxServer" value="1" {$goFaxServer} {$goFaxServerACL} onclick="changeState('goFaxAdmin'); changeState('goFaxPassword');"> <b>{t}FAX database{/t}</b>
  
   <table summary="">
    <tr>
	 <td>{t}FAX DB user{/t}</td>
	 <td><input name="goFaxAdmin" size=30 maxlength=60 {$goFaxAdminACL} id="goFaxAdmin" value="{$goFaxAdmin}" {$goFaxServerState}></td>
	</tr>
    <tr>
	 <td>{t}Password{/t}</td>
	 <td><input type=password name="goFaxPassword" id="goFaxPassword" size=30 maxlength=60 {$goFaxPasswordACL}   value="{$goFaxPassword}" {$goFaxServerState}></td>
	</tr>
   </table>

   <p class="seperator">&nbsp;</p>
   <br>

   <input type=checkbox name="goFonServer" value="1" {$goFonServer} {$goFonServerACL} onclick="changeState('goFonAdmin'); changeState('goFonPassword'); changeState('goFonCountryCode'); changeState('goFonAreaCode');"> <b>{t}Asterisk management{/t}</b>
  
   <table summary="">
    <tr>
	 <td>{t}Asterisk DB user{/t}</td>
	 <td><input name="goFonAdmin" size=30 maxlength=60 id="goFonAdmin" {$goFonServerACL} value="{$goFonAdmin}" {$goFonServerState}></td>
	</tr>
    <tr>
	 <td>{t}Password{/t}</td>
	 <td><input type=password name="goFonPassword" id="goFonPassword" size=30 maxlength=60 {$goFonPasswordACL}   value="{$goFonPassword}" {$goFonServerState}></td>
	</tr>
    <tr>
	 <td>{t}Country dial prefix{/t}</td>
	 <td><input name="goFonCountryCode" size=10 maxlength=30 id="goFonCountryCode" {$goFonCountryCodeACL}   value="{$goFonCountryCode}" {$goFonServerState}></td>
	</tr>
    <tr>
	 <td>{t}Local dial prefix{/t}</td>
	 <td><input name="goFonAreaCode" size=10 maxlength=30 id="goFonAreaCode" {$goFonServerState} {$goFonAreaCodeACL}   value="{$goFonAreaCode}"></td>
	</tr>
   </table>

  
  </td>
  
  
  <td style="vertical-align:top">

   <input type=checkbox name="goImapServer" value="1" {$goImapServer} {$goImapServerACL} onclick="changeState('goImapName'); changeState('goImapConnect');
   changeState('goImapAdmin'); changeState('goImapPassword'); changeState('goImapSievePort');"> <b>{t}IMAP admin access{/t}</b>
  
   <table summary="">
    <tr>
	 <td>{t}Server identifier{/t}</td>
	 <td><input name="goImapName" id="goImapName" size=40 maxlength=60 {$goImapNameACL} value="{$goImapName}" {$goImapServerState}></td>
	</tr>
    <tr>
	 <td>{t}Connect URL{/t}</td>
	 <td><input name="goImapConnect" id="goImapConnect" size=40 maxlength=60 {$goImapConnectACL} value="{$goImapConnect}" {$goImapServerState}></td>
	</tr>
    <tr>
	 <td>{t}Admin user{/t}</td>
	 <td><input name="goImapAdmin" id="goImapAdmin" size=30 maxlength=60 {$goImapAdminACL} value="{$goImapAdmin}" {$goImapServerState}></td>
	</tr>
    <tr>
	 <td>{t}Password{/t}</td>
	 <td><input type=password name="goImapPassword" id="goImapPassword" size=30 maxlength=60 {$goImapPasswordACL}
	 value="{$goImapPassword}" {$goImapServerState}></td>
	</tr>
    <tr>
	 <td>{t}Sieve port{/t}</td>
	 <td><input name="goImapSievePort" id="goImapSievePort" size=10 maxlength=30 {$goImapSievePortACL}
value="{$goImapSievePort}" {$goImapServerState}></td>
	</tr>
   </table>

   <p class="seperator">&nbsp;</p>
   <br>

   <input type=checkbox name="goLogDBServer" value="1" {$goLogDBServer} {$goLogDBServerACL} onclick="changeState('goLogAdmin');changeState('goLogPassword');"> <b>{t}Logging database{/t}</b>
  
   <table summary="">
    <tr>
	 <td>{t}Logging DB user{/t}</td>
	 <td><input name="goLogAdmin" id="goLogAdmin" size=30 maxlength=60 {$goLogAdminACL}
value="{$goLogAdmin}" {$goLogDBServerState}></td>
	</tr>
    <tr>
	 <td>{t}Password{/t}</td>
	 <td><input type=password name="goLogPassword" id="goLogPassword" size=30 maxlength=60 {$goLogPasswordACL}
value="{$goLogPassword}" {$goLogDBServerState}></td>
	</tr>
   </table>
  
  </td>
  
 </tr>
</table>

<input type="hidden" name="dbtab">
