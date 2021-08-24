<h2>{t}Object information{/t}</h2>

<div>
    {if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
        <button class="btn-small" type='submit' name='viewLdif'>{t}Show raw object entry{/t}</button>
    {/if}

    {if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
        {msgPool type='permView'}
    {else}
        {if $modifyTimestamp==""}
            <span>{t}Last modification{/t}: {t}Unknown{/t}</span>
        {else}
            <span class="right">{t}Last modification{/t}: {$modifyTimestamp}</span>
        {/if}
    {/if}
</div>

<div>
    {if $objectList!=""}
        {if !$completeACL|regex_replace:"/[cdmw]/":"" == "r"}
            {msgPool type='permView'}
        {else}
            {$objectList}
        {/if}
    {/if}

    {if !$aclREAD}
        <h2>{t}ACL trace{/t}</h2>
        {msgPool type='permView'}
    {else}
        {$acls}
    {/if}
</div>
