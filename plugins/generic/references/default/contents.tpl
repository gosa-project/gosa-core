<h2>{t}Object information{/t}</h2>

<div class="row">
    <div class="col s12">
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
</div>

<hr class="divider">

{if $objectList!=""}
    {if !$completeACL|regex_replace:"/[cdmw]/":"" == "r"}
        {msgPool type='permView'}
    {else}
        {$objectList}
    {/if}

    <hr class="divider">
{/if}

{if !$aclREAD}
    <h2>{t}ACL trace{/t}</h2>

    <div class="row">
        <div class="col s12">
            {msgPool type='permView'}
        </div>
    </div>
{else}
    {$acls}
{/if}
