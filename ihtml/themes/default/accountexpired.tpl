<body style="background-color: #8B858B;background-image:none;">
{* GOsa login - smarty template *}
{$php_errors}
{$errors}

<!-- Spacer for some browsers -->
<div class='gosaLoginSpacer'></div>

<table summary="" class='gosaLoginMask' style="text-align:right;" align='center'>
 <tr>
  <td class='gosaLoginBack' style="width:606px;">
   <div class='gosaLoginBack'>
       
    <p class='gosaLoginHeader'>
	{t}Your password has expired, insert your new password twice{/t}
    </p>
 
    <!-- Formular data, containing a table to center fields -->
    <form action='index.php' method='post' name='mainform' onSubmit='js_check(this);return true;'>
     <table summary="" style='vertical-align:middle; text-align:left;' cellspacing='7' align='center'>
      <tr>
        <td>
          <img src='{$password_img}' alt='{t}Password{/t}' title='{t}Password{/t}'>
        </td>
        <td>
    <input type='password' name='password' maxlength='25' value=''
     title='{t}Password{/t}' onFocus="nextfield= 'verifypassword';">
        </td>
      </tr>
      <tr>
        <td>
          <img src='{$password_img}' alt='{t}Verify Password{/t}' title='{t}Verify Password{/t}'>
        </td>
        <td>
	  <input type='password' name='verifypassword' maxlength='25' value=''
		 title='{t}Verify Password{/t}' onFocus="nextfield= 'changepassword';">
	</td>
      </tr>
      <tr>
        <td colspan='2' style='text-align:right;'>
	  <br />
	  <input type='submit' name='changepassword' value='{t}Change Password{/t}'
	  	 title='{t}Click here to Change your password{/t}'>
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
