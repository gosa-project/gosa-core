<script type="text/javascript" src="include/pwdStrength.js"></script>

<div class="system-wrapper">
  <div class="content-wrapper card-content-scroll change-password">
    <h2>{t}System password change{/t}</h2>
    <p>
      {t}To change the terminal root password use the fields below. The changes take effect during the next reboot. Please memorize the new password, because you wouldn't be able to log in.{/t}<br />
      <b>{t}Leave fields blank for password inheritance from default entries.{/t}</b><br />
      {t}Changing the password impinges on authentication only.{/t}
    </p>

    <div class="password-wrapper card-content-scroll">
      <div class="row first-level">
        <div class="col s8 xl6 first-level">
          <div class="row">
            <div class="col s6 input-field password">
              {factory type="password" name="new_password" id="new_password" onfocus="nextfield='repeated_password';"}
              <div class="password-security">
                <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#ee3d60;display:block;height:3px;">
                </span>
              </div>

              <label for="new_password">{t}New password{/t}</label>
              <i class="material-icons toggle-pw">visibility</i>
            </div>

            <div class="col s6 input-field password">
              {factory type="password" name="repeated_password" id="repeated_password" onfocus="nextfield='password_finish';"}
              <label for="repeated_password">{t}Repeat new password{/t}</label>
              <i class="material-icons toggle-pw">visibility</i>
            </div>
          </div>
        </div>

        <div class="col s4 xl6">
          <div class="col s12 xl8 offset-xl4">
            <div class="password-conditions">
              <h3>{t}Password requirements{/t}:</h3>
              <ul>
                <li class="condition">
                  <i id="pw-length" class="material-icons">close</i>
                  {t}The password must have at least {$pwLength} characters.{/t}
                </li>
                {if $lower}
                  <li class="condition">
                    <i id="pw-lowercase" class="material-icons">close</i>
                    {t}{$lower}{/t}
                  </li>
                {/if}
                {if $upper}
                  <li class="condition">
                    <i id="pw-uppercase" class="material-icons">close</i>
                    {$upper}
                  </li>
                {/if}
                {if $digits}
                  <li class="condition">
                    <i id="pw-numbers" class="material-icons">close</i>
                    {$digits}
                  </li>
                {/if}
                {if $special}
                  <li class="condition">
                    <i id="pw-special" class="material-icons">close</i>
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
</div>

<div class="plugin-actions card-action">
  <button class="btn-small primary" type="submit" id="password_finish" name="password_finish">{t}Set password{/t}</button>
  <button class="btn-small primary" type="submit" id="password_cancel" name="password_cancel">{msgPool type=cancelButton}</button>
</div>

<input type="hidden" id="formSubmit">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  nextfield = 'new_password';
  focus_field('new_password');
</script>
