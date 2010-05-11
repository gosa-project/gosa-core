

<h3>{t}Command property verifier{/t}</h3>

<p>
    {t}The command to check for{/t}
    <input type='text' name='command' value="{$value}" style='width: 100%;'>
</p>

<p>
    {t}The generated output{/t}
    {$output}
</p>

<button type='submit' name='execute'>{t}Test{/t}</button> 

<hr>
<div class="plugin-actions">
    <button type='submit' name='commandVerifier_save'>{msgPool type='okButton'}</button>
    <button type='submit' name='commandVerifier_cancel'>{msgPool type='cancelButton'}</button>
</div>
