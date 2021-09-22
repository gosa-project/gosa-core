{if $multiple_support}
    <input type="hidden" value="1" name="nisnetgroup_mulitple_edit">
{/if}

<div class="netgroup-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h2>{t}NIS Netgroup{/t}</h2>

            <div class="input-field">
                {if $multiple_support}
                <input type='text' id="dummy1" name="dummy1" value="{t}Multiple edit{/t}" disabled>
                {else}
                
                {render acl=$cnACL}
                <input type='text' id="cn" name="cn" value="{$cn}" title="{t}Name of the NIS Netgroup{/t}">
                {/render}
                {/if}
                        
                <label for="cn">{t}NIS Netgroup name{/t}{$must}</label>
            </div>

            {render acl=$descriptionACL checkbox=$multiple_support checked=$use_description}
            <div class="input-field">
                <input type='text' id="description" name="description" value="{$description}" title="{t}Descriptive text for this NIS Netgroup{/t}">
                <label for="description">{t}Description{/t}</label>
            </div>
            {/render}

            {if !$multiple_support}
            {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
            <div class="input-field ldap-tree">
                {$base}
            </div>
            {/render}
            {/if}
        </div>

        <div class="col s12 xl6">
            {render acl=$memberCnACL}
                {$memberList}
                <label for="members"><b>{t}NIS Netgroup members{/t}</b></label>
            {/render}
            <button class="btn-small" type=submit name="edit_membership">{msgPool type=addButton}</button>
        </div>
    </div>
</div>

<input type="hidden" name="nisnetgroupedit" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
</script>
