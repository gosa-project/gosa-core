<script type="text/javascript" src="include/pwdStrength.js"></script>

<p>
  {t}To change your personal password use the fields below. The changes take effect immediately. Please memorize the new password, because you wouldn't be able to login without it.{/t}
</p>

<hr>

{if $passwordExpired}
    <b><font color='red'>{t}Your Password has expired. Please choose a new password.{/t}</font></b>
<hr>
{/if}


{if !$proposalEnabled}
  <table cellpadding=4 border=0 summary="{t}Password change dialog{/t}">
    <tr>
      <td><b><LABEL for="current_password">{t}Current password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='current_password' id='current_password' onfocus="nextfield= 'new_password';"}
      </td>
    </tr>
    <tr>
      <td><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='new_password' id='new_password' 
              onkeyup="testPasswordCss(\$('new_password').value)"  onfocus="nextfield= 'repeated_password';"}
      </td>
    </tr>
    <tr>
      <td><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='repeated_password' id='repeated_password' onfocus="nextfield= 'password_finish';"}
      </td>
    </tr>
    <tr>
      <td><b>{t}Password strength{/t}</b></td>
      <td>
      <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#DC143C;display:block;height:7px;">
      <span id="meterFull" style="padding:0;margin:0;z-index:100;width:0;background-color:#006400;display:block;height:7px;"></span></span>
      </td>
    </tr>
  </table>
{else}
  <table cellpadding=4 border=0 summary="{t}Password change dialog{/t}">
    <tr>
      <td><b><LABEL for="current_password">{t}Current password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='current_password' id='current_password' onfocus="nextfield= 'new_password';"}
      </td>
    </tr>
    <tr>
      <td>
        <input type='radio' value='1' id='proposalSelected' name='proposalSelected' onClick='updateFields();'
            {if $proposalSelected} checked {/if}>&nbsp;<b>{t}Use proposal{/t}</b>
      </td>
      <td>
        <div id='proposalText' 
            style='
                  width:180px;
                  border:1px solid #CCC;
                  padding:3px;
                  padding-top:5px;
                  padding-bottom:4px;
            '>{$proposal}</div>
      </td>
      <td style='vertical-align: middle;'>
            {image path='images/lists/reload.png' action='refreshProposal'}
      </td>
    </tr>
    <tr>
      <td>
        <input type='radio' value='0' name='proposalSelected' onClick='updateFields();'
            {if !$proposalSelected} checked {/if}>&nbsp;<b>{t}Manually specify a password{/t}</b>
      </td>
    </tr>
    <tr>
      <td style='padding-left:40px;'><b><LABEL for="new_password">{t}New password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='new_password' id='new_password' 
              onkeyup="testPasswordCss(\$('new_password').value)"  onfocus="nextfield= 'repeated_password';"}
      </td>
    </tr>
    <tr>
      <td style='padding-left:40px;'><b><LABEL for="repeated_password">{t}Repeat new password{/t}</LABEL></b></td>
      <td>
          {factory type='password' name='repeated_password' id='repeated_password' onfocus="nextfield= 'password_finish';"}
      </td>
    </tr>
    <tr>
      <td  style='padding-left:40px;'><b>{t}Password strength{/t}</b></td>
      <td>
      <span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:#DC143C;display:block;height:7px;">
      <span id="meterFull" style="padding:0;margin:0;z-index:100;width:0;background-color:#006400;display:block;height:7px;"></span></span>
      </td>
    </tr>
  </table>
{/if}
<br>

<hr>
<div class="plugin-actions">
  <button type='submit' name='password_finish'>{t}Set password{/t}</button>
  <button type=reset id="password_cancel" name="password_cancel">{t}Clear fields{/t}</button>
</div>

<input type='hidden' id='formSubmit'>

<input type="hidden" name="ignore">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    nextfield= 'current_password';
	focus_field('current_password');


    function updateFields()
    {
        if($('proposalSelected').checked){
            $('new_password').disable();
            $('repeated_password').disable();
            $('proposalText').setStyle(
                 'background-color:#FFF;' +
                 'color:#000;' + 
                 'width:180px;' + 
                 'border:1px solid #CCC;' + 
                 'padding:3px;' + 
                 'padding-top:5px;' + 
                 'padding-bottom:4px;');
        }else{
            $('new_password').enable();
            $('repeated_password').enable();
            $('proposalText').setStyle(
                 'background-color:#F0F0F0;' +
                 'color:#666;' + 
                 'width:180px;' + 
                 'border:1px solid #CCC;' + 
                 'padding:3px;' + 
                 'padding-top:5px;' +
                 'padding-bottom:4px;');
        }
    }
    updateFields();
</script>

