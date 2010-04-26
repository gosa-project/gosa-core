
<h3>{t}Object info{/t}</h3>
{if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
    {msgPool type='permView'}
{else}

    {if $modifyTimestamp==""}
        Last modified: {t}Unknown{/t}
    {else}
        Last modified: {$modifyTimestamp}
    {/if}

    {if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
        <br>
        {t}View current entry ldif{/t}&nbsp;<button type='submit' name='viewLdif'>{t}View{/t}</button>
    {/if}
{/if}
<hr>
<table summary='{t}Object references{/t}' class='reference-tab'>
    <tr>
        <td style='width:48%'>
            {if !$completeACL|regex_replace:"/[cdmw]/":"" == "r"}
                {msgPool type='permView'}
            {else}
                {$objectList}
            {/if}
        </td>
        <td class='right-border'>
          &nbsp;
        </td>
        <td>
            {if !$aclREAD}
                <h3>{t}ACL trace{/t}</h3>
                {msgPool type='permView'}
            {else}
                {$acls}
            {/if}
        </td>
    </tr>
</table>
