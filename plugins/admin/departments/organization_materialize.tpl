<!--//////////////////////  ORGANIZATION (o)    //////////////////////-->

<div class="row">
    <div class="col s6">
        <span class="card-title">{t}Properties{/t}</span>

        <div class="input-field">
            {render acl=$oACL}
            <input type='text' id="o" name="o" size=25 maxlength=60 value="{$o}"
                title="{t}Name of organization to create{/t}">
            {/render}
            <label for="c">{t}Name of organization{/t}{$must}</label>
        </div>

        <div class="input-field">
            {render acl=$descriptionACL}
            <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}"
                title="{t}Descriptive text for department{/t}">
            {/render}
            <label for="description">{t}Description{/t}{$must}</label>
        </div>

        <div class="input-field">
            {render acl=$businessCategoryACL}
            <input type='text' id="businessCategory" name="businessCategory" size=25 maxlength=80
                value="{$businessCategory}" title="{t}Category for this subtree{/t}">
            {/render}
            <label for="businessCategory">{t}Category{/t}</label>
        </div>

        {if !$is_root_dse}
        {render acl=$baseACL}
        <div class="input-field ldap-tree">
            {$base}
        </div>
        {/render}
        {/if}

        {if $manager_enabled}
        <div class="input-field manager">
            {render acl=$managerACL}
            <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled
                title='{$manager}'>
            {/render}
            <label for="manager">{t}Manager{/t}</label>
            <div class="icons">
                <div class="icon">
                    {render acl=$managerACL}
                    {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
                    {/render}
                </div>

                {if $manager!=""}
                <div class="icon">
                    {render acl=$managerACL}
                    {image path="<i class='material-icons input-icons md-24 tooltipped' data-postion='bottom'
                        data-tooltip='{$manager}'>info</i>" title="{$manager}"}
                    {/render}
                </div>

                <div class="icon">
                    {render acl=$managerACL}
                    {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
                    {/render}
                </div>

                {else}
                <div class="icon">
                    {render acl=$managerACL}
                    {image path="<i class='material-icons md-disabled'>delete</i>"}
                    {/render}
                </div>

                <div class="icon">
                    {render acl=$managerACL}
                    {image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom'
                        data-tooltip='{t}Manager{/t}'>info</i>"}
                    {/render}
                </div>
                {/if}
            </div>
        </div>
        {/if}
    </div>
    <div class="col s6">
        <span class="card-title">{t}Location{/t}</span>

        <div class="input-field">
            {render acl=$stACL}
            <input type='text' id="st" name="st" size=25 maxlength=60 value="{$st}"
                title="{t}State where this subtree is located{/t}">
            {/render}
            <label for="st">{t}State{/t}</label>
        </div>

        <div class="input-field">
            {render acl=$lACL}
            <input type='text' id="l" name="l" size=25 maxlength=60 value="{$l}"
                title="{t}Location of this subtree{/t}">
            {/render}
            <label for="l">{t}Location{/t}</label>
        </div>

        <div class="input-field">
            {render acl=$postalAddressACL}
            <textarea class="materialize-textarea" id="postalAddress" name="postalAddress"
                title="{t}Postal address of this subtree{/t}">{$postalAddress}</textarea>
            {/render}
            <label for="postalAddress">{t}Address{/t}</label>
        </div>

        <div class="input-field">
            {render acl=$telephoneNumberACL}
            <input type='text' id="telephoneNumber" name="telephoneNumber" size=25 maxlength=60
                value="{$telephoneNumber}" title="{t}Base telephone number of this subtree{/t}">
            {/render}
            <label for="telephoneNumber">{t}Phone{/t}</label>
        </div>

        <div class="input-field">
            {render acl=$facsimileTelephoneNumberACL}
            <input type='text' id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=25 maxlength=60
                value="{$facsimileTelephoneNumber}" title="{t}Base facsimile telephone number of this subtree{/t}">
            {/render}
            <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
        </div>
    </div>
</div>


<hr>

<span class="card-title">{t}Administrative settings{/t}</span>

<div class="input-field">
    <label>
        {render acl=$gosaUnitTagACL}
        <input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
        {/render}
        <span class="black-text">{t}Tag department as an independent administrative unit{/t}</span>
    </label>
</div>

<input type='hidden' name='dep_generic_posted' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('o');
</script>
<input type='hidden' name='dep_generic_posted' value='1'>