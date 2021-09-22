<p>
        {t escape=no}With the GOsa2 Password Management Add-On you can mass reset user passwords in various ways.{/t}
</p>

{if $pwreset_configured != TRUE}
<input type="hidden" name="phase_01">

<br><h3>{t}Configure password reset options{/t}</h3>

<p>
{t}Please configure options for this run of resetting user credentials.{/t}
</p>

<hr>
<table summary="{t}Configure planned password reset operations{/t}">
        <tr><td colspan="3" style="height:0.3em;"></td></tr>
        <tr>
            <td colspan="3">
                    <input {if $preset_pwreset_mode==0} checked {/if} type='radio' name='pwreset_mode' value='0'>{t}Upload a credentials file (CSV format).{/t}
                    <br>
            </td>
        </tr>
        <tr><td colspan="3" style="height:0.3em;"></td></tr>
        <tr><td></td><td colspan="2"><b>{t}File format:{/t}</b> {t}CSV, comma-separated, no quotes, two columns:{/t} <tt>&lt;uid&gt;,&lt;userPassword&gt;<tt></td></tr>
        <tr>
                <td style="width: 6ex;">&nbsp;</td>
                <td style="vertical-align: middle;">
                    <LABEL for="credfile">{t}Select CSV file for uploading:{/t}</LABEL>
                </td>
                <td style="vertical-align: middle;">
                        <input type="hidden" name="MAX_FILE_SIZE" value="2097152">
                        <input id="credfile" name="credfile" type="file" value="{t}Browse{/t}">
                </td>
        </tr>
        <tr><td colspan="3" style="height:0.8em;"></td></tr>
        <tr>
                <td colspan="3">
                    <input {if $preset_pwreset_mode==1} checked {/if} type='radio' name='pwreset_mode' value='1'>{t}Reset passwords of accounts in a certain organizational unit of the LDAP tree.{/t}
                    <br>
                </td>
        </tr>
        <tr><td colspan="3" style="height:0.3em;"></td></tr>
        <tr>
                <td style="width: 6ex;">&nbsp;</td>
                <td style="vertical-align: middle;">
                        <LABEL for="subtree_ou_id">{t}Change passwords for accounts in this OU subtree:{/t}</LABEL>
                </td>
                <td style="vertical-align: middle;">
                        <select id="subtree_ou_id" name="subtree_ou_id" size="1" title="">
                        {html_options options=$ous_available selected=$preset_subtree_ou_id}
                        </select>
                </td>
        </tr>
        <tr>
                <td style="width: 6ex;">&nbsp;</td>
                <td style="vertical-align: middle;">
                        <LABEL for="pwlength">{t}Length of auto-generated passwords:{/t}</LABEL>
                </td>
                <td style="vertical-align: middle;">
                        <select id="pwlength" name="pwlength" size="1" title="">
                        {html_options options=range(8,32) selected=$preset_pwlength-8}
                        </select>
                </td>
        </tr>
</table>

{elseif $pwreset_reviewed != TRUE}
<input type="hidden" name="phase_02">

<br><h3>{t}Review password resets{/t}</h3>

{if (!empty($accounts))}
<p>
{t}Please review outstanding password reset operations. If individual users should be skipped from this password reset operation, deselect them below.{/t}
</p>

<hr>

