{if $readOnly}
<div class='card-action'>
    <button class='btn-small primary' type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
</div>
{else}
<div class='card-action'>
    <button class='btn-small primary' type='submit' name='edit_finish'>{msgPool type=okButton}</button>
    {if $displayApplyBtn}
    <button class='btn-small primary' type='submit' name='edit_apply'>{msgPool type=applyButton}</button>
    {/if}
    <button class='btn-small primary' type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
</div>
{/if}
