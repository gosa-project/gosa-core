<h2>{t}Command verifier{/t}</h2>
<p>
    {t}Here you can execute commands in the way GOsa does and check the generated results or errors. This can be very useful especially for the post events (postcreate, postmodify and postremove) due to the fact that these hook are executed silently.{/t}
</p>

<p>
    <b>
        {t}Please be careful here, all commands will really be executed on your machine and may break things!{/t}
    </b>
</p>

<hr>

<div class="col s12 valign-wrapper command">
    <div class="col s2">
        {t}The command to check for{/t}
    </div>
    <div class="input-field col s6">
        <input class="textarea" id="command" type='text' name='command' value="{$value}" style='font-family:monospace;'>
    </div>
</div>

<button class="btn-small" type='submit' name='execute'>{t}Test{/t}</button>

{if $output}
{$output}
{/if}

<div class="card-action">
    <button class="btn-small gonicus-color" type='submit' name='commandVerifier_save'>{msgPool type='okButton'}</button>
    <button class="btn-small gonicus-color" type='submit' name='commandVerifier_cancel'>{msgPool type='cancelButton'}</button>
</div>