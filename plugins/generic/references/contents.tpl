
<h3>{t}Object info{/t}</h3>
<table width='100%' summary='{t}Object info{/t}'>
    <tr>
        <td style='width:48%;'>
            {if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
                {msgPool type='permView'}
            {else}
                {if $modifyTimestamp==""}
                    Last modified: {t}Unknown{/t}
                {else}
                    Last modified: {$modifyTimestamp}
                {/if}
            {/if}
        </td>
        <td class='right-border'>
          &nbsp;
        </td>
        <td>
            {if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
                {t}View current entry ldif{/t}&nbsp;<button type='submit' name='viewLdif'>{t}View{/t}</button>
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
        <td class='right-border'>
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
