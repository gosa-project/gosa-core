<div class="new-device-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Integrating unknown devices{/t}</h3>
            <p>{t}The current device has been detected by the ARP monitor used by GOsa. You can integrate this device into your running DHCP/DNS infrastructure by submitting this form. The device entry will disappear from the list of the systems and move to the DNS/DHCP configuration.{/t}</p>

            <div class="input-field">
                <input type='text' id="cn" name="cn" maxlength=60 value="{$cn}">
                <label for="cn">{t}DNS name{/t}{$must}</label>
            </div>

            <div class="input-field">
                <input type='text' id="description" name="description" maxlength=60 value="{$description}">
                <label for="description">{t}Description{/t}</label>
            </div>
        </div>

		{if $netconfig}
			<div class="col s12 xl6">
				{$netconfig}
			</div>
		{/if}

        <div class="col s12 xl6">
            <h3>{t}GOto{/t}</h3>

            <label>
                <p>
                    <input type=checkbox name="gotoIntegration" value="1" onChange="changeState('SystemType');changeState('ObjectGroup');" {if $gotoIntegration} checked {/if}>
                    <span>
                        {t}GOto integration{/t}
                    </span>
                </p>
            </label>

            <div class="input-field">
                <select id="SystemType" size="1" name="SystemType" title="{t}System type{/t}" onChange="document.mainform.submit();" {if !$gotoIntegration} disabled {/if}>
                    {html_options values=$SystemTypeKeys output=$SystemTypes selected=$SystemType}
                </select>
                <label for="SystemType">{t}System type{/t}</label>
            </div>

            <div class="input-field">
                <select id="ObjectGroup" size="1" name="ObjectGroup" title="{t}Object group{/t}" {if !$gotoIntegration} disabled {/if}>
		            <option value='none'>{t}none{/t}</option>
                    {html_options options=$ogroups selected=$ObjectGroup}
                </select>
                <label for="ObjectGroup">{t}Choose an object group as template{/t}</label>
            </div>

            <input type='hidden' name='ArpNewDevice_posted' value='1'>
        </div>
    </div>
</div>
