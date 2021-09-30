<h3>{t}Import jobs{/t}</h3>
<p>
{t}You can import a list of jobs into the GOsa job queue. This should be a semicolon separated list of items in the following format:{/t}
</p>
<i>{t}time stamp{/t} ; {t}MAC-address{/t} ; {t}job type{/t} ; {t}object group{/t} [ ; {t}import base{/t} ; {t}full host name{/t} ; {t}IP address{/t} ; {t}DHCP group{/t} ]</i>
<br>
<br>
{if !$count}
{t}Example{/t}:
<br>
20080626162556 <b>;</b> 00:0C:29:99:1E:37 <b>;</b> job_trigger_activate_new <b>;</b> goto-client <b>;</b> dc=test,dc=gonicus,dc=de
<br>
<br>
{/if}

<hr>
&nbsp;
<table summary="{t}Goto import{/t}">
	<tr>	
		<td>
			{t}Select list to import{/t}
		</td>
		<td>
			<input type='file' name='file' value="{t}Browse{/t}">
			<button type='submit' name='import'>{t}Upload{/t}</button>

		</td>
	</tr>
</table>

	{if  $count}
		<hr>
		<br>
		<br>
		<div style='width:100%; height:300px; overflow: scroll;'>
		<table style='width:100%; background-color: #CCCCCC; ' summary="{t}Import summary{/t}">

			<tr>
				<td><b>{t}Time stamp{/t}</b></td>
				<td><b>{t}MAC{/t}</b></td>
				<td><b>{t}Event{/t}</b></td>
				<td><b>{t}Object group{/t}</b></td>
				<td><b>{t}Base{/t}</b></td>
				<td><b>{t}FQDN{/t}</b></td>
				<td><b>{t}IP{/t}</b></td>
				<td><b>{t}DHCP{/t}</b></td>
			</tr>
		{foreach from=$info item=item key=key}
			{if $item.ERROR}
				<tr style='background-color: #F0BBBB;'>
					<td>{$item.TIMESTAMP}</td>
					<td>{$item.MAC}</td>
					<td>{$item.HEADER}</td>
					<td>{$item.OGROUP}</td>
					<td>{$item.BASE}</td>
					<td>{$item.FQDN}</td>
					<td>{$item.IP}</td>
					<td>{$item.DHCP}</td>
				</tr>	
				<tr style='background-color: #F0BBBB;'>
					<td colspan="7"><b>{$item.ERROR}</b></td>
				</tr>
			{else}
				{if ($key % 2)}
					<tr class="rowxp0"> 
				{else}
					<tr class="rowxp1"> 
				{/if}
					<td>{$item.TIMESTAMP}</td>
					<td class='left-border'>{$item.MAC}
</td>
					<td class='left-border'>{$item.HEADER}
</td>
					<td class='left-border'>{$item.OGROUP}
</td>
					<td class='left-border'>{$item.BASE}
</td>
					<td class='left-border'>{$item.FQDN}
</td>
					<td class='left-border'>{$item.IP}
</td>
					<td class='left-border'>{$item.DHCP}
</td>
				</tr>
			{/if}
		{/foreach}
		</table>
		</div>
	{/if}
<br>
<hr>
<div style='text-align:right;width:99%; padding-right:5px; padding-top:5px;'>
	<button type='submit' name='start_import'>{t}Import{/t}</button>&nbsp;

	<button type='submit' name='import_abort'>{msgPool type=backButton}</button>

</div>
<br>
