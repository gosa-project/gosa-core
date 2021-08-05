{image path='images/warning.png'}
<hr>
<p>
 {$Warning}
</p>
 {t}Please double check if your really want to do this since there is no way for GOsa to get your data back.{/t}
 {t}Best thing to do before performing this action would be to save the current script in a file. So - if you've done so - press 'Delete' to continue or 'Cancel' to abort.{/t}
</p>

<hr>
<div class="plugin-actions">
  <button type=submit name="delete_script_confirm">{msgPool type=delButton}</button>
  <button type=submit name="delete_script_cancel">{msgPool type=cancelButton}</button>
</div>

