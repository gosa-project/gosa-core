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

          <button class="btn-small primary" type="submit" name="login" id="login"
            title="{t}Click here to log in{/t}">{t}Log in{/t}</button>
        </div>

      </div>
      <input type='hidden' name='javascript' value='false' />

    </form>
  </div>

  <script type="text/javascript" src="themes/default/js/materialize.js"></script>
  <script type="text/javascript" defer src="themes/default/js/eventListener.js"></script>

  <script type="text/javascript">
    document.cookie = "gosatest=empty;path=/;samesite=lax";
    if (document.cookie.indexOf("gosatest=") > -1)
      document.cookie = "gosatest=empty;path=/;expires=Thu, 01-Jan-1970 00:00:01 GMT;samesite=lax";
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
