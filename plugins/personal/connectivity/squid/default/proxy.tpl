<div class="squid-wrapper">
    {if $multiple_support}
        <div class="multi-check">
            <label>
                <input type="checkbox" name="use_proxy" value="1" onClick="changeState('proxy')" {if $use_proxy} checked {/if}>
                <span></span>
            </label>

            <label>
                <input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState} {if !$use_proxy} disabled {/if}>
                <span><h3>{t}Proxy account{/t}</h3></span>
            </label>
        </div>
    {else}
        {render acl=$proxyAccountACL}
            <label>
                <input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState} onClick="
                {if $gosaProxyFlagF_W}
                    changeState('filterF');
                {/if}

                {if $gosaProxyFlagT_W}
                    changeState('filterT');
                    changeTripleSelectState('proxy', 'filterT', 'startHour');
                    changeTripleSelectState('proxy', 'filterT', 'startMinute');
                    changeTripleSelectState('proxy', 'filterT', 'stopMinute');
                    changeTripleSelectState('proxy', 'filterT', 'stopHour');
                {/if}

                {if $gosaProxyFlagB_W}
                    changeState('filterB');
                    changeTripleSelectState('proxy', 'filterB', 'quota_unit');
                    changeTripleSelectState('proxy', 'filterB', 'quota_size');
                    changeTripleSelectState('proxy', 'filterB', 'gosaProxyQuotaPeriod');
                {/if}
                ">
                <span><h3>{t}Proxy account{/t}</h3></span>
            </label>
        {/render}
    {/if}

    <div class="row">
        <div class="col s12 xl6">
            {render acl=$gosaProxyFlagFACL checkbox=$multiple_support checked=$use_filterF}
                <label>
                    <input type="checkbox" name="filterF" id="filterF" value="F" {$filterF} {$pstate}>
                    <span>{t}Filter unwanted content (i.e. pornographic or violence related){/t}</span>
                </label>
            {/render}

            {render acl=$gosaProxyFlagTACL checkbox=$multiple_support checked=$use_filterT}
                <label>
                    <input type="checkbox" name="filterT" id="filterT" value="T" {$filterT} {$pstate}  onClick="javascript:{$ProxyWorkingStateChange}">
                    <span>{t}Limit proxy access to working time{/t}</span>
                </label>
            {/render}

            {render acl=$gosaProxyFlagTACL}
                <div class="inline-wrapper">
                    <div class="input-field">
                        <select size="1" id="startHour" name="startHour" {if $Tstate!=""} disabled {/if}  >
                        {html_options values=$hours output=$hours selected=$starthour}
                        </select>
                    </div>

                    <div class="input-field">
                        <select size="1" id="startMinute" name="startMinute" {if $Tstate!=""} disabled {/if}  >
                        {html_options values=$minutes output=$minutes selected=$startminute}
                        </select>

                    </div>

                    <div class="input-field">
                        <select size="1" id="stopHour" name="stopHour" {if $Tstate!=""} disabled {/if} >
                            {html_options values=$hours output=$hours selected=$stophour}
                        </select>
                    </div>

                    <div class="input-field">
                        <select size="1" id="stopMinute" name="stopMinute" {if $Tstate!=""} disabled {/if}>
                            {html_options values=$minutes output=$minutes selected=$stopminute}
                        </select>
                    </div>
                </div>
            {/render}
        </div>

        <div class="col s12 xl6">
            {render acl=$gosaProxyFlagBACL checkbox=$multiple_support checked=$use_filterB}
                <label>
                    <input type="checkbox" id="filterB" name="filterB" value="B" {$filterB} {if $pstate=="disabled"} disabled {/if} onClick="{$changeB}">
                    <span>{t}Restrict proxy usage by quota{/t}</span>
                </label>
            {/render}

            {render acl=$gosaProxyFlagBACL}
                <div class="inline-wrapper">
                    <div class="input-field">
                        <input type='text' name="quota_size" id="quota_size" size=7 maxlength=10 value="{$quota_size}" {if $Bstate!=""} disabled {/if}>
                    </div>

                    <div class="input-field">
                        <select size="1" name="quota_unit" id="quota_unit" {if $Bstate!=""} disabled {/if} >
                            {html_options options=$quota_unit selected=$quota_u}
                        </select>
                    </div>

                    <div class="input-field">
                        <select size="1" name="gosaProxyQuotaPeriod" id="gosaProxyQuotaPeriod" {if $Bstate!=""} disabled {/if} >
                            {html_options options=$quota_time selected=$gosaProxyQuotaPeriod}
                        </select>
                        <label for="gosaProxyQuotaPeriod">{t}per{/t}</label>
                    </div>
                </div>
            {/render}
        </div>
    </div>
</div>
