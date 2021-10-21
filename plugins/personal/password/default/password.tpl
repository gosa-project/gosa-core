
{if $SASL}

  <br>
  <b>{t}Your password cannot be changed from within GOsa{/t}</b>

  <input type="hidden" name="ignore">
{else}
  <script type="text/javascript" src="include/pwdStrength.js"></script>

  <div class="content-wrapper card-content-scroll change-password">
    <h2>{t}Change password{/t}</h2>
    <p>
      {t}To change your personal password use the fields below. The changes take effect immediately. Please memorize the new password, because you wouldn't be able to login without it.{/t}
    </p>

    {if $passwordExpired}
      <b>
        <p style="color:red">{t}Your Password has expired. Please choose a new password.{/t}</p>
      </b>
      <hr class="divider">
    {/if}

    <div class="password-wrapper card-content-scroll">
      <div class="row first-level">
        <div class="col s8 xl6 first-level">
          <div class="row">
            <div class="col s6 input-field password">
              {factory class='left' type='password' name='current_password' id='current_password' onfocus="nextfield='new_password';"}
              <label for="current_password">{t}Current password{/t}</label>
              <i class='material-icons toggle-pw'>visibility</i>
            </div>
          </div>

          {if $proposalEnabled}
            <div class="pw-proposal">
              <label>
                <p>
                  <input type='radio' value='1' id='proposalSelected_true' name='proposalSelected'
                    onClick="changeState('new_password'); changeState('repeated_password');" {if $proposalSelected} checked{/if}>
                  <span>{t}Use proposal{/t}</span>

                  <div class="input-field proposal" id='proposalText'>{$proposal}</div>
                  <div class="refresh">
                    {image path='<i class="material-icons">refresh</i>' action='refreshProposal'}
                  </div>
                </p>
              </label>

              <label>
                <p>
                  <input type='radio' value='0' name='proposalSelected' id='proposalSelected_false'
                    onClick="changeState('new_password'); changeState('repeated_password'); nextfield='new_password'; focus_field('new_password');"
                    {if !$proposalSelected} checked {/if}>
                  <span>{t}Manually specify a password{/t}</span>
                </p>
              </label>
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
        </div>

        <div class="col s4 xl6">
          <div class="col s12 xl8 offset-xl4">
            <div class="password-conditions">
              <h3>{t}Password requirements{/t}:</h3>
              <ul>
                <li class="condition">
                  <i id="pw-length" class='material-icons'>close</i>
                  {$length}
                </li>
                {if $lower}
                  <li class="condition">
                    <i id="pw-lowercase" class='material-icons'>close</i>
                    {$lower}
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
  </div>

  <div class="card-action">
    <button class="btn-small primary" type='submit' id="password_finish" name='password_finish' disabled>{t}Set password{/t}</button>
    <button class="btn-small primary" type=reset id="password_cancel" name="password_cancel">{t}Clear fields{/t}</button>
  </div>

  <input type='hidden' id='formSubmit'>

  <input type="hidden" name="ignore">

  <!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    nextfield = 'current_password';
    focus_field('current_password');
  </script>

{/if}
