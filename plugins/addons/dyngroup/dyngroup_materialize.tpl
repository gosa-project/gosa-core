<h5>{t}List of dynamic rules{/t}</h5>

{foreach item=item key=key from=$labeledURIparsed}

<div class="row valign-wrapper">
    <div class="input-field col s3">
        <input style='width:98%;' type='text' value='{$item.base}' name='base_{$key}' id="base_{$key}">
        <label for="base_{$key}">{t}Base{/t}</label>
    </div>
    <div class="input-field col s2">
        <select name='scope_{$key}' size='1'>
            {html_options options=$scopes selected=$item.scope}
        </select>
        <label>{t}Scope{/t}</label>
    </div>
    <div class="input-field col s2">
            <input type='text' name='attr_{$key}' value='{$item.attr}' id="attr_{$key}">
            <label for="attr_{$key}">{t}Attribute{/t}</label>
    </div>
    <div class="input-field col s3">
        <input name='filter_{$key}' type='text' style='width:98%;' value='{$item.filter}' id="filter_{$key}">
        <label for="filter_{$key}">{t}Filter{/t}</label>
    </div>
    <div class="col s2">
        <button class="btn-small gonicus-color" name='delUri_{$key}'>{msgPool type='delButton'}</button>
    </div>
</div>

{/foreach}
<div class="row">
    <button class="btn-small gonicus-color" name='addUri'>{msgPool type='addButton'}</button>
</div>

<script type="text/javascript">
    document.addEventListener('DOMContentLoaded', function() {
        var elems = document.querySelectorAll('select');
        var instances = M.FormSelect.init(elems, {});
    });
</script>
