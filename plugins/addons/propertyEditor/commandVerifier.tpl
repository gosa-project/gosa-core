<h3>{t}Command verifier{/t}</h3>
<p>
 {t}Here you can execute commands in the way GOsa does and check the generated results or errors. This can be very useful especially for the post events (postcreate, postmodify and postremove) due to the fact that these hook are executed silently.{/t}
</p>

<p>
 <b>
  {t}Please be careful here, all commands will really be executed on your machine and may break things!{/t}
 </b>
</p>

<hr>

<p>
 {t}The command to check for{/t}
 <input type='text' name='command' value="{$value}" style='width: 600px;'>
 <button type='submit' name='execute'>{t}Test{/t}</button> 
</p>

{if $output}
 <hr>
 {$output}
{/if}

<hr>

<div class="plugin-actions">
    <button type='submit' name='commandVerifier_save'>{msgPool type='okButton'}</button>
    <button type='submit' name='commandVerifier_cancel'>{msgPool type='cancelButton'}</button>
</div>
