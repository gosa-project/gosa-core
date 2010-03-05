  <body>
   {$php_errors}
   <div class='title-bar'>
    <ul>
     <li>{$logo}</li>
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
       <div class='logged-in-label'>{$loggedin}</div>
     </li>
    </ul>
   </div>
   <form action='main.php{$plug}' id='mainform' name='mainform' method='post' enctype='multipart/form-data'>
   {$menu}
   {$msg_dialogs}
   <div class='plugin-area'>
     {$pathMenu}
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
