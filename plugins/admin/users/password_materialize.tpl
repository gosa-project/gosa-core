<script type="text/javascript" src="include/pwdStrength.js"></script>

<div class="content-wrapper card-content-scroll change-password">
  <h2>{t}Change password{/t}</h2>
  <p>
    {t}To change the user password use the fields below. The changes take effect immediately. Please memorize the newpassword, because the user wouldn't be able to login without it.{/t}
  </p>

  <div class="row first-level">
    <div class="col s6 first-level">
      {if $proposalEnabled}
        <div class="row pw-proposal valign-wrapper">
          <div class="col s3">
            <label>
              <input type='radio' value='1' id='proposalSelected_true' name='proposalSelected'
                onClick="changeState('new_password'); changeState('repeated_password');" {if $proposalSelected} checked
                {/if}>
              <span class="black-text">{t}Use proposal{/t}</span>
            </label>
          </div>
          <div class="col s9">
            <div class="input-field col s10 proposal" id='proposalText'>{$proposal}</div>
            <div class="col s2 refresh">{image path='<i class="material-icons">refresh</i>' action='refreshProposal'}
            </div>
          </div>
        </div>

        <div class="row">
          <div class="col s6">
            <label>
              <input type='radio' value='0' name='proposalSelected' id='proposalSelected_false'
                onClick="changeState('new_password'); changeState('repeated_password'); nextfield='new_password'; focus_field('new_password');"
                {if !$proposalSelected} checked {/if}>
              <span>{t}Manually specify a password{/t}</span>
            </label>
          </div>
        </div>
      {/if}

      <div class="row">
        <div class="col s6 input-field password">
          {factory type='password' name='new_password' id='new_password'
          onkeyup="testPasswordCss(\$('new_password').value)" onfocus="nextfield= 'repeated_password';"}
          <div class="password-security">
            <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#ee3d60;display:block;height:3px;">
            </span>
          </div>
          <label for="new_password">{t}New password{/t}</label>
          <i class='material-icons toggle-pw'>visibility</i>
        </div>

        <div class="col s6 input-field password">
          {factory type='password' name='repeated_password' id='repeated_password' onfocus="nextfield=
            'password_finish';"}
          <label for="repeated_password">{t}Repeat new password{/t}</label>
          <i class='material-icons toggle-pw'>visibility</i>
        </div>
      </div>

      <div class="row">
        {if $passwordChangeForceable}
          <div class="col s6 input-field enforcePasswordChange">
            <label>
              <input type='checkbox' name='enforcePasswordChange' value='1' id='enforcePasswordChange' {if
                $enforcePasswordChange} checked {/if}>
              <span>{t}Enforce password change on next login.{/t}</span>
            </label>
          </div>
        {/if}
      </div>
    </div>

    <div class="col s6">
      <div class="col s8 offset-s4">
        <div class="password-conditions">
          <h3>{t}Password requirements{/t}:</p>
          <ul>
            <li class="condition">
              <i id="pw-length" class='material-icons'>close</i>
              {t}The password must have at least {$pwLength} characters.{/t}
            </li>
            {if $lower}
            <li class="condition">
              <i id="pw-lowercase" class='material-icons'>close</i>
              {t}{$lower}{/t}
            </li>
            {/if}
            {if $upper}
            <li class="condition">
              <i id="pw-uppercase" class='material-icons'>close</i>
              {$upper}
            </li>
            {/if}
            {if $digits}
            <li class="condition">
              <i id="pw-numbers" class='material-icons'>close</i>
              {$digits}
            </li>
            {/if}
            {if $special}
            <li class="condition">
              <i id="pw-special" class='material-icons'>close</i>
              {$special} !, @, #, $, %, ^, &, *, ?, _, ~.
            </li>
            {/if}
          </ul>
        </div>
      </div>
    </div>
  </div>
</div>

<div class="card-action">
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
