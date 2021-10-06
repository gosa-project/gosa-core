{if !$fqdn_found}
    <h2>{t}This host cannot be added to NIS Netgroups{/t}</h2>
    <p>{t}Only hosts with a fully qualified domain name or those with DNS settings enabled in GOsa can be added to NIS Netgroups!{/t}</p>

{else}

    <b><LABEL for="netgroups">{t}Member of the following NIS Netgroups{/t}</LABEL></b>
    {render acl=$netgroupsACL}
        {$memberList}
    {/render}
    <br>
    {render acl=$netgroupsACL}
    <input type=submit name="edit_membership" value="{msgPool type=addButton}">
    {/render}
    <input type="hidden" name="nisnetgroupedit" value="1">
{/if}
