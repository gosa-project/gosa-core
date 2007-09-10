{if $dns_take_over}
<table summary="" width="100%">
	<tr>
		<td style="width:100%;vertical-align:top;">
			<h2>{t}DHCP take over initiated{/t}</h2>
			{$warning}
			{t}This includes 'all' DHCP subsections that are located within this server. Please double check if your really want to do this.{/t}
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
<tr>
	<td style="width:100%;vertical-align:top;">
		<b>{t}Take over dhcp configuration from following server{/t}</b>&nbsp;
		<select name='take_over_src'>
			{html_options options=$dhcp_server_list}
		</select>
		<input type="submit" name="take_over" value="{t}Apply{/t}">
	</td>
</tr>
<tr>
	<td style="width:100%;vertical-align:top;">
	<h2>{t}DHCP sections{/t}</h2>
		{$DhcpList}
<!--		<input type="submit" name="takeOver" value="{t}Take over{/t}" {$dhcpACL}>-->
	</td>
</tr>
</table>
{/if}
