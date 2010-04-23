
<h3>{t}Object info{/t}</h3>
{if !$referencesREAD}
    {msgPool type='permView'}
{else}
    test
{/if}
<hr>
<table summary='{t}Object references{/t}' class='reference-tab'>
    <tr>
        <td style='width:48%'>
            {if !$referencesREAD}
                <h3>{t}Object references{/t}</h3>
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
