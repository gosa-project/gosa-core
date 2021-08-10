<!--//////////////////////      LOCALITY (l)      //////////////////////-->

<span class="card-title">{t}Properties{/t}</span>

<div class="input-field">
    {render acl=$dcACL}
    <input type='text' id="dc" name="dc" value="{$dc}" title="{t}Name of domain to create{/t}">
    <label for="dc">{t}Domain name{/t}{$must}</label>
    {/render}
</div>

<div class="input-field">
    {render acl=$descriptionACL}
    <input type='text' id="description" name="description" value="{$description}"
        title="{t}Descriptive text for department{/t}">
    <label for="description">{t}Description{/t}</label>
    {/render}
</div>

<div class="row">
    {render acl=$baseACL}
    <div class="input-field col s4 ldap-tree">
        {$base}
    </div>
    {/render}
</div>

{if $manager_enabled}
<div class="row">
    <div class="input-field col s4 manager">
        {render acl=$managerACL}
        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
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
</div>

{/if}


<hr>
<span class="card-title">{t}Administrative settings{/t}</span>
{render acl=$gosaUnitTagACL}
<label>
    <input type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
    <span>{t}Tag department as an independent administrative unit{/t}</span>
</label>
{/render}


<input type='hidden' name='dep_generic_posted' value='1'>