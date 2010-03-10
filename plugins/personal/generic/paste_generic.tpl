<h3>{t}User settings{/t}</h3>

<table width="100%" summary="{t}Paste user{/t}">
 <tr>
 <td class="right-border" style="width:50%">
   <table width="100%" summary="{t}Personal information{/t}">
 	<tr>
 	  <td><label for="sn">{t}Last name{/t}</label></td>
 	  <td><input type='text' id="sn" name="sn" size=25 maxlength=60  value="{$sn}"></td>
 	</tr>
 	<tr>
 	  <td><label for="givenName">{t}First name{/t}</label></td>
 	  <td><input type='text' id="givenName" name="givenName" size=25 maxlength=60 value="{$givenName}"></td>
 	</tr>
 	<tr>
 	  <td><label for="uid">{t}Login{/t}</label></td>
 	  <td><input type='text' id="uid" name="uid" size=25 maxlength=60 value="{$uid}"></td>
 	</tr>
 	<tr>
 		<td>
 			{t}Password{/t}
 		</td>
 		<td>
 			<input type="radio" {if $passwordTodo=="clear"} checked{/if} name="passwordTodo" value="clear">{t}Clear password{/t}<br>
 			<input type="radio" {if $passwordTodo=="new"}   checked{/if} name="passwordTodo" value="new">{t}Set new password{/t}
 		</td>
 	</tr>
   </table>
 </td>
 <td>
  <table summary="{t}The users picture{/t}">
   <tr>
    <td style="vertical-align: middle; width:147px; height:200px; background-color:gray;">
     <img src="getbin.php?rand={$rand}" alt='' style='width:147px;' >
    </td>
   </tr>
  </table>
  <p>
   <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
    <input id="picture_file" name="picture_file" type="file" size="20" maxlength="255" accept="image/*.jpg">
     &nbsp;
    <button type='submit' name='picture_remove'>{t}Remove picture{/t}</button>
   </p>
  </td>
 </tr>
</table>
<br>
