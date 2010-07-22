
<!DOCTYPE html>
<html>
  <head>
    <title>GOsa - {t}Change your password{/t}</title>
    <meta name="generator" content="my hands">
    <meta name="description" content="GOsa - Login">
    <meta name="author" lang="de" content="Cajus Pollmeier">
    <meta http-equiv="Expires" content="Mon, 26 Jul 1997 05:00:00 GMT">
    <meta http-equiv="Last-Modified" content="{$date} GMT">
    <meta http-equiv="Cache-Control" content="no-cache">
    <meta http-equiv="Pragma" content="no-cache">
    <meta http-equiv="Cache-Control" content="post-check=0, pre-check=0">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=9">

    <style type="text/css">@import url('themes/default/style.css');</style>
    <link rel="stylesheet" type="text/css" href="themes/default/printer.css" media="print">

    <!--[if IE]>
    <style type="text/css">
      div.listContainer { height: 121px; overflow-x:hidden; overflow-y:auto; }
    </style>
    <![endif]-->

    <!-- Include correct theme icon sets -->
    <style type="text/css">
      div.img, div.img div, input[type=submit].img{
        background-image:url(themes/default/images/img.png);
      }
    </style>
    <link rel="shortcut icon" href="favicon.ico">
    <script language="javascript" src="include/prototype.js" type="text/javascript"></script>
    <script language="javascript" src="include/gosa.js" type="text/javascript"></script>
    <script language="javascript" src="include/scriptaculous.js" type="text/javascript"></script>
    <script language="javascript" src="include/effects.js" type="text/javascript"></script>
    <script language="javascript" src="include/dragdrop.js" type="text/javascript"></script>
    <script language="javascript" src="include/controls.js" type="text/javascript"></script>
    <script language="javascript" src="include/pulldown.js" type="text/javascript"></script>
    <script language="javascript" src="include/datepicker.js" type="text/javascript"></script>
  </head>

  <body>

    {$php_errors}

    <div class='title-bar'>
        <ul>
            <li>{$logo}</li>
        </ul>
    </div>

    <div style='padding:3px;'>

    <form action='password.php{$params}' method='post' name='mainform' onSubmit='js_check(this);return true;'>

        {$msg_dialogs}

        {if $changed}

            <h3>{t}Your password has been changed successfully.{/t}</h3>

        {else}

                <h3>{t}Password change{/t}</h3>

                {if $ssl}<div class='login-warning'>{$ssl}</div>{/if}

                <!-- Display error message on demand -->
                {if $message}<div class='login-warning'>{$message}</div>{/if}

                <p class="infotext">{t}This dialog provides a simple way to change your password. Enter the current password and the new password (twice) in the fields below and press the 'Change' button.{/t}</p>

                <table summary="{t}Change password{/t}">
                    {if $show_directory_chooser}
                        <tr>
                            <td>{t}Directory{/t}</td>
                            <td>
                                <select name='server'  title='{t}Directory{/t}'>
                                    {html_options options=$server_options selected=$server_id}
                                </select>
                            </td>
                        </tr>
                    {/if}
                <tr>
                    <td><b>{t}Username{/t}</b></td>
                    <td>
                        {if $display_username}
                            <input type='text' name='uid' maxlength='40' value='{$uid}' 
                                title='{t}Username{/t}' onFocus="nextfield= 'current_password';">
                        {else}
                            <i>{$uid}</i>
                        {/if}
                    </td>
                </tr>
                <tr>
                    <td><b><LABEL for="current_password">{t}Current password{/t}</LABEL></b></td>
                    <td>
                        {factory type='password' name='current_password' id='current_password' onfocus="nextfield= 'new_password';"}
                    </td>
                </tr>
                <tr>
                    <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
                    <td>
                        {factory type='password' name='new_password' id='new_password'
                            onkeyup="testPasswordCss(\$('new_password').value)"  onfocus="nextfield= 'new_password_repeated';"}
                    </td>
                </tr>
                <tr>
                    <td><b><LABEL for="new_password_repeated">{t}Repeat new password{/t}</LABEL></b></td>
                    <td>
                        {factory type='password' name='new_password_repeated' id='new_password_repeated' 
                                onfocus="nextfield= 'password_finish';"}
                    </td>
                </tr>
                <tr>
                    <td><b>{t}Password strength{/t}</b></td>
                    <td>
                        <span id="meterEmpty" style="padding:0;margin:0;width:100%;
                                background-color:#DC143C;display:block;height:7px;">
                        <span id="meterFull" style="padding:0;margin:0;z-index:100;
                                width:0;background-color:#006400;display:block;height:7px;"></span></span>
                    </td>
                </tr>
            </table>

            <hr>

            <div class="plugin-actions">
                <button type='submit' name='apply' 
                    title='{t}Click here to change your password{/t}'>{t}Set password{/t}</button>
                <input type='hidden' id='formSubmit'>
            </div>


            <!-- check, if cookies are enabled -->
            <p class='warning'>
             <script language="JavaScript" type="text/javascript">
                <!--
                    document.cookie = "gosatest=empty;path=/";
                    if (document.cookie.indexOf( "gosatest=") > -1 )
                        document.cookie = "gosatest=empty;path=/;expires=Thu, 01-Jan-1970 00:00:01 GMT";
                    else
                        document.write("{$cookies}");
                -->
             </script>
            </p>

          {$errors}
          <input type="hidden" name="php_c_check" value="1">

        {/if}
    </form>
    </div>

    <script language="JavaScript" type="text/javascript">
        next_msg_dialog();
    </script>

  </body>
</html>
