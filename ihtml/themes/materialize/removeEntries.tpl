<div class="card z-depth-0">
    <div class="card-content">
        <span class="card-title">{image path="<i class='material-icons md-48 orange-text'>warning</i>"
            align="top"}{t}Attention{/t}</span>

        <hr>

        {$info}

        <span class="red lighten-3">
            {t}If you're sure you want to do this press 'Delete' to continue or 'Cancel' to abort.{/t}
        </span>

        <hr>

        <div class="card-action">
            <button class="btn-small gonicus-color" type='submit' name='delete_confirmed'>{msgPool
                type=delButton}</button>
            <button class="btn-small gonicus-color" type='submit' name='delete_cancel'>{msgPool
                type=cancelButton}</button>
        </div>
    </div>
</div>