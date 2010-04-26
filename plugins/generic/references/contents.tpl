
<h3>{t}Object information{/t}</h3>
<table width='100%' summary='{t}Object info{/t}'>
    <tr>
        <td style='width:48%;'>
            {if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
                <button type='submit' name='viewLdif'>{t}Show raw object entry{/t}</button>
            {/if}
        </td>
        <td class='right-border' style='width:2px'>
          &nbsp;
        </td>
        <td>
            {if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
                {msgPool type='permView'}
            {else}
                {if $modifyTimestamp==""}
                    {t}Last modification{/t}: {t}Unknown{/t}
                {else}
                    {t}Last modification{/t}: {$modifyTimestamp}
                {/if}
            {/if}
        </td>
    </tr>
</table>

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
        <td class='right-border'  style='width:2px'>
          &nbsp;
        </td>
        <td>
            {if !$aclREAD}
                <h3>{t}ACL trace{/t}</h3>
                {msgPool type='permView'}
            {else}
                <div style='height:350px; overflow: scroll;'>
                {$acls}
                </div>
            {/if}
        </td>
    </tr>
</table>
