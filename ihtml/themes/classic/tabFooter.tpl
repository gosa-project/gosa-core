{if $readOnly}
<p style='text-align:right'>
    <button type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
</p>
{else}
<p style='text-align:right'>
    <button type='submit' name='edit_finish'>{msgPool type=okButton}</button>
    {if $displayApplyBtn}
    <button type='submit' name='edit_apply'>{msgPool type=applyButton}</button>
    {/if}
    <button type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
{/if}
