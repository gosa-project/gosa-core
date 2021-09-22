<div class="row valign-wrapper">
  <div class="col s2 attention-icon">
    {image path='<i class="material-icons orange-text">warning</i>'}
  </div>

  <div class="col s10 attention-content">
    <h2>{t}Attention{/t}</h2>

    {$info}

    <div class="attention">
      <label>
        <span>{t}If you're sure you want to do this press 'Delete' to continue or 'Cancel' to abort.{/t}</span>
      </label>
    </div>
  </div>
</div>

<div class="card-action">
  <button class="btn-small primary" type='submit' name='delete_confirmed'>{msgPool type=delButton}</button>
  <button class="btn-small primary" type='submit' name='delete_cancel'>{msgPool type=cancelButton}</button>
</div>
