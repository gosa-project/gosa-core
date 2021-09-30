<div class='islocked-wrapper'>
    <div class='row valign-wrapper'>
        {image path="<i class='material-icons'>lock</i>" align="top"}
        <h2 style="margin: unset;">{t}Locking conflict detected{/t}</h2>
    </div>

    <hr class="divider">

    <p>{$message}</p>

    <p>
        {t}If this lock detection is false, the other person has obviously closed the web browser during the edit operation. You may want to take over the lock by pressing the 'Edit anyway' button.{/t}
    </p>

    <hr>

    <div class='card-action'>
        <button class="btn-small" type="submit" name="delete_lock">{$action}</button> 
        {if $allow_readonly}
        <button class="btn-small" type="submit" name="open_readonly">{t}Read only{/t}</button> 
        {/if}
        <button class="btn-small" type="submit" name="cancel_lock">{t}Cancel{/t}</button>
    </div>

    <input type="hidden" name="dn" value="{$dn}">
</div>