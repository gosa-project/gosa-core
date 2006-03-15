<h2><img	class="center"	alt=""	align="middle"	src="images/network.png">	{t}Network	settings{/t}</h2>


<table	summary=""	width="100%" cellspacing=0 cellpadding=0>
	<tr>
		<td	style="width:50%; vertical-align: top;">
			<table	summary="">
				<tr>
					<td><LABEL	for="ipHostNumber">{t}IP-address{/t}{if $IPisMust}{$staticAddress}{/if}</LABEL></td>
					<td><input	id="ipHostNumber"	name="ipHostNumber"	size=25	maxlength=80	value="{$ipHostNumber}"></td>
				</tr>
				<tr>
					<td><LABEL	for="macAddress">{t}MAC-address{/t}</LABEL>{$staticAddress}</td>
					<td><input	name="macAddress"	id="macAddress"	size=25	maxlength=80	value="{$macAddress}"></td>
				</tr>
      </table> 
    </td>
    {if $DNS_is_account==true}
		<td	style="width:50%;vertical-align:top;border-left:1px solid #b0b0b0;" valign="top">
      <input type="checkbox" name="enableDNS" value="1" 
        {if $DNSAccount==true}checked{/if}
        onClick="
          setHidden('test2');
          changeState('zoneName');
          changeState('dNSTTL');
          changeState('AddNewRecord');
          {$changeStateForRecords}
                ">
      {t}Enable DNS for this device{/t}
      <input type="submit" name="reloadThisDNSStuff" value="{t}Refresh{/t}">
      <div style="padding-left:20px;">  
			<table	summary="" id="test2">
				<tr>
					<td><LABEL	for="zoneName">{t}Zone{/t}</LABEL></td>
					<td>
              <select name="zoneName" id="zoneName" {if $DNSAccount == false} disabled {/if}>
                {html_options values=$ZoneKeys output=$Zones selected=$zoneName}
              </select>
          </td>
				</tr>
				<tr>
          <td>{t}TTL{/t}</td>
          <td><input type="text" name="dNSTTL" value="{$dNSTTL}" id="dNSTTL" {if $DNSAccount == false} disabled {/if}>
          </td> 
				</tr>
				<tr>
          <td valign="top">{t}Dns records{/t}</td>
          <td>
            {$records}
            </td>
				</tr>
			</table>
      </div>
		</td>
       {if $DNSAccount != true}
        <script language="javascript">
        <!--
          document.getElementById('test2').style.display='none';
        -->
        </script>
       {/if}
    {/if}
    
	</tr>
</table>
<input type="hidden" name="network_tpl_posted" value="1">

<!--
vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
