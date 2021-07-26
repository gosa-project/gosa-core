<h3>{t}Schema specific settings{/t}</h3>

{if !$checkFailed}
 <p class="green-text">{t}Schema check succeeded{/t}</p>
{else}
 <img alt='' src='images/small_warning.png' class='center'>
 <p class="red-text">{t}Schema check failed{/t}</p>

 {if $ocCount == 0}
  <p>
   {t}Could not read any schema information, all checks skipped. Adjust your LDAP ACLs.{/t}
   {if !$database_initialised}
    {t}It seems that your LDAP database wasn't initialized yet. This maybe the reason, why GOsa can't read your schema configuration!{/t}
   {/if}
  </p>
 {else}
  {$message}
 {/if}
{/if}


<input type='hidden' value='1' name='step7_posted'>
