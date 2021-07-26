<!--//////////////////////      LOCALITY (l)      //////////////////////-->

<div class="card no-shadow">
    <div class="card-content">
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
            {$base}
            {/render}
        </div>

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

                <script type="text/javascript">
                    document.addEventListener('DOMContentLoaded', function () {
                        var elems = document.querySelectorAll('.tooltipped');
                        var instances = M.Tooltip.init(elems, {});
                    });
                </script>
                {/if}
            </div>
        </div>
        {/if}


        <hr>
        <h5>{t}Administrative settings{/t}</h5>
        {render acl=$gosaUnitTagACL}
        <label>
            <input type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
            <span>{t}Tag department as an independent administrative unit{/t}</span>
        </label>
        {/render}

    </div>
</div>

<input type='hidden' name='dep_generic_posted' value='1'>