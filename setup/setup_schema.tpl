<h3>{t}Schema specific settings{/t}</h3>

{if !$checkFailed}
 <font style="color:green">{t}Schema check succeeded{/t}</font>
{else}
 <img alt='' src='images/small_warning.png' class='center'>
 <font style="color:red">{t}Schema check failed{/t}</font>

 {if $ocCount == 0}
  <p>
   {t}Could not read any schema informations, all checks skipped. Adjust your ldap acls.{/t}
   {if !$database_initialised}
    {t}It seems that your ldap database wasn't initialized yet. This maybe the reason, why GOsa can't read your schema configuration!{/t}
   {/if}
  </p>
 {else}
  {$message}
 {/if}
{/if}


<input type='hidden' value='1' name='step7_posted'>
