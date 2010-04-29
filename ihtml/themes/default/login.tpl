  <body>
   {$php_errors}
   <div class='title-bar'>
    <ul>
     <li>{$logo}</li>
     <li class='right table-wrapper'>
       <div class='logged-in-label'>{$version}</div>
     </li>
    </ul>
   </div>

   <form action='index.php' method='post' name='mainform' onSubmit='js_check(this);return true;'>
    {image path="images/empty.png" align="top" action="focus"}
    {$msg_dialogs}

    {if $ssl}<div class='login-warning'>{$ssl}</div>{/if}
    {if $lifetime}<div class='login-warning'>{$lifetime}</div>{/if}
    <script language="JavaScript" type="text/javascript">
      <!--
        document.cookie = "gosatest=empty;path=/";
        if (document.cookie.indexOf( "gosatest=") > -1 )
    	document.cookie = "gosatest=empty;path=/;expires=Thu, 01-Jan-1970 00:00:01 GMT";
        else
    	document.write("<div class='login-warning'>{$cookies}<\/div>");
      -->
    </script>
     
  
    <div class='login-box'>
     <div class='login-box-header'>
     {t}Login to GOsa{/t}
     </div>

     <div class='login-box-container'>
       <div class='login-element-container'>
        <div style='background-color:green'>
         <div class='login-label'><label for='username'>{t}Username{/t}</label></div>
         <div class='login-input'>
          <input type='text' name='username' maxlength='40' value='{$username}'
            	 title='{t}Username{/t}' onFocus="nextfield= 'password';" />
         </div>
        </div>
         <div class="clear"></div>
       </div>

       <div class='login-element-container'>
        <div class='login-label'><label for='password'>{t}Password{/t}</label></div>
        <div class='login-input'>
         {factory type='password' name='password' id='password' onfocus="nextfield= 'login'"}
        </div>
        <div class="clear"></div>
       </div>

     </div>
     {if $message}<hr><div class='login-inline-message'>{$message}</div>{/if}
     {if $errors}<hr><div class='login-inline-message'>{$errors}</div>{/if}
     <hr>
     
     <div class='login-element-container'>
      <div class='login-label'>
       <select size='1' name='server'  title='{t}Choose the directory to work on{/t}'>
      	{html_options options=$server_options selected=$server_id}
       </select>
      </div>
      <div class='right'>
       <button type='submit' name='login' title='{t}Click here to log in{/t}' onFocus="nextfield='login';">{t}Log in{/t}</button>
       <input type='hidden' name='login' value='{t}Log in{/t}' />             
      </div>
      <div class="clear"></div>
     </div>
     
     
     <input type='hidden' name='javascript' value='false' />

    </div>

   </form>

   <div class="clear"></div>
   
   
   <script language="JavaScript" type="text/javascript">
   <!-- 
     enable_keyPress = false;
     nextfield= "{$nextfield}";
     focus_field("{$nextfield}");
     next_msg_dialog();
   -->
   </script>

 </body>
</html>
