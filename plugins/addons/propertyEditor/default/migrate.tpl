<hr class="divider">
<div>
  <span class="black-text"><h5>{t}Property migration assistant{/t} - {t}Migration steps left{/t}: {$leftSteps}</h5></span>
</div>
<hr class="divider">
<div class="row valign-wrapper">
    {$content}
</div>
<hr>

<div class="card-action">
    <button class="btn-small primary" name='propertyMigrate_save'>{msgPool type='okButton'}</button>
    <button class="btn-small primary" name='propertyMigrate_cancel'>{msgPool type='cancelButton'}</button>
   </div>
<input type="hidden" name="ignore">
