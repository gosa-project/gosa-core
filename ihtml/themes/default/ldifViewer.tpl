<h3>{t}Raw LDAP entry{/t}</h3>
<hr>

{if $success}
<pre>
{$ldif}
</pre>
{else}
  <p>{msgPool type=ldapError err=$error}</p>
{/if}
<hr>
<div class="plugin-actions">
    <button name='cancelLdifViewer'>{msgPool type='cancelButton'}</button>
</div>
