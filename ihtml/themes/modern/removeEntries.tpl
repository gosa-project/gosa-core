<div class=plugin>
 <div class='inline-warning'>
  {image path="images/warning.png" align="top"}
  <div class='inline-warning-text'>{t}Attention{/t}</div>
 </div>

 <hr>

 <div class='object-list-container'>
  {$info}
 </div>

 <p>
  {t}If you're sure you want to do this press 'Delete' to continue or 'Cancel' to abort.{/t}
 </p>

 <hr>

 <div class="plugin-actions">
  <button type='submit' name='delete_confirmed'>{msgPool type=delButton}</button>
  <button type='submit' name='delete_cancel'>{msgPool type=cancelButton}</button>
 </div>
</div>
