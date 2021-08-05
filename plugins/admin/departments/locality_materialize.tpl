<!--//////////////////////	LOCALITY (l)    //////////////////////-->

<span class="card-title">{t}Properties{/t}</span>

<div class="input-field">
    {render acl=$lACL}
    <input type='text' id="l" name="l" size=25 maxlength=60 value="{$l}" title="{t}Name of locality to create{/t}">
    {/render}
    <label for="c">{t}Locality name{/t}{$must}</label>
</div>

<div class="input-field">
    {render acl=$descriptionACL}
    <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}"
        title="{t}Descriptive text for department{/t}">
    {/render}
    <label for="description">{t}Description{/t}{$must}</label>
</div>

{if !$is_root_dse}
<div class="row">
    {render acl=$baseACL}
    <div class="input-field col s3">
        {$base}
    </div>
    {/render}
</div>
{/if}

{if $manager_enabled}
<div class="row valign-wrapper">
    <div class="col s8">
        <div class="input-field">
            {render acl=$managerACL}
            <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled
                title='{$manager}'>
            {/render}
            <label for="manager">{t}Manager{/t}</label>
        </div>
    </div>

    <div class="col s4">
        {render acl=$managerACL}
        {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
        {/render}

        {if $manager!=""}
        {render acl=$managerACL}
        {image path="<i class='material-icons input-icons md-24 tooltipped' data-postion='bottom'
            data-tooltip='{$manager}'>info</i>" title="{$manager}"}
        {/render}

        {render acl=$managerACL}
        {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
        {/render}
        {/if}
    </div>
</div>
{/if}

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