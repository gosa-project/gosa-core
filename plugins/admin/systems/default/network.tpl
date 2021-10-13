<div class="network-wrapper">
  <h3>{t}Network	settings{/t}</h3>

  {render acl=$ipHostNumberACL}
    <div class="input-field add">
      <input type="text" name="ipHostNumber" id="ipHostNumber" value="{$ipHostNumber}">
      <label for="ipHostNumber">{t}IP-address{/t}{if $IPisMust}{$must}{/if}</label>

      <button class="btn-small" type="submit" name="propose_ip" id="propose_ip">{t}Propose IP{/t}</button>

      {image path="<i class='material-icons input-icons'>add</i>" action="additionalHostNumbers_add"}
    </div>

    {foreach from=$additionalHostNumbers item=item key=key}
      <div class="input-field add">
        <input type="text" name="additionalHostNumbers_{$key}" value="{$item}">
        <label for="additionalHostNumbers_{$key}">{t}IP-address {$key}{/t}</label>
        {image path="<i class='material-icons input-icons'>delete</i>" action="additionalHostNumbers_del_{$key}"}
      </div>
    {/foreach}
  {/render}

  {render acl=$macAddressACL}
    <div class="input-field add">
      <input type="text" name="macAddress" id="macAddress" value="{$macAddress}">
      <label for="macAddress">{t}MAC-address{/t}{if $MACisMust}{$must}{/if}</label>

      {render acl=$autonetACL}
        <button class="btn-small" type="submit" name="autonet">{t}Auto detect{/t}</button>
      {/render}
    </div>

    {if $macManufacturer!=""}
        <p class="mac-manufacturer"><span style="font-size:xx-small;">{$macManufacturer}</p>
    {/if}
  {/render}

  <hr class="divider">

  {if $dhcpEnabled}
    {if $dhcpParentNodeCnt}
      {render acl=$dhcpSetupACL}
        <div class="input-field">
          <input type="checkbox" name="dhcp_is_Account" onClick="document.mainform.submit();" {if $dhcp_is_Account} checked {/if}>
        </div>
      {/render}

      <p>{t}Enable DHCP for this device{/t}</p>

      {render acl=$dhcpSetupACL}
        {image path="<i class='material-icons input-icons'>refresh</i>"}
      {/render}
    {else}
      <label>
        <input type="checkbox" name="dummy" disabled>
        <span for="dummy">{t}Enable DHCP for this device{/t} ({t}not configured{/t})</span>
        {image path="<i class='material-icons input-icons'>refresh</i>"}
      </label>
    {/if}

    {if $dhcp_is_Account}
      {render acl=$dhcpSetupACL}
        <div class="input-field add">
          <select name="dhcpParentNode" size=1>
            {html_options options=$dhcpParentNodes selected=$dhcpParentNode}
          </select>
          <label for="dhcpParentNode">{t}Parent node{/t}</label>
          <button type="submit" name="dhcpEditOptions">{t}Edit settings{/t}</button>
        </div>
      {/render}
    {/if}
  {/if}

  {if $DNSenabled==true}
    {if $ZoneCnt}
      {render acl=$dnsSetupACL}
        {if $DNS_is_account == true}
          {if $hide_dns_check_box}
            <label>
              <input type="hidden" name="DNS_is_account" value="1">
              <input type="checkbox" name="dummy" value="1" disabled checked="checked" />
            </label>
          {else}
            <label>
              <input type="checkbox" name="DNS_is_account" value="1" checked="checked" onclick="$('test2').toggle(); $('propose_ip').toggle();" />
            </label>
          {/if}
        {else}
            <label>
              <input type="checkbox" name="DNS_is_account" value="1" onclick="$('test2').toggle();$('propose_ip').toggle();" />
            </label>
        {/if}
      {/render}

      <p>{t}Enable DNS for this device{/t}</p>

      {render acl=$dnsSetupACL}
        <div class="input-field">
          <select name="zoneName" id="zoneName" size=1>
              {html_options values=$ZoneKeys output=$Zones selected=$zoneName}
          </select>
          <label for="zoneName">{t}Zone{/t}</label>
        </div>

        <div class="input-field">
            <input type="text" name="dNSTTL" value="{$dNSTTL}" id="dNSTTL">
            <label for="dNSTTL">{t}TTL{/t}</label>
        </div>

        <h3>{t}DNS records{/t}</h3>
        <div class="dns-records">
          {$records}
        </div>
      {/render}
    {else}
      <label>
        <input type="checkbox" name="dummy" value="1" disabled {if $DNS_is_account} checked {/if}>
        <span for="dummy">{t}Enable DNS for this device{/t} ({t}not configured{/t})</span>
        {image path="<i class='material-icons input-icons'>refresh</i>"}
      </label>

    {/if}
  {/if}

  <input type="hidden" name="network_tpl_posted" value="1">

  <!--
  vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
  -->
</div>
