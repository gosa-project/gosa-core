<h3>{t}Command verifier{/t}</h3>

<p>
 {t}The command to check for{/t}
 <input type='text' name='command' value="{$value}" style='width: 600px;'>
 <button type='submit' name='execute'>{t}Test{/t}</button> 
</p>

<hr>
  {$output}
<hr>
<div class="plugin-actions">
    <button type='submit' name='commandVerifier_save'>{msgPool type='okButton'}</button>
    <button type='submit' name='commandVerifier_cancel'>{msgPool type='cancelButton'}</button>
</div>
