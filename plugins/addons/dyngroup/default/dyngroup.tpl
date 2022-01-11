<div class="dyngroup-wrapper">
    <h3>{t}List of dynamic rules{/t}</h3>

    {foreach item=item key=key from=$labeledURIparsed name=dynamicRules}

        <div class="row valign-wrapper">
            <div class="input-field col s6 xl3">
                <input style='width:98%;' type='text' value='{$item.base}' name='base_{$key}' id="base_{$key}">
                <label for="base_{$key}">{t}Base{/t}</label>
            </div>
            <div class="input-field col s6 xl3">
                <select name='scope_{$key}' size='1'>
                    {html_options options=$scopes selected=$item.scope}
                </select>
                <label>{t}Scope{/t}</label>
            </div>
            <div class="input-field col s6 xl3">
                    <input type='text' name='attr_{$key}' value='{$item.attr}' id="attr_{$key}">
                    <label for="attr_{$key}">{t}Attribute{/t}</label>
            </div>
            <div class="input-field col s6 xl3">
                <input name='filter_{$key}' type='text' style='width:98%;' value='{$item.filter}' id="filter_{$key}">
                <label for="filter_{$key}">{t}Filter{/t}</label>
            </div>
            <div class="delete-btn col s3 offset-s3 xl1">
                <button class="btn-small" name='delUri_{$key}'>{msgPool type='delButton'}</button>
            </div>
        </div>
        {if not $smarty.foreach.dynamicRules.last}
            <hr class="divider">
        {/if}
    {/foreach}

    <div class="action">
        <button class="btn-small" name='addUri'>{msgPool type='addButton'}</button>
    </div>
</div>
