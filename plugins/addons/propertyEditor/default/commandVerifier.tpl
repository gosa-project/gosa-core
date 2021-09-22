<div class="command-verification">
    <h2>{t}Command verifier{/t}</h2>
    <p>
        {t}Here you can execute commands in the way GOsa does and check the generated results or errors. This can be very useful especially for the post events (postcreate, postmodify and postremove) due to the fact that these hook are executed silently.{/t}
    </p>

    <p>
        <b>
            {t}Please be careful here, all commands will really be executed on your machine and may break things!{/t}
        </b>
    </p>

    <hr class="divider">

    <div class="row">
        <div class="col s6 command">
            <div class="input-field">
                <input id="command" type='text' name='command' value="{$value}">
                <button class="btn-small" type='submit' name='execute'>{t}Test{/t}</button>
                <label for="command">{t}The command to check for{/t}</label>
            </div>
        </div>
    </div>

    {if $output}
        {$output}
    {/if}

    <div class="card-action">
        <button class="btn-small primary" type='submit' name='commandVerifier_save'>{msgPool type='okButton'}</button>
        <button class="btn-small primary" type='submit' name='commandVerifier_cancel'>{msgPool type='cancelButton'}</button>
    </div>
</div>
