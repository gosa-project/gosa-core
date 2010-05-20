 <body>
  {$php_errors}
  <div class='title-bar'>
   <ul>
    <li><img src='themes/default/images/logo.png'></li>
    <li>{t}GOsa setup wizard{/t}</li>
    <li>{$version}</li>
   </ul>
  </div>

  <form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
   {$msg_dialogs}

    <br>
   <div class='navigation'>
    <div class='menu'>
     {$navigation}	
    </div>	
   </div>

   <div class='plugin-area'>
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
