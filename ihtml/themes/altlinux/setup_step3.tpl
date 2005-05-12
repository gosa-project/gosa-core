<h1>{t}Setup continued...{/t}</h1>

<p>
 {t}Now we're going include your LDAP server and create an initial configuration. After you've entered the server URI below, a quick check is performed if required LDAP schemas are in place. Samba versions are autodetected by the installed objectclasses. Details on how your LDAP tree is organized will be asked later on.{/t}
</p>

<b>{t}Please enter the server URI{/t}</b>
<br>
<input type='text' name='uri' maxlength='80' size='40' value='{$uri}'>

<div style="width:99%; text-align:right;">
 <input type='submit' name='continue3' value='{t}Continue{/t}' {$mode}>
</div>
