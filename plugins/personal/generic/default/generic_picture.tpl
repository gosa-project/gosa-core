<div class="user-image">
    <h3>{t}User picture{/t}</h3>
    <div class="grey preview-image">
        <img src="getbin.php?rand={$rand}" alt='' class="materialboxed">
    </div>

    <div class="image-action">
        <button class="btn-small" type='submit' name='picture_remove'>{t}Remove picture{/t} </button>

        <div class="file-field input-field">
            <div class="btn-small">
                <span>{t}Browse{/t}</span>
                <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
                <input id="picture_file" name="picture_file" type="file" accept="image/*.jpg">
            </div>
            <div class="file-path-wrapper">
                <input placeholder='{t}No picture selected{/t}' type="text" class="file-path validate">
            </div>
        </div>
    </div>

    <div class="card-action">
        <button class="btn-small primary" type='submit' name='picture_edit_finish'>{msgPool type=saveButton}</button>
        <button class="btn-small primary" type='submit' name='picture_edit_cancel'>{msgPool type=cancelButton}</button>
    </div>
</div>
