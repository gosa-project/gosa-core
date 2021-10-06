
<h3>{t}This dialog allows you to configure all components of this DNS zone on a single list.{/t}</h3>
<hr>

{if $disableDialog}
 <br>
 <b>{t}This dialog can't be used until the currently edited zone was saved or the zone entry exists in the LDAP directory.{/t}</b>
 {else}
 <br>
 {$table}
 <br>
 {render acl=$acl}
  <button type='submit' name='UserRecord' title="{t}Create a new DNS zone entry{/t}">{t}New entry{/t}</button>
 {/render}
 
{/if}
<hr>

<div class="plugin-actions">
  {render acl=$acl}
   <button type='submit' name='SaveZoneEntryChanges'>{msgPool type=saveButton}</button>
  {/render}
  <button type='submit' name='CancelZoneEntryChanges'>{msgPool type=cancelButton}</button>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page	
  focus_field('zoneName');  -->
</script>
