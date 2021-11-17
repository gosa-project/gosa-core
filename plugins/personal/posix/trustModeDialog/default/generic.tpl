<h3>{t}System trust{/t}</h3>

{if !$multiple_support}
    <div class="input-field">
        {render acl=$trustmodeACL}
        <select name="trustmode" id="trustmode" size=1
            onchange="changeSelectState('trustmode', 'add_ws');   changeSelectState('trustmode', 'del_ws');">
            {html_options options=$trustmodes selected=$trustmode}
        </select>
        {/render}
        <label>{t}Trust mode{/t}</label>
    </div>
    {render acl=$trustmodeACL}
    {$trustList}
    {/render}
    {render acl=$trustmodeACL}
    <button class="btn-small" {$trusthide}type='submit' name='add_ws' id="add_ws">
        {msgPool type=addButton}</button>
    {/render}

    {else}

    <div class="input-field">
        <input type="checkbox" name="use_trustmode" {if $use_trustmode} checked {/if}
            onClick="$('div_trustmode').toggle();">
        <span>{t}Trust mode{/t}</span>
    </div>

    <div {if !$use_trustmode} style="display: none;" {/if} id="div_trustmode">
        {render acl=$trustmodeACL}
        <select name="trustmode" id="trustmode" size=1
            onchange="changeSelectState('trustmode', 'add_ws'); changeSelectState('trustmode', 'del_ws');">
            {html_options options=$trustmodes selected=$trustmode}
        </select>
        {/render}
        {render acl=$trustmodeACL}
        {$trustList}
        {/render}
        {render acl=$trustmodeACL}
        <button class="btn-small" type='submit' name='add_ws' id="add_ws">
            {msgPool type=addButton}</button>
        {/render}
    </div>
{/if}
