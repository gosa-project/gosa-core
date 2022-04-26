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
        {if $two_factor_auth}
        <div class="card-content">
          <h1>{t}Two factor authentication{/t}</h1>

          <div class="container">
            <div class="row">
              <div class="col s12 xl10">
                <p>
                  {t}Your two factor authentication is active.{/t}
                </p>
                <p>
                  {t}To successfully verify the login you need to enter the six-digit authentication code.{/t}
                </p>
                <p>
                  {t}If you do not have an authentication code or the current code is not correct, contact an administrator.{/t}
                </p>
              </div>

              <div class="col s12 xl2">
                <div class="input-field auth">
                  <input type="text" name="otp_secret" maxlength="6" data-length="6" oninput="check_length(this.value)">
                  <label for="otp_token">{t}Authentication code{/t}</label>
                </div>
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
            <button class="btn-small primary" type="submit" name="verify" id="verify" title="{t}Verify two factor authentication{/t}" disabled>
              {t}Log in{/t}
            </button>
            <button class="btn-small primary" type="submit" onclick="cancel_auth()">{t}Cancel{/t}</button>
          </div>

        </div>
        {else}
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
        {/if}
      </div>
      <input type='hidden' name='javascript' value='false' />
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

  <script type="text/javascript">
    function cancel_auth() {
      window.location.href = "/"
    }

    function check_length(code) {
      let button = document.getElementById('verify')
      if (code.length >= 6) {
        button.disabled = false;
      } else {
        button.disabled = true;
      }
    }
  </script>

  <script language="JavaScript" type="text/javascript">
    enable_keyPress = false;
    nextfield = "{$nextfield}";
    focus_field("{$nextfield}");
    next_msg_dialog();
  </script>

</body>

</html>