<div class="system-wrapper">
    <div class="nis-netgroup-wrapper">
        {if !$fqdn_found}
            <h2>{t}This host cannot be added to NIS Netgroups{/t}</h2>
            <p>{t}Only hosts with a fully qualified domain name or those with DNS settings enabled in GOsa can be added to NIS Netgroups!{/t}</p>
        {else}
            <h2>{t}Member of the following NIS Netgroups{/t}</h2>
            {render acl=$netgroupsACL}
                {$memberList}

                <button class="btn-small" type="submit" name="edit_membership" id="edit_membership">{msgPool type=addButton}</button>
            {/render}
            <input type="hidden" name="nisnetgroupedit" value="1">
        {/if}
    </div>
</div>
