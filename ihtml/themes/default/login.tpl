<body>
  {$php_errors}
  <nav class="accent">
    <div class="nav-wrapper">
      <div class="brand-logo valign-wrapper">
        <img class="responsive-img" src="themes/default/images/logo.png">
      </div>
      <ul class="right hide-on-med-and-down">
        <li>
          {$version}
        </li>
      </ul>
    </div>
  </nav>

  <div class="container">

    <form action='index.php' method='post' name='mainform' id='mainform' onSubmit='js_check(this);return true;'>
      {if $otp_active && !$tfa_active}
      <div class="card tfa-wrapper">
        <div class="card-content">
          <h4>Do you want to activate two factor authentication?</h4>
          <p>Adding two-step verification keeps your account secure, even if your password is stolen.</p>
          <br>
          <p>
            After you have activated the two-step verification for your account, you will first need to log in as
            usual. In the second step you will be sent a one-time password (OTP). You can enter/scan this manually or
            via a QR code into a two-factor authenticator. The generated 6-digit code is then entered for the second
            verification.
          </p>
          <br>
          <p>
            For example, the Google Authenticator is suitable for authentication. Downloadable from the Google App
            Store for Android or the Apple Store for Apple smartphones.
          </p>
        </div>
        <div class="card-action">
          <button class="btn-small primary" type="submit" name="tfa_params" id="tfa_params">
            Two-factor-authentication enable
          </button>
          <button class="btn-small primary" type="submit" name="forward-login" id="forward-login"
            title="{t}Click here to log in{/t}">
            {t}Log in{/t}
          </button>
        </div>
      </div>
      {elseif $tfa_active}
      <div class="card tfa-auth-wrapper">
        <div class="card-content">
          <h2 class="center-align qr-headline">Two-factor-authentication</h2>

          <div class="qr-wrapper">
            <img class="materialboxed center-align" src="{$qr_code}" alt="qr code is broken or emtpy">
          </div>

          <div class="qr-hint">
            Please install Google Authenticator App in your phone, open it and then scan the QR-Code to add
            GOsa-Application authentication. After you have installed this application on your smartphone enter the code from the Google
            Authenticator into the input box to complete the verification process.
          </div>

          <div class="input-field add">
            <i class="material-icons prefix ">lock</i>
            <input type="text" name="tfa_code" id="icon_prefix" class="validate" autocomplete="off">
            <button class="btn-small primary" type="submit" name="verify_tfa"
              title="Click to verify the authentication code and log in">
              Verify
            </button>
            <label for="icon_prefix">Enter Code</label>
          </div>

          {if $message || $errors}
          <div class="attention">
            <i class="material-icons">error</i>

            {if $message}
            <div class='center-align'>{$message}</div>
            {/if}
            {if $errors}
            <div class='center-align'>{$errors}</div>
            {/if}
          </div>
          {/if}
        </div>
      </div>
      {else}
      {$msg_dialogs}

      {if $ssl}
      <div class='card-panel attention'>
        <i class="material-icons">warning</i>
        {$ssl}
      </div>
      {/if}
      {if $lifetime}
      <div class='card-panel attention'>
        <i class="material-icons">warning</i>
        {$lifetime}
      </div>
      {/if}

      <div class='card login-wrapper'>
        <div class="card-content">
          <h1>{t}Login to GOsa{/t}</h1>

          <div class="container">
            <div class="input-field login">
              <input type="text" name="username" maxlength="40" data-length="40" value="{$username}"
                onfocus="nextfield = 'password'">
              <label for="username">{t}User name{/t}</label>
            </div>
            <div class="input-field login">
              <input type='password' name='password' id='password' onfocus="nextfield = 'login'">
              <label for="password">{t}Password{/t}</label>
            </div>
          </div>
        </div>


        {if $message || $errors}
        <div class="attention">
          <i class="material-icons">error</i>

          {if $message}
          <div class='center-align'>{$message}</div>
          {/if}
          {if $errors}
          <div class='center-align'>{$errors}</div>
          {/if}
        </div>
        {/if}

        <div class="card-action">
          <div class="input-field">
            <select name="server" title="{t}Choose the directory to work on{/t}">
              {html_options options=$server_options selected=$server_id}
            </select>
          </div>

          <button class="btn-small primary" type="submit" name="login" id="login" title="{t}Click here to log in{/t}">
            {t}Log in{/t}
          </button>
        </div>
      </div>
      {/if}
      <input type='hidden' name='javascript' value='false' />
      {if $username}
      <input type="hidden" name="username" value="{$username}">
      {/if}
      {if $password}
      <input type="hidden" name="password" value="{$password}">
      {/if}
      {if $server}
      <input type="hidden" name="server" value="{$server}">
      {/if}
    </form>
  </div>


  <script type="text/javascript" src="themes/default/js/materialize.js"></script>
  <script type="text/javascript" defer src="themes/default/js/eventListener.js"></script>

  <script type="text/javascript">
    document.cookie = "gosatest=empty;path=/";
    if (document.cookie.indexOf("gosatest=") > -1)
      document.cookie = "gosatest=empty;path=/;expires=Thu, 01-Jan-1970 00:00:01 GMT";
    else
      document.write("<div class='card-panel red lighten-3'>{$cookies}<\/div>");
  </script>

  <script language="JavaScript" type="text/javascript">
    enable_keyPress = false;
    nextfield = "{$nextfield}";
    focus_field("{$nextfield}");
    next_msg_dialog();
  </script>

</body>

</html>