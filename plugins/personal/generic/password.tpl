<p>
 {t}You have changed the method your password is stored in the ldap database. For that reason you've to enter your password at this point again. GOsa will then encode it with the selected method.{/t}
</p>

<table summary="{t}Password change dialog{/t}" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
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
  <button type='submit' name='password_finish'>{t}Set password{/t}</button>

  &nbsp;
  <button type='submit' name='password_cancel'>{msgPool type=cancelButton}</button>

</p>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	nextfield= "new_password";
	focus_field('new_password');
  -->
</script>
