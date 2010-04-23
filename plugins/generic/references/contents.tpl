
<h3>{t}Object info{/t}</h3>
{if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
    {msgPool type='permView'}
{else}
    Last modified ....
{/if}
{if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
    <button type='submit' name='viewLdif'>{t}View entry{/t}</button>
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
