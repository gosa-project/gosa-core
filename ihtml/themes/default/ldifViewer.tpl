<div class="ldap-entry-wrapper">
  <h3>{t}Raw LDAP entry{/t}</h3>

  <hr class="divider">

  <div class="ldap-entry card-content-scroll">
    {if $success}
      <pre>{$ldif}</pre>
    {else}
      <p>{msgPool type=ldapError err=$error}</p>
    {/if}
  </div>

  <div class="plugin-actions card-action">
      <button class="btn-small primary" name='cancelLdifViewer'>{msgPool type='cancelButton'}</button>
  </div>
</div>
