<script type="text/javascript" src="include/pwdStrength.js"></script>

<h5>
  {t}To change the user password use the fields below. The changes take effect immediately. Please memorize the newpassword, because the user wouldn't be able to login without it.{/t}
</h5>

<hr>

<div class="container">

  {if $proposalEnabled}
  <div class="row valign-wrapper">
    <div class="col s3">
      <p>
        <label>
          <input type='radio' value='1' id='proposalSelected_true' name='proposalSelected'
            onClick="changeState('new_password'); changeState('repeated_password');" {if $proposalSelected} checked
            {/if}>
          <span class="black-text">{t}Use proposal{/t}</span>
        </label>
      </p>
    </div>
    <div class="col s9">
      <div class="input-field col s10 proposal" id='proposalText'>{$proposal}</div>
      <div class="input-field col s2">{image path='<i class="material-icons">refresh</i>' action='refreshProposal'}
      </div>
    </div>
  </div>

  <div class="row">
    <div class="col s3">
      <p>
        <label>
          <input type='radio' value='0' name='proposalSelected' id='proposalSelected_false'
            onClick="changeState('new_password'); changeState('repeated_password'); nextfield='new_password'; focus_field('new_password');"
            {if !$proposalSelected} checked {/if}>
          <span>{t}Manually specify a password{/t}</span>
        </label>
      </p>
    </div>
  </div>
  {/if}

  <div class="row valign-wrapper">
    <div class="col s3">
      {t}New password{/t}
    </div>
    <div class="input-field col s9">
      {factory type='password' name='new_password' id='new_password'
      onkeyup="testPasswordCss(\$('new_password').value)" onfocus="nextfield= 'repeated_password';"}
      <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#ee3d60;display:block;height:3px;">
        <span id="meterFull"
          style="padding:0;margin:0;z-index:100;width:0;background-color:#49af49;display:block;height:3px;"></span>
      </span>
      <label for="new_password"></label>
    </div>
  </div>

  <div class="row valign-wrapper">
    <div class="col s3">
      {t}Repeat new password{/t}
    </div>
    <div class="input-field col s9">
      {factory type='password' name='repeated_password' id='repeated_password' onfocus="nextfield=
      'password_finish';"}
      <label for="repeated_password"></label>
    </div>
  </div>

  {if $passwordChangeForceable}
  <div class="input-field">
    <label>
      <input type='checkbox' name='enforcePasswordChange' value='1' id='enforcePasswordChange' {if
        $enforcePasswordChange} checked {/if}>
      <span>{t}Enforce password change on next login.{/t}</span>
    </label>
  </div>
  {/if}

  <hr>

  <button class="btn-small gonicus-color" type='submit' name='password_finish'>{t}Set password{/t}</button>
  <button class="btn-small gonicus-color mg2" type='submit' id='password_cancel' name='password_cancel'>{msgPool
    type=cancelButton}</button>
</div>

<input type='hidden' id='formSubmit'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  nextfield = 'current_password';
  focus_field('current_password');
</script>