<p>
 {t}You have changed the method your password is stored in the ldap database. For that reason you've to enter your password at this point again. GOsa will then encode it with the selected method.{/t}
</p>

<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
    <td><input id="new_password" type="password" name="new_password" size="30" maxlength="40" onFocus="nextfield= 'repeated_password';"></td>
  </tr>
  <tr>
    <td><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
    <td><input id="repeated_password" type="password" name="repeated_password" size="30" maxlength="40" onFocus="nextfiled= 'password_finish'"></td>
  </tr>
</table>
<br>
<p class="plugbottom">
  <input type=submit name="password_finish" value="{t}Set password{/t}">
  &nbsp;
  <input type=submit name="password_cancel" value="{msgPool type=cancelButton}">
</p>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	nextfield= "new_password";
	focus_field('new_password');
  -->
</script>
