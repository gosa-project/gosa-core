<div class="pwreset-wrapper">
    <div class="row">
        {if $pwreset_configured != TRUE}
        <input type="hidden" name="phase_01">

        <h2>{t}Configure password reset options{/t}
        <i class="material-icons tooltip-icon" title="{t escape=no}With the GOsa2 Password Management Add-On you can mass reset user passwords in various ways.{/t}">info_outline</i></h2>
        <p>{t}Please configure options for this run of resetting user credentials.{/t}</p>

        <hr class="divider">

        <div class="col s12 xl6">
            <label>
                <input {if $preset_pwreset_mode==0} checked {/if} type='radio' name='pwreset_mode' value='0'>
                <span>{t}Upload a credentials file (CSV format).{/t}</span>
            </label>

            <div class="example-wrapper">
                <b>{t}File format:{/t}</b> {t}CSV, comma-separated, no quotes, two columns:{/t}
                <pre class="center-align">&lt;uid&gt;,&lt;userPassword&gt;</pre>
            </div>


            <div class="file-field input-field">
                <input type="hidden" name="MAX_FILE_SIZE" value="2097152">
                <div class="btn-small">
                    <span>{t}Browse{/t}</span>
                    <input id="credfile" name="credfile" type="file">
                </div>
                <div class="file-path-wrapper">
                    <input class="file-path validate" id="credfileInput" type="text" placeholder="{t}Select CSV file for uploading:{/t}">
                </div>
            </div>
        </div>

        <div class="col s12 xl6">
            <label>
                <input {if $preset_pwreset_mode==1} checked {/if} type='radio' name='pwreset_mode' value='1'>
                <span>{t}Reset passwords of accounts in a certain organizational unit of the LDAP tree.{/t}</span>
            </label>

            <div class="input-field">
                <select id="subtree_ou_id" name="subtree_ou_id" size="1" title="">
                    {html_options options=$ous_available selected=$preset_subtree_ou_id}
                </select>
                <label for="subtree_ou_id">{t}Change passwords for accounts in this OU subtree:{/t}</label>
            </div>

            <div class="input-field">
                <select id="pwlength" name="pwlength" size="1" title="">
                    {html_options options=range(8,32) selected=$preset_pwlength-8}
                </select>
                <label for="pwlength">{t}Length of auto-generated passwords:{/t}</label>
            </div>
        </div>


        {elseif $pwreset_reviewed != TRUE}
        <input type="hidden" name="phase_02">

        <h2>{t}Review password resets{/t}</h2>

        {if (!empty($accounts))}
        <p>{t}Please review outstanding password reset operations. If individual users should be skipped from this password reset operation, deselect them below.{/t}</p>

        <hr class="divider">

        <div class="col s12 xl12">
            <table id="managepw-table" class="striped">
                <thead>
                    <tr>
                        <th>{t}Really?{/t}</th>
                        <th>{t}User Id{/t}</th>
                        <th>{t}Fullname{/t}</th>
                        <th>{t}New Password{/t}</th>
                        <th>{t}LDAP DN{/t}</th>
                    </tr>
                </thead>
                <tbody>
                    {foreach from=$accounts item=val key=key}
                    <tr>
                        <td>
                            <label>
                                <input value="{$key}" {if $accounts[$key]['checked'] && $accounts[$key]['valid']}checked{/if} {if (!$accounts[$key]['valid'])}disabled{/if} type='checkbox' name='accounts[]' id='account_{$key}'>
                                <span></span>
                            </label>
                        </td>
                        <td>
                            {$accounts[$key]['uid']}
                        </td>
                        <td>
                            {$accounts[$key]['cn']}
                        </td>
                        <td>
                            <div class="input-field">
                                <input {if (!$accounts[$key]['valid'])}style="color:#A0A0A0;"{/if} {if (!$accounts[$key]['valid'])}disabled{/if} type="text" value="{$accounts[$key]['userPassword']}" name='password_{$key}' id='password_{$key}'>
                            </div>
                        </td>
                        <td>
                            {$accounts[$key]['dn']}
                        </td>
                    </tr>
                    {/foreach}
                </tbody>
            </table>
            {else}

            <p>{t}No accounts have been found in this part of the LDAP tree.{/t}</p>

            {/if}
        </div>
        {else}
        <h2>{t}Password reset operations have been accomplished{/t}</h2>

        <p>{t}Below you find a status report for this password reset operation. You can copy'n'paste the credentials table below into your favourite spreadsheet application (e.g., LibreOffice Calc).{/t}</p>

        <hr class="divider">
        <div class="col s12 s6">
            <table id="managepw-table" class="striped">
                <thead>
                    <tr>
                        <th>{t}Status{/t}</th>
                        <th>{t}User Id{/t}</th>
                        <th>{t}Fullname{/t}</th>
                        <th>{t}New Password{/t}</th>
                        <th>{t}LDAP DN{/t}</th>
                    </tr>
                </thead>
                <tbody>
                    {foreach from=$accounts item=account key=key}
                    <tr>
                        <td>
                            {if $accounts[$key]['status']==="ok"}
                            <b style="color:green;">{t}ok{/t}</b>
                            {elseif $accounts[$key]['status']==="failed"}
                            <b style="color:red;">{t}failed{/t}</b>
                            {elseif $accounts[$key]['status']==="skipped"}
                            <p>{t}skipped{/t}</p>
                            {elseif $accounts[$key]['status']==="ignored"}
                            <div style="color:#A0A0A0;">{t}ignored{/t}</div>
                            {/if}
                        </td>
                        <td>
                            {$accounts[$key]['uid']}
                        </td>
                        <td>
                            {$accounts[$key]['cn']}
                        </td>
                        <td>
                            <pre {if (!$accounts[$key]['valid']) || (!$accounts[$key]['checked'])}style="color:#A0A0A0;"{/if}>{$accounts[$key]['userPassword']}</pre>
                        </td>
                        <td>
                            <div {if (!$accounts[$key]['checked'])}style="color:#A0A0A0;"{/if}>{$accounts[$key]['dn']}</div>
                        </td>
                    </tr>
                    {/foreach}
                </tbody>
            </table>
        </div>
        {/if}
    </div>
</div>

<div class="card-action">
    {if $pwreset_configured != TRUE}
    <button class="btn-small primary" name="btn_pwreset_configured" type ="submit" style="overflow: hidden;">{t}Review upcoming password resets{/t}</button>
    {elseif $pwreset_reviewed != TRUE}
    {if (!empty($accounts))}
    <button class="btn-small primary" name="btn_pwreset_reviewed" type ="submit" style="overflow: hidden;">{t}Reset passwords of selected accounts{/t}</button>
    {else}
    <input type="hidden" name="phase_00">
    <button class="btn-small primary" name="btn_pwreset_back" type ="submit" style="overflow: hidden;">{t}Back{/t}</button>
    {/if}
    {else}
    <input type="hidden" name="phase_00">
    <button class="btn-small primary" name="btn_pwreset_restart" type ="submit" style="overflow: hidden;">{t}Restart Password Manager{/t}</button>
    {/if}
</div>

<input type="hidden" name="ignore">
