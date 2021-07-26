<h5>{t}User picture{/t}</h5>
<div class="grey">
  <img src="getbin.php?rand={$rand}" alt='' class="materialboxed h-center">
</div>

<div class="file-field input-field inline">
  <div class="btn-small gonicus-color">
    <span>{t}Browse{/t}</span>
    <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
    <input id="picture_file" name="picture_file" type="file" accept="image/*.jpg">
  </div>
  <div class="file-path-wrapper">
    <input placeholder='{t}No picture selected{/t}' type="text" class="file-path validate">
  </div>
  <button class="btn-small gonicus-color" type='submit' name='picture_remove'>{t}Remove picture{/t}</button>
</div>

<hr>

<div>
  <button class="btn-small gonicus-color" type='submit' name='picture_edit_finish'>{msgPool type=saveButton}</button>
  <button class="btn-small gonicus-color" type='submit' name='picture_edit_cancel'>{msgPool type=cancelButton}</button>
</div>