<table summary="{t}Review planned password reset operations{/t}">
        <tr>
            <th bgcolor="#BBBBBB">{t}Really?{/t}</th>
            <th bgcolor="#BBBBBB">{t}User Id{/t}</th>
            <th bgcolor="#BBBBBB">{t}Fullname{/t}</th>
            <th bgcolor="#BBBBBB">{t}New Password{/t}</th>
            <th bgcolor="#BBBBBB">{t}LDAP DN{/t}</th>
        </tr>
{foreach from=$accounts item=val key=key}
        <tr>
        <td style="text-align: center; vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            <input value="{$key}" {if $accounts[$key]['checked'] && $accounts[$key]['valid']}checked{/if} {if (!$accounts[$key]['valid'])}disabled{/if} type='checkbox' name='accounts[]' id='account_{$key}'>
        </td>
        <td style="vertical-align:middle;{if (!$accounts[$key]['valid'])}color:#A0A0A0;{/if}" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            {$accounts[$key]['uid']}
        </td>
        <td style="vertical-align:middle;{if (!$accounts[$key]['valid'])}color:#A0A0A0;{/if}" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            {$accounts[$key]['cn']}
        </td>
        <td style="vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            <input style="font-family:monospace;{if (!$accounts[$key]['valid'])}color:#A0A0A0;{/if}" {if (!$accounts[$key]['valid'])}disabled{/if} type="text" value="{$accounts[$key]['userPassword']}" name='password_{$key}' id='password_{$key}'>
        </td>
        <td style="vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            {$accounts[$key]['dn']}
        </td>
        </tr>
{/foreach}
</table>
{else}
<p>
{t}No accounts have been found in this part of the LDAP tree.{/t}
</p>
{/if}
{else}

<br><h3>{t}Password reset operations have been accomplished{/t}</h3>

<p>
{t}Below you find a status report for this password reset operation. You can copy'n'paste the credentials table below into your favourite spreadsheet application (e.g., LibreOffice Calc).{/t}
</p>

<hr>

<table summary="{t}Password reset operation status report{/t}">
        <tr>
            <th bgcolor="#BBBBBB">{t}Status{/t}</th>
            <th bgcolor="#BBBBBB">{t}User Id{/t}</th>
            <th bgcolor="#BBBBBB">{t}Fullname{/t}</th>
            <th bgcolor="#BBBBBB">{t}New Password{/t}</th>
            <th bgcolor="#BBBBBB">{t}LDAP DN{/t}</th>
        </tr>
{foreach from=$accounts item=account key=key}
        <tr>
        <td style="text-align: center; vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
{if $accounts[$key]['status']==="ok"}
            <b style="color:green;">{t}ok{/t}</b>
{elseif $accounts[$key]['status']==="failed"}
            <b style="color:red;">{t}failed{/t}</b>
{elseif $accounts[$key]['status']==="skipped"}
            <i>{t}skipped{/t}</i>
{elseif $accounts[$key]['status']==="ignored"}
            <div style="color:#A0A0A0;">{t}ignored{/t}</div>
{/if}
        </td>
        <td style="vertical-align:middle;{if (!$accounts[$key]['valid']) || (!$accounts[$key]['checked'])}color:#A0A0A0;{/if}" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            {$accounts[$key]['uid']}
        </td>
        <td style="vertical-align:middle;{if (!$accounts[$key]['valid']) || (!$accounts[$key]['checked'])}color:#A0A0A0;{/if}" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            {$accounts[$key]['cn']}
        </td>
        <td style="vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            <tt {if (!$accounts[$key]['valid']) || (!$accounts[$key]['checked'])}style="color:#A0A0A0;"{/if}>{$accounts[$key]['userPassword']}</tt>
        </td>
        <td style="vertical-align:middle;" {if $key % 2 == 1}bgcolor="#E0E0E0"{/if}>
            <div {if (!$accounts[$key]['checked'])}style="color:#A0A0A0;"{/if}>{$accounts[$key]['dn']}</div>
        </td>
        </tr>
{/foreach}
</table>
{/if}

<hr>
<div class="plugin-actions">

        {if $pwreset_configured != TRUE}
        <input name="btn_pwreset_configured" value="{t}Review upcoming password resets{/t}" type ="submit">
        {elseif $pwreset_reviewed != TRUE}
        {if (!empty($accounts))}
        <input name="btn_pwreset_reviewed" value="{t}Reset passwords of selected accounts{/t}" type ="submit">
        {else}
        <input type="hidden" name="phase_00">
        <input name="btn_pwreset_back" value="{t}Back{/t}" type ="submit">
        {/if}
        {else}
        <input type="hidden" name="phase_00">
        <input name="btn_pwreset_restart" value="{t}Restart Password Manager{/t}" type ="submit">
        {/if}
</div>

<input type="hidden" name="ignore">
