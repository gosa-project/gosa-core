<p>
 {t}You have changed the method your password is stored in the ldap database. For that reason you've to enter your password at this point again. GOsa will then encode it with the selected method.{/t}
</p>

<table summary="{t}Password change dialog{/t}" cellpadding=4 border=0>
  <tr>
    <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
    <td>
        {factory type='password' id='new_password' name='new_password' onfocus="nextfield='repeated_password'"}
  </tr>
  <tr>
    <td><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
    <td>
        {factory type='password' id='repeated_password' name='repeated_password' onfocus="nextfield='password_finish'"}
    </td>
  </tr>
</table>
<br>

<hr>
<div class="plugin-actions">
  <button type='submit' id='password_finish'name='password_finish'>{t}Set password{/t}</button>
  <button type='submit' id='password_cancel'name='password_cancel'>{msgPool type=cancelButton}</button>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	nextfield= "new_password";
	focus_field('new_password');
  -->
</script>
