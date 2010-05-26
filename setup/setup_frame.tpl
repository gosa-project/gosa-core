 <body>
  {$php_errors}
  <div class='title-bar'>
   <ul>
    <li><img src='themes/default/images/logo.png' alt='GOsa'></li>
    <li class='right' style='padding-top:10px;padding-right:5px'>{$version}</li>
   </ul>
  </div>

  <form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
   {$msg_dialogs}

   <div class='plugin-area' style='margin:10px;'>
    <div class='plugin'>
    {$errors}
    {$header}
    <hr>
    {$contents}
    <hr>
    {$bottom}
   </div>
   </div>
   <input type='hidden' name='setup_goto_step' value=''>
  </form>
 </body>
</html>
