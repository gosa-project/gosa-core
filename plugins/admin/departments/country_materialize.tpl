<!--//////////////////////      COUNTRY (c)     ////////////////////// -->

<div class="locality card-content-scroll">
    <h2>{t}Properties{/t}</h2>

    <div class="row content">
        {render acl=$cACL}
            <div class="col s6">
                <div class="input-field">
                    <input type='text' id="c" name="c" size=25 maxlength=60 value="{$c}" title="{t}Name of country to create{/t}">
                    <label for="c">{t}Country name{/t}{$must}</label>
                </div>
            </div>
        {/render}

        {render acl=$descriptionACL}
            <div class="col s6">
                <div class="input-field">
                    <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for department{/t}">
                    <label for="description">{t}Description{/t}{$must}</label>
                </div>
            </div>
        {/render}

        {render acl=$baseACL}
            {if !$is_root_dse}
                <div class="col s6">
                    <div class="input-field ldap-tree">
                        {$base}
                    </div>
                </div>
            {/if}
        {/render}

        {if $manager_enabled}
            {render acl=$managerACL}
                <div class="col s6">
                    <div class="input-field manager">
                        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
                        <label for="manager">{t}Manager{/t}</label>

                        <div class="icons">
                            <div class="icon">
                                {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
                            </div>

                            {if $manager!=""}
                                <div class="icon">
                                    {image path="<i class='material-icons input-icons tooltipped' data-postion='bottom' data-tooltip='{$manager}'>info</i>" title="{$manager}"}
                                </div>

                                <div class="icon">
                                    {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
                                </div>
                            {else}
                                <div class="icon">
                                    {image path="<i class='material-icons md-disabled'>delete</i>"}
                                </div>

                                <div class="icon">
                                    {image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom' data-tooltip='{t}Manager{/t}'>info</i>"}
                                </div>
                            {/if}
                        </div>
                    </div>
                </div>
            {/render}
        {/if}
    </div>

    <hr class="divider">

    <h3>{t}Administrative settings{/t}</h3>

    {render acl=$gosaUnitTagACL}
        <p>
            <label>
                <input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
                <span>{t}Tag department as an independent administrative unit{/t}</span>
            </label>
        </p>
    {/render}

    <input type='hidden' name='dep_generic_posted' value='1'>
</div>
