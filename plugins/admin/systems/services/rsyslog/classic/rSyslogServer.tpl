
<h3>{t}Syslog logging{/t}</h3>

<input type="checkbox" name="use_database" value="1" {if $use_database} checked {/if}
  onChange="changeState('gosaLogDB'); changeState('goLogAdmin');changeState('goLogPassword');"  class="center">

<b>{t}Server provides a Syslog MySQL database{/t}</b>

<table summary="{t}rSyslog database settings{/t}">
 <tr>
  <td>{t}Database{/t}{$must}</td>
  <td>
   {render acl=$gosaLogDBACL}
    <input name="gosaLogDB" id="gosaLogDB" type='text'
      value="{$gosaLogDB}" {if !$use_database} disabled {/if}>
   {/render}
  </td>
 </tr>
 <tr>
  <td>{t}Database user{/t}{$must}</td>
  <td>
   {render acl=$goLogAdminACL}
    <input name="goLogAdmin" id="goLogAdmin" type='text'
      value="{$goLogAdmin}" {if !$use_database} disabled {/if}>
   {/render}
  </td>
 </tr>
 <tr>
  <td>{t}Password{/t}{$must}</td>
  <td>
   {render acl=$goLogPasswordACL}
    <input type="password" name="goLogPassword" id="goLogPassword" size=30 maxlength=60 
      value="{$goLogPassword}"    {if !$use_database} disabled {/if}>
   {/render}
  </td>
 </tr>
</table>

<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>

<input type="hidden" name="rSyslogServerPosted" value="1">
