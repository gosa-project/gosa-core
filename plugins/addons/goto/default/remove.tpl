<div class="goto-remove-wrapper">
    <div class="valign-wrapper">
        {image path="<i class='material-icons orange-text'>warning</i>" align="top"}{t}Warning{/t}
    </div>
    
    <p>{$info}</p>
    
    <p>{t}So - if you're sure - press 'Delete' to continue or 'Cancel' to abort.{/t}</p>
    
    <div class="card-action">
        {if $multiple}
        <button class="btn-small" type='submit' name='delete_multiple_confirm'>{msgPool type=delButton}</button> 
        {else}
        <button class="btn-small" type='submit' name='delete_confirm'>{msgPool type=delButton}</button> 
        {/if}
        <button class="btn-small" type='submit' name='delete_cancel'>{msgPool type=cancelButton}</button>
    </div>
</div>