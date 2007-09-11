{if $dns_take_over}
<table summary="" width="100%">
    <tr>
        <td style="width:100%;vertical-align:top;">
            <h2>{t}DNS take over initiated{/t}</h2>
            {$warning}
            {t}This includes 'all' DNS zones that are located within this server. Please double check if your really want to do this.{/t}
            <p>
            {$warning2}
            <br>
            <input type='submit' name='cancel_take_over' value='{t}Cancel{/t}'>
            </p>
            <p>
            {t}Following objects will be taken over{/t}&nbsp;:
            </p>
            <p>
            <pre>{$info}</pre>
            </p>
        </td>
    </tr>
</table>
{else}
<table summary="" width="100%">
{if $dns_server_list_cnt}
<tr>
    <td style="width:100%;vertical-align:top;">
        <b>{t}Take over DHCP configuration from{t}</b>&nbsp;
        <select name='take_over_src'>
            {html_options options=$dns_server_list}
        </select>
        <input type="submit" name="take_over" value="{t}Take over{/t}">
    </td>
</tr>
{/if}
<tr>
	<td style="width:100%;vertical-align:top;">
<h2>{t}Zones{/t}</h2>
		{$ZoneList}
		<input type="submit" name="AddZone" value="{t}Add{/t}" {$servdnsACL}>
	</td>
</tr>
</table>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('AddZone');
  -->
</script>
{/if}
