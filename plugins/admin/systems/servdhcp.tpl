{if $dns_take_over}
<table summary="" width="100%">
	<tr>
		<td style="width:100%;vertical-align:top;">
			<h2>{t}DHCP take over initiated{/t}</h2>
			{$warning}
			{t}This includes 'all' DHCP subsections that are located within this server. Please double check if your really want to do this.{/t}
			<p>
			{$warning2}
		</td>
	</tr>
</table>
{else}
<table summary="" width="100%">
{if $dhcp_server_list_cnt}
<tr>
	<td style="width:100%;vertical-align:top;">
		<b>{t}Take over DHCP configuration from{/t}</b>&nbsp;
		<select name='take_over_src'>
			{html_options options=$dhcp_server_list}
		</select>
		<input type="submit" name="take_over" value="{t}Take over{/t}">
	</td>
</tr>
{/if}
<tr>
	<td style="width:100%;vertical-align:top;">
	<h2>{t}DHCP sections{/t}</h2>
		{$DhcpList}
	</td>
</tr>
</table>
{/if}
