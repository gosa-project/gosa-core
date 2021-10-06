<h3>{t}LDAP service{/t}</h3>
{t}LDAP URI{/t}{$must} 
{render acl=$goLdapBaseACL}
<input type="text" size="80" value="{$goLdapBase}"  name="goLdapBase" id="goLdapBaseId">
{/render}

<hr>
<div class="plugin-actions">
    <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
    <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
<input type="hidden" name="goLdapServerPosted" value="1">
