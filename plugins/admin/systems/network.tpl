<h2><img	class="center"	alt=""	align="middle"	src="images/network.png">	{t}Network	settings{/t}</h2>

<table	summary=""	width="100%">
	<tr>
		<td	style="border-right:1px	solid	#b0b0b0;	width:50%; vertical-align: top;">
			<table	summary="">
				<tr>
					<td><LABEL	for="ipHostNumber">{t}IP-address{/t}{$staticAddress}</LABEL></td>
					<td><input	id="ipHostNumber"	name="ipHostNumber"	size=25	maxlength=80	value="{$ipHostNumber}"></td>
				</tr>
      </table> 
    </td>
		<td	style="border-right:1px	solid	#b0b0b0;	width:50%">
			<table	summary="">
				<tr>
					<td><LABEL	for="macAddress">{t}MAC-address{/t}</LABEL>{$staticAddress}</td>
					<td><input	name="macAddress"	id="macAddress"	size=25	maxlength=80	value="{$macAddress}"></td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
</br>
<table	summary=""	width="100%">
	<tr>
		<td	style="border-right:1px	solid	#b0b0b0;	width:50%; vertical-align: top; ">
      <h2>{t}Domain name service{/t}</h2>
      <input type="checkbox" name="enableDNS" value="1" 
        {if $DNSAccount==true}checked{/if}
        onClick="
          changeState('zoneName');
          changeState('dNSTTL');
          changeState('dNSClass');
          changeState('AddNewRecord');
          {$changeStateForRecords}
                ">
      {t}Enable DNS for this device.{/t}
      <input type="submit" name="reloadThisDNSStuff" value="{t}Refresh{/t}">
			<table	summary="" style="padding-left:10px;" cellspacing=4>
				<tr>
					<td><LABEL	for="zoneName">{t}DNS-Zone{/t}</LABEL></td>
					<td>
              <select name="zoneName" id="zoneName" {if $DNSAccount == false} disabled {/if}>
                {html_options values=$ZoneKeys output=$Zones}
              </select>
          </td>
				</tr>
				<tr>
          <td>{t}DNS-TTL{/t}</td>
          <td><input type="text" name="dNSTTL" value="{$dNSTTL}" id="dNSTTL" {if $DNSAccount == false} disabled {/if}> 
          </td>
				</tr>
				<tr>
          <td>{t}DNS Class{/t} 
          </td>
          <td>
            <select name="dNSClass" id="dNSClass" {if $DNSAccount == false} disabled {/if}>
             {html_options values=$dNSClasses output=$dNSClasses selected=$dNSClass}
            </select>
          </td>
				</tr>
      </table> 
    </td>
		<td	style="border-right:1px	solid	#b0b0b0;	width:50%; vertical-align:top;">
      <h2>{t}Records{/t}</h2>
		  {$records}
    </td>
	</tr>
</table>
<input type="hidden" name="network_tpl_posted" value="1">
<!--
vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
