<h1>{t}Setup continued...{/t}</h1>

<p>
 {t}Now we're going include your LDAP server and create an initial configuration. After you've entered the server URI below, a quick check is performed if required LDAP schemas are in place. Samba versions are autodetected by the installed objectclasses. Details on how your LDAP tree is organized will be asked later on.{/t}
</p>

<p class="seperator">&nbsp;</p>
<br>

{t}Please enter the server URI to allow the setup to connect your LDAP server (Example: ldap://your.server:389).{/t}
<br>
<br>
URI&nbsp;<input type='text' name='uri' maxlength='80' size='40' value='{$uri}'>

<p class="plugbottom">
 <input type="hidden" name="next" value="4">
 <input type='submit' name='back' value='{t}Back{/t}'>
 <input type='submit' name='continue3' value='{t}Continue{/t}'>
</p>
