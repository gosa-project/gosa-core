<script type="text/javascript" src="include/pwdStrength.js"></script>

<p>
  {t}To change your personal password use the fields below. The changes take effect immediately. Please memorize the new password, because you wouldn't be able to login without it.{/t}
</p>

<p>
  {t}Changing the password affects your authentification on mail, proxy, samba and unix services.{/t}
</p>

<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td><b><LABEL for="current_password">{t}Current password{/t}</LABEL></b></td>
    <td><input id="current_password" type="password" name="current_password" size="30" maxlength="40"
		onFocus="nextfield= 'new_password';"></td>
  </tr>
  <tr>
    <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
    <td><input id="new_password" type="password" name="new_password" size="30" maxlength="40"
		onFocus="nextfield= 'repeated_password';" onkeyup="testPasswordCss(document.getElementById('new_password').value);"></td>
  </tr>
  <tr>
    <td><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
    <td><input id="repeated_password" type="password" name="repeated_password" size="30" maxlength="40"
		onFocus="nextfield= 'password_finish';"></td>
  </tr>
  <tr>
    <td><b>{t}Password strength{/t}</b></td>
    <td>
	<span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#DC143C;display:block;height:5px;">
	<span id="meterFull" style="padding:0;margin:0;z-index:100;width:0;background-color:#006400;display:block;height:5px;"></span></span>
    </td>
  </tr>
</table>

<br>

<p class="plugbottom">
  <button type='submit' name='password_finish'>{t}Set password{/t}</button>

  &nbsp;
  <input type=reset id="password_cancel" name="password_cancel" value="{t}Clear fields{/t}">
  <input type='hidden' id='formSubmit'>
</p>

<input type="hidden" name="ignore">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  nextfield= 'current_password';
	focus_field('current_password');
  -->
</script>

