<h2>{t}This dialog allows you to configure all components of this DNS zone on a single list.{/t}</h2>
<p class="seperator">&nbsp;</p>
{if $disableDialog}
	<br><b>
	{t}This dialog can't be used until the currently edited zone was saved or the zone entry exists in the ldap database.{/t}
	</b>
{else}
	<br>
	{$table}
	<br>
	<input type='submit' name='UserRecord' value='{t}New entry{/t}' title='{t}Create a new DNS zone entry{/t}'>
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
	focus_field('zoneName');
  -->
</script>
                                                                            
