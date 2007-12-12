<h2><img	class="center"	alt=""	align="middle"	src="images/network.png">	{t}Network	settings{/t}</h2>


<table	summary=""	width="100%" cellspacing=0 cellpadding=0>
	<tr>
		<td	style="width:50%; vertical-align: top;">
			<table	summary="">
				<tr>
					<td><LABEL	for="ipHostNumber">{t}IP-address{/t}{if $IPisMust}{$staticAddress}{/if}</LABEL></td>
					<td>
{render acl=$ipHostNumberACL}
            <input	id="ipHostNumber"	name="ipHostNumber"	size=25	maxlength=80	value="{$ipHostNumber}">
{/render}
{render acl=$ipHostNumberACL}
{if $DNSAccount == true}
      <input id="propose_ip" type="submit" name="propose_ip" value="{t}Propose ip{/t}" style="visibility:visible;">
      {else}
      <input id="propose_ip" type="submit" name="propose_ip" value="{t}Propose ip{/t}" style="visibility:hidden;">
      {/if}
{/render}
          </td>
				</tr>
				<tr>
					<td><LABEL	for="macAddress">{t}MAC-address{/t}</LABEL>{$staticAddress}</td>
					<td>
{render acl=$macAddressACL}
            <input	name="macAddress"	id="macAddress"	size=25	maxlength=80	value="{$macAddress}">
{/render}
            {if $autonet==true}
					    <input type="submit" name="autonet" value="{t}Autodetect{/t}">
            {/if}
          </td>
				</tr>
        {if $dhcpEnabled}
        <tr>
          <td colspan=2 style='padding-top:12px;'>
            <table>
              {if $dhcpParentNodeCnt}
              <tr>
                <td>
                  <input onClick='document.mainform.submit();'
                    {if $dhcp_is_Account} checked {/if} type='checkbox' name='dhcp_is_Account' class='center'>
                </td>
                <td>
                  {t}Enable DHCP for this device{/t}
                  <input type='image' src='images/list_reload.png' class='center'>
                </td>
              </tr>
              {else}
              <tr>
                <td>
                  <input type='checkbox' name='dummy' class='center' disabled>
                  {t}Enable DHCP for this device{/t} ({t}not configured{/t})
                  <input type='image' src='images/list_reload.png' class='center'>
                </td>
              </tr>
              {/if}
              {if $dhcp_is_Account}
              <tr>
                <td>&nbsp;</td>
                <td>
                  {t}Parent node{/t}
                  <select name='dhcpParentNode'>      
                    {html_options options=$dhcpParentNodes selected=$dhcpParentNode}
                  </select>
                  &nbsp;
                  <input type='submit' name='dhcpEditOptions' value='{t}Edit settings{/t}'>
                </td>
              </tr>
              {/if}
            </table>
          </td>
        </tr>
        {/if}
      </table> 
    </td>
    {if $DNS_is_account==true}
		<td	style="width:50%;vertical-align:top;border-left:1px solid #b0b0b0;" valign="top">

    {if $ZoneCnt}
      {if $DNSAccount == true}
      <input type="checkbox" name="DNS_is_account" value="1"  
        checked="checked"
        onclick="toggle('test2');
          changeState('zoneName');
          changeState('dNSTTL');
          toggle('propose_ip');
          changeState('AddNewRecord');
          {$changeStateForRecords}"/>
        {else}
      <input type="checkbox" name="DNS_is_account" value="1"  
        onclick="toggle('test2');
          changeState('zoneName');
          changeState('dNSTTL');
          toggle('propose_ip');
          changeState('AddNewRecord');"/>
        {/if}

      {t}Enable DNS for this device{/t}
      <input type='image' src='images/list_reload.png' class='center' name="reloadThisDNSStuff" >
      {if $DNSAccount == true}
      <div style="padding-left:20px;visibility:visible;" id="test2">
      {else}
      <div style="padding-left:20px;visibility:hidden;" id="test2">
      {/if}
			<table summary="">
				<tr>
					<td><LABEL	for="zoneName">{t}Zone{/t}</LABEL></td>
					<td>
{render acl=$zoneNameACL}
              <select name="zoneName" id="zoneName" {if $DNSAccount == false} disabled {/if}>
                {html_options values=$ZoneKeys output=$Zones selected=$zoneName}
              </select>
{/render}
          </td>
        </tr>
        <tr>
          <td>{t}TTL{/t}</td>
          <td>
{render acl=$dNSTTLACL}
            <input type="text" name="dNSTTL" value="{$dNSTTL}" id="dNSTTL" {if $DNSAccount == false} disabled {/if}>
{/render}
          </td>
        </tr>
        <tr>
          <td valign="top">{t}Dns records{/t}</td>
          <td>
            {if $DNS_is_account == true}
{render acl=$RecordsACL}
              {$records}
{/render}
            {/if}
          </td>
				</tr>
			</table>
      </div>
    {else}        
      <input type="checkbox" name="dummy" value="1" disabled class='center'>
      {t}Enable DNS for this device{/t} ({t}not configured{/t})
      <input type='image' src='images/list_reload.png' class='center'>
    {/if}

		</td>
    {/if}
    
	</tr>
</table>
<input type="hidden" name="network_tpl_posted" value="1">

<!--
vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
