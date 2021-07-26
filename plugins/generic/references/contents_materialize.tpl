<h5>{t}Object information{/t}</h5>

<div>
    {if $completeACL|regex_replace:"/[cdmw]/":"" == "r"}
    <button class="btn-small gonicus-color" type='submit' name='viewLdif'>{t}Show raw object entry{/t}</button>
    {/if}

    {if !$someACL|regex_replace:"/[cdmw]/":"" == "r"}
    {msgPool type='permView'}
    {else}
    {if $modifyTimestamp==""}
    {t}Last modification{/t}: {t}Unknown{/t}
    {else}
    {t}Last modification{/t}: {$modifyTimestamp}
    {/if}
    {/if}
</div>

<hr>

<div>
    {if $objectList!=""}
    {if !$completeACL|regex_replace:"/[cdmw]/":"" == "r"}
    {msgPool type='permView'}
    {else}
    {$objectList}
    {/if}
    {/if}
    
    {if !$aclREAD}
    <h5>{t}ACL trace{/t}</h5>
    {msgPool type='permView'}
    {else}
    {$acls}
    {/if}
</div>