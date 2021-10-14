<input type="text" id="lang" value="{$lang}" hidden="true">
<div class="rsyslog-wrapper">
    <div class="row">
        <h2>{t}System logs{/t}</h3>

        <div class="col s12 xl4">
            <div class="input-field">
                <select name="selected_server" onChange="document.mainform.submit();" size=1>
                    {foreach from=$servers item=item key=key}
                    <option value="{$key}" {if $key == $selected_server} selected {/if}>{$item.cn}</option>
                    {/foreach}
                </select>
                <label for="selected_server">{t}Server{/t}:</label>
            </div>
        </div>

        <div class="col s12 xl4">
            <div class="input-field">
                <select name="selected_host" onChange="document.mainform.submit();" size=1>
                    {foreach from=$hosts item=item key=key}
                    <option value="{$key}" {if $key == $selected_host} selected {/if}>{$item}</option>
                    {/foreach}
                </select>
                <label for="selected_host">{t}Host{/t}:</label>
            </div>
        </div>

        <div class="col s12 xl4">
            <div class="input-field">
                <select name="selected_priority" onChange="document.mainform.submit();" size=1>
                    {html_options values=$priorities options=$priorities selected=$selected_priority}
                </select>
                <label for="selected_priority">{t}Severity{/t}:</label>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col s12 xl4">
            <div class="input-field">
                <input type="text" id="startTime" name="startTime" class="datepicker" value="{$startTime}">
                <label for="startTime">{t}From{/t}:</label>
            </div>
        </div>

        <div class="col s12 xl4">
            <div class="input-field">
                <input type="text" id="stopTime" name="stopTime" class="datepicker" value="{$stopTime}">
                <label for="stopTime">{t}till{/t}:</label>
            </div>
        </div>

        <div class="col s12 xl4">
            <div class="input-field add">
                <input type="text" name="search_for" id="search_for" value="{$search_for}">
                <label for="search_for">{t}Search{/t}:</label>
                <button class="btn-small" type="submit" name="search">{t}Search{/t}</button>
            </div>
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <div class="col s12 xl12">
            {if $result.status != 'ok'}
                <p><b>{t}Error{/t}: &nbsp;{$result.status}</b></p>
                <p>{$result.error}</p>
            {else}

            <table id="rSyslog-table" class="striped">
                <thead>
                    <tr>
                        <th class="valign-wrapper">
                            <a href="?plug={$plug_id}&amp;sort_value=DeviceReportedTime">{t}Date{/t}</a>
                            {if $sort_value=="DeviceReportedTime"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                        <th >
                            <a href="?plug={$plug_id}&amp;sort_value=FromHost">{t}Source{/t}</a>
                            {if $sort_value=="FromHost"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                        <th >
                            <a href="?plug={$plug_id}&amp;sort_value=SysLogTag">{t}Header{/t}</a>
                            {if $sort_value=="SysLogTag"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                        <th >
                            <a href="?plug={$plug_id}&amp;sort_value=Facility">{t}Facility{/t}</a>
                            {if $sort_value=="Facility"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                        <th >
                            <a href="?plug={$plug_id}&amp;sort_value=Priority">{t}Severity{/t}</a>
                            {if $sort_value=="Priority"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                        <th >
                            <a href="?plug={$plug_id}&amp;sort_value=Message">{t}Message{/t}</a>
                            {if $sort_value=="Message"} {if $sort_type=="DESC"} {$downimg} {else} {$upimg} {/if} {/if}
                        </th>
                    </tr>
                </thead>
                <tbody>
                {if empty($result.entries)}
                    {for $i=1 to 1}
                        <tr><td colspan="100%">&nbsp;</td></tr>
                    {/for}
                {else}
                    {foreach from=$result.entries item=item key=key}
                        <tr>
                            <td title="{$item.DeviceReportedTime}">
                                {$item.DeviceReportedTime}
                            </td>
                            <td title="{$item.FromHost}">
                                {$item.FromHost}
                            </td>

                            <td title="{$item.SysLogTag}">
                                {$item.SysLogTag}
                            </td>

                            <td title="{$item.Facility}">
                                {$item.Facility}
                            </td>

                            <td title="{$item.Priority}">
                                {$item.Priority}
                            </td>

                            <td title="{$item.Message}" style="width:400px">
                                <div>
                                    {$item.Message}
                                </div>
                            </td>
                        </tr>
                    {/foreach}
                {/if}
                </tbody>
            </table>

            <div class="valign-wrapper">
                <div class="col s4">{$matches}</div>

                <div class="col s4">
                </div>

                <div class="col s4">
                    <div class="input-field col s2 right">
                        <select name="limit" onChange="document.mainform.submit();" size=1>
                            {html_options options=$limits selected=$limit}
                        </select>
                    </div>
                    {$page_sel}
                </div>
            </div>
            {/if}
        </div>
    </div>
</div>
