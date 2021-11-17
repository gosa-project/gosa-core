<div class="directory-structure-wrapper">
    <h2>{t}Properties{/t}</h2>

    <div class="row">
        <div class="col s6">

            {render acl=${$namingAttr}ACL}
            <div class="input-field">
                <input type='text' id="{$namingAttr}" name="{$namingAttr}" value="{$o}" title="{t}Name of {$trString[0]} to create{/t}">
                <label for="c">{t}Name of {$trString[0]}{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$descriptionACL}
                <div class="input-field">
                    <input type="text" id="description" name="description" value="{$description}" title="{t}Descriptive text for department{/t}">
                    <label for="description">{t}Description{/t}{$must}</label>
                </div>
            {/render}

            {render acl=$businessCategoryACL}
                <div class="input-field">
                    <input type='text' id="businessCategory" name="businessCategory" value="{$businessCategory}" title="{t}Category for this subtree{/t}">
                    <label for="businessCategory">{t}Category{/t}</label>
                </div>
            {/render}

            {if !$is_root_dse}
            {render acl=$baseACL}
                <div class="input-field ldap-tree">
                    {$base}
                </div>
            {/render}
            {/if}

            {if $manager_enabled}
            {render acl=$managerACL}
            <div class="input-field manager">
                <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
                <label for="manager">{t}Manager{/t}</label>

                <div class="icons">
                    <div class="icon">
                        {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
                    </div>

                    {if $manager!=""}
                    <div class="icon">
                        {image path="<i class='material-icons input-icons tooltipped' data-postion='bottom' data-tooltip='{$manager}'>info_outline</i>" title="{$manager}"}
                    </div>

                    <div class="icon">
                        {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
                    </div>
                    {else}
                    <div class="icon">
                        {image path="<i class='material-icons md-disabled'>delete</i>"}
                    </div>

                    <div class="icon">
                        {image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom' data-tooltip='{t}Manager{/t}'>info_outline</i>"}
                    </div>
                    {/if}
                </div>
            </div>
            {/render}
            {/if}
        </div>

        <div class="col s6">
            {render acl=$stACL}
            <div class="input-field">
                <input type='text' id="st" name="st" size=25 maxlength=60 value="{$st}" title="{t}State where this subtree is located{/t}">
                <label for="st">{t}State{/t}</label>
            </div>
            {/render}

            {render acl=$lACL}
            <div class="input-field">
                <input type='text' id="l" name="l" size=25 maxlength=60 value="{$l}" title="{t}Location of this subtree{/t}">
                <label for="l">{t}Location{/t}</label>
            </div>
            {/render}

            {render acl=$postalAddressACL}
            <div class="input-field">
                <textarea class="materialize-textarea" id="postalAddress" name="postalAddress" title="{t}Postal address of this subtree{/t}">{$postalAddress}</textarea>
                <label for="postalAddress">{t}Address{/t}</label>
            </div>
            {/render}

            {render acl=$telephoneNumberACL}
            <div class="input-field">
                <input type='text' id="telephoneNumber" name="telephoneNumber" size=25 maxlength=60 value="{$telephoneNumber}" title="{t}Base telephone number of this subtree{/t}">
                <label for="telephoneNumber">{t}Phone{/t}</label>
            </div>
            {/render}

            {render acl=$facsimileTelephoneNumberACL}
            <div class="input-field">
                <input type='text' id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=25 maxlength=60 value="{$facsimileTelephoneNumber}" title="{t}Base facsimile telephone number of this subtree{/t}">
                <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
            </div>
            {/render}
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <h3>{t}Administrative settings{/t}</h3>

        <div class="col s6">
            {render acl=$gosaUnitTagACL}
            <label>
                <p>
                    <input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
                    <span>{t}Tag department as an independent administrative unit{/t}</span>
                </p>
            </label>
            {/render}
        </div>
    </div>

    <input type='hidden' name='dep_generic_posted' value='1'>
</div>
