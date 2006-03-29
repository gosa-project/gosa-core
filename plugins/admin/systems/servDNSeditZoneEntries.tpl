<h2>{t}This dialog allows you to configure all components of this DNS zone on a single list.{/t}</h2>
{t}Be careful editing record types with this dialog. All changes will be saved immediately when using the save button.{/t}
<br>
<br>
<p class="seperator">&nbsp;</p>
{if $disableDialog}
	<br><b>
	{t}This dialog can't be used until the currently edited zone was saved or the zone entry exists in the ldap database.{/t}
	</b>
{else}
	{t}To add a new host entry just click here{/t}
	<input type='image' name='UserRecord_Free' src='images/select_default.png' alt='{t}New{/t}' title='{t}New{/t}'>
	<br>
	{$table}	
{/if}

<p class="seperator">&nbsp;</p>
<div style="text-algin:right;" align="right">
    <p>
        <input type="submit" name="SaveZoneEntryChanges" value="{t}Save{/t}">
        <input type="submit" name="CancelZoneEntryChanges" value="{t}Cancel{/t}">
    </p>
</div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.zoneName.focus();
  -->
</script>
                                                                            
