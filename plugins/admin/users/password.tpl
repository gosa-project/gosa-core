<p>
 {t}To change the user password use the fields below. The changes take effect immediately. Please memorize the new password, because the user wouldn't be able to login without it.{/t}
</p>

<p>
 {t}Changing the password affects your authentification on mail, proxy, samba and unix services.{/t}
</p>

<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td><b>{t}New password{/t}</b></td>
    <td><input type="password" name="new_password" size="30" maxlength="40" onFocus="nextfield= 'repeated_password';"></td>
  </tr>
  <tr>
    <td><b>{t}Repeat new password{/t}</b></td>
    <td><input type="password" name="repeated_password" size="30" maxlength="40" onFocus="nextfiled= 'password_finish'"></td>
  </tr>
</table>
<br>
<p class="plugbottom">
  <input type=submit name="password_finish" value="{t}Set password{/t}">
  &nbsp;
  <input type=submit name="password_cancel" value="{t}Cancel{/t}">
</p>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  nextfield= "new_password";
  document.mainform.new_password.focus();
  -->
</script>
