 <!-- Headline -->
  <body>
  {$php_errors}

  <!-- Title bar including logout button -->
  <div class='title-bar'>
   <ul>
    <li><img src='themes/modern/images/logo.png'></li>
    <li class='right table-wrapper'>
      <div class='logout-label'>
        <form action='logout.php' name='logoutframe' method='post' enctype='multipart/form-data'>
         <input type='submit' name='log-out' id='log-out' title='Log out' value=''>
         <input type='hidden' name='forcedlogout' value='1'>
         <input type="hidden" name="php_c_check" value="1">
        </form>
      </div>
    </li>
    <li class='right table-wrapper'>
      <div class='logged-in-label'>You're logged in as <span class='logged-in-user'>{$username}</span>
      </div>
    </li>
   </ul>
  </div>
  <!-- {$go_home} -->

  <form action='main.php{$plug}' name='mainform' method='post' enctype='multipart/form-data'>
  {$menu}

  {$msg_dialogs}

  <!-- Plugin area -->
  <div class='plugin-area'>
    <div class='plugin-path'>
     <ul class='path-navigation'>
      <li class='left right-border'>
        <div class='nav-home' onClick="openPlugin('');"></div>
      </li>
      <li class='left'>Welcome to GOsa</li>
      <li class='right left-border'>Help</li>
      <li class='right left-border'>Change password</li>
      <li class='right left-border'>My account</li>
     </ul>
    </div>
    {$contents}
  </div>

   {if $channel != ""}
    <input type="hidden" name="_channel_" value="{$channel}">
   {/if}
   {$errors}
   {$focus}
   <input type="hidden" name="php_c_check" value="1">
  </form>
 </body>
</html>
