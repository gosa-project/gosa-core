<body>
  {$php_errors}
  <nav class="gonicus-color">
    <div class="nav-wrapper">
      <div class="brand-logo valign-wrapper">
        <img class="responsive-img" src="themes/default/images/logo.png">
      </div>
      <ul class="right hide-on-med-and-down">
        <li class="mg2">
          {$version}
        </li>
      </ul>
    </div>
  </nav>

  <div class="container">

    <form action='index.php' method='post' name='mainform' id='mainform' onSubmit='js_check(this);return true;'>
      {$msg_dialogs}

      {if $ssl}<div class='card-panel red lighten-3'>{$ssl}</div>{/if}
      {if $lifetime}<div class='card-panel red lighten-3'>{$lifetime}</div>{/if}

      <div class='card'>
        <div class="row">
          <div class="card-content">
            <span class="card-title black-text">{t}Login to GOsa{/t}</span>
          </div>
        </div>

        <div class="container">
          <div class="row">
            <div class="input-field col s12">
              <input type="text" name="username" maxlength="40" data-length="40" value="{$username}" onfocus="nextfield = 'password'">
              <label for="username">{t}User name{/t}</label>
            </div>
          </div>
          <div class="row">
            <div class="input-field col s12">
              <input type='password' name='password' id='password' onfocus="nextfield = 'login'">
              <label for="password">{t}Password{/t}</label>
            </div>
          </div>
        </div>

        <div class="row">
          {if $message}
          <hr>
          <div class='center-align red-text'>{$message}</div>{/if}
          {if $errors}
          <hr>
          <div class='center-align red-text'>{$errors}</div>{/if}
          <hr>
        </div>

        <div class="row">
          <div class="input-field col s2">
            <select name="server" title="{t}Choose the directory to work on{/t}">
              {html_options options=$server_options selected=$server_id}
            </select>
          </div>

          <div class="col s2 offset-s8">
            <p class="center-align">
              <button class="btn-small gonicus-color" type="submit" name="login" id="login"
                title="{t}Click here to log in{/t}">{t}Log in{/t}</button>
            </p>
          </div>
        </div>

      </div>

      <input type='hidden' name='javascript' value='false' />

    </form>
  </div>

  <script type="text/javascript" src="themes/materialize/js/materialize.js"></script>

  <script type="text/javascript">
    document.cookie = "gosatest=empty;path=/";
    if (document.cookie.indexOf("gosatest=") > -1)
      document.cookie = "gosatest=empty;path=/;expires=Thu, 01-Jan-1970 00:00:01 GMT";
    else
      document.write("<div class='card-panel red lighten-3'>{$cookies}<\/div>");
  </script>

  <script type="text/javascript">
    document.addEventListener('DOMContentLoaded', function () {
      var elems = document.querySelectorAll('select');
      var instances = M.FormSelect.init(elems, {});
    });
  </script>

  <script language="JavaScript" type="text/javascript">
    enable_keyPress = false;
    nextfield = "{$nextfield}";
    focus_field("{$nextfield}");
    next_msg_dialog();
  </script>

</body>

</html>