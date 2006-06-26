<body style="background-color: #8B858B;background-image:none;">
{* GOsa login - smarty template *}
{$php_errors}

<!-- Spacer for some browsers -->
<div class='gosaLoginSpacer'></div>

<table summary="" class='gosaLoginMask' style="text-align:right;" align='center'>
 <tr>
  <td class='gosaLoginBack' style="width:606px;">
   <div class='gosaLoginBack'>
       
    <p class='gosaLoginHeader'>
	{t}Please use your username and password to log in{/t}
    </p>
  
    <!-- Display SSL warning message on demand -->
    <p class='gosaLoginWarning'> {$ssl} </p>
	     <!-- Display SSL warning message on demand -->
    <p class='gosaLoginWarning'> {$ssl} </p>
 
    <!-- Formular data, containing a table to center fields -->
    <form action='index.php' method='post' name='mainform' onSubmit='js_check(this);return true;'>
     {$errors}
     <table summary="" style='vertical-align:middle; text-align:left;' cellspacing='7' align='center'>
      <tr>
        <td>
          <img src='{$personal_img}' alt='{t}Username{/t}' title='{t}Username{/t}'>
        </td>
        <td>
	  <input type='text' name='username' maxlength='25' value='{$username}'
		 title='{t}Username{/t}' onFocus="nextfield= 'password';">
        </td>
      </tr>
      <tr>
        <td>
          <img src='{$password_img}' alt='{t}Password{/t}' title='{t}Password{/t}'>
        </td>
        <td>
	  <input type='password' name='password' maxlength='25' value=''
		 title='{t}Password{/t}' onFocus="nextfield= 'login';">
	</td>
      </tr>
      <tr>
        <td>
          <img src='{$directory_img}' alt='{t}Directory{/t}' title='{t}Directory{/t}'>
        </td>
        <td style='text-align:left;'>
	  <select name='server'  title='{t}Directory{/t}'>
	    {html_options options=$server_options selected=$server_id}
	  </select>
	</td>
      </tr>
      <tr>
        <td colspan='2' style='text-align:right;'>
	  <br />
	  <input type='submit' name='login' value='{t}Sign in{/t}'
	  	 title='{t}Click here to log in{/t}'>
	</td>
      </tr>
     </table>
		<input type='hidden' name='javascript' value='false'/>		
    </form>

    <!-- Display error message on demand -->
    <p class='gosaLoginWarning'> {$message} </p>
    <!-- check, if cookies are enabled -->
    <p class='gosaLoginWarning'>
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



   </div>
  </td>
 </tr>
</table>

<!-- Place cursor in username field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  nextfield= "{$nextfield}";
  document.mainform.{$nextfield}.focus();
  -->
</script>

<!-- Spacer for some browsers -->
<div class='gosaLoginSpacer'></div>
</body>
