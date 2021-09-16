<script type="text/javascript" src="include/pwdStrength.js"></script>
<p>
 {t}To change the terminal root password use the fields below. The changes take effect during the next reboot. Please memorize the new password, because you wouldn't be able to log in.{/t}
</p>
<p>
 <b>{t}Leave fields blank for password inheritance from default entries.{/t}</b>
</p>

<p>
 {t}Changing the password impinges on authentication only.{/t}
</p>

<table summary="{t}System password change{/t}">

  <tr>
    <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
    <td>
        {factory type='password' id='new_password' name='new_password'
            onkeyup="testPasswordCss(\$('new_password').value);" onfocus="nextfield='repeated_password';"}
    </td>
  </tr>
  <tr>
    <td><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
    <td>
        {factory type='password' id='repeated_password' name='repeated_password'
            onfocus="nextfield='password_finish'"}
    </td>
  </tr>
  <tr>
       <td>{t}Password strength{/t}</td>
       <td>
        <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#DC143C;display:block;height:5px;">
        <span id="meterFull" style="padding:0;margin:0;z-index:100;width:0;background-color:#006400;display:block;height:5px;"></span></span>
       </td>
      </tr>
 
</table>

<hr>
<div class="plugin-actions">
  <button type='submit' id='password_finish'name='password_finish'>{t}Set password{/t}</button>
  <button type='submit' id='password_cancel'name='password_cancel'>{msgPool type=cancelButton}</button>
</div>


<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">	
  <!-- // First input field on page
  	nextfield= 'new_password';
	focus_field('new_password');
  -->
</script>
