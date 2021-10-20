<div class="acl-content-wrapper">
    {if $dialogState eq 'head'}
    <h2>{t}Assigned ACL for current entry{/t}</h2>

    <div class="acl-role-head card-content-scroll">
        <div class="row">
            <div class="col s6">
                {render acl=$cnACL}
                <div class="input-field">
                    <input type="text" name='cn' value="{$cn}" id="cn">
                    <label for="cn">{t}Name{/t}</label>
                </div>
                {/render}
            </div>

            <div class="col s6">
                {render acl=$descriptionACL}
                <div class="input-field">
                    <input type="text" name='description' value="{$description}" id="description">
                    <label for="description">{t}Description{/t}</label>
                </div>
                {/render}
            </div>

            <div class="col s6">
                {render acl=$baseACL}
                <div class="input-field ldap-tree">
                    {$base}
                </div>
                {/render}
            </div>
        </div>

        {$aclList} 
        
        {render acl=$gosaAclTemplateACL}
            <button class="btn-small" type='submit' name='new_acl'>{t}New ACL{/t}</button> 
        {/render}
    </div>
    {/if} 
    
    {if $dialogState eq 'create'}
    <div class="content-head">
        <h2>{t}ACL type{/t}</h2>

        <div class="input-field">
            <select size="1" name="aclType" title="{t}Select an ACL type{/t}" onChange="document.mainform.submit()">
					{html_options options=$aclTypes selected=$aclType}
				</select>
        </div>

        {if $javascript eq 'false'}
        <button class="btn-small" type='submit' name='refresh'>{msgPool type=applyButton}</button> {/if}
    </div>

    <h3>{t}List of available ACL categories{/t}</h3>

    <div class="acl-role-create card-content-scroll">
        {$aclList}
    </div>

    <div class="card-action">
        {render acl=$gosaAclTemplateACL}
        <button class="btn-small primary" type='submit' name='submit_new_acl'>{msgPool type=applyButton}</button> 
        {/render}

        <button class="btn-small primary" type='submit' name='cancel_new_acl'>{msgPool type=cancelButton}</button>
    </div>
    {/if} {if $dialogState eq 'edit'}

    <h2>{$headline}</h2>

    {render acl=$gosaAclTemplateACL}
    <div class="acl-role-edit card-content-scroll">
        {$aclSelector}
    </div>
    {/render}

    <div class="card-action">
        {render acl=$gosaAclTemplateACL}
        <button class="btn-small primary" type='submit' name='submit_edit_acl'>{msgPool type=applyButton}</button> {/render}
        <button class="btn-small primary" type='submit' name='cancel_edit_acl'>{msgPool type=cancelButton}</button>
    </div>
    {/if}
    <input type='hidden' name='acl_role_posted' value='1'>
</div>